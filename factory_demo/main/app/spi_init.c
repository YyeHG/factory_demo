#include "spi_init.h"
#include "bsp/esp-box.h"
#include "esp_log.h"
#include "oledfont.h"
spi_device_handle_t handle;
static const char *TAG = "spi_init";

void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(GPIO_NUM_42, dc);
    // printf("dc\n");
}

esp_err_t spi_init()
{

    // esp_rom_gpio_pad_select_gpio(GPIO_NUM_42);   //选择gpio引脚
    // gpio_set_direction(GPIO_NUM_42, GPIO_MODE_OUTPUT); //设置功能模式
    // gpio_set_level(GPIO_NUM_42, 0);
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = ((1ULL << GPIO_NUM_42));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = true;
    gpio_config(&io_conf);

#if 0

	esp_err_t ret;
	static spi_bus_config_t buscfg = {
		.miso_io_num = SPI_PIN_MISO,
		.mosi_io_num = SPI_PIN_MOSI,
		.sclk_io_num = SPI_PIN_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 4094,
		.flags = SPICOMMON_BUSFLAG_MASTER,
	};

	static spi_device_interface_config_t devcfg = {
		.clock_speed_hz = SPI_MASTER_FREQ_40M,
		.mode = 0,
		.spics_io_num = SPI_PIN_CS,
		.queue_size = 7};
	ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
	ESP_LOGI(TAG, "spi_bus_initialize%x", ret);

	ret = spi_bus_add_device(SPI2_HOST, &devcfg, &handle);
	ESP_LOGI(TAG, "spi_bus_add_device%x", ret);

	return ret;
#endif
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = SPI_PIN_MISO,
        .mosi_io_num = SPI_PIN_MOSI,
        .sclk_io_num = SPI_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 16 * 320 * 2 + 8};
    spi_device_interface_config_t devcfg = {
#ifdef CONFIG_LCD_OVERCLOCK
        .clock_speed_hz = 26 * 1000 * 1000, // Clock out at 26 MHz
#else
        .clock_speed_hz = 10 * 1000 * 1000, // Clock out at 10 MHz
#endif
        .mode = 0,                               // SPI mode 0
        .spics_io_num = SPI_PIN_CS,              // CS pin
        .queue_size = 7,                         // We want to be able to queue 7 transactions at a time
        .pre_cb = lcd_spi_pre_transfer_callback, // Specify pre-transfer callback to handle D/C line
    };
    // Initialize the SPI bus
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_LOGI(TAG, "spi_bus_initialize%x", ret);
    // Attach the LCD to the SPI bus
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &handle);
    ESP_LOGI(TAG, "spi_bus_add_device%x", ret);

    return ret;
}

esp_err_t spi_writedata(uint8_t *data, uint8_t len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0)
        return -1;                                 // no need to send anything
    memset(&t, 0, sizeof(t));                      // Zero out the transaction
    t.length = len * 8;                            // Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;                            // Data
    t.user = (void *)1;                            // D/C needs to be set to 1
    ret = spi_device_polling_transmit(handle, &t); // Transmit!
    ESP_LOGI(TAG, " t.tx_data%x", *(uint32_t*)t.tx_buffer);
    // ESP_LOGI(TAG, "spi_writedata%x", ret);

    return ret;
}

esp_err_t spi_writedcmd(uint8_t cmd, bool keep_cs_active)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t)); // Zero out the transaction
    t.length = 8;             // Command is 8 bits
    t.tx_buffer = &cmd;       // The data is the cmd itself
    t.user = (void *)0;       // D/C needs to be set to 0
    if (keep_cs_active)
    {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE; // Keep CS active after data transfer
    }
    ret = spi_device_polling_transmit(handle, &t); // Transmit!
    // ESP_LOGI(TAG, "spi_writedcmd%x", ret);
    ESP_LOGI(TAG, " t.tx_cmd%x", *(uint32_t*)t.tx_buffer);
    return ret;
}

esp_err_t spi_read(uint8_t *data)
{
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void *)1;
    esp_err_t ret = spi_device_polling_transmit(handle, &t);
    // ESP_LOGI(TAG, "spi_read%d", ret);

    assert(ret == ESP_OK);
    *data = t.rx_data[0];
    ESP_LOGI(TAG, "spi_read%d", t.rx_data[0]);
    return ret;
}
void LCD_Address_Set(int x1,int y1,int x2,int y2)
{
    uint8_t spi_clr = 0;

    spi_writedcmd(0x2a,false);//列地址设置
    spi_clr = x1+1;spi_writedata(&spi_clr,1);
    spi_clr = x2+1;spi_writedata(&spi_clr,1);
    spi_writedcmd(0x2b,false);//行地址设置
    spi_clr = y1+26;spi_writedata(&spi_clr,1);
    spi_clr = y2+26;spi_writedata(&spi_clr,1);
    spi_writedcmd(0x2c,false);//储存器写
	
}

void LCD_Clear(uint16_t Color)
{
	uint16_t i,j;  	
    uint8_t spi_clr = Color;
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	  {
			for (j=0;j<LCD_H;j++)
				{
					spi_writedata(&spi_clr,1);
				}
	  }
}
void st7735_show()
{

    int cmd = 0;
    const lcd_init_cmd_t *lcd_init_cmds;
    lcd_init_cmds = st_init_cmds;
    while (lcd_init_cmds[cmd].databytes != 0xff)
    {
        spi_writedcmd(lcd_init_cmds[cmd].cmd, false);
        spi_writedata(lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes & 0x1F);
        if (lcd_init_cmds[cmd].databytes & 0x80)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        cmd++;
    }
    // LCD_Clear(WHITE);


}



void spi_ShowChar(int x1,int y1, char ch)
{


    int tmp = ch - 32;

    for (int i = 0; i < 6; i++)
    {
       spi_writedata(&asc2_0806[tmp][i],1);
    }
    ESP_LOGI(TAG, "tmp%d", tmp);
}
