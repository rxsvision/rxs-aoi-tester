/**
 *  \file
 *
 *  \copyright    Copyright (C) @CHRLIB_GIT_TIME_YEAR@ by Precitec Optronik GmbH
 *  \brief CHRocodileLibSpecialFunc.h provides a few convenient functions for setting up
 *  \\CHR~device standard parameters.
 *  All the functions are based on CHRocodileLib API functions.
 *  Exception functions used for download table from multi-channel devices, all the
 *  functions can be used for both synchronous and asynchronous connection.
 */

#ifndef __CHROCODILELIBSPECIALFUNC_H__
#define __CHROCODILELIBSPECIALFUNC_H__ @CHRLIB_GIT_HEADERDEF@ /// Latest change: @CHRLIB_GIT_ISODATE@
/* Latest Git commit hash: @CHRLIB_GIT_REVID@ */
#if (defined(_MSC_VER) && (_MSC_VER >= 1020)) || (defined(__GNUC_VERSION__) && __GNUC_VERSION__ >= 30400) || defined(__MCPP) || defined(__clang__)
#    pragma once
#endif /* Check for "#pragma once" support */

#include "CHRocodileLib.h"
#include "CHRocodileLibErrorDef.h"

/// \cond nohide
#ifndef __cplusplus
#    include <stdbool.h>
#    define nullptr 0
#endif
/// \endcond

/// \cond nohide
#define SYNC_CONN(h) (GetConnectionMode(h) == Connection_Synchronous)
/// \endcond

/** \defgroup ConvenienceHelpers Helper functions to more conveniently use the API.
 *  @{
 */

/**
 * \ingroup DeviceCmd
 * \brief Convert a numeric command ID to its mnemonic (short string) form
 *
 * \param[in] _nCmdID Command ID.
 * \param[out] _strCmd A buffer of at least 4 bytes size. The mnemonic form of the device command is returned in this buffer.
 *            This function will not take care of zero-terminating the buffer, so if you pass a buffer bigger than 4 bytes
 *            make sure to zero-terminate it yourself.
 */
inline void CmdIDToName(uint32_t _nCmdID, char *_strCmd)
{
    uint8_t *pTemp = (uint8_t *)(&_nCmdID);
    for (int32_t i = 0; i < 4; ++i)
        _strCmd[i] = (char)(pTemp[i]);
}

/**
 * \ingroup DeviceCmd
 * \ingroup DeviceRsp
 * \brief Validate the command ID originally passed from inside a response
 *
 * \param[in] _hRsp Response handle, either returned by ExecCommand() or passed through response and update callback function.
 * \param[in] _nCmdID Command ID.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t CheckCmdID(Rsp_h _hRsp, uint32_t _nCmdID)
{
    TResponseInfo sRspInfo;
    Res_t nRes;
    nRes = GetResponseInfo(_hRsp, &sRspInfo);
    if (CHR_SUCCESS(nRes))
        nRes = (sRspInfo.CmdID == _nCmdID) ? nRes : ERR_CMD_RESPONSE_ID_DIFFERENT;
    return (nRes);
}

/**
 * \ingroup DeviceCmd
 * \brief Execute a device command without parameters
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nCmdID Command ID.
 * \param[in] _bQuery Whether command is query or not.
 * \param[out] _pTicket Pointer to a variable receiving the internal ticket for the issued device command.
 * \param[out] _phRsp Pointer to a buffer receiving the response handle.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t ExecNoParamCmd(
    Conn_h _hConnection, uint32_t _nCmdID, int32_t _bQuery, int32_t *_pTicket, Rsp_h *_phRsp)
{
    Cmd_h hCmd;
    NewCommand(_nCmdID, _bQuery, &hCmd);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, _phRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);
    return (nRes);
}

/**
 * \ingroup DeviceRsp
 * \brief Extract a single integer parameter from the response to a device command
 *
 * \param[in] _hRsp Response handle, either returned by ExecCommand() or passed through response and update callback function.
 * \param[in] _nCmdID Command ID.
 * \param[in] _nIdx Parameter index.
 * \param[out] _pParam Pointer to an integer variable receiving the parameter value.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t ReadSingleIntParamRsp(Rsp_h _hRsp, uint32_t _nCmdID, uint32_t _nIdx, int32_t *_pParam)
{
    Res_t nRes = CheckCmdID(_hRsp, _nCmdID);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, _nIdx, _pParam);
    return (nRes);
}

/**
 * \ingroup DeviceCmd
 * \brief Execute a device command with one integer parameter
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nCmdID Command ID.
 * \param[in] _bQuery Whether command is query or not.
 * \param[in] _nParam The integer parameter to the device command.
 * \param[out] _pTicket Pointer to a variable receiving the internal ticket for the issued device command.
 * \param[out] _phRsp Pointer to a buffer receiving the response handle.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t ExecSingleIntParamCmd(
    Conn_h _hConnection,
    uint32_t _nCmdID,
    int32_t _bQuery,
    int32_t _nParam,
    int32_t *_pTicket,
    Rsp_h *_phRsp)
{
    Cmd_h hCmd;
    NewCommand(_nCmdID, _bQuery, &hCmd);
    AddCommandIntArg(hCmd, _nParam);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, _phRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);
    return (nRes);
}

/**
 * \ingroup DeviceRsp
 * \brief Extract a single integer array parameter from the response to a device command
 *
 * \param[in] _hRsp Response handle, either returned by ExecCommand() or passed through response and update callback function.
 * \param[in] _nCmdID Command ID.
 * \param[in] _nIdx Parameter index.
 * \param[out] _pParam Pointer to a variable receiving the pointer to the integer array data.
 * \param[out] _pnLength Pointer to an integer variable receiving the array length in number of elements.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t
ReadSingleIntArrayParamRsp(Rsp_h _hRsp, uint32_t _nCmdID, uint32_t _nIdx, const int32_t **_pParam, int32_t *_pnLength)
{
    Res_t nRes = CheckCmdID(_hRsp, _nCmdID);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArrayArg(_hRsp, _nIdx, _pParam, _pnLength);
    return (nRes);
}

/**
 * \ingroup DeviceCmd
 * \brief Execute a device command with one integer array parameter
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nCmdID Command ID.
 * \param[in] _bQuery Whether command is query or not.
 * \param[in] _pParam The integer array parameter to the device command.
 * \param[in] _nParamlength The length of the integer array in elements.
 * \param[out] _pTicket Pointer to a variable receiving the internal ticket for the issued device command.
 * \param[out] _phRsp Pointer to a buffer receiving the response handle.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t ExecSingleIntArrayParamCmd(
    Conn_h _hConnection,
    uint32_t _nCmdID,
    int32_t _bQuery,
    const int32_t *_pParam,
    int32_t _nParamlength,
    int32_t *_pTicket,
    Rsp_h *_phRsp)
{
    Cmd_h hCmd;
    NewCommand(_nCmdID, _bQuery, &hCmd);                 // TODO: error checking
    AddCommandIntArrayArg(hCmd, _pParam, _nParamlength); // TODO: error checking
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, _phRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);
    return (nRes);
}

/**
 * \ingroup DeviceCmd
 * \brief Execute a device command with one float parameter
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nCmdID Command ID.
 * \param[in] _bQuery Whether command is query or not.
 * \param[in] _nParam The float parameter to the device command.
 * \param[out] _pTicket Pointer to a variable receiving the internal ticket for the issued device command.
 * \param[out] _phRsp Pointer to a buffer receiving the response handle.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t ExecSingleFloatParamCmd(
    Conn_h _hConnection,
    uint32_t _nCmdID,
    int32_t _bQuery,
    float _nParam,
    int32_t *_pTicket,
    Rsp_h *_phRsp)
{
    Cmd_h hCmd;
    NewCommand(_nCmdID, _bQuery, &hCmd);
    AddCommandFloatArg(hCmd, _nParam);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, _phRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);
    return (nRes);
}

/**
 * \ingroup DeviceRsp
 * \brief Extract a single float parameter from the response to a device command
 *
 * \param[in] _hRsp Response handle, either returned by ExecCommand() or passed through response and update callback function.
 * \param[in] _nCmdID Command ID.
 * \param[in] _nIdx Parameter index.
 * \param[out] _pParam Pointer to a float variable receiving the parameter value.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t ReadSingleFloatParamRsp(Rsp_h _hRsp, uint32_t _nCmdID, uint32_t _nIdx, float *_pParam)
{
    Res_t nRes = CheckCmdID(_hRsp, _nCmdID);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, _nIdx, _pParam);
    return (nRes);
}

/**
 * \ingroup DeviceCmd
 * \brief Execute a device command with one float array parameter
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nCmdID Command ID.
 * \param[in] _bQuery Whether command is query or not.
 * \param[in] _pParam The float array parameter to the device command.
 * \param[in] _nParamlength The length of the float array in elements.
 * \param[out] _pTicket Pointer to a variable receiving the internal ticket for the issued device command.
 * \param[out] _phRsp Pointer to a buffer receiving the response handle.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t ExecSingleFloatArrayParamCmd(
    Conn_h _hConnection,
    uint32_t _nCmdID,
    int32_t _bQuery,
    const float *_pParam,
    int32_t _nParamlength,
    int32_t *_pTicket,
    Rsp_h *_phRsp)
{
    Cmd_h hCmd;
    NewCommand(_nCmdID, _bQuery, &hCmd);
    AddCommandFloatArrayArg(hCmd, _pParam, _nParamlength);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, _phRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);
    return (nRes);
}

/**
 * \ingroup DeviceRsp
 * \brief Extract a single float array parameter from the response to a device command
 *
 * \param[in] _hRsp Response handle, either returned by ExecCommand() or passed through response and update callback function.
 * \param[in] _nCmdID Command ID.
 * \param[in] _nIdx Parameter index.
 * \param[out] _pParam Pointer to a variable receiving the pointer to the float array data.
 * \param[out] _pnLength Pointer to an integer variable receiving the array length in number of elements.
 *
 * \return Standard API result code. Use the CHR_SUCCESS macro to check for success.
 */
inline Res_t
ReadSingleFloatArrayParamRsp(Rsp_h _hRsp, uint32_t _nCmdID, uint32_t _nIdx, const float **_pParam, int32_t *_pnLength)
{
    Res_t nRes = CheckCmdID(_hRsp, _nCmdID);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArrayArg(_hRsp, _nIdx, _pParam, _pnLength);
    return (nRes);
}

/**
 * \brief Order signal data from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _anSignals An array of integer numbers indicating the signals to be included in the data stream.
 * \param[in] _nSignalsCount Number of signals (array length).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetConnectionOutputSignals(
    Conn_h _hConnection, const int32_t *_anSignals, int32_t _nSignalsCount, int32_t *_pTicket)
{
    return (ExecSingleIntArrayParamCmd(
        _hConnection, CmdID_Output_Signals, false, _anSignals, _nSignalsCount, _pTicket, nullptr));
}

/**
 * \brief Read in response of connection output signal command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pSignalIDbuffer A pointer to the array where the signal IDs are stored.
 * \param[out] _pSignalCount A pointer to an integer which stores the number of signals output for current connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadConnectionOutputSignalRsp(Rsp_h _hRsp, const int32_t **_pSignalIDbuffer, int32_t *_pSignalCount)
{
    return (ReadSingleIntArrayParamRsp(_hRsp, CmdID_Output_Signals, 0, _pSignalIDbuffer, _pSignalCount));
}

/**
 * \brief Get IDs of output signal data for the current connection from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pSignalIDbuffer  A pointer to the array where the signal IDs are stored, only used for synchronous connection.
 * \param[out] _pSignalCount A pointer to an integer which stores the number of signals, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetConnectionOutputSignals(
    Conn_h _hConnection, const int32_t **_pSignalIDbuffer, int32_t *_pSignalCount, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Output_Signals, true, _pTicket, &hRsp);
    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadConnectionOutputSignalRsp(hRsp, _pSignalIDbuffer, _pSignalCount);
    return (nRes);
}

/**
 * \brief Read in response of device firmware query command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pstrVersion Firmware version (a zero-terminated string) contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */

inline Res_t ReadDeviceFirmwareVersionRsp(Rsp_h _hRsp, const char **_pstrVersion)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Firmware_Version);
    if (CHR_SUCCESS(nRes))
    {
        int32_t nLength;
        nRes = GetResponseStringArg(_hRsp, 0, _pstrVersion, &nLength);
    }
    return (nRes);
}

/**
 * \brief Query device firmware version from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pstrVersion Firmware version (a zero-terminated string) returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetDeviceFirmwareVersion(Conn_h _hConnection, const char **_pstrVersion, int32_t *_pTicket)
{
    Rsp_h hRsp;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Firmware_Version, false, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadDeviceFirmwareVersionRsp(hRsp, _pstrVersion);

    return nRes;
}

/**
 * \brief Set device measuring method (confocal or interferometric).
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nMethod Measuring method: 0 - chromatic confocal 1 - interferometric.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetMeasuringMethod(Conn_h _hConnection, int32_t _nMethod, int32_t *_pTicket)
{
    return (ExecSingleIntParamCmd(_hConnection, CmdID_Measuring_Method, false, _nMethod, _pTicket, nullptr));
}

/**
 * \brief Read in response of measuring method command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnMethod Measuring method contained in the response:  0 - 0 - chromatic confocal 1 - interferometric.
 *
 * \return Zero on success. Negative: error or warning code.
 */

inline Res_t ReadMeasuringMethodRsp(Rsp_h _hRsp, int32_t *_pnMethod)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Measuring_Method, 0, _pnMethod));
}

/**
 * \brief Query device measuring method from CHR device.E
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnMethod Measuring method returned from device:  0 - 0 - chromatic confocal 1 - interferometric, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetMeasuringMethod(Conn_h _hConnection, int32_t *_pnMethod, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Measuring_Method, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadMeasuringMethodRsp(hRsp, _pnMethod);

    return nRes;
}

/**
 * \brief Read in response of measuring scale command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnFullScale Measuring scale contained in response.
 *
 * \return Zero on success. Negative: error or warning code.
 */

inline Res_t ReadFullScaleRsp(Rsp_h _hRsp, int32_t *_pnFullScale)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Full_Scale, 0, _pnFullScale));
}

/**
 * \brief Query device measuring scale.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnFullScale Measuring scale returned from device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetFullScale(Conn_h _hConnection, int32_t *_pnFullScale, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Full_Scale, false, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadFullScaleRsp(hRsp, _pnFullScale);

    return nRes;
}

/**
 * \brief Set device scan rate.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection()
 * \param[in] _nScanRate Scan rate in Hz
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetScanRate(Conn_h _hConnection, float _nScanRate, int32_t *_pTicket)
{
    return (ExecSingleFloatParamCmd(_hConnection, CmdID_Scan_Rate, false, _nScanRate, _pTicket, nullptr));
}

/**
 * \brief Read in response of scan rate command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnScanRate Scan rate in Hz contained in response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadScanRateRsp(Rsp_h _hRsp, float *_pnScanRate)
{
    return (ReadSingleFloatParamRsp(_hRsp, CmdID_Scan_Rate, 0, _pnScanRate));
}

/**
 * \brief Query scan rate from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnScanRate Scan rate in Hz returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetScanRate(Conn_h _hConnection, float *_pnScanRate, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Scan_Rate, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadScanRateRsp(hRsp, _pnScanRate);

    return nRes;
}

/**
 * \brief Set data average.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nAVD Data average (from 1 to 999).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetDataAverage(Conn_h _hConnection, int32_t _nAVD, int32_t *_pTicket)
{
    return (ExecSingleIntParamCmd(_hConnection, CmdID_Data_Average, false, _nAVD, _pTicket, nullptr));
}

/**
 * \brief Read in response of data average command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnAVD Data average contained in response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadDataAverageRsp(Rsp_h _hRsp, int32_t *_pnAVD)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Data_Average, 0, _pnAVD));
}

/**
 * \brief Query data average.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnAVD Data average returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetDataAverage(Conn_h _hConnection, int32_t *_pnAVD, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Data_Average, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadDataAverageRsp(hRsp, _pnAVD);

    return nRes;
}

/**
 * \brief Set spectrum average.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nAVS Spectrum average (from 1 to 999).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetSpectrumAverage(Conn_h _hConnection, int32_t _nAVS, int32_t *_pTicket)
{
    return (ExecSingleIntParamCmd(_hConnection, CmdID_Spectrum_Average, false, _nAVS, _pTicket, nullptr));
}

/**
 * \brief Read in response of spectrum average command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnAVS Spectrum average contained in response.
 *
 * \return Zero on success. Negative: error or warning code.
 */

inline Res_t ReadSpectrumAverageRsp(Rsp_h _hRsp, int32_t *_pnAVS)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Spectrum_Average, 0, _pnAVS));
}

/**
 * \brief Query spectrum average from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnAVS Spectrum average returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetSpectrumAverage(Conn_h _hConnection, int32_t *_pnAVS, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Spectrum_Average, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadSpectrumAverageRsp(hRsp, _pnAVS);

    return nRes;
}

/**
 * \brief Set refractive index.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _paRefIdx Array containing refractive indices.
 * \param[in] _nLayerCount Layer count of being measured material (i.e. number of refractive indices to be set).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t
SetRefractiveIndices(Conn_h _hConnection, const float *_paRefIdx, int32_t _nLayerCount, int32_t *_pTicket)
{
    return (ExecSingleFloatArrayParamCmd(
        _hConnection, CmdID_Refractive_Indices, false, _paRefIdx, _nLayerCount, _pTicket, nullptr));
}

/**
 * \brief Read in response of refractive index command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _paRefIdx Array of refractive indices contained in the response.
 * \param[out] _pnLayerCount Layer count of being measured material (i.e. number of refractive indices) contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadRefractiveIndicesRsp(Rsp_h _hRsp, const float **_paRefIdx, int32_t *_pnLayerCount)
{
    return (ReadSingleFloatArrayParamRsp(_hRsp, CmdID_Refractive_Indices, 0, _paRefIdx, _pnLayerCount));
}

/**
 * \brief Query refractive index from CHR device
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _paRefIdx Array containing refractive indices returned by device, only used for synchronous connection.
 * \param[out] _pnLayerCount Layer count of being measured material (i.e. number of refractive indices), only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetRefractiveIndices(
    Conn_h _hConnection, const float **_paRefIdx, int32_t *_pnLayerCount, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Refractive_Indices, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadRefractiveIndicesRsp(hRsp, _paRefIdx, _pnLayerCount);

    return nRes;
}

/**
 * \brief Set Abbe numbers.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _paAbbe Array containing Abbe numbers (from 0 to 500).
 * \param[in] _nLayerCount Layer count of being measured material (i.e. number of Abbe numbers to be set).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetAbbeNumbers(Conn_h _hConnection, const float *_paAbbe, int32_t _nLayerCount, int32_t *_pTicket)
{
    return (ExecSingleFloatArrayParamCmd(
        _hConnection, CmdID_Abbe_Numbers, false, _paAbbe, _nLayerCount, _pTicket, nullptr));
}

/**
 * \brief Read in response of abbe number command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _paAbbe Array of Abbe numbers contained in the response.
 * \param[out] _pnLayerCount Layer count of being measured material (i.e. number of Abbe numbers) contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadAbbeNumbersRsp(Rsp_h _hRsp, const float **_paAbbe, int32_t *_pnLayerCount)
{
    return (ReadSingleFloatArrayParamRsp(_hRsp, CmdID_Abbe_Numbers, 0, _paAbbe, _pnLayerCount));
}

/**
 * \brief Query Abbe numbers from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _paAbbe Array containing Abbe numbers returned by device, only used for synchronous connection.
 * \param[out] _pnLayerCount Layer count of being measured material (i.e. number of Abbe numbers), only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
GetAbbeNumbers(Conn_h _hConnection, const float **_paAbbe, int32_t *_pnLayerCount, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Abbe_Numbers, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadAbbeNumbersRsp(hRsp, _paAbbe, _pnLayerCount);

    return nRes;
}

/**
 * \brief Select refractive index tables to use.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _paTable Array containing indices of refractive index table (from 0 to 16). 0: no special table used
 * \param[in] _nLayerCount Layer count of being measured materials (i.e. number of indices of refractive index table).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetRefractiveIndexTables(
    Conn_h _hConnection, const int32_t *_paTable, int32_t _nLayerCount, int32_t *_pTicket)
{
    return (ExecSingleIntArrayParamCmd(
        _hConnection, CmdID_Refractive_Index_Tables, false, _paTable, _nLayerCount, _pTicket, nullptr));
}

/**
 * \brief Read in response of refractive index table command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _paTable Array of indices of currently used Refractive index table contained in the response.
 * \param[out] _pnLayerCount Layer count of being measured material (i.e. number of Abbe numbers) contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadRefractiveIndexTablesRsp(Rsp_h _hRsp, const int32_t **_paTable, int32_t *_pnLayerCount)
{
    return (ReadSingleIntArrayParamRsp(_hRsp, CmdID_Refractive_Index_Tables, 0, _paTable, _pnLayerCount));
}

/**
 * \brief Query currently used refractive index tables from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _paTable Array containing indices of currently used Refractive index table returned by device, only used for synchronous connection.
 * \param[out] _pnLayerCount Layer count of being measured materials (i.e. number of indices of refractive index table), only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetRefractiveIndexTables(
    Conn_h _hConnection, const int32_t **_paTable, int32_t *_pnLayerCount, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Refractive_Index_Tables, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadRefractiveIndexTablesRsp(hRsp, _paTable, _pnLayerCount);

    return nRes;
}

/**
 * \brief Set lamp intensity.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nIntensity Lamp intensity in percent.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetLampIntensity(Conn_h _hConnection, float _nIntensity, int32_t *_pTicket)
{
    return (
        ExecSingleFloatParamCmd(_hConnection, CmdID_Lamp_Intensity, false, _nIntensity, _pTicket, nullptr));
}

/**
 * \brief Read in response of lamp intensity command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnIntensity Lamp intensity in percent contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadLampIntensityRsp(Rsp_h _hRsp, float *_pnIntensity)
{
    return (ReadSingleFloatParamRsp(_hRsp, CmdID_Lamp_Intensity, 0, _pnIntensity));
}

/**
 * \brief Query lamp intensity from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection()
 * \param[out] _pnIntensity Lamp intensity in percent returned by device, only used for synchronous connection.
  * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetLampIntensity(Conn_h _hConnection, float *_pnIntensity, Res_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Lamp_Intensity, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadLampIntensityRsp(hRsp, _pnIntensity);

    return nRes;
}

/**
 * \brief Set device light source "Auto Adapt" mode related settings.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _bAuto "Auto Adapt" mode on/off: 1 - on; 0 - off.
 * \param[in] _nLevel CCD exposure level in percent (from 0 to 100), only if "Auto Adapt" mode is on.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetLightSourceAutoAdapt(Conn_h _hConnection, int32_t _bAuto, float _nLevel, int32_t *_pTicket)
{
    Cmd_h hCmd;
    NewCommand(CmdID_Light_Source_Auto_Adapt, false, &hCmd);
    AddCommandIntArg(hCmd, _bAuto);
    AddCommandFloatArg(hCmd, _nLevel);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, nullptr);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);
    return (nRes);
}

/**
 * \brief Read in response of light source "Auto Adapt" command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pbAuto "Auto Adapt" mode contained in the response.
 * \param[out] _pnLevel CCD exposure level in percent in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadLightSourceAutoAdaptRsp(Rsp_h _hRsp, int32_t *_pbAuto, float *_pnLevel)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Light_Source_Auto_Adapt);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pbAuto);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, 1, _pnLevel);
    return (nRes);
}

/**
 * \brief Query device light source "Auto Adapt" mode settings from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pbAuto "Auto Adapt" mode returned by device, only used for synchronous connection.
 * \param[out] _pnLevel CCD exposure level in percent returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
GetLightSourceAutoAdapt(Conn_h _hConnection, int32_t *_pbAuto, float *_pnLevel, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Light_Source_Auto_Adapt, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadLightSourceAutoAdaptRsp(hRsp, _pbAuto, _pnLevel);

    return nRes;
}

/**
 * \brief Select optical probe in confocal measuring mode.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nIndex Optical probe index (from 0 to 15).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetOpticalProbe(Conn_h _hConnection, int32_t _nIndex, int32_t *_pTicket)
{
    return (ExecSingleIntParamCmd(_hConnection, CmdID_Optical_Probe, false, _nIndex, _pTicket, nullptr));
}

/**
 * \brief Read in response of optical probe selection command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnIndex Optical probe index contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadOpticalProbeRsp(Rsp_h _hRsp, int32_t *_pnIndex)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Optical_Probe, 0, _pnIndex));
}

/**
 * \brief Query currently used optical probe in confocal measuring mode from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnIndex Optical probe index returned by device, only used for synchronous connection.
  * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetOpticalProbe(Conn_h _hConnection, int32_t *_pnIndex, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Optical_Probe, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadOpticalProbeRsp(hRsp, _pnIndex);

    return nRes;
}

/**
 * \brief Set peak detection threshold for confocal mode.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nThreshold Detection threshold (depends on the sensor model).
  * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetConfocalDetectionThreshold(Conn_h _hConnection, float _nThreshold, int32_t *_pTicket)
{
    return (ExecSingleFloatParamCmd(
        _hConnection, CmdID_Confocal_Detection_Threshold, false, _nThreshold, _pTicket, nullptr));
}

/**
 * \brief Read in response of peak detection threshold command for confocal mode.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnThreshold Detection threshold contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadConfocalDetectionThresholdRsp(Rsp_h _hRsp, float *_pnThreshold)
{
    return (ReadSingleFloatParamRsp(_hRsp, CmdID_Confocal_Detection_Threshold, 0, _pnThreshold));
}

/**
 * \brief Query peak detection threshold for confocal mode from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnThreshold Detection threshold used by device, only used for synchronous connection.
  * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
GetConfocalDetectionThreshold(Conn_h _hConnection, float *_pnThreshold, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Confocal_Detection_Threshold, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadConfocalDetectionThresholdRsp(hRsp, _pnThreshold);

    return nRes;
}

/**
 * \brief Set peak detection threshold for the quality of a signal in the interferometric mode.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nThreshold Quality threshold (from 1 to 999).
  * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetInterQualityThreshold(Conn_h _hConnection, float _nThreshold, int32_t *_pTicket)
{
    return (ExecSingleFloatParamCmd(
        _hConnection, CmdID_Interferometric_Quality_Threshold, false, _nThreshold, _pTicket, nullptr));
}

/**
 * \brief Read in response of peak detection qualtity threshold command for interferometric mode.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnThreshold Quality threshold contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadInterQualityThresholdRsp(Rsp_h _hRsp, float *_pnThreshold)
{
    return (ReadSingleFloatParamRsp(_hRsp, CmdID_Interferometric_Quality_Threshold, 0, _pnThreshold));
}

/**
 * \brief Query peak detection threshold for the quality of a signal in the interferometric mode from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnThreshold Quality threshold used by device, only used for synchronous connection.
  * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetInterQualityThreshold(Conn_h _hConnection, float *_pnThreshold, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Interferometric_Quality_Threshold, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadInterQualityThresholdRsp(hRsp, _pnThreshold);

    return nRes;
}

/**
 * \brief Set duty cycle.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nDutyCycle Duty cyle (0..49, 100).
  * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetDutyCycle(Conn_h _hConnection, float _nDutyCycle, int32_t *_pTicket)
{
    return (ExecSingleFloatParamCmd(_hConnection, CmdID_Duty_Cycle, false, _nDutyCycle, _pTicket, nullptr));
}

/**
 * \brief Read in response of duty cycle command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnDutyCycle Duty cycle contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadDutyCycleRsp(Rsp_h _hRsp, float *_pnDutyCycle)
{
    return (ReadSingleFloatParamRsp(_hRsp, CmdID_Duty_Cycle, 0, _pnDutyCycle));
}

/**
 * \brief Query duty cycle from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnDutyCycle Current duty cycle returned by the device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetDutyCycle(Conn_h _hConnection, float *_pnDutyCycle, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Duty_Cycle, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadDutyCycleRsp(hRsp, _pnDutyCycle);

    return nRes;
}

/**
 * \brief Enable or disable limits/detection window for peak detection under confocal (\\CHRtwoC~device) and
 * interferometric mode ({\\CHRtwo} and {\\CHRone}).
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _bActive 1: limit active; 0: limit inactive.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetDetectionWindowActive(Conn_h _hConnection, int32_t _bActive, int32_t *_pTicket)
{
    return (ExecSingleIntParamCmd(
        _hConnection, CmdID_Detection_Window_Active, false, _bActive, _pTicket, nullptr));
}

/**
 * \brief Read in response of detection window active command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pbActive Active state contained in the response: 1 - active; 0 - inactive.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadDetectionWindowActiveRsp(Rsp_h _hRsp, int32_t *_pbActive)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Detection_Window_Active, 0, _pbActive));
}

/**
 * \brief Query detection window active state from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pbActive Active state returned by device: 1 - active; 0 - inactive, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetDetectionWindowActive(Conn_h _hConnection, int32_t *_pbActive, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Detection_Window_Active, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadDetectionWindowActiveRsp(hRsp, _pbActive);

    return nRes;
}

/**
 * \brief Set windows where peaks can be detected.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _anDWs A float array containing window definitions: window 1 left limit, window 1 right limit, window 2
 * left limit, window 2 right limit... \param[in] _nWindowCount Number of windows (the size of detection window array
 * should be twice the number of windows!).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetDetectionWindow(Conn_h _hConnection, const float *_anDWs, int32_t _nWindowCount, int32_t *_pTicket)
{
    return (ExecSingleFloatArrayParamCmd(
        _hConnection, CmdID_Detection_Window, false, _anDWs, 2 * _nWindowCount, _pTicket, nullptr));
}

/**
 * \brief Read in response of detection window command.
 *
 * \param[in] _hRsp Response handle, either returned by ExecCommand() or passed through response and update callback function.
 * \param[out] _panDWs A float array containing window definitions: window 1 left limit, window 1 right limit, window 2
 * left limit, window 2 right limit..., contained in the response. 
 * \param[out] _pnWindowCount Number of windows (the size of detection window array should be twice the number of windows), contained in the response.
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadDetectionWindowRsp(Rsp_h _hRsp, const float **_panDWs, int32_t *_pnWindowCount)
{
    Res_t nRes = ReadSingleFloatArrayParamRsp(_hRsp, CmdID_Detection_Window, 0, _panDWs, _pnWindowCount);
    *_pnWindowCount = *_pnWindowCount / 2;
    return (nRes);
}

/**
 * \brief Query windows where peaks can be detected from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _panDWs A float array containing window definitions: window 1 left limit, window 1 right limit, window 2
 * left limit, window 2 right limit..., only used for synchronous connection. 
 * \param[out] _pnWindowCount Number of windows (the size of detection window array should be twice the number of windows!), only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetDetectionWindow(
    Conn_h _hConnection, const float **_panDWs, int32_t *_pnWindowCount, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Detection_Window, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadDetectionWindowRsp(hRsp, _panDWs, _pnWindowCount);

    return nRes;
}

/**
 * \brief Set CCD range.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nStartPixel Start pixel for the CCD range.
 * \param[in] _nStopPixel Stop pixel for the CCD range.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetCCDRange(Conn_h _hConnection, int32_t _nStartPixel, int32_t _nStopPixel, int32_t *_pTicket)
{
    Cmd_h hCmd;
    NewCommand(CmdID_CCD_Range, false, &hCmd);
    AddCommandIntArg(hCmd, _nStartPixel);
    AddCommandIntArg(hCmd, _nStopPixel);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, nullptr);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    return (nRes);
}

/**
 * \brief Read in response of CCD range command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnStartPixel Start pixel for the CCD range contained in the response.
 * \param[out] _pnStopPixel Start pixel for the CCD range in percent contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadCCDRangeRsp(Rsp_h _hRsp, int32_t *_pnStartPixel, int32_t *_pnStopPixel)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_CCD_Range);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnStartPixel);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 1, _pnStopPixel);
    return (nRes);
}

/**
 * \brief Query CCD range from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnStartPixel Start pixel for the CCD range returned by device, only used for synchronous connection.
 * \param[out] _pnStopPixel Stop pixel for the CCD range returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
GetCCDRange(Conn_h _hConnection, int32_t *_pnStartPixel, int32_t *_pnStopPixel, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_CCD_Range, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadCCDRangeRsp(hRsp, _pnStartPixel, _pnStopPixel);

    return nRes;
}

/**
 * \brief Set number of the peaks to be detected.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nPeakNum Number of peaks (from 1 to 16).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetNumberOfPeaks(Conn_h _hConnection, int32_t _nPeakNum, int32_t *_pTicket)
{
    return (ExecSingleIntParamCmd(_hConnection, CmdID_Number_Of_Peaks, false, _nPeakNum, _pTicket, nullptr));
}

/**
 * \brief Read in response of peak number command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnPeakNum Number of peaks contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t ReadNumberOfPeaksRsp(Rsp_h _hRsp, int32_t *_pnPeakNum)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Number_Of_Peaks, 0, _pnPeakNum));
}

/**
 * \brief Query number of the peaks to be detected from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnPeakNum Number of peaks returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetNumberOfPeaks(Conn_h _hConnection, int32_t *_pnPeakNum, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Number_Of_Peaks, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadNumberOfPeaksRsp(hRsp, _pnPeakNum);

    return nRes;
}

// valid for \\CHRgentwo~devices

/**
 * \brief Set the sequence to order detected peaks in interferometric mode.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nPeakOrdering Ordering sequence: 0 - according to the quality, peak with highest quality comes first; 1 -
 * position ascending, nearest peak comes first; 2 - position descending, furthest peak comes first.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetPeakOrdering(Conn_h _hConnection, int32_t _nPeakOrdering, int32_t *_pTicket)
{
    return (
        ExecSingleIntParamCmd(_hConnection, CmdID_Peak_Ordering, false, _nPeakOrdering, _pTicket, nullptr));
}

/**
 * \brief Read in response of peak ordering sequence command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnPeakOrdering Peak ordering sequence contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadPeakOrderingRsp(Rsp_h _hRsp, int32_t *_pnPeakOrdering)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Peak_Ordering, 0, _pnPeakOrdering));
}

/**
 * \brief Query the sequence to order detected peaks from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pnPeakOrdering Peak ordering sequence returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetPeakOrdering(Conn_h _hConnection, int32_t *_pnPeakOrdering, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Peak_Ordering, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadPeakOrderingRsp(hRsp, _pnPeakOrdering);

    return nRes;
}

// valid for \\CHRgentwo~devices
/**
 * \brief Set median width and percentile used for calculating sample median.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection()
 * \param[in] _nMedianWidth Median width (from 0 to 255)
 * \param[in] _nPercentile Percentile
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetMedian(Conn_h _hConnection, int32_t _nMedianWidth, float _nPercentile, int32_t *_pTicket)
{
    Cmd_h hCmd;
    NewCommand(CmdID_Median, false, &hCmd);
    AddCommandIntArg(hCmd, _nMedianWidth);
    AddCommandFloatArg(hCmd, _nPercentile);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, nullptr);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    return (nRes);
}

/**
 * \brief Read in response of median command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnMedianWidth Median width contained in the response.
 * \param[out] _pnPercentile Percentile contained in the response.
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadMedianRsp(Rsp_h _hRsp, int32_t *_pnMedianWidth, float *_pnPercentile)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Median);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnMedianWidth);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, 1, _pnPercentile);
    return (nRes);
}

/**
 * \brief Query median width and percentile used for calculating sample median from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection()
 * \param[out] _pnMedianWidth Median width returned by device, only used for synchronous connection.
 * \param[out] _pnPercentile Percentile returned by device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
GetMedian(Conn_h _hConnection, int32_t *_pnMedianWidth, float *_pnPercentile, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Median, true, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadMedianRsp(hRsp, _pnMedianWidth, _pnPercentile);

    return nRes;
}

/**
 * \brief Send command to save current setting in device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SaveCurrentSettingInDevice(Conn_h _hConnection, int32_t *_pTicket)
{
    return (ExecNoParamCmd(_hConnection, CmdID_Save_Settings, false, _pTicket, nullptr));
}

/**
 * \brief Read in response of dark reference command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pMinFrequency (Virtual) scan rate in Hertz at which the CCD would be saturated by the stray light (contained in the response).
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadDarkReferenceRsp(Rsp_h _hRsp, float *_pMinFrequency)
{
    return (ReadSingleFloatParamRsp(_hRsp, CmdID_Dark_Reference, 0, _pMinFrequency));
}

/**
 * \brief Take dark reference, wait for the response and return lowest scan rate.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pMinFrequency (Virtual) scan rate in Hertz at which the CCD would be saturated by the stray light, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t DarkReference(Conn_h _hConnection, float *_pMinFrequency, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Dark_Reference, false, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)) && (_pMinFrequency))
        nRes = ReadDarkReferenceRsp(hRsp, _pMinFrequency);

    return nRes;
}

/**
 * \brief Start data stream from device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t StartDataStream(Conn_h _hConnection, int32_t *_pTicket)
{
    return (ExecNoParamCmd(_hConnection, CmdID_Start_Data_Stream, false, _pTicket, nullptr));
}

/**
 * \brief Stop data stream from device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection()
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t StopDataStream(Conn_h _hConnection, int32_t *_pTicket)
{
    return (ExecNoParamCmd(_hConnection, CmdID_Stop_Data_Stream, false, _pTicket, nullptr));
}

/**
 * \brief Set analog output configuration.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nIndex Analog output index (0 for channel 1; 1 for channel 2).
 * \param[in] _nSignalID ID of the signal to be sent through selected analog output.
 * \param[in] _nMin Signal lower limit value, corresponds to minimum output voltage.
 * \param[in] _nMax Signal upper limit value, corresponds to maximum output voltage.
 * \param[in] _nVolMin Minimum output voltage.
 * \param[in] _nVolMax Maximum output voltage.
 * \param[in] _nVolInvalid Voltage corresponding to invalid measuring result.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetAnalogOutput(
    Conn_h _hConnection,
    int32_t _nIndex,
    int32_t _nSignalID,
    float _nMin,
    float _nMax,
    float _nVolMin,
    float _nVolMax,
    float _nVolInvalid,
    int32_t *_pTicket)
{
    Cmd_h hCmd;
    NewCommand(CmdID_Analog_Output, false, &hCmd);
    AddCommandIntArg(hCmd, _nIndex);
    AddCommandIntArg(hCmd, _nSignalID);
    AddCommandFloatArg(hCmd, _nMin);
    AddCommandFloatArg(hCmd, _nMax);
    AddCommandFloatArg(hCmd, _nVolMin);
    AddCommandFloatArg(hCmd, _nVolMax);
    AddCommandFloatArg(hCmd, _nVolInvalid);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, nullptr);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    return (nRes);
}

/**
 * \brief Read in response of analog output configuration command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnIndex Analog output index (0 for channel 1; 1 for channel 2) contained in the response.
 * \param[out] _pnSignalID ID of the signal sent through selected analog output, contained in the response.
 * \param[out] _pnMin Signal lower limit value, corresponds to minimum output voltage, contained in the response.
 * \param[out] _pnMax Signal upper limit value, corresponds to maximum output voltage, contained in the response.
 * \param[out] _pnVolMin Minimum output voltage contained in the response.
 * \param[out] _pnVolMax Maximum output voltage contained in the response.
 * \param[out] _pnVolInvalid Voltage corresponding to invalid measuring result contained in the response.
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadAnalogOutputRsp(
    Rsp_h _hRsp,
    int32_t *_pnIndex,
    int32_t *_pnSignalID,
    float *_pnMin,
    float *_pnMax,
    float *_pnVolMin,
    float *_pnVolMax,
    float *_pnVolInvalid)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Analog_Output);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnIndex);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 1, _pnSignalID);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, 2, _pnMin);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, 3, _pnMax);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, 4, _pnVolMin);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, 5, _pnVolMax);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, 6, _pnVolInvalid);
    return (nRes);
}

/**
 * \brief Query analog output configuration from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nIndex Analog output index (0 for channel 1; 1 for channel 2).
 * \param[out] _pnSignalID ID of the signal sent through selected analog output, only used for synchronous connection.
 * \param[out] _pnMin Signal lower limit value, corresponds to minimum output voltage, only used for synchronous connection.
 * \param[out] _pnMax Signal upper limit value, corresponds to maximum output voltage, only used for synchronous connection.
 * \param[out] _pnVolMin Minimum output voltage, only used for synchronous connection.
 * \param[out] _pnVolMax Maximum output voltage, only used for synchronous connection.
 * \param[out] _pnVolInvalid Voltage corresponding to invalid measuring result, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetAnalogOutput(
    Conn_h _hConnection,
    int32_t _nIndex,
    int32_t *_pnSignalID,
    float *_pnMin,
    float *_pnMax,
    float *_pnVolMin,
    float *_pnVolMax,
    float *_pnVolInvalid,
    int32_t *_pTicket)
{
    Rsp_h hRsp;
    Res_t nRes = ExecSingleIntParamCmd(_hConnection, CmdID_Analog_Output, true, _nIndex, _pTicket, &hRsp);
    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
    {
        int32_t nTemp;
        nRes = ReadAnalogOutputRsp(hRsp, &nTemp, _pnSignalID, _pnMin, _pnMax, _pnVolMin, _pnVolMax, _pnVolInvalid);
    }
    return (nRes);
}

// encoder counter related function

/**
 * \brief Set the encoder position.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nAxis The index of encoder axis to be set: for {\\CHRone} - 0..2; for {\\CHRtwoCLSMPS} - 0..4.
 * \param[in] _nPosition Encoder position to be set.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetEncoderCounter(Conn_h _hConnection, int32_t _nAxis, int32_t _nPosition, int32_t *_pTicket)
{
    Cmd_h hCmd;
    NewCommand(CmdID_Encoder_Counter, false, &hCmd);
    AddCommandIntArg(hCmd, _nAxis);
    AddCommandIntArg(hCmd, _nPosition);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, nullptr);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);
    return (nRes);
}

/**
 * \brief Read in response of encoder counter command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnAxis The index of encoder axis contained in the response.
 * \param[out] _pnPosition Encoder position contained in the response.
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadEncoderCounterRsp(Rsp_h _hRsp, int32_t *_pnAxis, int32_t *_pnPosition)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Encoder_Counter);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnAxis);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 1, _pnPosition);
    return (nRes);
}

/**
 * \brief Query device current encoder position.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nAxis The index of encoder axis to be queried: for {\\CHRone} - 0..2; for {\\CHRtwoCLSMPS} - 0..4.
 * \param[out] _pnPosition Encoder position returned from the device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
GetEncoderCounter(Conn_h _hConnection, int32_t _nAxis, int32_t *_pnPosition, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecSingleIntParamCmd(_hConnection, CmdID_Encoder_Counter, true, _nAxis, _pTicket, &hRsp);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
    {
        int32_t nAxis;
        nRes = ReadEncoderCounterRsp(hRsp, &nAxis, _pnPosition);
    }

    return nRes;
}

//not valid for CHR 1 device
/**
 * \brief Set the encoder counter counting source.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nAxis The index of encoder axis to be set: for {\\CHRone} - 0..2; for {\\CHRtwoCLSMPS} - 0..4.
 * \param[in] _nSource Encoder counting source to be set (see \\ref{sec:EncoderCounterTriggerConst}).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
SetEncoderCounterSource(Conn_h _hConnection, int32_t _nAxis, int32_t _nSource, int32_t *_pTicket)
{
    Cmd_h hCmd;
    NewCommand(CmdID_Encoder_Counter_Source, false, &hCmd);
    AddCommandIntArg(hCmd, _nAxis);
    AddCommandIntArg(hCmd, _nSource);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, nullptr);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    return (nRes);
}

/**
 * \brief Read in response of encoder counter source command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnAxis The index of encoder axis contained in the response.
 * \param[out] _pnSource Encoder counting source contained in the response.
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadEncoderCounterSourceRsp(Rsp_h _hRsp, int32_t *_pnAxis, int32_t *_pnSource)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Encoder_Counter_Source);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnAxis);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 1, _pnSource);
    return (nRes);
}

/**
 * \brief Query current encoder counter counting source from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nAxis The index of encoder axis to be queried: for {\\CHRone} - 0..2; for {\\CHRtwoCLSMPS} - 0..4.
 * \param[out] _pnSource Encoder counting source returned from device. (see \\ref{sec:EncoderCounterTriggerConst}), only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t
GetEncoderCounterSource(Conn_h _hConnection, int32_t _nAxis, int32_t *_pnSource, int32_t *_pTicket)
{
    Rsp_h hRsp;
    Res_t nRes =
        ExecSingleIntParamCmd(_hConnection, CmdID_Encoder_Counter_Source, true, _nAxis, _pTicket, &hRsp);
    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
    {
        int32_t nAxis;
        nRes = ReadEncoderCounterSourceRsp(hRsp, &nAxis, _pnSource);
    }
    return (nRes);
}

/**
 * \brief Set the encoder preload function.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nAxis Encoder axis index to be set: for {\\CHRone} - 0..2; for {\\CHRtwoCLSMPS} - 0..4.
 * \param[in] _nPreloadValue Encoder position to be preloaded.
 * \param[in] _nEncoderPreloadConfig Preload function settings (see \\ref{sec:EncoderPreloadConst}).
 * \param[in] _nEncoderTriggerSource Trigger source of the preload function (see \\ref{sec:EncoderCounterTriggerConst}).
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetEncoderPreloadFunction(
    Conn_h _hConnection,
    int32_t _nAxis,
    int32_t _nPreloadValue,
    int32_t _nEncoderPreloadConfig,
    int32_t _nEncoderTriggerSource,
    int32_t *_pTicket)
{
    Cmd_h hCmd;
    NewCommand(CmdID_Encoder_Preload_Function, false, &hCmd);
    AddCommandIntArg(hCmd, _nAxis);
    AddCommandIntArg(hCmd, _nPreloadValue);
    AddCommandIntArg(hCmd, _nEncoderPreloadConfig);
    AddCommandIntArg(hCmd, _nEncoderTriggerSource);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, nullptr);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    return (nRes);
}

/**
 * \brief Read in response of encoder preload function command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnAxis The index of encoder axis contained in the response.
 * \param[out] _pnPreloadValue Preload encoder position contained in the response.
 * \param[out] _pnEncoderPreloadConfig Preload function settings contained in the response (see \\ref{sec:EncoderPreloadConst}).
 * \param[out] _pnEncoderTriggerSource Trigger source of the preload function contained in the response (see \\ref{sec:EncoderCounterTriggerConst}).
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadEncoderPreloadFunctionRsp(
    Rsp_h _hRsp,
    int32_t *_pnAxis,
    int32_t *_pnPreloadValue,
    int32_t *_pnEncoderPreloadConfig,
    int32_t *_pnEncoderTriggerSource)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Encoder_Preload_Function);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnAxis);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 1, _pnPreloadValue);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 2, _pnEncoderPreloadConfig);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 3, _pnEncoderTriggerSource);
    return (nRes);
}

/**
 * \brief Enable encoder trigger.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _bEnabled Enable encoder trigger, otherwise device is triggered by synchronization signal.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetEncoderTriggerEnabled(Conn_h _hConnection, int32_t _bEnabled, int32_t *_pTicket)
{
    return (ExecSingleIntParamCmd(
        _hConnection, CmdID_Encoder_Trigger_Enabled, false, _bEnabled, _pTicket, nullptr));
}

/**
 * \brief Read in response of encoder trigger command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pbEnabled Encoder trigger enabled status contained in the response.
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadEncoderTriggerEnabledRsp(Rsp_h _hRsp, int32_t *_pbEnabled)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Encoder_Trigger_Enabled, 0, _pbEnabled));
}

/**
 * \brief Query current encoder trigger status from CHR device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pbEnabled Encoder trigger enabled status returned from device, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetEncoderTriggerEnabled(Conn_h _hConnection, int32_t *_pbEnabled, int32_t *_pTicket)
{
    Rsp_h hRsp = Invalid_Handle;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Encoder_Trigger_Enabled, true, _pTicket, &hRsp);
    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadEncoderTriggerEnabledRsp(hRsp, _pbEnabled);

    return (nRes);
}

/**
 * \brief Set the property for encoder triggering.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nEncoderAxis Encoder axis index: for {\\CHRone} - 0..2; for {\\CHRtwoCLSMPS} - 0..4.
 * \param[in] _nStartPos Start position for encoder triggering.
 * \param[in] _nStopPos Stop position for encoder triggering.
 * \param[in] _nInterval Distance between adjacent triggering points in increments.
 * \param[in] _bTriggerOnReturnMove Whether triggering is active during the return move from stop position to start
 * position.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetEncoderTriggerProperty(
    Conn_h _hConnection,
    int32_t _nEncoderAxis,
    int32_t _nStartPos,
    int32_t _nStopPos,
    float _nInterval,
    int32_t _bTriggerOnReturnMove,
    int32_t *_pTicket)
{
    Cmd_h hCmd;
    NewCommand(CmdID_Encoder_Trigger_Property, false, &hCmd);
    AddCommandIntArg(hCmd, _nEncoderAxis);
    AddCommandIntArg(hCmd, _nStartPos);
    AddCommandIntArg(hCmd, _nStopPos);
    AddCommandFloatArg(hCmd, _nInterval);
    AddCommandIntArg(hCmd, _bTriggerOnReturnMove);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, nullptr);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    return (nRes);
}

/**
 * \brief Read in response of encoder trigger property setting command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnEncoderAxis Encoder axis index contained in the response.
 * \param[out] _pnStartPos Start position for encoder triggering, contained in the response.
 * \param[out] _pnStopPos Stop position for encoder triggering, ocontained in the response.
 * \param[out] _pnInterval Distance between adjacent triggering points in increments, contained in the response.
 * \param[out] _pbTriggerOnReturnMove Whether triggering is active during the return move from stop position to start
 * position, contained in the response.
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadEncoderTriggerPropertyRsp(
    Rsp_h _hRsp,
    int32_t *_pnEncoderAxis,
    int32_t *_pnStartPos,
    int32_t *_pnStopPos,
    float *_pnInterval,
    int32_t *_pbTriggerOnReturnMove)
{
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Encoder_Trigger_Property);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnEncoderAxis);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 1, _pnStartPos);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 2, _pnStopPos);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseFloatArg(_hRsp, 3, _pnInterval);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 4, _pbTriggerOnReturnMove);
    return (nRes);
}

/**
 * \brief Query device current encoder trigger property.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection()
 * \param[out] _pnEncoderAxis Encoder axis index: for {\\CHRone} - 0..2; for {\\CHRtwoCLSMPS} - 0..4, only used for synchronous connection.
 * \param[out] _pnStartPos Start position for encoder triggering, only used for synchronous connection.
 * \param[out] _pnStopPos Stop position for encoder triggering, only used for synchronous connection.
 * \param[out] _pnInterval Distance between adjacent triggering points in increments, only used for synchronous connection.
 * \param[out] _pbTriggerOnReturnMove Whether triggering is active during the return move from stop position to start
 * position, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t GetEncoderTriggerProperty(
    Conn_h _hConnection,
    int32_t *_pnEncoderAxis,
    int32_t *_pnStartPos,
    int32_t *_pnStopPos,
    float *_pnInterval,
    int32_t *_pbTriggerOnReturnMove,
    int32_t *_pTicket)
{
    Rsp_h hRsp;
    Res_t nRes = ExecNoParamCmd(_hConnection, CmdID_Encoder_Trigger_Property, true, _pTicket, &hRsp);
    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
        nRes = ReadEncoderTriggerPropertyRsp(
            hRsp, _pnEncoderAxis, _pnStartPos, _pnStopPos, _pnInterval, _pbTriggerOnReturnMove);
    return (nRes);
}

/**
 * \brief Set device triggering mode.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nTriggerMode Device triggering mode. For details see Appendix \\ref{sec:TriggerModeConst}.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t SetDeviceTriggerMode(Conn_h _hConnection, int32_t _nTriggerMode, int32_t *_pTicket)
{
    return (ExecSingleIntParamCmd(
        _hConnection, CmdID_Device_Trigger_Mode, false, _nTriggerMode, _pTicket, nullptr));
}

/**
 * \brief Read in response of device triggering mode setting command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnTriggerMode Device triggering mode contained in the response.
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadDeviceTriggerModedRsp(Rsp_h _hRsp, int32_t *_pnTriggerMode)
{
    return (ReadSingleIntParamRsp(_hRsp, CmdID_Device_Trigger_Mode, 0, _pnTriggerMode));
}

/**
 * \brief Read in response of spectrum download command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnSpecType Spectrum type contained in the response (definition see Appendix \\ref{sec:SpectraConst}).
 * \param[out] _pnStartChannelIdx Index of starting channel for which spectrum should be downloaded, contained in the response.
 * \param[out] _pnChannelNumber Number of channels to be downloaded, contained in the response.
 * \param[out] _pSpectrumBuffer A pointer to a short/word pointer where the spectra are stored, contained in the response.
 * \param[out] _pSpectrumLength Length of a spectrum for single channel, contained in the response.
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadDownloadDeviceSpectrumRsp(
    Rsp_h _hRsp,
    int32_t *_pnSpecType,
    int32_t *_pnStartChannelIdx,
    int32_t *_pnChannelNumber,
    const int16_t **_pSpectrumBuffer,
    int32_t *_pSpectrumLength)
{
    TResponseInfo sInfo;
    GetResponseInfo(_hRsp, &sInfo);
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Download_Spectrum);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnSpecType);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 1, _pnStartChannelIdx);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 2, _pnChannelNumber);
    if (CHR_SUCCESS(nRes))
    {
        const int8_t *pSpec;
        int32_t nLength;
        nRes = GetResponseBlobArg(_hRsp, sInfo.ParamCount-1, &pSpec, &nLength);
        if (_pSpectrumBuffer)
            *_pSpectrumBuffer = (const int16_t *)pSpec;
        if (_pSpectrumLength)
            *_pSpectrumLength = nLength / 2;
    }
    return (nRes);
}

/**
 * \brief Download spectrum from device with one measuring channel.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nSpecType Spectrum type (definition see Appendix \\ref{sec:SpectraConst}).
 * \param[out] _pSpectrumBuffer A pointer to a short/word pointer where the spectrum is stored, only used for synchronous connection.
 * \param[out] _pSpectrumLength A pointer to an integer which stores the length of the spectrum, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t DownloadDeviceSpectrum(
    Conn_h _hConnection,
    int32_t _nSpecType,
    const int16_t **_pSpectrumBuffer,
    int32_t *_pSpectrumLength,
    int32_t *_pTicket)
{
    if (_pSpectrumBuffer)
        *_pSpectrumBuffer = nullptr;
    if (_pSpectrumLength)
        *_pSpectrumLength = 0;
    Cmd_h hCmd;
    Rsp_h hRsp = Invalid_Handle;
    NewCommand(CmdID_Download_Spectrum, false, &hCmd);
    AddCommandIntArg(hCmd, _nSpecType);
    AddCommandIntArg(hCmd, 0);
    AddCommandIntArg(hCmd, 1);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, &hRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
    {
        int32_t nType, nStartIdx, nCount;
        nRes = ReadDownloadDeviceSpectrumRsp(hRsp, &nType, &nStartIdx, &nCount, _pSpectrumBuffer, _pSpectrumLength);
    }
    return (nRes);
}

/**
 * \brief Download spectra for more than one channel from MPS/CLS device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nSpecType Spectrum type (definition see Appendix \\ref{sec:SpectraConst}).
 * \param[in] _nStartChannelIdx Index of starting channel for which spectrum should be downloaded.
 * \param[in] _nChannelNumber Number of channels to be downloaded.
 * \param[out] _pSpectrumBuffer A pointer to a short/word pointer where the spectra are stored, only used for synchronous connection.
 * \param[out] _pSingleSpectrumLength A pointer to an integer which stores the length of a single spectrum, only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t DownloadDeviceSpectraMultiChannel(
    Conn_h _hConnection,
    int32_t _nSpecType,
    int32_t _nStartChannelIdx,
    int32_t _nChannelNumber,
    const int16_t **_pSpectrumBuffer,
    int32_t *_pSingleSpectrumLength,
    int32_t *_pTicket)
{
    if (_pSpectrumBuffer)
        *_pSpectrumBuffer = nullptr;
    if (_pSingleSpectrumLength)
        *_pSingleSpectrumLength = 0;
    Cmd_h hCmd;
    Rsp_h hRsp = Invalid_Handle;
    NewCommand(CmdID_Download_Spectrum, false, &hCmd);
    AddCommandIntArg(hCmd, _nSpecType);
    AddCommandIntArg(hCmd, _nStartChannelIdx);
    AddCommandIntArg(hCmd, _nChannelNumber);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, &hRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
    {
        int32_t nType, nStartIdx, nCount;
        int32_t nLength;
        nRes = ReadDownloadDeviceSpectrumRsp(hRsp, &nType, &nStartIdx, &nCount, _pSpectrumBuffer, &nLength);
        if (_pSingleSpectrumLength)
            *_pSingleSpectrumLength = nLength / nCount;
    }
    return (nRes);
}

/**
 * \brief Read in response of table download command.
 *
 * \param[in] _hRsp Response handle, either returned by "ExecCommand" or passed through response and update callback function.
 * \param[out] _pnTableType Table type.
 * \param[out] _pnTableIdx Index of the downloaded table.
 * \param[out] _pTable A pointer to a byte pointer where the downloaded table is stored (managed by the library).
 * \param[out] _nTableLength Table length (in bytes).
 *
 *
 * \return Zero on success. Negative: error or warning code.
 *
 */
inline Res_t ReadDownloadTableRsp(
    Rsp_h _hRsp, int32_t *_pnTableType, int32_t *_pnTableIdx, const int8_t **_pTable, int32_t *_nTableLength)
{
    if (_pTable)
        *_pTable = nullptr;
    if (_nTableLength)
        *_nTableLength = 0;
    Res_t nRes = CheckCmdID(_hRsp, CmdID_Download_Upload_Table);
    TResponseInfo sInfo;
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseInfo(_hRsp, &sInfo);
    if (CHR_SUCCESS(nRes))
        nRes = GetResponseIntArg(_hRsp, 0, _pnTableType);
    if (CHR_SUCCESS(nRes) && (*_pnTableType != -1))
        nRes = GetResponseIntArg(_hRsp, 1, _pnTableIdx);
    if (CHR_SUCCESS(nRes))
    {
        const int8_t *pTemp;
        int32_t nLength;
        nRes = GetResponseBlobArg(_hRsp, sInfo.ParamCount - 1, &pTemp, &nLength);
        if (CHR_SUCCESS(nRes))
        {
            if (_pTable)
                *_pTable = pTemp;
            if (_nTableLength)
                *_nTableLength = nLength;
        }
    }
    return (nRes);
}

/**
 * \brief Download calibration table from device with one measurement channel.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nCHRTableIndex Index of calibration table (0-16).
 * \param[out] _pTable A pointer to a byte pointer where the calibration table is stored (managed by the library), only used for synchronous connection.
 * \param[out] _pnTableLength A pointer to an integer which stores the length of the calibration table (in bytes), only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t DownloadConfocalCalibrationTable(
    Conn_h _hConnection,
    uint32_t _nCHRTableIndex,
    const int8_t **_pTable,
    int32_t *_pnTableLength,
    int32_t *_pTicket)
{
    if (_pTable)
        *_pTable = nullptr;
    if (_pnTableLength)
        *_pnTableLength = 0;
    Cmd_h hCmd;
    Rsp_h hRsp = Invalid_Handle;
    NewCommand(CmdID_Download_Upload_Table, true, &hCmd);
    AddCommandIntArg(hCmd, Table_Confocal_Calibration);
    AddCommandIntArg(hCmd, (int32_t)_nCHRTableIndex);
    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, &hRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
    {
        int32_t nTableType, nTableIdx;
        nRes = ReadDownloadTableRsp(hRsp, &nTableType, &nTableIdx, _pTable, _pnTableLength);
    }

    return (nRes);
}

/**
 * \brief Download current calibration table from multi-channel device, only valid for synchronous connection.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nCHRTableIndex Index of calibration table (0-16).
 * \param[out] _pTable A pointer to a byte pointer where the calibration table is stored (managed by the library), only used for synchronous connection.
 * \param[out] _pnTableLength A pointer to an integer which stores the length of the calibration table (in bytes), only used for synchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t DownloadMultiChannelConfocalCalibrationTable(
    Conn_h _hConnection, uint32_t _nCHRTableIndex, const int8_t **_pTable, int32_t *_pnTableLength)
{
    if (_pTable)
        *_pTable = nullptr;
    if (_pnTableLength)
        *_pnTableLength = 0;
    Cmd_h hCmd;
    NewCommand(CmdID_Download_Upload_Table, true, &hCmd);
    AddCommandIntArg(hCmd, Table_Confocal_Calibration_Multi_Channel);
    AddCommandIntArg(hCmd, (int32_t)_nCHRTableIndex);
    Rsp_h hRsp;
    Res_t nRes = ExecCommand(_hConnection, hCmd, &hRsp);
    if (CHR_SUCCESS(nRes))
    {
        int32_t nTableType, nTableIdx;
        nRes = ReadDownloadTableRsp(hRsp, &nTableType, &nTableIdx, _pTable, _pnTableLength);
    }

    return (nRes);
}

/**
 * \brief Download wavelength table from device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[out] _pTable A pointer to a byte pointer where the calibration table is stored (managed by the library), only used for synchronous connection.
 * \param[out] _pnTableLength A pointer to an integer which stores the length of the wavelength table (in bytes), only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t DownloadWaveLengthTable(
    Conn_h _hConnection, const int8_t **_pTable, int32_t *_pnTableLength, int32_t *_pTicket)
{
    if (_pTable)
        *_pTable = nullptr;
    if (_pnTableLength)
        *_pnTableLength = 0;
    Cmd_h hCmd;
    Rsp_h hRsp = Invalid_Handle;
    NewCommand(CmdID_Download_Upload_Table, true, &hCmd);
    AddCommandIntArg(hCmd, Table_WaveLength);

    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, &hRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
    {
        int32_t nTableType, nTableIdx;
        nRes = ReadDownloadTableRsp(hRsp, &nTableType, &nTableIdx, _pTable, _pnTableLength);
    }

    return (nRes);
}

/**
 * \brief Download refractive index table from device.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _nTableIndex Index of refractive table (1-17).
 * \param[out] _pTable A pointer to a byte pointer where the calibration table is stored (managed by the library), only used for synchronous connection.
 * \param[out] _pnTableLength A pointer to an integer which stores the length of the wavelength table (in bytes), only used for synchronous connection.
 * \param[out] _pTicket A pointer to an integer variable where the internal ticket for command is stored, only used for asynchronous connection.
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t DownloadRefractiveIndexTable(
    Conn_h _hConnection,
    int32_t _nTableIndex,
    const int8_t **_pTable,
    int32_t *_pnTableLength,
    int32_t *_pTicket)
{
    if (_pTable)
        *_pTable = nullptr;
    if (_pnTableLength)
        *_pnTableLength = 0;
    Cmd_h hCmd;
    Rsp_h hRsp = Invalid_Handle;
    NewCommand(CmdID_Download_Upload_Table, true, &hCmd);
    AddCommandIntArg(hCmd, Table_Refractive_Index);
    AddCommandIntArg(hCmd, _nTableIndex);

    Res_t nRes;
    if (SYNC_CONN(_hConnection))
        nRes = ExecCommand(_hConnection, hCmd, &hRsp);
    else
        nRes = ExecCommandAsync(_hConnection, hCmd, nullptr, nullptr, _pTicket);

    if (CHR_SUCCESS(nRes) && (SYNC_CONN(_hConnection)))
    {
        int32_t nTableType, nTableIdx;
        nRes = ReadDownloadTableRsp(hRsp, &nTableType, &nTableIdx, _pTable, _pnTableLength);
    }

    return (nRes);
}

/**
 * \brief Download mask table from multi-channel device, only valid for synchronous connection.
 *
 * \param[in] _hConnection Connection handle returned by OpenConnection() or OpenSharedConnection().
 * \param[in] _bCLS Device type: 1 - CLS device; 0 - MPS device.
 * \param[out] _pTable A pointer to a byte pointer where the calibration table is stored (managed by the library).
 * \param[out] _pnTableLength A pointer to an integer which stores the length of the wavelength table (in bytes).
 *
 * \return Zero on success. Negative: error or warning code.
 */
inline Res_t DownloadMultiChannelMaskTable(
    Conn_h _hConnection, int32_t _bCLS, const int8_t **_pTable, int32_t *_pnTableLength)
{
    if (_pTable)
        *_pTable = nullptr;
    if (_pnTableLength)
        *_pnTableLength = 0;
    Cmd_h hCmd;
    NewCommand(CmdID_Download_Upload_Table, true, &hCmd);
    if (_bCLS)
        AddCommandIntArg(hCmd, Table_CLS_Mask);
    else
        AddCommandIntArg(hCmd, Table_MPS_Mask);

    Rsp_h hRsp;
    Res_t nRes = ExecCommand(_hConnection, hCmd, &hRsp);
    if (nRes >= 0)
    {
        int32_t nTableType, nTableIdx;
        nRes = ReadDownloadTableRsp(hRsp, &nTableType, &nTableIdx, _pTable, _pnTableLength);
    }

    return (nRes);
}
/** @} */

#endif // __CHROCODILELIBSPECIALFUNC_H__
