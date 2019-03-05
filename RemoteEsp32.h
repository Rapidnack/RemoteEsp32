#ifndef _REMOTE_ESP32_H
#define _REMOTE_ESP32_H

#include "arduino.h"
#include <Wire.h>
#include <SPI.h>

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
		static const int COMMAND_BUFFER_SIZE = 40*1024;
		static const int ERR_UNKNOWN_COMMAND = -88;

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
		static const int CMD_NC = 21;
		static const int CMD_NOIB = 99;
		// esp32-hal.h: 100~
		static const int TEMPERATURE_READ = 100;
		static const int MICROS = 101;
		static const int MILLIS = 102;
		static const int DELAY = 103;
		static const int DELAY_MICROSECONDS = 104;
		// esp32-hal-adc.h: 200~
		static const int ANALOG_READ = 200;
		static const int ANALOG_READ_RESOLUTION = 201;
		static const int ANALOG_SET_WIDTH = 202;
		static const int ANALOG_SET_CYCLES = 203;
		static const int ANALOG_SET_SAMPLES = 204;
		static const int ANALOG_SET_CLOCK_DIV = 205;
		static const int ANALOG_SET_ATTENUATION = 206;
		static const int ANALOG_SET_PIN_ATTENUATION = 207;
		static const int HALL_READ = 208;
		static const int ADC_ATTACH_PIN = 209;
		static const int ADC_START = 210;
		static const int ADC_BUSY = 211;
		static const int ADC_END = 212;
		// esp32-hal-bt.h: 300~
		// esp32-hal-dac.h: 400~
		static const int DAC_WRITE = 400;
		// esp32-hal-gpio.h: 500~
		static const int PIN_MODE = 500;
		static const int DIGITAL_WRITE = 501;
		static const int DIGITAL_READ = 502;
		static const int ATTACH_INTERRUPT = 503;
		static const int DETACH_INTERRUPT = 505;
		// esp32-hal-i2c.h (Wire.h): 600~
		static const int I2C_BEGIN = 600;
		static const int I2C_SET_CLOCK = 601;
		static const int I2C_GET_CLOCK = 602;
		static const int I2C_SET_TIME_OUT = 603;
		static const int I2C_GET_TIME_OUT = 604;
		static const int I2C_LAST_ERROR = 605;
		static const int I2C_GET_ERROR_TEXT = 606;
		static const int I2C_WRITE_TRANSMISSION = 607;
		static const int I2C_READ_TRANSMISSION = 608;
		static const int I2C_BEGIN_TRANSMISSION16 = 609;
		static const int I2C_BEGIN_TRANSMISSION8 = 610;
		static const int I2C_BEGIN_TRANSMISSION = 611;
		static const int I2C_END_TRANSMISSION_BOOL = 612;
		static const int I2C_END_TRANSMISSION_BYTE = 613;
		static const int I2C_END_TRANSMISSION = 614;
		static const int I2C_REQUEST_FROM16_BOOL = 615;
		static const int I2C_REQUEST_FROM16_BYTE = 616;
		static const int I2C_REQUEST_FROM16 = 617;
		static const int I2C_REQUEST_FROM8_BYTE = 618;
		static const int I2C_REQUEST_FROM8 = 619;
		static const int I2C_REQUEST_FROM_INT = 620;
		static const int I2C_REQUEST_FROM = 621;
		static const int I2C_WRITE = 622;
		static const int I2C_WRITE_BYTES = 623;
		static const int I2C_AVAILABLE = 624;
		static const int I2C_READ = 625;
		static const int I2C_PEEK = 626;
		static const int I2C_FLUSH = 627;
		static const int I2C_WRITE_TEXT = 628;
		static const int I2C_WRITE_UINT32 = 629;
		static const int I2C_WRITE_INT32 = 630;
		static const int I2C_WRITE_UINT16 = 631;
		static const int I2C_WRITE_INT16 = 632;
		// esp32-hal-ledc.h: 700~
		static const int LEDC_SETUP = 700;
		static const int LEDC_WRITE = 701;
		static const int LEDC_WRITE_TONE = 702;
		static const int LEDC_WRITE_NOTE = 703;
		static const int LEDC_READ = 704;
		static const int LEDC_READ_FREQ = 705;
		static const int LEDC_ATTACH_PIN = 706;
		static const int LEDC_DETACH_PIN = 707;
		// esp32-hal-log.h: 800~
		// esp32-hal-matrix.h: 900~
		// esp32-hal-psram.h: 1000~
		// esp32-hal-sigmadelta.h: 1100~
		static const int SIGMA_DELTA_SETUP = 1100;
		static const int SIGMA_DELTA_WRITE = 1101;
		static const int SIGMA_DELTA_READ = 1102;
		static const int SIGMA_DELTA_ATTACH_PIN = 1103;
		static const int SIGMA_DELTA_DETACH_PIN = 1104;
		// esp32-hal-spi.h (SPI.h): 1200~
		static const int SPI_BEGIN = 1200;
		static const int SPI_END = 1201;
		static const int SPI_SET_HW_CS = 1202;
		static const int SPI_SET_BIT_ORDER = 1203;
		static const int SPI_SET_DATA_MODE = 1204;
		static const int SPI_SET_FREQUENCY = 1205;
		static const int SPI_SET_CLOCK_DIVIDER = 1206;
		static const int SPI_GET_CLOCK_DIVIDER = 1207;
		static const int SPI_BEGIN_TRANSACTION = 1208;
		static const int SPI_END_TRANSACTION = 1209;
		static const int SPI_TRANSFER = 1210;
		static const int SPI_TRANSFER16 = 1211;
		static const int SPI_TRANSFER32 = 1212;
		static const int SPI_TRANSFER_BYTES = 1213;
		static const int SPI_TRANSFER_BITS = 1214;
		static const int SPI_WRITE = 1215;
		static const int SPI_WRITE16 = 1216;
		static const int SPI_WRITE32 = 1217;
		static const int SPI_WRITE_BYTES = 1218;
		static const int SPI_WRITE_PIXELS = 1219;
		static const int SPI_WRITE_PATTERN = 1220;
		// esp32-hal-timer.h: 1300~
		// esp32-hal-touch.h: 1400~
		static const int TOUCH_SET_CYCLES = 1400;
		static const int TOUCH_READ = 1401;
		static const int TOUCH_ATTACH_INTERRUPT = 1402;
		// esp32-hal-uart.h: 1500~

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
		byte buffer[COMMAND_BUFFER_SIZE];
		virtual void processCommand(byte* buffer, int command, int p1, int p2, int extsBytes);
};

extern RemoteEsp32Class RemoteEsp32;
extern SPIClass SPI1;

#endif
