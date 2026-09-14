/* HPM5301 documentation test fixture. */
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "hpm_clock_drv.h"
#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"
#include <math.h>

ATTR_PLACE_AT_NONCACHEABLE_BSS volatile float superwatch_wave[16];
ATTR_PLACE_AT_NONCACHEABLE_BSS volatile uint32_t superwatch_tick;
ATTR_PLACE_AT_NONCACHEABLE_BSS volatile uint32_t superwatch_build_id;

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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
int main(void) {
    board_init();
    SEGGER_RTT_Init();
    superwatch_build_id=0x20260914U;
    SEGGER_SYSVIEW_Conf();
    if(xTaskCreate(wave_task,"Wave16",512,NULL,3,NULL)!=pdPASS) for(;;){}
    if(xTaskCreate(telemetry_task,"Telemetry",512,NULL,2,NULL)!=pdPASS) for(;;){}
    vTaskStartScheduler();
    for (;;) {}
}
