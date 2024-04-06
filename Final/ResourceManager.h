#pragma once
#include "pch.h"

class GEDEResourceManager
{
public:
	
	
	GEDEResourceManager();

	std::string GetSoundsPath();

private:
	std::string labFilesPath;
	std::string soundsPath;
};

