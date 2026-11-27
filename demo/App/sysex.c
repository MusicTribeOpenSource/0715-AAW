/*
 * sysex.c - SysEx parser for the USB MIDI ports, see sysex.h
 */
#include "sysex.h"
#include "version.h"

#define NUM_CABLES          2U      /* "BCR-32 CTRL", "BCR-32 ZAQ" */
#define MSG_MAX             16U     /* longer messages are not commands of ours */
#define REPLY_MAX           32U     /* 0x74 reply (3 packets) + 0x00 reply (5 packets) */

#define SYSEX_START         0xF0U
#define SYSEX_END           0xF7U
#define DEVICE_ALL          0x7FU
#define DEVICE_ADDRESS      0x00U   /* device ID 1 */
#define MODEL_BCR32         0x64U
#define MODEL_ALL           0x7FU

#define CMD_VERSION_QUERY   0x00U
#define CMD_MODE_QUERY      0x74U
#define CMD_ENTER_BOOT      0x77U
#define MODE_APPLICATION    0x00U

/* USB-MIDI code index numbers (low nibble of the packet header) */
#define CIN_SYSEX_START     0x4U    /* start or continue, 3 bytes */
#define CIN_SYSEX_END1      0x5U    /* end, 1 byte (or single byte common message) */
#define CIN_SYSEX_END2      0x6U    /* end, 2 bytes */
#define CIN_SYSEX_END3      0x7U    /* end, 3 bytes */

typedef struct
{
    uint8_t buf[MSG_MAX];
    uint8_t len;
    uint8_t active;         /* between F0 and F7 */
    uint8_t overflow;       /* message longer than MSG_MAX */
} Assembly_t;

static Assembly_t s_asm[NUM_CABLES];
static uint8_t s_reply[REPLY_MAX];
static uint32_t s_reply_len;
static uint8_t s_boot;

static const uint8_t s_header[4] = { SYSEX_START, 0x00U, 0x20U, 0x32U };

/* SysEx reply as USB-MIDI packets on the cable the request came from:
 * 3 bytes per packet (CIN 4), the last packet CIN 5/6/7 for 1/2/3 bytes */
static void QueueReply(uint8_t cable, const uint8_t *msg, uint32_t len)
{
    const uint8_t c = (uint8_t)(cable << 4);
    uint32_t i, k, n;

    if (s_reply_len + ((len + 2U) / 3U) * 4U > REPLY_MAX)
    {
        return;     /* previous reply not sent yet */
    }
    for (i = 0U; i < len; i += n)
    {
        n = ((len - i) > 3U) ? 3U : (len - i);
        if ((len - i) > 3U)
        {
            s_reply[s_reply_len++] = c | CIN_SYSEX_START;
        }
        else
        {
            s_reply[s_reply_len++] = c | (uint8_t)(CIN_SYSEX_END1 + n - 1U);
        }
        for (k = 0U; k < 3U; k++)
        {
            s_reply[s_reply_len++] = (k < n) ? msg[i + k] : 0x00U;
        }
    }
}

static void ReplyModeQuery(uint8_t cable)
{
    const uint8_t msg[9] = { SYSEX_START, 0x00U, 0x20U, 0x32U, DEVICE_ADDRESS, MODEL_BCR32,
                             CMD_MODE_QUERY, MODE_APPLICATION, SYSEX_END };

    QueueReply(cable, msg, sizeof(msg));
}

/* product version 1.0.0, then the demo version (version.h) */
static void ReplyVersionQuery(uint8_t cable)
{
    const uint8_t msg[14] = { SYSEX_START, 0x00U, 0x20U, 0x32U, DEVICE_ADDRESS, MODEL_BCR32,
                              CMD_VERSION_QUERY, 0x01U, 0x00U, 0x00U, SOFTWARE_VERSION_MAIN,
                              SOFTWARE_VERSION_SUB, SOFTWARE_VERSION_BUG, SYSEX_END };

    QueueReply(cable, msg, sizeof(msg));
}

static void HandleMessage(uint8_t cable, const uint8_t *m, uint32_t len)
{
    uint32_t i;

    if (len != 8U)
    {
        return;
    }
    for (i = 0U; i < sizeof(s_header); i++)
    {
        if (m[i] != s_header[i])
        {
            return;
        }
    }
    if (((m[4] != DEVICE_ALL) && (m[4] != DEVICE_ADDRESS)) ||
        ((m[5] != MODEL_BCR32) && (m[5] != MODEL_ALL)) || (m[7] != SYSEX_END))
    {
        return;
    }
    if (m[6] == CMD_MODE_QUERY)
    {
        ReplyModeQuery(cable);
    }
    else if (m[6] == CMD_VERSION_QUERY)
    {
        ReplyVersionQuery(cable);
    }
    else if (m[6] == CMD_ENTER_BOOT)
    {
        s_boot = 1U;
    }
}

static void AddByte(uint8_t cable, uint8_t b)
{
    Assembly_t *a = &s_asm[cable];

    if (b == SYSEX_START)
    {
        a->len = 0U;
        a->active = 1U;
        a->overflow = 0U;
    }
    if (a->active == 0U)
    {
        return;
    }
    if (a->len < MSG_MAX)
    {
        a->buf[a->len++] = b;
    }
    else
    {
        a->overflow = 1U;
    }
    if (b == SYSEX_END)
    {
        if (a->overflow == 0U)
        {
            HandleMessage(cable, a->buf, a->len);
        }
        a->active = 0U;
    }
}

void Sysex_UsbRx(const uint8_t *pkt, uint32_t len)
{
    uint32_t i, k, n;

    for (i = 0U; (i + 4U) <= len; i += 4U)
    {
        const uint8_t cable = (uint8_t)(pkt[i] >> 4);
        const uint8_t cin = (uint8_t)(pkt[i] & 0x0FU);

        if (cable >= NUM_CABLES)
        {
            continue;
        }
        switch (cin)
        {
        case CIN_SYSEX_START:
        case CIN_SYSEX_END3:
            n = 3U;
            break;
        case CIN_SYSEX_END2:
            n = 2U;
            break;
        case CIN_SYSEX_END1:
            n = 1U;
            break;
        default:
            n = 0U;     /* not SysEx: notes, controllers, real-time, ... */
            break;
        }
        for (k = 0U; k < n; k++)
        {
            AddByte(cable, pkt[i + 1U + k]);
        }
    }
}

uint32_t Sysex_UsbTx(uint8_t *pkt, uint32_t max)
{
    uint32_t i, n = 0U;

    if ((s_reply_len > 0U) && (s_reply_len <= max))
    {
        for (i = 0U; i < s_reply_len; i++)
        {
            pkt[i] = s_reply[i];
        }
        n = s_reply_len;
        s_reply_len = 0U;
    }
    return n;
}

uint8_t Sysex_BootRequested(void)
{
    return s_boot;
}
