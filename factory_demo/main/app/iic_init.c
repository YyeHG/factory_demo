#include "iic_init.h"
#include "string.h"
#include "stdlib.h"
#include "bsp/esp-box.h"
#include "esp_log.h"
// #include "oledfont.h"


static const char *TAG = "iic_init";
void i2c_init()
{
    int ret;
    static i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_NUM_41; // SDA IO映射
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = GPIO_NUM_40; // SCL IO映射
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = 400000; // I2C CLK 频率
    conf.clk_flags = 0;
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));       // 配置I2C
    ret = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0); // I2C功能安装使能函数
    ESP_LOGI(TAG, "i2c_driver_install%d", ret);
}

int i2c_write_cmd(uint8_t command)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create(); // 创建I2C连接函数

    ret = i2c_master_start(cmd);                                                 // 写启动信号到缓存函数
    ret = i2c_master_write_byte(cmd, OLED_WRITE_ADDR | WRITE_BIT, ACK_CHECK_EN); // 写数据
    ret = i2c_master_write_byte(cmd, WRITE_CMD, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, command, ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);                                           // 写停止信号
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS); // I2C发送函数
    i2c_cmd_link_delete(cmd);                                             // 删除I2C链接
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "ret%d", ret);
        return ret;
    }
    return ret;
}

int i2c_write_data(uint8_t data)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, OLED_WRITE_ADDR | WRITE_BIT, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, WRITE_DATA, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
    {
        return ret;
    }
    return ret;
}

int I2C_ReadData(uint8_t *pData, uint16_t dataLen)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, OLED_WRITE_ADDR | READ_BIT, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, WRITE_DATA, ACK_CHECK_EN);
    i2c_master_read(cmd, pData, dataLen, ACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(1, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}
void oled_init(void)
{
    i2c_write_cmd(0xAE); //--turn off oled panel
    i2c_write_cmd(0x00); //---set low column address
    i2c_write_cmd(0x10); //---set high column address
    i2c_write_cmd(0x40); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    i2c_write_cmd(0x81); //--set contrast control register
    i2c_write_cmd(0xCF); // Set SEG Output Current Brightness
    i2c_write_cmd(0xA1); //--Set SEG/Column Mapping      0xa0左右反置 0xa1正常
    i2c_write_cmd(0xC8); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    i2c_write_cmd(0xA6); //--set normal display
    i2c_write_cmd(0xA8); //--set multiplex ratio(1 to 64)
    i2c_write_cmd(0x3f); //--1/64 duty
    i2c_write_cmd(0xD3); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    i2c_write_cmd(0x00); //-not offset
    i2c_write_cmd(0xd5); //--set display clock divide ratio/oscillator frequency
    i2c_write_cmd(0x80); //--set divide ratio, Set Clock as 100 Frames/Sec
    i2c_write_cmd(0xD9); //--set pre-charge period
    i2c_write_cmd(0xF1); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    i2c_write_cmd(0xDA); //--set com pins hardware configuration
    i2c_write_cmd(0x12);
    i2c_write_cmd(0xDB); //--set vcomh
    i2c_write_cmd(0x40); // Set VCOM Deselect Level
    i2c_write_cmd(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
    i2c_write_cmd(0x02); //
    i2c_write_cmd(0x8D); //--set Charge Pump enable/disable
    i2c_write_cmd(0x14); //--set(0x10) disable
    i2c_write_cmd(0xA4); // Disable Entire Display On (0xa4/0xa5)
    i2c_write_cmd(0xA6); // Disable Inverse Display On (0xa6/a7)
    i2c_write_cmd(0xAF);
    
}
#if 0
static uint8_t g_oled_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
int oled_write_long_data(uint8_t *data, uint16_t len)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, OLED_WRITE_ADDR | WRITE_BIT, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, WRITE_DATA, ACK_CHECK_EN);
    ret = i2c_master_write(cmd, data, len, ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
    {
        return ret;
    }
    return ret;
}



void oled_update_screen(void)
{
    uint8_t line_index;
    for (line_index = 0; line_index < 8; line_index++)
    {
        i2c_write_cmd(0xb0 + line_index);
        i2c_write_cmd(0x00);
        i2c_write_cmd(0x10);

        oled_write_long_data(&g_oled_buffer[SSD1306_WIDTH * line_index], SSD1306_WIDTH);
    }
}

void oled_claer(void)
{
    // 清0缓存
    memset(g_oled_buffer, SSD1306_COLOR_BLACK, sizeof(g_oled_buffer));
    oled_update_screen();
}

void oled_all_on(void)
{
    // 置ff缓存
    memset(g_oled_buffer, 0xff, sizeof(g_oled_buffer));
    oled_update_screen();
}

void OLED_SetPos(uint8_t x, uint8_t y)
{

    i2c_write_cmd(x & 0x0F);
    i2c_write_cmd(0x10 | ((x >> 4) & 0x07));
    i2c_write_cmd(y + 0xB0);
}

void OLED_ShowChar(int x, int y, char ch)
{

    OLED_SetPos(x, y);

    int tmp = ch - 32;

    for (int i = 0; i < 6; i++)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, OLED_WRITE_ADDR | WRITE_BIT, ACK_CHECK_EN);
        i2c_master_write_byte(cmd, WRITE_DATA, ACK_CHECK_EN);
        i2c_master_write_byte(cmd, asc2_0806[tmp][i], ACK_CHECK_EN);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
    }
}

void OLED_ShowString(int x, int y, char *ch)
{
    char *tem = ch;
    int i = 0;
    while (*tem != '\0')
    {

        OLED_ShowChar((x + 6 * i), y, *tem);
        tem++;
        i++;
    }
}
#endif
