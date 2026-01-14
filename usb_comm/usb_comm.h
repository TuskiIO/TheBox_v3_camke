/**
 * @file    usb_comm.h
 * @brief   USB通信封装层，替代原UDP通信层
 *          为STM32F722移植项目提供USB CDC通信接口
 * @author 移植自MagLocV2_STM32H7
 */

#ifndef __USB_COMM_H__
#define __USB_COMM_H__

#include "stm32f7xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * USB通信协议定义
 * ============================================================================ */

/* 帧头定义 - 使用不同帧头区分命令和响应方向 */
#define FRAME_HEADER_CMD_0    0xAA    /* PC→MCU命令帧头第1字节 */
#define FRAME_HEADER_CMD_1    0x55    /* PC→MCU命令帧头第2字节 */
#define FRAME_HEADER_CMD_2    0x00    /* PC→MCU命令帧头第3字节 */
#define FRAME_HEADER_CMD_3    0xFF    /* PC→MCU命令帧头第4字节 */

#define FRAME_HEADER_RESP_0   0x55    /* MCU→PC响应帧头第1字节 */
#define FRAME_HEADER_RESP_1   0xAA    /* MCU→PC响应帧头第2字节 */
#define FRAME_HEADER_RESP_2   0xFF    /* MCU→PC响应帧头第3字节 */

/* 命令类型定义 */
typedef enum {
    USB_CMD_GET_DATA      = 0x01,    /* 开始/停止数据采集 */
    USB_CMD_SET_CFG       = 0x02,    /* 设置传感器配置 */
    USB_CMD_GET_CFG       = 0x03,    /* 读取传感器配置 */
    USB_CMD_RESERVED      = 0xFF     /* 保留 */
} usb_command_type_t;

/* 命令帧格式 (PC → MCU) - 4字节帧头 */
/* | 0xAA | 0x55 | 0x00 | 0xFF | CMD_TYPE | DATA_LEN | DATA[] | CRC16_L | CRC16_H | */

/* 响应帧格式 (MCU → PC) - 3字节帧头 */
/* | 0x55 | 0xAA | 0xFF | DATA[] | CRC16_L | CRC16_H | */

/* ============================================================================
 * 通信层配置
 * ============================================================================ */
#define USB_COMM_RX_BUF_SIZE    2048
#define USB_COMM_TX_BUF_SIZE    2048
#define MAX_RECV_CALLBACKS      10

/* 类型定义 */
typedef void (*usb_recv_function)(void *arg, void* data, uint32_t recv_len);

/* 回调注册结构体 */
typedef struct {
    usb_recv_function  recv_fn;
    void              *args;
} usb_recv_callback_t;

/**
 * @brief 初始化USB通信模块
 */
void usb_comm_init(void);

/**
 * @brief 注册接收回调函数
 * @param recv_fn 接收回调函数
 * @param args    回调函数参数
 * @retval 0 成功，-1 失败（回调已满）
 */
int usb_comm_register_recv(usb_recv_function recv_fn, void *args);

/**
 * @brief 通过USB发送数据
 * @param data      要发送的数据
 * @param data_len  数据长度
 * @retval 发送的字节数，<0表示失败
 */
int usb_comm_send(void *data, uint32_t data_len);

/**
 * @brief USB接收处理函数（在中断或主循环中调用）
 * @param data      接收到的数据
 * @param data_len  数据长度
 */
void usb_comm_receive_handler(uint8_t *data, uint32_t data_len);

/**
 * @brief USB命令解析处理函数
 * @param data      接收到的命令数据
 * @param data_len  数据长度
 * @note  内部调用，解析命令帧头和命令类型
 */
void usb_comm_parse_command(uint8_t *data, uint32_t data_len);

/**
 * @brief 打包并发送响应数据
 * @param data      响应数据内容
 * @param data_len  数据长度
 * @retval 0 成功，<0 失败
 */
int usb_comm_send_response(uint8_t *data, uint16_t data_len);

/* ============================================================================
 * 主循环标志引用（定义在main.c中）
 * ============================================================================ */
extern volatile uint8_t usb_get_sensor_data_flag;
extern volatile uint8_t usb_set_sensor_cfg_flag;
extern volatile uint8_t usb_get_sensor_cfg_flag;
extern uint8_t usb_to_485_buf[];

/**
 * @brief USB printf函数（类似标准printf，通过USB发送）
 * @param format    格式化字符串
 * @param ...      可变参数
 */
void usb_printf(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif /* __USB_COMM_H__ */
