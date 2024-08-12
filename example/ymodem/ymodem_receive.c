#include "ymodem_receive.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#undef this
#define this        (*ptThis)

static ymodem_receive_t s_tYmodemReceive;
static uint8_t *pchBuf = NULL;
static rt_device_t dev;

static uint16_t ymodem_recv_file_name(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
    ymodem_receive_t *(ptThis) = (ymodem_receive_t *)ptObj;
    assert(NULL != ptObj);

    this.wOffSet = 0;
    strcpy(this.chFileName, (char *)&pchBuffer[0]);
    this.pchFileSize = (char *)&pchBuffer[strlen(this.chFileName) + 1];
    this.wFileSize = atol(this.pchFileSize);

    printf("Ymodem file_name:%s \r\n", this.chFileName);
    printf("Ymodem file_size:%d \r\n", this.wFileSize);

    return hwSize;
}

static uint16_t ymodem_recv_file_data(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
    ymodem_receive_t *(ptThis) = (ymodem_receive_t *)ptObj;
    assert(NULL != ptObj);


    return hwSize;
}

static uint16_t ymodem_read_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_receive_t *(ptThis) = (ymodem_receive_t *)ptObj;
    assert(NULL != ptObj);
    return rt_device_read(dev, 0, pchByte, hwSize);
}

static uint16_t ymodem_write_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_receive_t *(ptThis) = (ymodem_receive_t *)ptObj;
    assert(NULL != ptObj);

    return rt_device_write(dev, 0, pchByte, hwSize);
}

static rt_err_t _rym_rx_ind(rt_device_t dev, rt_size_t size)
{
    return 0;
}

int ry(void)
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
        .fnOnFileData = ymodem_recv_file_data,
        .fnOnFilePath = ymodem_recv_file_name,
        .fnReadData = ymodem_read_data,
        .fnWriteData = ymodem_write_data,
    };
    ymodem_init(&s_tYmodemReceive.parent, &s_tOps);
    dev =  rt_console_get_device();
    odev_rx_ind = dev->rx_indicate;
    rt_device_set_rx_indicate(dev, _rym_rx_ind);
    odev_flag = dev->open_flag;
    dev->open_flag &= ~RT_DEVICE_FLAG_STREAM;
    rt_device_open(dev, RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_RDWR);

    do {
        tYmodemState = ymodem_receive(&s_tYmodemReceive.parent);
        rt_thread_delay(1);
    } while(tYmodemState == STATE_ON_GOING || tYmodemState == STATE_INCORRECT_CHAR);

    rt_free(pchBuf);
    dev->open_flag = odev_flag;
    rt_device_set_rx_indicate(dev, odev_rx_ind);
    return 0;
}

MSH_CMD_EXPORT(ry, ymodem send workqueue example);

