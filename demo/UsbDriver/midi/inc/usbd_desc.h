// usbd_desc.h




#ifndef __USBD_DESC__H__
#define __USBD_DESC__H__

#ifdef __cplusplus
 extern "C" {
#endif


#include "usbd_def.h"


#define USB_CONFIG_DESC_SIZE    133
//#define USBD_MAX_POWER          50  /* The unit is in 2mA. ex: 50 * 2mA = 100mA */
#define USBD_REMOTE_WAKEUP      0
#define USB_DATA_PACKET_SIZE        64U
#define MIDI_IN_EP                                   0x81  /* EP1 for data IN */
#define MIDI_OUT_EP                                  0x02  /* EP1 for data OUT */


#define DEVICE_SERIAL            "00000120"


/* descriptor types of usb specifications */
#define USB_DESCTYPE_DEVICE                            0x01U       /*!< USB device descriptor type*/
#define USB_DESCTYPE_CONFIGURATION                     0x02U       /*!< USB configuration descriptor type*/
#define USB_DESCTYPE_STRING                            0x03U       /*!< USB string descriptor type*/
#define USB_DESCTYPE_INTERFACE                         0x04U       /*!< USB interface descriptor type*/
#define USB_DESCTYPE_ENDPOINT                          0x05U       /*!< USB endpoint descriptor type*/
#define USB_DESCTYPE_DEVICE_QUALIFIER                  0x06U       /*!< USB device qualtfier descriptor type*/
#define USB_DESCTYPE_OTHER_SPEED_CONFIGURATION         0x07U       /*!< USB other speed configuration descriptor type*/
#define USB_DESCTYPE_INTERFACE_POWER                   0x08U       /*!< USB interface power descriptor type*/ 

#define USBD_LANGID_STR_IDX                            0x00U     /*!< USB language ID string index*/
#define USBD_MFC_STR_IDX                               0x01U     /*!< USB manufacturer string index*/
#define USBD_PRODUCT_STR_IDX                           0x02U     /*!< USB product string index*/
#define USBD_SERIAL_STR_IDX                            0x03U     /*!< USB serial string index*/
#define USBD_CONFIG_STR_IDX                            0x04U     /*!< USB configuration string index*/
#define USBD_INTERFACE_STR_IDX                         0x05U     /*!< USB interface string index*/

#define USB_DESCTYPE_AUDIO_CS_UNDEFINED                0x20        /*!< Audio Class-Specific Descriptor Type, add by XuZhiguang*/ 
#define USB_DESCTYPE_AUDIO_CS_DEVICE                   0x21        /*!< Audio Class-Specific Descriptor Type, add by XuZhiguang*/ 
#define USB_DESCTYPE_AUDIO_CS_CONFIGURATION            0x22        /*!< Audio Class-Specific Descriptor Type, add by XuZhiguang*/ 
#define USB_DESCTYPE_AUDIO_CS_STRING                   0x23        /*!< Audio Class-Specific Descriptor Type, add by XuZhiguang*/ 
#define USB_DESCTYPE_AUDIO_CS_INTERFACE                0x24        /*!< Audio Class-Specific Descriptor Type, add by XuZhiguang*/ 
#define USB_DESCTYPE_AUDIO_CS_ENDPOINT                 0x25        /*!< Audio Class-Specific Descriptor Type, add by XuZhiguang*/ 

#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_UNDEFINED        0x00        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_HEADER           0x01        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_INPUT_TERMINAL   0x02        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_OUTPUT_TERMINAL  0x03        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_MXER_UNIT        0x04        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_SELECTOR_UNIT    0x05        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_FEATURE_UNIT     0x06        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_JPROCESSING_UNIT 0x07        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_AUDIO_CS_SUBTYPE_EXTENSION_UNIT   0x08        /*!< Audio Class-Specific Descriptor Subtype, add by XuZhiguang*/

#define USB_DESCTYPE_MS_CS_SUBTYPE_UNDEFINED           0x00        /*!< MS Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_MS_CS_SUBTYPE_HEADER              0x01        /*!< MS Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_MS_CS_SUBTYPE_MIDI_IN_JACK        0x02        /*!< MS Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_MS_CS_SUBTYPE_MIDI_OUT_JACK       0x03        /*!< MS Class-Specific Descriptor Subtype, add by XuZhiguang*/
#define USB_DESCTYPE_MS_CS_SUBTYPE_ELEMENT             0x04        /*!< MS Class-Specific Descriptor Subtype, add by XuZhiguang*/ 
 
#define USB_MS_MIDI_JACK_TYPE_UNDEFINED                0x00
#define USB_MS_MIDI_JACK_TYPE_EMBEDDED                 0x01
#define USB_MS_MIDI_JACK_TYPE_EXTERNAL                 0x02 

#define WIDE_STRING(string)  _WIDE_STRING(string)
#define _WIDE_STRING(string) L##string

#define USBD_STRING_DESC(string) \
    (uint8_t *)&(struct { \
        uint8_t _len; \
        uint8_t _type; \
        wchar_t _data[sizeof(string)]; \
    }) { \
        sizeof(WIDE_STRING(string)) + 2U - 2U, \
        USB_DESCTYPE_STRING, \
        WIDE_STRING(string) \
    }


typedef struct
{
    uint8_t bLength;                      /*!< size of the descriptor */
    uint8_t bDescriptorType;              /*!< type of the descriptor */
} usb_descriptor_header_struct; 

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size */

    uint16_t bcdUSB;                      /*!< BCD of the supported USB specification */
    uint8_t  bDeviceClass;                /*!< USB device class */
    uint8_t  bDeviceSubClass;             /*!< USB device subclass */
    uint8_t  bDeviceProtocol;             /*!< USB device protocol */
    uint8_t  bMaxPacketSize0;             /*!< size of the control (address 0) endpoint's bank in bytes */
    uint16_t idVendor;                    /*!< vendor ID for the USB product */
    uint16_t idProduct;                   /*!< unique product ID for the USB product */
    uint16_t bcdDevice;                   /*!< product release (version) number */
    uint8_t  iManufacturer;               /*!< string index for the manufacturer's name */
    uint8_t  iProduct;                    /*!< string index for the product name/details */
    uint8_t  iSerialNumber;               /*!< string index for the product's globally unique hexadecimal serial number */
    uint8_t  bNumberConfigurations;       /*!< total number of configurations supported by the device */
} usb_descriptor_device_struct; 

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size */

    uint16_t wTotalLength;                /*!< size of the configuration descriptor header,and all sub descriptors inside the configuration */
    uint8_t  bNumInterfaces;              /*!< total number of interfaces in the configuration */
    uint8_t  bConfigurationValue;         /*!< configuration index of the current configuration */
    uint8_t  iConfiguration;              /*!< index of a string descriptor describing the configuration */
    uint8_t  bmAttributes;                /*!< configuration attributes */
    uint8_t  bMaxPower;                   /*!< maximum power consumption of the device while in the current configuration */
} usb_descriptor_configuration_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size */

    uint8_t bInterfaceNumber;             /*!< index of the interface in the current configuration */
    uint8_t bAlternateSetting;            /*!< alternate setting for the interface number */
    uint8_t bNumEndpoints;                /*!< total number of endpoints in the interface */
    uint8_t bInterfaceClass;              /*!< interface class ID */
    uint8_t bInterfaceSubClass;           /*!< interface subclass ID */
    uint8_t bInterfaceProtocol;           /*!< interface protocol ID */
    uint8_t iInterface;                   /*!< index of the string descriptor describing the interface */
} usb_descriptor_interface_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
    uint8_t  bEndpointAddress;            /*!< logical address of the endpoint */
    uint8_t  bmAttributes;                /*!< endpoint attributes */
    uint16_t wMaxPacketSize;              /*!< size of the endpoint bank, in bytes */
    uint8_t  bInterval;                   /*!< polling interval in milliseconds for the endpoint if it is an INTERRUPT or ISOCHRONOUS type */
} usb_descriptor_endpoint_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
    uint8_t   bEndpointAddress;
    uint8_t   bmAttributes;
    uint16_t  wMaxPacketSize;
    uint8_t   bInterval;
    uint8_t   bRefresh;
    uint8_t   bSynchAddress;
} usb_descriptor_bulk_endpoint_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
    uint8_t   bDescriptorSubtype;
    uint8_t   bNumEmbMIDIJack;
    uint8_t   BaAssocJackID;
#ifdef DOUBLE_MIDI_IF
    uint8_t   BaAssocJackID_1;
#endif	
} usb_descriptor_ms_cs_endpoint_struct;


typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size */
	
    uint8_t  bDescriptorSubtype;
    uint16_t bcdADC; 
    uint16_t wTotalLength;  
    uint8_t  bInCollection;
    uint8_t  baInterfaceNr;
}usb_descriptor_cs_ac_interface_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size */
	
    uint8_t  bDescriptorSubtype;
    uint16_t bcdADC; 
    uint16_t wTotalLength;  
}usb_descriptor_cs_ms_interface_struct;


typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size */
	
    uint8_t  bDescriptorSubtype;
    uint8_t  bJackType;   
    uint8_t  bJackID;
    uint8_t  iJack;
}usb_descriptor_midi_adaptor_in_jack_struct;

typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size */
	
    uint8_t  bDescriptorSubtype;
    uint8_t  bJackType;   
    uint8_t  bJackID;
    uint8_t  bNrInputPins;
    uint8_t  baSourceID;
    uint8_t  baSourcePin;
    uint8_t  iJack;
}usb_descriptor_midi_adaptor_out_jack_struct;


typedef struct
{
    usb_descriptor_header_struct Header;  /*!< descriptor header, including type and size. */
	
    uint16_t wLANGID;                     /*!< LANGID code */
}usb_descriptor_language_id_struct; 

/* USB configuration descriptor struct */
typedef struct
{
    usb_descriptor_configuration_struct          Config;
    usb_descriptor_interface_struct              Standard_AC_Interface;
    usb_descriptor_cs_ac_interface_struct        Class_specific_AC_Interface;
    usb_descriptor_interface_struct              Standard_MS_Interface;       /*MIDIStreaming Interface*/
    usb_descriptor_cs_ms_interface_struct        Class_specific_MS_Interface;
    usb_descriptor_midi_adaptor_in_jack_struct   midi_in_jack_0_embed;
    usb_descriptor_midi_adaptor_in_jack_struct   midi_in_jack_0_ecternal;
    usb_descriptor_midi_adaptor_out_jack_struct  midi_out_jack_0_embed;
    usb_descriptor_midi_adaptor_out_jack_struct  midi_out_jack_0_ecternal;
#ifdef TWO_MIDI_IF
    usb_descriptor_midi_adaptor_in_jack_struct   midi_in_jack_1_embed;
    usb_descriptor_midi_adaptor_in_jack_struct   midi_in_jack_1_ecternal;
    usb_descriptor_midi_adaptor_out_jack_struct  midi_out_jack_1_embed;
    usb_descriptor_midi_adaptor_out_jack_struct  midi_out_jack_1_ecternal;
#endif
    usb_descriptor_bulk_endpoint_struct          bulk_in_Endpoint;
    usb_descriptor_ms_cs_endpoint_struct         ms_cs_in_Endpoint;
    usb_descriptor_bulk_endpoint_struct          bulk_out_Endpoint;
    usb_descriptor_ms_cs_endpoint_struct         ms_cs_out_Endpoint;
} usb_descriptor_configuration_set_struct; 

void DrvUsbInit(void);
void DrvUsbDeInit(void);
/** Descriptor for the Usb device. */
extern USBD_DescriptorsTypeDef FS_Desc;
 void DrvUsbFlushTx(void);

#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC__H__ */


