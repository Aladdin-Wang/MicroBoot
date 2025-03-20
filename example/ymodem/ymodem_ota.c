#include "ymodem_ota.h"
#include <assert.h>

#undef this
#define this        (*ptThis)

__attribute__((aligned(32)))
uint8_t s_chQueueBuffer[1024] ;

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

    if( APP_PART_SIZE  < this.wFileSize) {
        printf("file size outrange flash size. \r\n");
        return 0;
    }
    target_flash_init(APP_PART_ADDR);
    uint32_t wEraseSize = target_flash_erase(APP_PART_ADDR, this.wFileSize);
    target_flash_uninit(APP_PART_ADDR);
    if( wEraseSize < this.wFileSize) {
        printf("target flash erase error. \r\n");
        return 0;
    }

    printf("flash erase success:%d \r\n", wEraseSize);
    begin_download();
    return hwSize;
}

static uint16_t ymodem_recv_file_data(ymodem_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize)
{
    ymodem_receive_t *(ptThis) = (ymodem_receive_t *)ptObj;
    assert(NULL != ptObj);

    uint32_t  wRemainLen = this.wFileSize - this.wOffSet;
    uint32_t  wWriteLen = hwSize;

    if(wWriteLen > wRemainLen) {
        wWriteLen = wRemainLen;
    }
    target_flash_init(APP_PART_ADDR);
    uint16_t wWriteSize = target_flash_write(APP_PART_ADDR + this.wOffSet, pchBuffer, wWriteLen);
    target_flash_uninit(APP_PART_ADDR);
    if( wWriteSize != wWriteLen) {
        printf("target flash write data error. 0x%x.\r\n", wWriteSize);
        return 0;
    }

    uint16_t hwWriteCheck = ymodem_crc16(pchBuffer, wWriteLen);
    target_flash_init(APP_PART_ADDR);	
    uint16_t wReadSize = target_flash_read(APP_PART_ADDR + this.wOffSet, pchBuffer, wWriteLen);
    target_flash_uninit(APP_PART_ADDR);
    if( wReadSize != wWriteLen) {
        printf("target flash wReadSize data error. 0x%x.\r\n", wReadSize);
        return 0;
    }

    uint16_t hwReadCheck = ymodem_crc16(pchBuffer, wWriteLen);

    if(hwWriteCheck != hwReadCheck) {
        printf("Check error. WriteCheck:0x%x ReadCheck:0x%x.\r\n", hwWriteCheck, hwReadCheck);
        return 0;
    }

    this.wOffSet += wWriteLen;

    if(this.wOffSet == this.wFileSize) {
        finalize_download();			
        printf("Download firmware to flash success.\n");
    }
    
    return hwSize;
}

static uint16_t ymodem_read_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_receive_t *(ptThis) = (ymodem_receive_t *)ptObj;
    assert(NULL != ptObj);
    return this.ptReadByte->fnGetByte(this.ptReadByte, pchByte, hwSize);;
}

static uint16_t ymodem_write_data(ymodem_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    ymodem_receive_t *(ptThis) = (ymodem_receive_t *)ptObj;
    assert(NULL != ptObj);
    emit(ymodem_rec_sig, ptThis,
         args(
             pchByte,
             hwSize
         ));

    return hwSize;
}

fsm_rt_t ymodem_ota_receive(ymodem_t *ptObj)
{
    ymodem_state_t tState = ymodem_receive(ptObj);

    if(tState == STATE_ON_GOING) {
        return fsm_rt_on_going;
    }else if(tState == STATE_INCORRECT_NBlk || tState == STATE_INCORRECT_CHAR) {
        return fsm_rt_user_req_drop;
    }else if(tState == STATE_TIMEOUT) {
        return fsm_rt_user_req_timeout;
    }else if(tState == STATE_FINSH ){
        return fsm_rt_cpl;
    }else {
        return fsm_rt_cpl;
    }
}

ymodem_ota_recive_t *ymodem_ota_receive_init(ymodem_ota_recive_t *ptObj, peek_byte_t *ptReadByte)
{
    ymodem_ota_recive_t *(ptThis) = ptObj;
    assert(NULL != ptObj);
    this.tCheckAgent.pAgent = &this.tYmodemReceive.parent;
    this.tCheckAgent.fnCheck = (check_hanlder_t *)ymodem_ota_receive;
    this.tCheckAgent.ptNext = NULL;
    this.tCheckAgent.hwPeekStatus = 0;
    this.tCheckAgent.bIsKeepingContext = true;
    this.tYmodemReceive.ptReadByte = ptReadByte;
    ymodem_ops_t s_tOps = {
        .pchBuffer = s_chQueueBuffer,
        .hwSize = sizeof(s_chQueueBuffer),
        .fnOnFileData = ymodem_recv_file_data,
        .fnOnFilePath = ymodem_recv_file_name,
        .fnReadData = ymodem_read_data,
        .fnWriteData = ymodem_write_data
    };
    ymodem_init(&this.tYmodemReceive.parent, &s_tOps);

    return ptObj;
}

