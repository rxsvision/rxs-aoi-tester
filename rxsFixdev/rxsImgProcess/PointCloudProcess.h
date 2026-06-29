#pragma once
#include "imgProcess.h"

#include <vector>
#include <map>
#include <string>
#include <io.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>


#pragma warning(disable : 4996)

//#define			_NO_PRODUCT_USED_				1


#if _NO_PRODUCT_USED_

//-----------原本使用---------后期产品化优化可直接删除-----
typedef struct _myRoiarea {
	rxsPointCouldp P[3];
	CloudEntity	ce;
	unsigned id;
	/// <summary>
	/// 类型:0 boundBox (max min 角点与 dm) , 1 点域 (center ， xyz各项长度，保留)
	/// </summary>
	u8 Type;
	const char* forName;

	bool operator==(const char* name) {
		return (memcmp(forName, name, strlen(forName)) == 0);
	}

	bool operator==(_myRoiarea c) {
		bool cv = (memcmp(forName, c.forName, strlen(forName)) == 0);
		if (!cv) return cv;
		cv = (id == c.id);
		if (!cv) return cv;
		cv = (Type == c.Type);
		if (!cv) return cv;
		//需要CE结构体中内容完全等价
		cv = (memcmp(&ce, &c.ce, sizeof(CloudEntity)) == 0);
		if (!cv) return cv;
		cv = (memcmp(P, c.P, sizeof(rxsPointCouldp) * 3) == 0);
		return cv;
	}
}roiArea;



class rxsRoiAreaManger {
private:
	std::vector<roiArea> roiGrps;
	short fill(std::string str);
	std::map<std::string, int> namesGrps;
	void removenames(const char* name);
	void updateNames();
public:
	bool Remove(roiArea);
	bool Remove(const char* name);
	bool Add(const char* Name, rxsPointCouldp coordp[3], rxsPointCouldp* Entiy, unsigned long pNums, u8 type);
	void Clear();
	roiArea* Get(unsigned index);
	roiArea* Get(int ID);
	roiArea* Get(std::string forName, int& index);
	int Get(roiArea*);
	unsigned Count();
	std::string SaveStringGet();
	short PushCfgString(std::string);
};




#else








//------------------产品化优化使用------------
typedef struct _myRoiarea {
	unsigned id;
	roiRect	Area;
	CloudEntity	CE;

	bool operator==(std::string name) {
		return (Area.rName == name);
	}

	bool operator==(_myRoiarea c) {
		bool cv = (Area.rName == c.Area.rName);
		if (!cv) return cv;
		cv = (id == c.id);
		if (!cv) return cv;
		cv = (Area.Flag == c.Area.Flag);
		if (!cv) return cv;
		//需要CE结构体中内容完全等价
		cv = (memcmp(&CE, &c.CE, sizeof(CloudEntity)) == 0);
		if (!cv) return cv;
		cv = (memcmp(&Area.SORC, &c.Area.SORC, sizeof(rxsPointCouldp)) == 0);
		if (!cv) return cv;
		cv = (memcmp(&Area.End, &c.Area.End, sizeof(rxsPointCouldp)) == 0);
		if (!cv) return cv;
		cv = (memcmp(&Area.LWH, &c.Area.LWH, sizeof(rxsPointCouldp)) == 0);
		return cv;
	}
}RoiArea;



class rxsRoiAreaManger {
private:
	std::vector<RoiArea> roiGrps;
	short fill(std::string str);

public:
	~rxsRoiAreaManger();

	bool Remove(RoiArea);
	bool Remove(std::string name);
	bool Add(std::string Name, rxsPointCouldp coordp[2], rxsPointCouldp* Entiy, unsigned long pNums, u8 flag);
	void Clear();
	RoiArea* Get(unsigned index);
	RoiArea* Get(int ID);
	RoiArea* Get(std::string forName, int& index);
	int Get(RoiArea*);
	unsigned Count();
	std::string SaveStringGet();
	short PushCfgString(std::string);
};




#endif // _NO_PRODUCT_USED_






typedef struct myRectArea {
	double min_x;
	double max_x;
	double min_y;
	double max_y;
}BaseFace;