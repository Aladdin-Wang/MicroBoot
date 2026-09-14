/* HPM5301 documentation test fixture. */
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_csr_drv.h"
#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"
#include <math.h>

ATTR_PLACE_AT_NONCACHEABLE_BSS volatile float superwatch_wave[16];
ATTR_PLACE_AT_NONCACHEABLE_BSS volatile uint32_t superwatch_tick;
ATTR_PLACE_AT_NONCACHEABLE_BSS volatile uint32_t superwatch_build_id;
ATTR_PLACE_AT_NONCACHEABLE_BSS volatile uint32_t trap_unlock;
ATTR_PLACE_AT_NONCACHEABLE_BSS volatile uint32_t trap_request;
ATTR_PLACE_AT_NONCACHEABLE_BSS volatile uint32_t trap_record[5];

__attribute__((noinline)) static void demo_illegal_instruction(void) {
    __asm volatile (".word 0xffffffff");
}
void freertos_risc_v_application_exception_handler(uint32_t cause, uint32_t epc) {
    trap_record[0]=cause;
    trap_record[1]=epc;
    trap_record[2]=read_csr(CSR_MTVAL);
    trap_record[3]=read_csr(CSR_MSTATUS);
    trap_record[4]=0x54524150U;
    for (;;) { __asm volatile("nop"); }
}

static void wave_task(void *arg) {
    (void)arg;
    TickType_t wake=xTaskGetTickCount();
    for (;;) {
        float phase=6.28318530718f*(float)(superwatch_tick%1000U)/1000.0f;
        for(unsigned i=0;i<16;i++)
            superwatch_wave[i]=100.0f*sinf(phase+6.28318530718f*(float)i/16.0f);
        superwatch_tick++;
        vTaskDelayUntil(&wake,pdMS_TO_TICKS(1));
    }
}
static void telemetry_task(void *arg) {
    (void)arg;
    for (;;) {
        SEGGER_RTT_printf(0,"RTOS wave tick=%u build=%x\r\n",(unsigned)superwatch_tick,(unsigned)superwatch_build_id);
        if(trap_unlock==0x48504D53U && trap_request==0x54524150U) {
            trap_request=0;
            demo_illegal_instruction();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
int main(void) {
    board_init();
    SEGGER_RTT_Init();
    superwatch_build_id=0x20260915U;
    SEGGER_SYSVIEW_Conf();
    if(xTaskCreate(wave_task,"Wave16",512,NULL,3,NULL)!=pdPASS) for(;;){}
    if(xTaskCreate(telemetry_task,"Telemetry",512,NULL,2,NULL)!=pdPASS) for(;;){}
    vTaskStartScheduler();
    for (;;) {}
}
