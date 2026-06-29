#pragma once
#include "rxsFixdal.h"
#include <vector>
#include <map>
#include <string>
#include <io.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>


#define pSystem			((rxsMainFrame*)mainFrameptr)

#pragma warning(disable : 4996)


bool TotalLeastSquares(double* Xgather, double* Ygather, unsigned gatherNum, double& k, double& b);
///直线度
double straightness(double* Xgather, double* Ygather, unsigned gatherNum, double k, double b, double& pDiff, double& nDiff);
void valueCenter(double* srcVgather, unsigned gatherNum);








class Cpk {
public:
	Cpk(double maxVal, double minVal);
	Cpk(double midVal);
	~Cpk();
	double GetVal(double*, unsigned);
private:
	double midVal;
	double uplVal;
	double downlVal;
	double Average(double* sampleData, unsigned sdLen);
	double Standarddeviation(double* sampleData, unsigned sdLen, double average, bool isGeneral = false);
};

