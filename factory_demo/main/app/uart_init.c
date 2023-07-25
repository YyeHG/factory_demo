#include "uart_init.h"
#include "bsp/esp-box.h"
#include "esp_log.h"
static const char *TAG = "uart_init";

void uart_init()
{

    /*定义数据结构*/
    uart_config_t uart_config = {
        .baud_rate = BaudRate,                 // 波特率
        .data_bits = UART_DATA_8_BITS,         // 发送数据长度
        .parity = UART_PARITY_DISABLE,         // 数据验证方式
        .stop_bits = UART_STOP_BITS_1,         // 停止位1位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // 硬件流控
        .source_clk = UART_SCLK_APB,           
    };
    /*安装驱动*/
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, 1024 * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, GPIO_NUM_38, GPIO_NUM_39, RTS, CTS)); // 38tx,39rx


}


void rec_buf()
{
    char rec_buf[8]={0};
    uart_read_bytes(UART_PORT_NUM,rec_buf,8,portMAX_DELAY);
    for (int i = 0; i < 8; i++)
    {
        ESP_LOGI(TAG, "rec_buf[%d]=%x",i,rec_buf[i]);

    }
    
    
}
