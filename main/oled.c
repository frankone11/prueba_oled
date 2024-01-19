#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "driver/i2c.h"

#include "oled.h"
#include "arial.h"

#define I2C_OLED_MASTER_SCL_IO           GPIO_NUM_5       /*!< gpio number for I2C master clock */
#define I2C_OLED_MASTER_SDA_IO           GPIO_NUM_4       /*!< gpio number for I2C master data  */
#define I2C_OLED_MASTER_NUM              I2C_NUM_0        /*!< I2C port number for master dev */
#define I2C_OLED_MASTER_TX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define I2C_OLED_MASTER_RX_BUF_DISABLE   0                /*!< I2C master do not need buffer */

#define ACK_CHECK_EN                        0x1              /*!< I2C master will check ack from slave*/

char buffer[BUFFER_TAM];


bool Oled_DameBuffer(uint8_t *buf, uint16_t tam)
{
	if(!buf)
		return false;

	if(tam > BUFFER_TAM)
		tam = BUFFER_TAM;

	memcpy(buf, buffer, tam);
	return true;

}

bool Oled_ColocaBuffer(const uint8_t *buf, uint16_t tam)
{
	if(!buf)
		return false;
	if(tam > BUFFER_TAM)
		tam = BUFFER_TAM;

	memcpy(buffer, buf, tam);
	return true;
}

uint16_t Oled_DameTamBuffer(void)
{
	return BUFFER_TAM;
}

void Comando(uint8_t comando)
{
	uint8_t buf[2] = { 0, 0 };
	buf[1] = comando;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR, ACK_CHECK_EN);
	i2c_master_write(cmd, buf, 2, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_OLED_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}

void Comandos(const uint8_t *buff, uint8_t tam)
{
	if(tam > MAX_DAT)
		return;
	uint8_t buf[MAX_DAT + 1] = {0};
	for(uint8_t i = 0; i < tam; i++)
		buf[i+1] = buff[i];

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR, ACK_CHECK_EN);
	i2c_master_write(cmd, buf, tam+1, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_OLED_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}

void Dato(uint8_t dato)
{
	uint8_t buf[2] = { 0x40U, 0};
	buf[1] = dato;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ADDR, ACK_CHECK_EN);
	i2c_master_write(cmd, buf, 2, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_OLED_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
}

void Oled_Inicializa(void)
{
	memset(buffer, 0, BUFFER_TAM);
	
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = I2C_OLED_MASTER_SDA_IO;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = I2C_OLED_MASTER_SCL_IO;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.clk_stretch_tick = 300; // 300 ticks, Clock stretch is about 210us, you can make changes according to the actual situation.
	ESP_ERROR_CHECK(i2c_driver_install(I2C_OLED_MASTER_NUM, conf.mode));
	ESP_ERROR_CHECK(i2c_param_config(I2C_OLED_MASTER_NUM, &conf));

	const uint8_t init1[] =
	{
			0xAEU,
			0xD5U,
			0x80U,
			0xA8U
	};

	Comandos( init1, sizeof(init1));
	Comando( 0x3FU);

	const uint8_t init2[] =
	{
			0xD3U,
			0x00U,
			0x40U,
			0x8DU
	};

	Comandos( init2, sizeof(init2));

	Comando( 0x14U);

	const uint8_t init3[] =
	{
			0x20U,
			0x00U,
			0xA1U, //A0 | 1
			0xC8U
	};

	Comandos( init3, sizeof(init3));

	Comando( 0xDAU);
	Comando( 0x12U);
	Comando( 0x81U);
	Comando( 0xCFU);
	Comando( 0xD9U);
	Comando( 0XF1U);

	const uint8_t init5[] =
	{
			0xDBU,
			0x40U,
			0xA4U,
			0xA6U,
			0x2EU,
			0xAFU,
	};

	Comandos( init5, sizeof(init5));

	const uint8_t initxy [] =
	{
			0x21U,
			0x00U,
			0x7FU,
			0x22U,
			0x00U,
			0x07U
	};

	Comandos( initxy, sizeof(initxy));

	Oled_LimpiaDisplay();
}

void Oled_Dibuja(void)
{
	const uint8_t initxy [] =
	{
			0x21U,
			0x00U,
			0x7FU,
			0x22U,
			0x00U,
			0x07U
	};

	Comandos(initxy, sizeof(initxy));

	for(uint8_t j = 0; j < 128; j++)
	{
		for(uint8_t i = 0; i < 8; i++)
			Dato(buffer[(j*8)+i]);
	}
}

void Oled_LimpiaDisplay()
{
	memset(buffer, 0, 1024);
	Oled_Dibuja();
}

void Oled_ColocaCaracter(uint8_t *dir, const char caracter)
{
	uint8_t noval[8] = { 0x18U, 0x3CU, 0x76U, 0x9BU, 0x66U, 0x3CU, 0x18U, 0x00U };
	const tImage *img = NULL;

	if(caracter < 0x20 || caracter == 127)
		memcpy(dir, noval, 8);
	else
	{
		img = Arial.chars[caracter - 0x20U].image;
		memcpy(dir, img->data, img->dataSize);
	}
}

void Oled_EnviaCadenaXY(const char *txt, uint8_t fila, uint8_t columna)
{
	uint16_t offset;
	offset = (fila * 128) + (columna * 8);
	if(!txt)
		return;
	while(*txt)
	{
		Oled_ColocaCaracter((uint8_t *)(buffer + offset), *txt++);
		offset += 8;
	}
}

void Oled_Finaliza(void)
{
	i2c_driver_delete(I2C_OLED_MASTER_NUM);
}