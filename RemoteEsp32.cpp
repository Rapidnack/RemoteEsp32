#include "RemoteEsp32.h"

RemoteEsp32Class RemoteEsp32;
SPIClass SPI1(HSPI);

RemoteEsp32Class::callbackFunction RemoteEsp32Class::currentCallback = (callbackFunction)NULL;

Interrupt RemoteEsp32Class::interruptList[40];
Interrupt RemoteEsp32Class::touchInterruptList[40];

RemoteEsp32Class::RemoteEsp32Class()
{
	notifyStream = NULL;
	buffer = (byte*)malloc(BUFFER_LEN);
}

RemoteEsp32Class::~RemoteEsp32Class()
{
	free(buffer);
}

void RemoteEsp32Class::attach(callbackFunction userFunction)
{
	currentCallback = userFunction;
}

void RemoteEsp32Class::detach()
{
	currentCallback = (callbackFunction)NULL;
}

void RemoteEsp32Class::processInput(Stream *stream)
{
	int n = stream->available();
	if (n >= 16)
	{
		//Serial.print("available: ");
		//Serial.println(n);

		stream->readBytes(buffer, 16);
		//printBytes(buffer, 16);

		int command = bytesToInt(buffer);
		int p1 = bytesToInt(buffer + 4);
		int p2 = bytesToInt(buffer + 8);
		int extsBytes = bytesToInt(buffer + 12);
		//Serial.print("command: ");
		//Serial.print(command);
		//Serial.print("\tp1: ");
		//Serial.print(p1);
		//Serial.print("\tp2: ");
		//Serial.print(p2);
		//Serial.print("\textsBytes: ");
		//Serial.println(extsBytes);
		if (extsBytes > 0) {
			stream->readBytes(buffer + 16, extsBytes);
		}
		intToBytes(0, buffer + 4);
		intToBytes(0, buffer + 8);
		intToBytes(0, buffer + 12);

		if (command == CMD_NOIB) {
			for (int i = 0; i < 40; i++) {
				interruptList[i].attach = 0;
				interruptList[i].pin = i;
				touchInterruptList[i].attach = 0;
				touchInterruptList[i].pin = i;
			}
			notifyStream = stream;
		} else if (command < 10000) {
			processCommand(buffer, command, p1, p2, extsBytes);
		} else {
			currentCallback(buffer, command, p1, p2, extsBytes);
		}

		p1 = bytesToInt(buffer + 4);
		p2 = bytesToInt(buffer + 8);
		extsBytes = bytesToInt(buffer + 12);
		//Serial.print("\tp1: ");
		//Serial.print(p1);
		//Serial.print("\tp2: ");
		//Serial.print(p2);
		//Serial.print("\textsBytes: ");
		//Serial.println(extsBytes);
		//printBytes(buffer, 16 + extsBytes);
		stream->write(buffer, 16 + extsBytes);
	}
}

void RemoteEsp32Class::processInterrupt()
{
	if (notifyStream == NULL)
		return;

	byte notifyBuf[8];
	for (int i = 0; i < 40; i++) {
		if (interruptList[i].count > 0) {
			interruptList[i].count = 0;

			intToBytes(i, notifyBuf);
			intToBytes(interruptList[i].time, notifyBuf + 4);
			notifyStream->write(notifyBuf, 8);
			//Serial.print("isr: ");
			//Serial.println(i);

			if (interruptList[i].attach) {
				attachInterruptArg(i, (void (*)())isr, &interruptList[i], interruptList[i].mode);
			}
		}
	}
	for (int i = 0; i < 40; i++) {
		if (touchInterruptList[i].count > 0) {
			touchInterruptList[i].count = 0;

			intToBytes(i, notifyBuf);
			intToBytes(touchInterruptList[i].time, notifyBuf + 4);
			notifyStream->write(notifyBuf, 8);
			//Serial.print("touchIsr: ");
			//Serial.println(i);

			if (touchInterruptList[i].attach) {
				switch (i) {
				case 4: touchAttachInterrupt(i, touchIsr4, touchInterruptList[i].mode); break;
				case 12: touchAttachInterrupt(i, touchIsr12, touchInterruptList[i].mode); break;
				case 13: touchAttachInterrupt(i, touchIsr13, touchInterruptList[i].mode); break;
				case 14: touchAttachInterrupt(i, touchIsr14, touchInterruptList[i].mode); break;
				case 15: touchAttachInterrupt(i, touchIsr15, touchInterruptList[i].mode); break;
				case 27: touchAttachInterrupt(i, touchIsr27, touchInterruptList[i].mode); break;
				}
			}
		}
	}
}

void RemoteEsp32Class::processCommand(byte* buffer, int command, int p1, int p2, int extsBytes)
{
	switch (command) {
	
	// esp32-hal.h: 100~
	case TEMPERATURE_READ: intToBytes((int)(temperatureRead() * 10), buffer + 8); break;
	case MICROS: intToBytes((int)micros(), buffer + 8); break;
	case MILLIS: intToBytes((int)millis(), buffer + 8); break;
	case DELAY: delay((uint32_t)p1); break;
	case DELAY_MICROSECONDS: delayMicroseconds((uint32_t)p1); break;
 
	// esp32-hal-adc.h: 200~
	case ANALOG_READ: intToBytes(analogRead(p1), buffer + 8); break;
	case ANALOG_READ_RESOLUTION: analogReadResolution(p1); break;
	case ANALOG_SET_WIDTH: analogSetWidth(p1); break;
	case ANALOG_SET_CYCLES: analogSetCycles(p1); break;
	case ANALOG_SET_SAMPLES: analogSetSamples(p1); break;
	case ANALOG_SET_CLOCK_DIV: analogSetClockDiv(p1); break;
	case ANALOG_SET_ATTENUATION: analogSetAttenuation((adc_attenuation_t)p1); break;
	case ANALOG_SET_PIN_ATTENUATION: analogSetPinAttenuation(p1, (adc_attenuation_t)p2); break;
	case HALL_READ: intToBytes(hallRead(), buffer + 8); break;
	case ADC_ATTACH_PIN: intToBytes(adcAttachPin(p1), buffer + 8); break;
	case ADC_START: intToBytes(adcStart(p1), buffer + 8); break;
	case ADC_BUSY: intToBytes(adcBusy(p1), buffer + 8); break;
	case ADC_END: intToBytes(adcEnd(p1), buffer + 8); break;
	
	// esp32-hal-bt.h: 300~
	
	// esp32-hal-dac.h: 400~
	case DAC_WRITE: dacWrite(p1, p2); break;

	// esp32-hal-gpio.h: 500~	
	case PIN_MODE: pinMode(p1, p2); break;
	case DIGITAL_WRITE: digitalWrite(p1, p2); break;
	case DIGITAL_READ: intToBytes(digitalRead(p1), buffer + 8); break;
	case ATTACH_INTERRUPT:
		interruptList[p1].attach = 1;
		interruptList[p1].mode = p2;
		attachInterruptArg(p1, (void (*)())isr, &interruptList[p1], p2);
		break;
	case DETACH_INTERRUPT:
		interruptList[p1].attach = 0;
		touchInterruptList[p1].attach = 0;
		detachInterrupt(p1);
		break;

	// esp32-hal-i2c.h (Wire.h): 600~
	case I2C_BEGIN: {
			int p3 = bytesToInt(buffer + 16);
			int p4 = bytesToInt(buffer + 20);
			if (p1 == 1)
				Wire1.begin(p2, p3, (uint32_t)p4);
			else
				Wire.begin(p2, p3, (uint32_t)p4);
		}
		break;
	case I2C_SET_CLOCK:
		if (p1 == 1)
			Wire1.setClock((uint32_t)p2);
		else
			Wire.setClock((uint32_t)p2);
		break;
	case I2C_GET_CLOCK:
		if (p1 == 1)
			intToBytes((int)Wire1.getClock(), buffer + 8);
		else
			intToBytes((int)Wire.getClock(), buffer + 8);
		break;
	case I2C_SET_TIME_OUT:
		if (p1 == 1)
			Wire1.setTimeOut(p2);
		else
			Wire.setTimeOut(p2);
		break;
	case I2C_GET_TIME_OUT:
		if (p1 == 1)
			intToBytes(Wire1.getTimeOut(), buffer + 8);
		else
			intToBytes(Wire.getTimeOut(), buffer + 8);
		break;
	case I2C_LAST_ERROR:
		if (p1 == 1)
			intToBytes(Wire1.lastError(), buffer + 8);
		else
			intToBytes(Wire.lastError(), buffer + 8);
		break;
	case I2C_GET_ERROR_TEXT: {
			String s;
			if (p1 == 1)
				s = String(Wire1.getErrorText(p2));
			else
				s = String(Wire.getErrorText(p2));
			extsBytes = s.length();
			intToBytes(extsBytes, buffer + 12);
			s.toCharArray((char*)(buffer + 16), BUFFER_LEN - 16);
		}
		break;
	case I2C_WRITE_TRANSMISSION: {
			int p3 = bytesToInt(buffer + 16);
			if (p1 == 1)
				intToBytes((int)Wire1.writeTransmission(p2, buffer + 20, extsBytes, (bool)p3), buffer + 8);
			else
				intToBytes((int)Wire.writeTransmission(p2, buffer + 20, extsBytes, (bool)p3), buffer + 8);
		}
		break;
	case I2C_READ_TRANSMISSION: {
			int p3 = bytesToInt(buffer + 16);
			uint32_t readCount;
			if (p1 == 1)
				intToBytes((int)Wire1.readTransmission(p2, buffer + 20, extsBytes, (bool)p3, &readCount), buffer + 8);
			else
				intToBytes((int)Wire.readTransmission(p2, buffer + 20, extsBytes, (bool)p3, &readCount), buffer + 8);
			extsBytes = 4;
			intToBytes(extsBytes, buffer + 12);
			intToBytes((int)readCount, buffer + 16);
		}
		break;
	case I2C_BEGIN_TRANSMISSION16:
		if (p1 == 1)
			Wire1.beginTransmission((uint16_t)p2);
		else
			Wire.beginTransmission((uint16_t)p2);
		break;
	case I2C_BEGIN_TRANSMISSION8:
		if (p1 == 1)
			Wire1.beginTransmission((uint8_t)p2);
		else
			Wire.beginTransmission((uint8_t)p2);
		break;
	case I2C_BEGIN_TRANSMISSION:
		if (p1 == 1)
			Wire1.beginTransmission(p2);
		else
			Wire.beginTransmission(p2);
		break;
	case I2C_END_TRANSMISSION_BOOL:
		if (p1 == 1)
			intToBytes(Wire1.endTransmission((bool)p2), buffer + 8);
		else
			intToBytes(Wire.endTransmission((bool)p2), buffer + 8);
		break;
	case I2C_END_TRANSMISSION_BYTE:
		if (p1 == 1)
			intToBytes(Wire1.endTransmission((uint8_t)p2), buffer + 8);
		else
			intToBytes(Wire.endTransmission((uint8_t)p2), buffer + 8);
		break;
	case I2C_END_TRANSMISSION:
		if (p1 == 1)
			intToBytes(Wire1.endTransmission(), buffer + 8);
		else
			intToBytes(Wire.endTransmission(), buffer + 8);
		break;
	case I2C_REQUEST_FROM16_BOOL: {
			int p3 = bytesToInt(buffer + 16);
			int p4 = bytesToInt(buffer + 20);
			if (p1 == 1)
				intToBytes((int)Wire1.requestFrom((uint16_t)p2, (uint8_t)p3, (bool)p4), buffer + 8);
			else
				intToBytes((int)Wire.requestFrom((uint16_t)p2, (uint8_t)p3, (bool)p4), buffer + 8);
		}
		break;
	case I2C_REQUEST_FROM16_BYTE: {
			int p3 = bytesToInt(buffer + 16);
			int p4 = bytesToInt(buffer + 20);
			if (p1 == 1)
				intToBytes((int)Wire1.requestFrom((uint16_t)p2, (uint8_t)p3, (uint8_t)p4), buffer + 8);
			else
				intToBytes((int)Wire.requestFrom((uint16_t)p2, (uint8_t)p3, (uint8_t)p4), buffer + 8);
		}
		break;
	case I2C_REQUEST_FROM16: {
			int p3 = bytesToInt(buffer + 16);
			if (p1 == 1)
				intToBytes((int)Wire1.requestFrom((uint16_t)p2, (uint8_t)p3), buffer + 8);
			else
				intToBytes((int)Wire.requestFrom((uint16_t)p2, (uint8_t)p3), buffer + 8);
		}
		break;
	case I2C_REQUEST_FROM8_BYTE: {
			int p3 = bytesToInt(buffer + 16);
			int p4 = bytesToInt(buffer + 20);
			if (p1 == 1)
				intToBytes((int)Wire1.requestFrom((uint8_t)p2, (uint8_t)p3, (uint8_t)p4), buffer + 8);
			else
				intToBytes((int)Wire.requestFrom((uint8_t)p2, (uint8_t)p3, (uint8_t)p4), buffer + 8);
		}
		break;
	case I2C_REQUEST_FROM8: {
			int p3 = bytesToInt(buffer + 16);
			if (p1 == 1)
				intToBytes((int)Wire1.requestFrom((uint8_t)p2, (uint8_t)p3), buffer + 8);
			else
				intToBytes((int)Wire.requestFrom((uint8_t)p2, (uint8_t)p3), buffer + 8);
		}
		break;
	case I2C_REQUEST_FROM_INT: {
			int p3 = bytesToInt(buffer + 16);
			int p4 = bytesToInt(buffer + 20);
			if (p1 == 1)
				intToBytes((int)Wire1.requestFrom(p2, p3, p4), buffer + 8);
			else
				intToBytes((int)Wire.requestFrom(p2, p3, p4), buffer + 8);
		}
		break;
	case I2C_REQUEST_FROM: {
			int p3 = bytesToInt(buffer + 16);
			if (p1 == 1)
				intToBytes((int)Wire1.requestFrom(p2, p3), buffer + 8);
			else
				intToBytes((int)Wire.requestFrom(p2, p3), buffer + 8);
		}
		break;
	case I2C_WRITE:
		if (p1 == 1)
			intToBytes((int)Wire1.write(p2), buffer + 8);
		else
			intToBytes((int)Wire.write(p2), buffer + 8);
		break;
	case I2C_WRITE_BYTES:
		if (p1 == 1)
			intToBytes((int)Wire1.write(buffer + 16, extsBytes), buffer + 8);
		else
			intToBytes((int)Wire.write(buffer + 16, extsBytes), buffer + 8);
		break;
	case I2C_AVAILABLE:
		if (p1 == 1)
			intToBytes(Wire1.available(), buffer + 8);
		else
			intToBytes(Wire.available(), buffer + 8);
		break;
	case I2C_READ:
		if (p1 == 1)
			intToBytes(Wire1.read(), buffer + 8);
		else
			intToBytes(Wire.read(), buffer + 8);
		break;
	case I2C_PEEK:
		if (p1 == 1)
			intToBytes(Wire1.peek(), buffer + 8);
		else
			intToBytes(Wire.peek(), buffer + 8);
		break;
	case I2C_FLUSH:
		if (p1 == 1)
			Wire1.flush();
		else
			Wire.flush();
		break;
	case I2C_WRITE_TEXT: {
			buffer[16 + extsBytes] = 0;
			char *p3 = (char*)(buffer + 16);
			if (p1 == 1)
				Wire1.write(p3);
			else
				Wire.write(p3);
		}
		break;
	case I2C_WRITE_UINT32:
		if (p1 == 1)
			intToBytes((int)Wire1.write((unsigned long)p2), buffer + 8);
		else
			intToBytes((int)Wire.write((unsigned long)p2), buffer + 8);
		break;
	case I2C_WRITE_INT32:
		if (p1 == 1)
			intToBytes((int)Wire1.write((long)p2), buffer + 8);
		else
			intToBytes((int)Wire.write((long)p2), buffer + 8);
		break;
	case I2C_WRITE_UINT16:
		if (p1 == 1)
			intToBytes((int)Wire1.write((unsigned int)p2), buffer + 8);
		else
			intToBytes((int)Wire.write((unsigned int)p2), buffer + 8);
		break;
	case I2C_WRITE_INT16:
		if (p1 == 1)
			intToBytes((int)Wire1.write((int)p2), buffer + 8);
		else
			intToBytes((int)Wire.write((int)p2), buffer + 8);
		break;
	case I2C_DUMP_INTS:
		if (p1 == 1)
			Wire1.dumpInts();
		else
			Wire.dumpInts();
		break;
	case I2C_DUMP_I2C:
		if (p1 == 1)
			Wire1.dumpI2C();
		else
			Wire.dumpI2C();
		break;
	
	// esp32-hal-ledc.h: 700~
	case LEDC_SETUP: {
			int p3 = bytesToInt(buffer + 16);
			intToBytes((int)ledcSetup(p1, (double)p2, p3), buffer + 8);
		}
		break;
	case LEDC_WRITE: ledcWrite(p1, p2); break;
	case LEDC_WRITE_TONE: intToBytes((int)ledcWriteTone(p1, (double)p2), buffer + 8); break;
	case LEDC_WRITE_NOTE: {
			int p3 = bytesToInt(buffer + 16);
			intToBytes((int)ledcWriteNote(p1, (note_t)p2, p3), buffer + 8);
		}
		break;
	case LEDC_READ: intToBytes((int)ledcRead(p1), buffer + 8); break;
	case LEDC_READ_FREQ: intToBytes((int)ledcReadFreq(p1), buffer + 8); break;
	case LEDC_ATTACH_PIN: ledcAttachPin(p1, p2); break;
	case LEDC_DETACH_PIN: ledcDetachPin(p1); break;
	
	// esp32-hal-log.h: 800~
	
	// esp32-hal-matrix.h: 900~
	
	// esp32-hal-psram.h: 1000~
	
	// esp32-hal-sigmadelta.h: 1100~
	case SIGMA_DELTA_SETUP: intToBytes((int)sigmaDeltaSetup(p1, (uint32_t)p2), buffer + 8); break;
	case SIGMA_DELTA_WRITE: sigmaDeltaWrite(p1, p2); break;
	case SIGMA_DELTA_READ: intToBytes(sigmaDeltaRead(p1), buffer + 8); break;
	case SIGMA_DELTA_ATTACH_PIN: sigmaDeltaAttachPin(p1, p2); break;
	case SIGMA_DELTA_DETACH_PIN: sigmaDeltaDetachPin(p1); break;
	
	// esp32-hal-spi.h (SPI.h): 1200~
	case SPI_BEGIN: {
			int p3 = bytesToInt(buffer + 16);
			int p4 = bytesToInt(buffer + 20);
			int p5 = bytesToInt(buffer + 24);
			if (p1 == 2)
				SPI1.begin(p2, p3, p4, p5);
			else
				SPI.begin(p2, p3, p4, p5);
		}
		break;
	case SPI_END:
		if (p1 == HSPI)
			SPI1.end();
		else
			SPI.end();
		break;
	case SPI_SET_HW_CS:
		if (p1 == HSPI)
			SPI1.setHwCs(p2);
		else
			SPI.setHwCs(p2);
		break;
	case SPI_SET_BIT_ORDER:
		if (p1 == HSPI)
			SPI1.setBitOrder(p2);
		else
			SPI.setBitOrder(p2);
		break;
	case SPI_SET_DATA_MODE:
		if (p1 == HSPI)
			SPI1.setDataMode(p2);
		else
			SPI.setDataMode(p2);
		break;
	case SPI_SET_FREQUENCY:
		if (p1 == HSPI)
			SPI1.setFrequency((uint32_t)p2);
		else
			SPI.setFrequency((uint32_t)p2);
		break;
	case SPI_SET_CLOCK_DIVIDER:
		if (p1 == HSPI)
			SPI1.setClockDivider((uint32_t)p2);
		else
			SPI.setClockDivider((uint32_t)p2);
		break;
	case SPI_GET_CLOCK_DIVIDER:
		if (p1 == HSPI)
			intToBytes((int)SPI1.getClockDivider(), buffer + 8);
		else
			intToBytes((int)SPI.getClockDivider(), buffer + 8);
		break;
	case SPI_BEGIN_TRANSACTION: {
			int p3 = bytesToInt(buffer + 16);
			int p4 = bytesToInt(buffer + 20);
			SPISettings settings;
			settings._clock = p2;
			settings._bitOrder = p3;
			settings._dataMode = p4;
			if (p1 == HSPI)
				SPI1.beginTransaction(settings);
			else
				SPI.beginTransaction(settings);
		}
		break;
	case SPI_END_TRANSACTION:
		if (p1 == HSPI)
			SPI1.endTransaction();
		else
			SPI.endTransaction();
		break;
	case SPI_TRANSFER:
		if (p1 == HSPI)
			SPI1.transfer(p2);
		else
			SPI.transfer(p2);
		break;
	case SPI_TRANSFER16:
		if (p1 == HSPI)
			SPI1.transfer16(p2);
		else
			SPI.transfer16(p2);
		break;
	case SPI_TRANSFER32:
		if (p1 == HSPI)
			SPI1.transfer32(p2);
		else
			SPI.transfer32(p2);
		break;
	case SPI_TRANSFER_BYTES:
		intToBytes(extsBytes, buffer + 12);
		if (p1 == HSPI)
			SPI1.transferBytes(buffer + 16, buffer + 16, (uint32_t)extsBytes);
		else
			SPI.transferBytes(buffer + 16, buffer + 16, (uint32_t)extsBytes);
		break;
	case SPI_TRANSFER_BITS: {
			int p3 = bytesToInt(buffer + 16);
			uint32_t out;
			if (p1 == HSPI)
				SPI1.transferBits((uint)p2, &out, p3);
			else
				SPI.transferBits((uint)p2, &out, p3);
			intToBytes((int)out, buffer + 8);
		}
		break;
	case SPI_WRITE:
		if (p1 == HSPI)
			SPI1.write(p2);
		else
			SPI.write(p2);
		break;
	case SPI_WRITE16:
		if (p1 == HSPI)
			SPI1.write16(p2);
		else
			SPI.write16(p2);
		break;
	case SPI_WRITE32:
		if (p1 == HSPI)
			SPI1.write32(p2);
		else
			SPI.write32(p2);
		break;
	case SPI_WRITE_BYTES:
		if (p1 == HSPI)
			SPI1.writeBytes(buffer + 16, (uint32_t)extsBytes);
		else
			SPI.writeBytes(buffer + 16, (uint32_t)extsBytes);
		break;
	case SPI_WRITE_PIXELS:
		if (p1 == HSPI)
			SPI1.writePixels((const void*)(buffer + 16), (uint32_t)extsBytes);
		else
			SPI.writePixels((const void*)(buffer + 16), (uint32_t)extsBytes);
		break;
	case SPI_WRITE_PATTERN:
		if (p1 == HSPI)
			SPI1.writePattern(buffer + 16, (uint32_t)extsBytes, (uint32_t)p2);
		else
			SPI.writePattern(buffer + 16, (uint32_t)extsBytes, (uint32_t)p2);
		break;
		
	// esp32-hal-timer.h: 1300~
		
	// esp32-hal-touch.h: 1400~
	case TOUCH_SET_CYCLES: touchSetCycles(p1, p2); break;
	case TOUCH_READ: intToBytes(touchRead(p1), buffer + 8); break;
	case TOUCH_ATTACH_INTERRUPT:
		touchInterruptList[p1].attach = 1;
		touchInterruptList[p1].mode = p2;
		switch (p1) {
		case 4: touchAttachInterrupt(p1, touchIsr4, p2); break;
		case 12: touchAttachInterrupt(p1, touchIsr12, p2); break;
		case 13: touchAttachInterrupt(p1, touchIsr13, p2); break;
		case 14: touchAttachInterrupt(p1, touchIsr14, p2); break;
		case 15: touchAttachInterrupt(p1, touchIsr15, p2); break;
		case 27: touchAttachInterrupt(p1, touchIsr27, p2); break;
		}
		break;
		
	// esp32-hal-uart.h: 1500~

	default:
		intToBytes(ERR_UNKNOWN_COMMAND, buffer + 4);
		break;

	}
}

int RemoteEsp32Class::bytesToInt(byte *bytes)
{
	return    (bytes[3] << 24)
		+ (bytes[2] << 16)
		+ (bytes[1] << 8)
		+  bytes[0];
}

void RemoteEsp32Class::intToBytes(int value, byte *bytes)
{
	int b = 0;
	bytes[b++] = value & 0xff;
	bytes[b++] = (value >> 8) & 0xff;
	bytes[b++] = (value >> 16) & 0xff;
	bytes[b++] = (value >> 24) & 0xff;
}

void RemoteEsp32Class::intsToBytes(int *ints, int intsSize, byte *bytes) {
	int b = 0;
	for (int i = 0; i < intsSize; i++) {
		bytes[b++] = ints[i] & 0xff;
		bytes[b++] = (ints[i] >> 8) & 0xff;
		bytes[b++] = (ints[i] >> 16) & 0xff;
		bytes[b++] = (ints[i] >> 24) & 0xff;
	}
}

void RemoteEsp32Class::printBytes(byte *bytes, int bytesSize)
{
	for (int i = 0; i < bytesSize; i++)
	{
		Serial.print(bytes[i], HEX);
		Serial.print(" ");
	}
	Serial.println();
}

void IRAM_ATTR RemoteEsp32Class::isr(void* arg) {
	Interrupt* s = static_cast<Interrupt*>(arg);
	detachInterrupt(s->pin);
	s->count++;
	s->time = millis();
}

void IRAM_ATTR RemoteEsp32Class::touchIsr4() {
	Interrupt* s = &touchInterruptList[4];
	detachInterrupt(s->pin);
	s->count++;
	s->time = millis();
}

void IRAM_ATTR RemoteEsp32Class::touchIsr12() {
	Interrupt* s = &touchInterruptList[12];
	detachInterrupt(s->pin);
	s->count++;
	s->time = millis();
}

void IRAM_ATTR RemoteEsp32Class::touchIsr13() {
	Interrupt* s = &touchInterruptList[13];
	detachInterrupt(s->pin);
	s->count++;
	s->time = millis();
}

void IRAM_ATTR RemoteEsp32Class::touchIsr14() {
	Interrupt* s = &touchInterruptList[14];
	detachInterrupt(s->pin);
	s->count++;
	s->time = millis();
}

void IRAM_ATTR RemoteEsp32Class::touchIsr15() {
	Interrupt* s = &touchInterruptList[15];
	detachInterrupt(s->pin);
	s->count++;
	s->time = millis();
}

void IRAM_ATTR RemoteEsp32Class::touchIsr27() {
	Interrupt* s = &touchInterruptList[27];
	detachInterrupt(s->pin);
	s->count++;
	s->time = millis();
}

