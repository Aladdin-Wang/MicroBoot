#include "ymodem_send.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>
#include <dfs_fs.h>
#include <dfs_file.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#undef this
#define this        (*ptThis)
static ymodem_send_t s_tYmodemSend;
static uint8_t *pchBuf = NULL;
static rt_device_t dev;
static uint16_t file_count = 0;

static int pf = 0;
static char directory[50];

static uint16_t ymodem_send_file_name(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;
    struct stat file_buf;
    file_count++;

    if(file_count > 1) {
        file_count = 0;
        return 0;
    }
    pf = open(directory, O_RDONLY);
		stat(directory, &file_buf);
    rt_sprintf((char *)pchBuf, "%s%c%d", directory, '\0', file_buf.st_size);
    return hwSize;
}

static uint16_t ymodem_send_file_data(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;
	  if(pf == 0){
        pf = open(directory, O_RDONLY);			
		}
    uint16_t hwReadSize = read(pf, (void*)pchBuffer, hwSize);

    return hwReadSize;
}

static uint16_t ymodem_read_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;

    return rt_device_read(dev, 0, pchByte, hwSize);
}

static uint16_t ymodem_write_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_send_t *(ptThis) = (ymodem_send_t *)ptObj;
    rt_pin_write(GET_PIN(A, 8), PIN_HIGH);
		rt_hw_us_delay(200);
    rt_device_write(dev, 0, pchByte, hwSize);
	  rt_hw_us_delay(200);
    rt_pin_write(GET_PIN(A, 8), PIN_LOW);
    return 	hwSize;
}

static rt_err_t _rym_rx_ind(rt_device_t dev, rt_size_t size)
{
    return 0;
}


int sy(uint8_t argc, char **argv)
{
	  const char *file_path;
    ymodem_state_t tYmodemState;
    rt_uint16_t odev_flag;
    rt_err_t (*odev_rx_ind)(rt_device_t dev, rt_size_t size);
	
	  if (argc < 2)
    {
        rt_kprintf("invalid file path.\n");
        return -RT_ERROR;
    }
		file_path = argv[1];
		rt_strncpy(directory, file_path, strlen(file_path));
		
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
	  close(pf);
		file_count = 0;
		rt_kprintf("exit ymodem,tYmodemState = %d \r\n",tYmodemState);
    return 0;
}

MSH_CMD_EXPORT(sy,  YMODEM Send e.g: sy file_path);

