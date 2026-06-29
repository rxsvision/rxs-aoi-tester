
#include "rxsHrdwdal.h"
#include "proteced.h"
#include <Windows.h>
#include <string>
#include <iostream>


using namespace std;


CST_TZls::CST_TZls() :
    FHWObject(),
    r232Connection()
{
    result = new string;
}

short CST_TZls::Link(const char* cmdline) {
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

short CST_TZls::unLink() {

    if (result != 0) {
        delete (string*)result;
        result = 0;
    }
    return 0;
}


int CST_TZls::Writer(void* data, unsigned len) {
    return SendReq(data, len);
}

void* CST_TZls::Reader(unsigned& len) {
    return GetRsp(len);
}




short CST_TZls::brightnessDGet(char channel) {
    char buff[3] = { 'S',channel,'#' };
    if (Writer(buff, 3) > 0) {
        unsigned rbl = 0;
        char* rb = (char*)Reader(rbl);
        return (rbl > 0 && ((rb[0] > 96 && rb[0] < 105) || (rb[0] > 64 && rb[0] < 73)));
    }
    return -1;
}

bool CST_TZls::brightnessDSet(char channel, short val) {
    char buff[8];
    sprintf_s(buff, "S%c%04d#", channel, val);
    buff[7] = 0;
    if (Writer(buff, 7) > 0) {
        unsigned rbl = 0;
        char* rb = (char*)Reader(rbl);
        return (rbl > 0 && ((rb[0] > 96 && rb[0] < 105) || (rb[0] > 64 && rb[0] < 73)));
    }
    return false;
}



bool CST_TZls::ConstantbrightModeSet(bool on_off) {
    char cmd[3] = { 'T',(on_off ? 'H' : 'L'),'#' };
    if (Writer((void*)cmd, 3) > 0) {
        unsigned rbl = 0;
        char* rb = (char*)Reader(rbl);
        return (rbl > 0 && (rb[0] == 'H' || rb[0] == 'h' || rb[0] == 'L' || rb[0] == 'l'));
    }
    return false;
}


bool CST_TZls::StrobeModeSet(char channel,int pw) {
    char buff[9];
    sprintf_s(buff, "SP%c%04d#", channel, pw);
    buff[8] = 0;
    if (Writer((void*)buff, 8) > 0) {
        unsigned rbl = 0;
        char* rb = (char*)Reader(rbl);
        return (rbl > 0 && (rb[0] == 'P' || rb[0] == 'p'));
    }
    return false;
}


bool CST_TZls::ModeGet() {
    char cmd[2] = { 'T','#' };
    if (Writer((void*)cmd, 2) > 0) {
        unsigned rbl = 0;
        char* rb = (char*)Reader(rbl);
        return (rbl > 0 && (rb[0] == 'S' || rb[0] == 's'));
    }
    return false;
}

                                                                                                                                               