/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-21     Administrator       the first version
 */
#ifndef CHECK_STR_GET_BYTE_H_
#define CHECK_STR_GET_BYTE_H_
typedef  struct _get_byte_t get_byte_t;
typedef uint16_t get_byte_hanlder_t(get_byte_t *ptThis,uint8_t *pchByte, uint16_t hwLength);
struct _get_byte_t{
    void *pTarget;
    get_byte_hanlder_t *fnGetByte;
};

#endif /* CHECK_STR_GET_BYTE_H_ */
