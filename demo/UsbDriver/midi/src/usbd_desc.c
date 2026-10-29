 // usbd_desc.c
 
#include "usbd_core.h"
 #include "usbd_desc.h"
 #include "usbd_conf.h"
 #include "usbd_enum.h"
 #include "usbd_midi.h"
 
 
 #define	 USBD_VID					  (0x1397)   // vendor id
 
 #define USBD_PID               0x12CF

 
 #define USBD_LANGID_STRING             0x0409   /* English (United States) */
 
 
 
 
 //__attribute__((section(".RAM2")))
 usb_core_driver hUsbDeviceFS;
 extern usb_class_core stUSBD_MIDI;
 
 // USB standard device descriptor.
 __ALIGN_BEGIN uint8_t USBD_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
 {
 	0x12,                       /*bLength */
 	USB_DESC_TYPE_DEVICE,       /*bDescriptorType*/
 	0x00,                       /*bcdUSB */
 	0x02,
 	0x00,//0x02,                       /*bDeviceClass*/
 	0x00,//0x02,                       /*bDeviceSubClass*/
 	0x00,                       /*bDeviceProtocol*/
 	USB_MAX_EP0_SIZE,           /*bMaxPacketSize*/
 	LOBYTE(USBD_VID),           /*idVendor*/
 	HIBYTE(USBD_VID),           /*idVendor*/
 	LOBYTE(USBD_PID),           /*idProduct*/
 	HIBYTE(USBD_PID),           /*idProduct*/
 	0x02,                       /*bcdDevice rel. 2.02: two MIDI ports, one external jack pair per cable */
 	0x02,
 	USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
 	USBD_IDX_PRODUCT_STR,       /*Index of product string*/
 	USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
 	USBD_MAX_NUM_CONFIGURATION  /*bNumConfigurations*/
 
 };
 
 
 
 
 // USB MIDI device Configuration Descriptor
 
 __ALIGN_BEGIN static uint8_t USBD_MIDI_CfgDesc[USB_CONFIG_DESC_SIZE]  __ALIGN_END =
 {
     0x09,  							/* bLength  */
 	USB_DESC_TYPE_CONFIGURATION,	/* bDescriptorType , refer to USB protocol P251 Table 9-5 */
 	USB_CONFIG_DESC_SIZE,
 	0x00,						/* wTotalLength  */
 	0x02,						/* bNumInterfaces */
 	0x01,						/* bConfigurationValue */
 	0x00,						/* iConfiguration , unused if 0 */
 	0x80,	                    /* bmAttributes  bus power,no remote wakeup*/
 	0x32,				        /* MaxPower    (2x mA)  0x32*/

 	/*************** 3 AudioControl Interface Descriptors ***************/
 	/*3.1 Standard AC Interface Descriptor*/
 	0x09,							/* bLength  */
 	USB_DESC_TYPE_INTERFACE,	/* bDescriptorType  */
 	0x00,						/* bInterfaceNumber  */
 	0x00,						/* bAlternateSetting  */
 	0x00,						/* bNumEndpoints  */
 	0x01,						/* bInterfaceClass  */
 	0x01,						/* bInterfaceSubClass  */
 	0x00,						/* bInterfaceProtocol  */
 	0x00,						/* iInterface  */

 	/*3.2 Class-specific AC Interface Descriptor*/
 	0x09,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x01,						/* bDescriptorSubtype */
 	0x00,
 	0x01,						/* bcdADC 0x0100 */
 	0x09,
 	0x00,						/* wTotalLength 0x0009 */
 	0x01,						/* bInCollection */
 	0x01,						/* baInterfaceNr(1) */

 	/*************** 4 MIDIStreaming Interface ***************/
 	/* Two virtual cables = two MIDI ports on the host: cable 0 "BCR-32 CTRL" (controller, CtrlTribe),
 	 * cable 1 "BCR-32 ZAQ" (sequencer). Embedded IN jacks 1/2 take the host's data (OUT endpoint),
 	 * embedded OUT jacks 3/7 carry the device's data (IN endpoint); 2/4/6/8 are the DIN side.
 	 * macOS keeps the port layout it saw first for a device: after the single-port firmware it
 	 * showed one port until its MIDI configuration was renewed, whatever the descriptor said. */
 	/*4.1 Standard MS Interface Descriptor*/
 	0x09,						/* bLength */
 	USB_DESC_TYPE_INTERFACE,	/* bDescriptorType */
 	0x01,						/* bInterfaceNumber = 1 */
 	0x00,						/* bAlternateSetting */
 	0x02,						/* bNumEndpoints */
 	0x01,						/* bInterfaceClass */
 	0x03,						/* bInterfaceSubClass */
 	0x00,						/* bInterfaceProtocol */
 	0x00,						/* iInterface */

 	/*4.2 Class-specific MS Interface Descriptor*/
 	0x07,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x01,						/* bDescriptorSubType = MS_HEADER */
 	0x00,						
 	0x01,						/* bcdADC = 0x0100 */
 	0x61,
 	0x00,						/* wTotalLength = 0x0061: header, 8 jacks, 2 endpoints with CS descriptors (spec appendix B) */
 	/* Jacks listed per cable (embedded IN, external IN, embedded OUT, external OUT), the layout
 	 * common devices use (AQ64L, TinyUSB); every cable has its own external jack pair */
 	/*4.3.1 MIDI IN Jack - Embedded, cable 0 */
 	0x06,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x02,						/* bDescriptorSubtype MIDI IN Jack */
 	0x01,						/* bJackType Embedded */
 	0x01,						/* bJackID = 1 */
 	0x04,						/* iJack = "BCR-32 CTRL" */
 	/*4.3.2 MIDI IN Jack - External (DIN IN side of cable 0) */
 	0x06,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x02,						/* bDescriptorSubtype MIDI IN Jack */
 	0x02,						/* bJackType External */
 	0x02,						/* bJackID = 2 */
 	0x00,						/* iJack */
 	/*4.3.3 MIDI OUT Jack - Embedded, cable 0 */
 	0x09,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x03,						/* bDescriptorSubType = MIDI OUT JACK */
 	0x01,						/* bJackType Embedded */
 	0x03,						/* bJackID = 3 */
 	0x01,						/* bNrInputPins = 1 */
 	0x02,						/* baSourceID = Jack2 */
 	0x01,						/* baSourcePin */
 	0x04,						/* iJack = "BCR-32 CTRL" */
 	/*4.3.4 MIDI OUT Jack - External (DIN OUT), fed by cable 0 */
 	0x09,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x03,						/* bDescriptorSubType = MIDI OUT JACK */
 	0x02,						/* bJackType External */
 	0x04,						/* bJackID = 4 */
 	0x01,						/* bNrInputPins = 1 */
 	0x01,						/* baSourceID = Jack1 */
 	0x01,						/* baSourcePin */
 	0x00,						/* iJack */
 	/*4.4.1 MIDI IN Jack - Embedded, cable 1 */
 	0x06,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x02,						/* bDescriptorSubtype MIDI IN Jack */
 	0x01,						/* bJackType Embedded */
 	0x05,						/* bJackID = 5 */
 	0x05,						/* iJack = "BCR-32 ZAQ" */
 	/*4.4.2 MIDI IN Jack - External (DIN IN side of cable 1) */
 	0x06,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x02,						/* bDescriptorSubtype MIDI IN Jack */
 	0x02,						/* bJackType External */
 	0x06,						/* bJackID = 6 */
 	0x00,						/* iJack */
 	/*4.4.3 MIDI OUT Jack - Embedded, cable 1 */
 	0x09,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x03,						/* bDescriptorSubType = MIDI OUT JACK */
 	0x01,						/* bJackType Embedded */
 	0x07,						/* bJackID = 7 */
 	0x01,						/* bNrInputPins = 1 */
 	0x06,						/* baSourceID = Jack6 */
 	0x01,						/* baSourcePin */
 	0x05,						/* iJack = "BCR-32 ZAQ" */
 	/*4.4.4 MIDI OUT Jack - External (DIN OUT), fed by cable 1 */
 	0x09,						/* bLength */
 	0x24,						/* bDescriptorType */
 	0x03,						/* bDescriptorSubType = MIDI OUT JACK */
 	0x02,						/* bJackType External */
 	0x08,						/* bJackID = 8 */
 	0x01,						/* bNrInputPins = 1 */
 	0x05,						/* baSourceID = Jack5 */
 	0x01,						/* baSourcePin */
 	0x00,						/* iJack */
 	/*************** 5 Bulk OUT Endpoint Descriptors ***************/
 	/*5.1 Standard Bulk OUT Endpoint Descriptor */
 	0x09,						/* bLength */
 	USB_DESC_TYPE_ENDPOINT,						/* bDescriptorType = Endpoint Descriptor */
 	MIDI_OUT_EP,						/* bEndpointAddress = 2, output */
 	0x02,						/* bmAttributes = 2, bulk */
 	0x40,						
 	0x00,						/* wMaxPacketSize = 0x0040 = 64 bytes */
 	0x00,						/* bInterval */
 	0x00,						/* bRefresh */
 	0x00,						/* bSynchAddress */
 	/*5.2 Class-specific MS Bulk OUT Endpoint Descriptor*/
 	0x06,						/* bLength = 4 + 2 jacks */
 	0x25,						/* bDescriptorType */
 	0x01,						/* bDescriptorSubtype */
 	0x02,						/* bNumEmbMIDIJack = 2 embedded MIDI IN jacks */
 	0x01,						/* baAssocJackID(1) = 1, cable 0 */
 	0x05,						/* baAssocJackID(2) = 5, cable 1 */
 	/*************** 6 Bulk IN Endpoint Descriptors ***************/
 	/*6.1 Standard Bulk IN Endpoint Descriptor */
 	0x09,						/* bLength */
 	USB_DESC_TYPE_ENDPOINT,						/* bDescriptorType = Endpoint Descriptor */
 	0x81,						/* bEndpointAddress = 1, input */
 	0x02,						/* bAttributes = 2, bulk */
 	0x40,						
 	0x00,						/* wMaxPacketSize = 0x0040 = 64 bytes */
 	0x00,						/* bInterval */
 	0x00,						/* bRefresh */
 	0x00,						/* bSynchAddress */
 	/*6.2 Class-specific MS Bulk IN Endpoint Descriptor*/
 	0x06,						/* bLength = 4 + 2 jacks */
 	0x25,						/* bDescriptorType */
 	0x01,						/* bDescriptorSubtype */
 	0x02,						/* bNumEmbMIDIJack = 2 embedded MIDI OUT jacks */
 	0x03,						/* baAssocJackID(1) = 3, cable 0 */
 	0x07						/* baAssocJackID(2) = 7, cable 1 */
 };


 const __ALIGN_BEGIN uint8_t stUSBD_MIDI_desEpOut[7] __ALIGN_END = 
 {
 	/*5.1 Standard Bulk OUT Endpoint Descriptor*/
 	0x09,						/* bLength */
 	USB_DESC_TYPE_ENDPOINT,		/* bDescriptorType = Endpoint Descriptor */
 	MIDI_OUT_EP,						/* bEndpointAddress = 2, output*/
 	0x02,						/* bmAttributes = 2, bulk not shared  */
 	0x40,
 	0x00,						/* wMaxPacketSize = 0x0040 = 64bytes  */
 	0x00,						/* bInterval  */
 };
 
 const __ALIGN_BEGIN uint8_t stUSBD_MIDI_desEpIn[7] __ALIGN_END = 
 {
 	/*6.1 Standard Bulk IN Endpoint Descriptor*/
 	0x09,						/* bLength */
 	USB_DESC_TYPE_ENDPOINT,		/* bDescriptorType  = Endpoint Descriptor */
 	0x81,						/* bEndpointAddress = 1, input */
 	0x02,						/* bAttributes = 2, bulk not shared */
 	0x40,
 	0x00,						/* wMaxPacketSize = 0x0040 = 64bytes */
 	0x00					    /* bInterval */
 };
 
 
 // USB lang indentifier descriptor.
 __ALIGN_BEGIN  uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
 {
      USB_LEN_LANGID_STR_DESC,
      USB_DESC_TYPE_STRING,
      LOBYTE(USBD_LANGID_STRING),
      HIBYTE(USBD_LANGID_STRING)
 };
 
 // manufacturer string MFC : BEHRINGER
 __ALIGN_BEGIN uint8_t ManufacturerStringDesc[] __ALIGN_END =
 {
 	0x14,						/* bLength */
 	USB_DESC_TYPE_STRING,	    /* bDescriptorType */
 	'B',0,
 	'e',0,
 	'h',0,
 	'r',0,
 	'i',0,
 	'n',0,
 	'g',0,
 	'e',0,
 	'r',0
 };
 
 // Product String,  Product name: BE-SPRINT
 
 __ALIGN_BEGIN  uint8_t ProductStringDesc[34] __ALIGN_END =
 {
 	34,						    // bLength = strlen(name) * 2 + 2 
 	USB_DESC_TYPE_STRING,		// bDescriptorType 
 	'B'  ,0,
 	'e'  ,0,
 	'h'  ,0,
 	'r'  ,0,
 	'i'  ,0,
	'n'  ,0,
 	'g'  ,0,
 	'e'  ,0,
    'r'  ,0,
 	' '  ,0,
 	'B'  ,0,
 	'C'  ,0,
 	'R'  ,0,
 	'-'  ,0,
 	'3'  ,0,
	'2'  ,0,
 };
 
 // Product serial number 
 
 /* serial_string_get() writes 12 UTF-16 chars (26 bytes) at init; size the buffer for that
  * so it no longer overruns the next descriptor. bLength stays 0x0C, so hosts keep seeing
  * the same 5-char serial number as before. */
 __ALIGN_BEGIN  uint8_t SerialStringDesc[0x1A] __ALIGN_END=
 {/*Product serial number - can be bootloader version */
 	0x0C,						        /* bLength */
 	USB_DESC_TYPE_STRING,				/* bDescriptorType */
 	'0',0,
 	'.',0,
 	'1',0,
 	'.',0,
 	'0',0
 };
 
 // Jack name of cable 0 (string index 4)

__ALIGN_BEGIN  uint8_t ConfigStringDec[] __ALIGN_END =
{
	0x18,						    // bLength = 2 + 2 * 11
	USB_DESC_TYPE_STRING,		// bDescriptorType
	'B'  ,0,
	'C'  ,0,
	'R'  ,0,
	'-'  ,0,
	'3'  ,0,
	'2'  ,0,
	' '  ,0,
	'C'  ,0,
	'T'  ,0,
	'R'  ,0,
	'L'  ,0,
};

// Jack name of cable 1 (string index 5)

__ALIGN_BEGIN  uint8_t InterfaceStringDec[] __ALIGN_END =
{
	0x16,						    // bLength = 2 + 2 * 10
	USB_DESC_TYPE_STRING,		// bDescriptorType
	'B'  ,0,
	'C'  ,0,
	'R'  ,0,
	'-'  ,0,
	'3'  ,0,
	'2'  ,0,
	' '  ,0,
	'Z'  ,0,
	'A'  ,0,
	'Q'  ,0,
};

  /* Sized to STR_IDX_MAX: unused indices are NULL and get STALLed instead of reading past the table */
  uint8_t *  UsbString[STR_IDX_MAX] =
 {
 	USBD_LangIDDesc,
 	ManufacturerStringDesc,
 	ProductStringDesc,
 	SerialStringDesc,

	ConfigStringDec,
	InterfaceStringDec,

 };
 
 
 
 
 usb_desc stUsbFS_Desc = 
 {
 	USBD_DeviceDesc,           // dev_desc
 	USBD_MIDI_CfgDesc,         // config_desc
 	NULL,                      // bos_desc
 	(void *  *)UsbString,         // strings
 };
 
 
 const usb_desc_ep* usbd_get_desc_edpin(void)
 {
 	return (usb_desc_ep *)stUSBD_MIDI_desEpIn;
 }
 
 const usb_desc_ep* usbd_get_desc_edpout(void)
 {
 	return (usb_desc_ep *)stUSBD_MIDI_desEpOut;
 }
 
 void DrvUsbDeInit(void)
 {
	 usbd_disconnect_switch(&hUsbDeviceFS);
	 //usb_dev_stop(&hUsbDeviceFS);
 }
 
 void DrvUsbInit(void)
 {
 
//		UpdateProductStringDesc();
    usbd_init(&hUsbDeviceFS, USB_CORE_ENUM_FS, &stUsbFS_Desc, &stUSBD_MIDI);
    usb_clock_active(&hUsbDeviceFS);
    usb_rwkup_set(&hUsbDeviceFS);

    /* add delay time */
    for(__IO uint16_t i = 0U; i < 1000U; i++){
        for(__IO uint16_t i = 0U; i < 200U; i++);
    }

    usb_rwkup_reset(&hUsbDeviceFS);   

 }

 void DrvUsbFlushTx(void)
 {
	 UsbFlushTx(&hUsbDeviceFS);
	 UsbMidiTxBusyClear();	/* a transfer flushed from the FIFO never completes */
 }
 void UpdateProductStringDesc(void)
 {
	 uint8_t ui8IterationIndex = 0;
	 uint8_t tempDividend = 0;
	 uint8_t ui8ChangeIndex = 0;
	 uint8_t ui8DeviceId = 0;
	 
	 if(ui8DeviceId > 99)
	 {
		 ui8ChangeIndex = 30;
	 }
	 else if(ui8DeviceId > 9)
	 {
		 ui8ChangeIndex = 32;
		 ProductStringDesc[30] = '0';
	 }
	 else
	 {
		 ui8ChangeIndex = 34;
		 ProductStringDesc[30] = '0';
		 ProductStringDesc[32] = '0';
	 }
		 
	 while(ui8DeviceId)
	 {
		 tempDividend = ui8DeviceId %10;
		 ProductStringDesc[34 - ui8IterationIndex * 2] = tempDividend + 48;
		 ui8IterationIndex++;
		 ui8DeviceId = ui8DeviceId/10;
	 }
 }
 
	 
 
 
 /******************** Copyright (c) 2016 - 2019 MUSIC TRIBE *******************/
 
 
