/* @file monitor.h
 * @author Luis Felice and Angela Valderrama.
 * @brief 
 * @version 0.1
 * @date 2023-03-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MONITOR_H
#define MONITOR_H
    #include <mqueue.h>
    
    mqd_t mq_init(int size, int msg_len);

#endif