// usbd_midi.h

 
#ifndef __USB_MIDI_H
#define __USB_MIDI_H



#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#include "usbd_core.h"

void USBTxNBytes(uint8_t* src, uint32_t nBytes);
void UsbMidiRxResume(void);
uint8_t UsbMidiTxBusy(void);
void UsbMidiTxBusyClear(void);
uint32_t UsbMidiRxRead(uint8_t* dst, uint32_t max);
uint8_t IsUsbConnected(void);

extern usb_class_core stUSBD_MIDI;

#define USBD_MIDI_CLASS    &stUSBD_MIDI


#ifdef __cplusplus
}
#endif


#endif  /* __USB_MIDI_H */



