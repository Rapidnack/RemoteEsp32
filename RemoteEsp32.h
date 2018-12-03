#ifndef _REMOTE_ESP32_H
#define _REMOTE_ESP32_H

#include "arduino.h"
#include <Wire.h>
#include <SPI.h>

#define BUFFER_LEN 10240
//#define ADC_MAX_WIDTH 6

#define CMD_NC 21
#define CMD_NOIB 99

#define ERR_UNKNOWN_COMMAND -88

// esp32-hal.h: 100~
#define TEMPERATURE_READ 100
#define MICROS 101
#define MILLIS 102
#define DELAY 103
#define DELAY_MICROSECONDS 104

// esp32-hal-adc.h: 200~
#define ANALOG_READ 200
#define ANALOG_READ_RESOLUTION 201
#define ANALOG_SET_WIDTH 202
#define ANALOG_SET_CYCLES 203
#define ANALOG_SET_SAMPLES 204
#define ANALOG_SET_CLOCK_DIV 205
#define ANALOG_SET_ATTENUATION 206
#define ANALOG_SET_PIN_ATTENUATION 207
#define HALL_READ 208
#define ADC_ATTACH_PIN 209
#define ADC_START 210
#define ADC_BUSY 211
#define ADC_END 212

// esp32-hal-bt.h: 300~

// esp32-hal-dac.h: 400~
#define DAC_WRITE 400

// esp32-hal-gpio.h: 500~
#define PIN_MODE 500
#define DIGITAL_WRITE 501
#define DIGITAL_READ 502
#define ATTACH_INTERRUPT 503
#define DETACH_INTERRUPT 505

// esp32-hal-i2c.h (Wire.h): 600~
#define I2C_BEGIN 600
#define I2C_SET_CLOCK 601
#define I2C_GET_CLOCK 602
#define I2C_SET_TIME_OUT 603
#define I2C_GET_TIME_OUT 604
#define I2C_LAST_ERROR 605
#define I2C_GET_ERROR_TEXT 606
#define I2C_WRITE_TRANSMISSION 607
#define I2C_READ_TRANSMISSION 608
#define I2C_BEGIN_TRANSMISSION16 609
#define I2C_BEGIN_TRANSMISSION8 610
#define I2C_BEGIN_TRANSMISSION 611
#define I2C_END_TRANSMISSION_BOOL 612
#define I2C_END_TRANSMISSION_BYTE 613
#define I2C_END_TRANSMISSION 614
#define I2C_REQUEST_FROM16_BOOL 615
#define I2C_REQUEST_FROM16_BYTE 616
#define I2C_REQUEST_FROM16 617
#define I2C_REQUEST_FROM8_BYTE 618
#define I2C_REQUEST_FROM8 619
#define I2C_REQUEST_FROM_INT 620
#define I2C_REQUEST_FROM 621
#define I2C_WRITE 622
#define I2C_WRITE_BYTES 623
#define I2C_AVAILABLE 624
#define I2C_READ 625
#define I2C_PEEK 626
#define I2C_FLUSH 627
#define I2C_WRITE_TEXT 628
#define I2C_WRITE_UINT32 629
#define I2C_WRITE_INT32 630
#define I2C_WRITE_UINT16 631
#define I2C_WRITE_INT16 632
#define I2C_DUMP_INTS 635
#define I2C_DUMP_I2C 636

// esp32-hal-ledc.h: 700~
#define LEDC_SETUP 700
#define LEDC_WRITE 701
#define LEDC_WRITE_TONE 702
#define LEDC_WRITE_NOTE 703
#define LEDC_READ 704
#define LEDC_READ_FREQ 705
#define LEDC_ATTACH_PIN 706
#define LEDC_DETACH_PIN 707

// esp32-hal-log.h: 800~

// esp32-hal-matrix.h: 900~

// esp32-hal-psram.h: 1000~

// esp32-hal-sigmadelta.h: 1100~
#define SIGMA_DELTA_SETUP 1100
#define SIGMA_DELTA_WRITE 1101
#define SIGMA_DELTA_READ 1102
#define SIGMA_DELTA_ATTACH_PIN 1103
#define SIGMA_DELTA_DETACH_PIN 1104

// esp32-hal-spi.h (SPI.h): 1200~
#define SPI_BEGIN 1200
#define SPI_END 1201
#define SPI_SET_HW_CS 1202
#define SPI_SET_BIT_ORDER 1203
#define SPI_SET_DATA_MODE 1204
#define SPI_SET_FREQUENCY 1205
#define SPI_SET_CLOCK_DIVIDER 1206
#define SPI_GET_CLOCK_DIVIDER 1207
#define SPI_BEGIN_TRANSACTION 1208
#define SPI_END_TRANSACTION 1209
#define SPI_TRANSFER 1210
#define SPI_TRANSFER16 1211
#define SPI_TRANSFER32 1212
#define SPI_TRANSFER_BYTES 1213
#define SPI_TRANSFER_BITS 1214
#define SPI_WRITE 1215
#define SPI_WRITE16 1216
#define SPI_WRITE32 1217
#define SPI_WRITE_BYTES 1218
#define SPI_WRITE_PIXELS 1219
#define SPI_WRITE_PATTERN 1220

// esp32-hal-timer.h: 1300~

// esp32-hal-touch.h: 1400~
#define TOUCH_SET_CYCLES 1400
#define TOUCH_READ 1401
#define TOUCH_ATTACH_INTERRUPT 1402

// esp32-hal-uart.h: 1500~

struct Interrupt {
	int attach;
	int pin;
	int mode;
	volatile unsigned int count;
	volatile unsigned int time;
};

class RemoteEsp32Class
{
	public:
		typedef void (*callbackFunction)(byte* buffer, int command, int p1, int p2, int extsBytes);

		RemoteEsp32Class();
		~RemoteEsp32Class();

		void attach(callbackFunction userFunction);
		void detach();
		void processInput(Stream *stream);
		void processInterrupt();
		int bytesToInt(byte *bytes);
		void intToBytes(int value, byte *bytes);
		void intsToBytes(int *ints, int intsSize, byte *bytes);
		void printBytes(byte *bytes, int bytesSize);

	private:
		static callbackFunction currentCallback;
		static Interrupt interruptList[40];
		static Interrupt touchInterruptList[40];
		static void IRAM_ATTR isr(void* arg);
		static void IRAM_ATTR touchIsr4();
		static void IRAM_ATTR touchIsr12();
		static void IRAM_ATTR touchIsr13();
		static void IRAM_ATTR touchIsr14();
		static void IRAM_ATTR touchIsr15();
		static void IRAM_ATTR touchIsr27();

		Stream* notifyStream;
		byte* buffer;
		//int adcPins[ADC_MAX_WIDTH] = { 36, 39, 32, 33, 34, 35 };
		//int adcBuf[ADC_MAX_WIDTH * 2];
		virtual void processCommand(byte* buffer, int command, int p1, int p2, int extsBytes);
};

extern RemoteEsp32Class RemoteEsp32;
extern SPIClass SPI1;

#endif
