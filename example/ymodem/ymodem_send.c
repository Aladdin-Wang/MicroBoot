#include "ymodem_send.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>

#undef this
#define this        (*ptThis)
static ymodem_send_t s_tYmodemSend;
static uint8_t *pchBuf = NULL;
static rt_device_t dev;

static uint32_t count1 = 0, count2 = 0;
static uint16_t ymodem_send_file_name(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;

    count1++;
    count2 = 0;

    if(count1 > 2) {
        count1 = 0;
        return 0;
    }

    sprintf((char *)pchBuf, "%d_%s%c%d", count1, "123.txt", '\0', 102400);

    return hwSize;
}

static uint16_t ymodem_send_file_data(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;

    if(count2 == (102400 / 1024 + 1)) {
        hwSize = 102400 % 1024;
        memset(pchBuffer, count2 + 0X30, hwSize);
        count2 = 0;
    } else {
        hwSize = 1024;
        memset(pchBuffer, count2 + 0X30, 1024);
    }

    if(count2 == 9) {
        memset(pchBuffer, count2 + 0X30, 1024);
    }

    count2++;
    return hwSize;
}

static uint16_t ymodem_read_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;

    return rt_device_read(dev, 0, pchByte, hwSize);
}

static uint16_t ymodem_write_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;

    return rt_device_write(dev, 0, pchByte, hwSize);
}

static rt_err_t _rym_rx_ind(rt_device_t dev, rt_size_t size)
{
    return 0;
}


int sy(void)
{
    ymodem_state_t tYmodemState;
    rt_uint16_t odev_flag;
    rt_err_t (*odev_rx_ind)(rt_device_t dev, rt_size_t size);
    pchBuf =  rt_malloc(1024);

    if (pchBuf == RT_NULL) {
        return 0;
    }

    ymodem_ops_t s_tOps = {
        .pchBuffer = pchBuf,
        .hwSize = 1024,
        .fnOnFileData = ymodem_send_file_data,
        .fnOnFilePath = ymodem_send_file_name,
        .fnReadData = ymodem_read_data,
        .fnWriteData = ymodem_write_data,
    };
    ymodem_init(&s_tYmodemSend.parent, &s_tOps);
    dev =  rt_console_get_device();
    odev_rx_ind = dev->rx_indicate;
    rt_device_set_rx_indicate(dev, _rym_rx_ind);
    odev_flag = dev->open_flag;
    dev->open_flag &= ~RT_DEVICE_FLAG_STREAM;
    rt_device_open(dev, RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_RDWR);

    do {
        tYmodemState = ymodem_send(&s_tYmodemSend.parent);
        rt_thread_delay(1);
    } while(tYmodemState == STATE_ON_GOING || tYmodemState == STATE_INCORRECT_CHAR);

    rt_free(pchBuf);
    dev->open_flag = odev_flag;
    rt_device_set_rx_indicate(dev, odev_rx_ind);
    return 0;
}

MSH_CMD_EXPORT(sy, ymodem send workqueue example);

