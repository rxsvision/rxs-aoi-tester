#ifndef __CHROCODILELIBPLUGINDEF_H__
#define __CHROCODILELIBPLUGINDEF_H__
/* Latest Git commit hash: @CHRLIB_GIT_REVID@ */
#if (defined(_MSC_VER) && (_MSC_VER >= 1020)) || (defined(__GNUC_VERSION__) && __GNUC_VERSION__ >= 30400) || defined(__MCPP) || defined(__clang__)
#    pragma once
#endif /* Check for "#pragma once" support */

#if (__cplusplus >= 201103L)
#    include <cstdint>
#else
#    if defined(_MSC_VER) && (_MSC_VER >= 1900)
#        include <stdint.h>
#    elif defined(_MSC_VER) /* versions of MSVC older than VS2015 need this */
typedef unsigned __int64 uint64_t;
typedef signed __int64 int64_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;
typedef unsigned short int uint16_t;
typedef signed short int int16_t;
#    else
#        include <stdint.h>
#    endif
#endif

static const char Test_Plugin_Name[] = "TestPlugin";

static const uint32_t CmdID_TestPlugin_CMD_A = 0x41545354;
static const uint32_t CmdID_TestPlugin_CMD_B = 0x42545354;
static const uint32_t CmdID_TestPlugin_CMD_C = 0x43545354;
static const uint32_t CmdID_TestPlugin_CMD_D = 0x44545354;

static const char DownSample_Plugin_Name[] = "DownSamplePlugin";

static const uint32_t CmdID_DownSample_Rate = 0x54525344;

static const char CLS2_Calib_Plugin_Name[] = "CLS2CalibrationPlugin";

static const uint32_t CmdID_CalibFile_Name = 0x464C4143;
static const uint32_t CmdID_Calib_Active = 0x414C4143;
static const uint32_t CmdID_CalibFile_List = 0x4C464143;


#endif // __CHROCODILELIBPLUGINDEF_H__
