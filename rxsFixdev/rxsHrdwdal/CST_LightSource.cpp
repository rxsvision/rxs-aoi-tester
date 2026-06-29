#include "rxsHrdwdal.h"
#include "proteced.h"
#include <Windows.h>
#include <string>
#include <iostream>


using namespace std;


CST_LightSource::CST_LightSource() :
    FHWObject(),
    r232Connection()
{
    result = new string;
}

short CST_LightSource::Link(const char* cmdline) {
    vector<string> sprr = stringSplit(cmdline, ',');
    int vi = 0;
    float vf = 0;

    if (sprr.size() < 2) { return -1; }

    unsigned char dataSize = 8;
    if (sprr.size() > 2) {
        dataSize = std::atoi(sprr[2].c_str());
    }

    /*
    ONESTOPBIT      1个停止位
    ONE5STOPBITS    1.5个停止位
    TWOSTOPBITS     2个停止位
    */
    unsigned char stop_bits = ONESTOPBIT;
    if (sprr.size() > 3) {
        vf = std::atoi(sprr[3].c_str());
        if (vf == 2) {
            stop_bits = TWOSTOPBITS;
        }
        else if (vf == 1.5) {
            stop_bits = ONE5STOPBITS;
        }
    }

    /*
        EVENPARITY  偶校验
        MARKPARITY  标号校验
        NOPARITY    无校验
        ODDPARITY   奇校验
        SPACEPARITY 空格校验
    */
    unsigned char parity = NOPARITY;
    if (sprr.size() > 4) {
        if (sprr[4] == "NONE") {
            parity = NOPARITY;
        }
    }
    return Comopen(sprr[0].c_str(), std::atoi(sprr[1].c_str()), parity, dataSize, stop_bits);
}

short CST_LightSource::unLink() {

    if (result != 0) {
        delete (string*)result;
        result = 0;
    }
    return 0;
}


int CST_LightSource::Writer(void* data, unsigned len) {
    return SendReq(data, len);
}

void* CST_LightSource::Reader(unsigned& len) {
    return GetRsp(len);
}

void CST_LightSource::Reset() {
    memset(pblk, 0, sizeof(Partition) * Cnum);

}

CST_LightSource::Partition* CST_LightSource::PartitionStatus() {
    return pblk;
}


unsigned char CST_LightSource::brightnessGet(short fq) {
    char buff[20];
    memset(buff, 0, 20);
    char FQ[4] = { 'A','B' ,'C' ,'D' };
    sprintf_s(buff, "S%c#", FQ[fq]);
    string cmd = buff;
    if (Writer((void*)cmd.c_str(), cmd.length()) > 0) {
        unsigned rbl = 0;
        char* rb = (char*)Reader(rbl);
        if (rbl > 0) {
            (*(string*)result) = rb;
            return ((string*)result)->c_str()[1];
        }
    }
    return 0;
}


bool CST_LightSource::brightnessSet(short BlockIndex, ColorIndex ci, short val) {
    char buff[20];
    memset(buff, 0, 20);
    char FQ[4] = { 'A','B' ,'C' ,'D' };
    char fq[4] = { 'a','b' ,'b' ,'d' };
    sprintf_s(buff, "S%c%1d%03d#", FQ[BlockIndex], (short)ci, val);
    string cmd = buff;
    if (Writer((void*)cmd.c_str(), cmd.length()) > 0) {
        unsigned rbl = 0;
        char* rb = (char*)Reader(rbl);
        pblk[BlockIndex].curVal = val;
        return (rbl > 0 && (rb[0] == FQ[BlockIndex] || rb[0] == fq[BlockIndex]));
    }
    return false;
}

bool CST_LightSource::brightnessONOFF(ColorIndex* ofIndexVal, int len) {
    char buff[20];
    memset(buff, 0, 20);
    int nis = len + 1;
    char* FQ = new char[nis];
    FQ[nis - 1] = 0;
    for (short x = 0; x < len; x++) {
        FQ[x] = (ofIndexVal[x] + 0x30);
        if (ofIndexVal[x] == None) {
            pblk[x].isOn = false;
            pblk[x].curCi = None;
        }
        else {
            pblk[x].isOn = true;
            pblk[x].curCi = ofIndexVal[x];
        }
    }
    sprintf_s(buff, "SW%s#", FQ);
    string cmd = buff;
    delete[] FQ;
    FQ = 0;
    if (Writer((void*)cmd.c_str(), cmd.length()) > 0) {
        unsigned rbl = 0;
        char* rb = (char*)Reader(rbl);
        return (rbl > 0 && (rb[0] == 'S' || rb[0] == 's'));
    }
    return false;
}
