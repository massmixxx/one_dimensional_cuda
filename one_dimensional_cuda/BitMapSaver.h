#pragma once
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdexcept>

class BitMapSaver
{
public:
	BitMapSaver();
	~BitMapSaver();

	static void SaveBitmap(unsigned char*, int, int, std::string name);
};

