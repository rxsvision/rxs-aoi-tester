#pragma once
#include "dwg.h"
#include <Windows.h>

#define CLASSEXPORT		__declspec(dllexport)


class CLASSEXPORT CADImport
{
public:
	CADImport(const char* exePath = 0);
	~CADImport();

	/// <summary>
	/// 
	/// </summary>
	bool Load(const char*,unsigned int opts);
	void Close();

	bool isExternalApp();
protected:
	bool isDxf(const char* path);
	int ReadFile(const char*);
	void Free();
private:
	HMODULE dllHander;
	Dwg_Data dwg;
	void* extExe;
	bool openExe();
};

