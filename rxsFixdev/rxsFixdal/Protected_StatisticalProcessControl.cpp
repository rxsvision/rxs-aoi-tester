#pragma once
#include "rxsFixdal.h"
#include "protected.h"



Cpk::Cpk(double maxV, double minV)
{
    midVal = -1.0f;
    uplVal = maxV;
    downlVal = minV;
}

Cpk::Cpk(double midV) {
    midVal = midV;
    uplVal = -1.0f;
    downlVal = -1.0f;
}

Cpk::~Cpk()
{

}

double Cpk::Average(double* sampleData, unsigned sdLen) {
    double sum = 0.0f;
    for (unsigned x = 0; x < sdLen; x++) {
        sum += sampleData[x];
    }
    return sum / sdLen;
}


double Cpk::Standarddeviation(double* sampleData, unsigned sdLen, double average, bool isGeneral) {
    double c = 0;
    double sum = 0;
    for (unsigned x = 0; x < sdLen; x++) {
        c = sampleData[x] - average;
        c = c * c;
        sum += c;
    }
    double val = sum / (isGeneral ? sdLen : (sdLen - 1));
    return sqrt(val);
}

double Cpk::GetVal(double* sampleData, unsigned sdLen) {
    double avr = Average(sampleData, sdLen);
    double sigma = Standarddeviation(sampleData, sdLen, avr);

    if (uplVal < 0 && downlVal < 0) {
        //方法1 中心规格 + 宽度方式

    }
    else {
        //方法2 规格上下限方式
        double cpu = (uplVal - avr) / (3 * sigma);
        double cpl = (avr - downlVal) / (3 * sigma);

        cpu = (cpu > 0) ? cpu : 0 - cpu;
        cpl = (cpl > 0) ? cpl : 0 - cpl;
        return (cpu > cpl ? cpl : cpu);
    }
}

