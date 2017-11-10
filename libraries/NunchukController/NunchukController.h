#ifndef NunchukController
#define NunchukController

#include <Arduino.h>
#include <Wire.h>
#include <binary.h>



namespace NunchukController
{
	#define SHAKE_SENSITIVITY 600
	#define DEFAULT_JOY_CENTRE_X 130
	#define DEFAULT_JOY_CENTRE_Y 130

	uint8_t buffer[6];
	uint8_t joy_center_x = DEFAULT_JOY_CENTRE_X, joy_center_y = DEFAULT_JOY_CENTRE_Y;

	static void initncc()
	{
		Wire.begin();
		delay(1);
		Wire.beginTransmission(0x52);
		Wire.write((uint8_t)0xF0);
		Wire.write((uint8_t)0x55);
		Wire.endTransmission();
		delay(1);
		Wire.beginTransmission(0x52);
		Wire.write((uint8_t)0xFB);
		Wire.write((uint8_t)0x00);
		Wire.endTransmission();
		delay(100);
	}

	static bool get_data()
	{
		uint8_t i = 0;
		Wire.requestFrom(0x52, 6);
		while (Wire.available() && i < 6)
		{
			buffer[i++] = Wire.read();
		}
		Wire.beginTransmission(0x52);
		Wire.write(0x00);
		Wire.endTransmission();

		return i >= 5;
	}
	static bool inline button_c()
{
	return !((buffer[5] >> 1) & 1);
}
	static bool inline button_z()
{
	return !(buffer[5] & 1);
}
	static signed int inline joy_x()
{
	return (signed int)(buffer[0] - joy_center_x);
}
	static signed int inline joy_y()
{
	return (signed int)(buffer[1] - joy_center_y);
}
	static inline uint16_t accel_x()
{
	return ((0x0000 | buffer[2]) << 2) + ((buffer[5] & B00001100) >> 2);
}
	static inline uint16_t accel_y()
{
	return ((0x0000 ^ buffer[3]) << 2) + ((buffer[5] & B00110000) >> 4);
}
	static inline uint16_t accel_z()
{
	return ((0x0000 ^ buffer[4]) << 2) + ((buffer[5] & B11000000) >> 6);
}
	static bool shake()
{
	static int16_t oax, oay, oaz;

	bool sh = false;

	if (abs(oax - (int16_t)NunchukController::accel_x()) > SHAKE_SENSITIVITY)
	{
		sh = true;
	}
	else if (abs(oay - (int16_t)NunchukController::accel_y()) > SHAKE_SENSITIVITY)
	{
		sh = true;
	}
	else if (abs(oaz - (int16_t)NunchukController::accel_z()) > SHAKE_SENSITIVITY)
	{
		sh = true;
	}

	oax = NunchukController::accel_x();
	oay = NunchukController::accel_y();
	oaz = NunchukController::accel_z();

	return sh;
}
	static void inline calibrate()
{
	joy_center_x = NunchukController::joy_x();
	joy_center_y = NunchukController::joy_y();
}
};

#endif // !NunchukController
