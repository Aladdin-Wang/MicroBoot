#ifndef MULTIPLE_TIMER_H_
#define MULTIPLE_TIMER_H_
#include ".\app_cfg.h"
#if USE_SERVICE_TIMER == ENABLED
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "perf_counter.h"

typedef struct {
    int64_t             lStart;
    int64_t             lUsedTotal;
    int32_t             wUsedRecent;

    void (*timeout_func)(void *parameter);    /**< timeout function */
    void               *parameter;            /**< timeout function's parameter */
    uint16_t                        : 15;
    uint16_t            bEnabled    : 1;
} m_timer_info_t;

typedef struct m_timer_info_agent_t m_timer_info_agent_t;
struct m_timer_info_agent_t {
    m_timer_info_t *ptInfo;
    m_timer_info_agent_t *ptNext;
    m_timer_info_agent_t *ptPrev;
};


#endif
#endif /* QUEUE_QUEUE_H_ */
