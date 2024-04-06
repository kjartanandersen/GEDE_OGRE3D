#include "pch.h"
#include "ResourceManager.h"
#include <Windows.h>
#include <filesystem>
#include <OgreFileSystem.h>
#include <iostream>

#define BUFSIZE MAX_PATH

namespace fs = std::filesystem;

GEDEResourceManager::GEDEResourceManager()
{
	TCHAR Buffer[BUFSIZE];

	GetCurrentDirectory(BUFSIZE, Buffer);

	fs::path p1 = Buffer;

	labFilesPath = p1.parent_path().string() + "\\LabFiles\\";
	soundsPath = labFilesPath + "Sounds\\";
	

}

std::string GEDEResourceManager::GetSoundsPath()
{
	return soundsPath;
}