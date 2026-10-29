// usbd_midi.c
#include "usbd_midi.h"
#include "usbd_conf.h"
#include "usbd_core.h"
#include "usbd_def.h"
#include "usbd_enum.h"
//#include "cmsis_os2.h"
#include "usbd_desc.h"
#include "stdio.h"

/* Receive ring buffer:
 * written by the USB ISR (DataOut), read by the main loop (UsbMidiRxRead) */
#define MIDI_RX_RING_SIZE	256U	/* power of two */
static uint8_t s_rx_ring[MIDI_RX_RING_SIZE];
static volatile uint32_t s_rx_head;	/* ISR writes */
static volatile uint32_t s_rx_tail;	/* main loop reads */

static uint32_t RxRingFree(void)
{
    return MIDI_RX_RING_SIZE - (s_rx_head - s_rx_tail);
}

usb_dev* volatile pInstance = NULL;
extern usb_core_driver hUsbDeviceFS;
volatile uint8_t USB_Tx_State = 0;

__ALIGN_BEGIN uint8_t USB_Rx_Buffer[USB_FS_MAX_PACKET_SIZE] __ALIGN_END;	/* one packet: every receive is armed with MIDI_RX_PACKET_SIZE */
/* The OUT endpoint is not armed while the receive buffer cannot take a whole packet: the host
 * retries (NAK) until the application has read enough, so a fast sender loses nothing */
static volatile uint8_t ui8UsbRxPaused = 0;
/* one packet per transfer, so a transfer never brings more than the receive buffer was checked for */
#define MIDI_RX_PACKET_SIZE	USB_FS_MAX_PACKET_SIZE
#define MIDI_TX_TRANSFER_MAX	400		/* longest transfer */
/* an IN transfer is under way until its completion (data_in): a send before it would overwrite it */
static volatile uint8_t ui8UsbTxBusy = 0;

static uint8_t USBD_MIDI_Init(usb_dev* pdev, uint8_t cfgidx);
static uint8_t USBD_MIDI_DeInit(usb_dev* pdev, uint8_t cfgidx);
static uint8_t USBD_MIDI_DataIn(usb_dev* pdev, uint8_t epnum);
static uint8_t USBD_MIDI_DataOut(usb_dev* pdev, uint8_t epnum);

#define NO_CMD 0xFFU

usb_class_core stUSBD_MIDI = {
    .command = NO_CMD,
    .alter_set = 0U,

    .init = USBD_MIDI_Init,
    .deinit = USBD_MIDI_DeInit,
    .req_proc = NULL,
    .set_intf = NULL,
    .ctlx_in = NULL,
    .ctlx_out = NULL,

    .data_in = USBD_MIDI_DataIn,
    .data_out = USBD_MIDI_DataOut,
    .SOF = NULL,
    .incomplete_isoc_in = NULL,
    .incomplete_isoc_out = NULL
};

const usb_desc_ep* usbd_get_desc_edpin(void);
const usb_desc_ep* usbd_get_desc_edpout(void);

static uint8_t USBD_MIDI_Init(usb_dev* pdev, uint8_t cfgidx)
{
    pInstance = pdev;
    ui8UsbRxPaused = 0;
    ui8UsbTxBusy = 0;

    //	USBD_OpenEP(pdev, MIDI_IN_EP, USBD_EP_TYPE_BULK, MIDI_DATA_IN_PACKET_SIZE);
    //	USBD_OpenEP(pdev, MIDI_OUT_EP, USBD_EP_TYPE_BULK, MIDI_DATA_OUT_PACKET_SIZE);
    //	USBD_PrepareReceive(pdev,MIDI_OUT_EP, (uint8_t*)(USB_Rx_Buffer), MIDI_DATA_OUT_PACKET_SIZE);

    /* initialize the data endpoints */
    usbd_ep_setup(pdev, usbd_get_desc_edpin());
    usbd_ep_setup(pdev, usbd_get_desc_edpout());
    usbd_ep_recev(pdev, MIDI_OUT_EP, (uint8_t*)(USB_Rx_Buffer), MIDI_RX_PACKET_SIZE);

    return 0;
}

static uint8_t USBD_MIDI_DeInit(usb_dev* pdev, uint8_t cfgidx)
{
    pInstance = NULL;
    ui8UsbTxBusy = 0;

    //	USBD_CloseEP(pdev, MIDI_IN_EP);
    //	USBD_CloseEP(pdev, MIDI_OUT_EP);

    usbd_ep_clear(pdev, MIDI_IN_EP);
    usbd_ep_clear(pdev, MIDI_OUT_EP);

    return 0;
}

static uint8_t USBD_MIDI_DataIn(usb_dev* pdev, uint8_t epnum)
{

    ui8UsbTxBusy = 0;	/* MIDI_IN_EP is the only IN data endpoint */

    return USBD_OK;
}

uint16_t USB_Rx_Cnt;
    uint8_t* pRecv;
static uint8_t USBD_MIDI_DataOut(usb_dev* pdev, uint8_t epnum)
{
    USB_Rx_Cnt = pdev->dev.transc_out[epnum].xfer_count;

    pRecv = (uint8_t*)USB_Rx_Buffer;


    /* copy the packet out before the buffer is armed again */
    while (USB_Rx_Cnt) 
		{
      s_rx_ring[s_rx_head & (MIDI_RX_RING_SIZE - 1U)] = *pRecv;
      s_rx_head++;
      pRecv++;
			USB_Rx_Cnt--;
			
    }

    if (RxRingFree() >= MIDI_RX_PACKET_SIZE)
        usbd_ep_recev(pdev, MIDI_OUT_EP, (uint8_t*)(USB_Rx_Buffer), MIDI_RX_PACKET_SIZE);
    else
        ui8UsbRxPaused = 1;
    return 0;
}

/* Called by the reader of the receive buffer: arm the OUT endpoint again once a packet fits */
void UsbMidiRxResume(void)
{
    uint32_t ui32Primask;

    if (!ui8UsbRxPaused || (RxRingFree() < MIDI_RX_PACKET_SIZE))
        return;
    ui32Primask = __get_PRIMASK();
    __disable_irq();
    if (ui8UsbRxPaused && (pInstance != NULL))
    {
        ui8UsbRxPaused = 0;
        usbd_ep_recev(pInstance, MIDI_OUT_EP, (uint8_t*)(USB_Rx_Buffer), MIDI_RX_PACKET_SIZE);
    }
    __set_PRIMASK(ui32Primask);
}

/* Main loop side of the receive ring buffer: copies up to max bytes, re-arms the OUT
 * endpoint if it was paused for lack of space */
uint32_t UsbMidiRxRead(uint8_t* dst, uint32_t max)
{
    uint32_t n = 0;

    while ((n < max) && (s_rx_tail != s_rx_head))
    {
        dst[n++] = s_rx_ring[s_rx_tail & (MIDI_RX_RING_SIZE - 1U)];
        s_rx_tail++;
    }
    UsbMidiRxResume();
    return n;
}

/*******************************************************************************
* @FunctionName  :  DrvUsbTxWords
* @Description   :  none
* @Output        :  src, nWords
* @ReturnValue   :  none

*******************************************************************************/
void DrvUsbTxWords(uint32_t* src, uint32_t nWords)
{
   /* if (hUsbDeviceFS.dev.cur_status == USBD_STATE_CONFIGURED) {
        uint32_t u32Wait;

        if (gFlagB.isBusyUsbTx) {
            u32Wait = SystemCoreClock / 10000;
            while (gFlagB.isBusyUsbTx && --u32Wait)
                ;
        }

        gFlagB.isBusyUsbTx = true;
        usbd_ep_send(pInstance, MIDI_IN_EP, (uint8_t*)src, (nWords * 4));
    }*/
}

uint8_t* USBD_MIDI_GetCfgDesc(uint16_t* length)
{
    //*length = sizeof (USBD_MIDI_CfgDesc);

    // return USBD_MIDI_CfgDesc;
    return NULL;
}

/*******************************************************************************
 * @FunctionName  :  IsUsbConnected
 * @Description   :  none
 * @Input         :  none
 * @Output        :  none
 * @ReturnValue   :  true--connected , otherwise not
 *******************************************************************************/
uint8_t IsUsbConnected(void)
{
    return (hUsbDeviceFS.dev.cur_status == USBD_STATE_CONFIGURED);
}

void USBTxNBytes(uint8_t* src, uint32_t nBytes)
{
    static uint32_t mLstTxTime = 0;
    uint32_t u32SizeUsbTx;
    uint32_t i = 0, len;
    // static uint32_t nU32Buf[EP_MIDI_OUT_WORD_SIZE];

    u32SizeUsbTx = nBytes;

    if (u32SizeUsbTx) 
    {
      if (hUsbDeviceFS.dev.cur_status == USBD_STATE_CONFIGURED) 
      {
        static uint8_t nU32Buf[MIDI_TX_TRANSFER_MAX];
        i = 0;
        len = nBytes;
                
        if (len > MIDI_TX_TRANSFER_MAX)
            len = MIDI_TX_TRANSFER_MAX;

        for (i = 0; i < len; i++) {
            nU32Buf[i] = *src++;
        }

        if (i) 
        {

            ui8UsbTxBusy = 1;
            usbd_ep_send((usb_core_driver*)&hUsbDeviceFS, MIDI_IN_EP, (uint8_t*)nU32Buf, /*(i * 4)*/ i);

        }

    }
  }
}

uint8_t UsbMidiTxBusy(void)
{
    return ui8UsbTxBusy;
}

/* the host did not take the transfer for a long time: the next one goes out anyway */
void UsbMidiTxBusyClear(void)
{
    ui8UsbTxBusy = 0;
}
