#include "rxsHrdwdal.h"
#include "proteced.h"
#include <Windows.h>
#include <string>
#include <iostream>


using namespace std;


#define RC_BUFFZISE    1024


short AdjustFocal::Link(const char* cmdline) {
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



void* AdjustFocal::Reader(unsigned& len) {
    return GetRsp(len);
}

int AdjustFocal::Writer(void* c, unsigned len) {
    return SendReq(c, len);
}

bool AdjustFocal::Range(int& min, int& max) {
    min = MinV;
    max = MaxV;
    return true;
}

short AdjustFocal::CurVal() {
    return val;
}

bool AdjustFocal::Set(short v) {
    if (v<MinV || v>MaxV) { return false; }
    unsigned char vals[2] = { v ,v >> 8 };
    unsigned char cmd[8] = { 0x53,0x31,0x01,0x02,vals[0],vals[1],0,0x3E };
    int sum = 0;
    //注意此处求校验和的方法
    for (int i = 0; i < 6; i++) {
        sum += cmd[i];
    }
    cmd[6] = (sum & 0x00FF);//crc sum
    if (SendReq(cmd, 8) > 0) {
        unsigned len = 0;
        char* rev = (char*)GetRsp(len);
        if (rev == 0 || len < 1) { return false; }
        val = v;
        return true;
    }
    return false;
}

AdjustFocal::AdjustFocal() :
    FHWObject(),
    r232Connection()
{
    MinV = 0;
    MaxV = 1023;
    val = 0;
}