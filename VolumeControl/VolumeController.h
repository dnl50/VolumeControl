#pragma once
#include <datetimeapi.h>

class VolumeController
{

private:


public:
	VolumeController();
	~VolumeController();

	HRESULT initCOM();
	HRESULT uninitCOM();

};

