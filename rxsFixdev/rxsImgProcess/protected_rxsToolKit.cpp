#pragma once

#include "protected_rxsToolKit.h"
#include <pcl/io/pcd_io.h>
#include <pcl/common/transforms.h>

#include <opencv2/opencv.hpp>

#include <mutex>


typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> CloudT;
typedef CloudT::Ptr CP;

CP G_Cloud(new CloudT);

HMODULE	dllHander;
HMODULE	dllHander_v;
HMODULE dllHander_vrh;


std::mutex rtkLock;

rxsMainFrame* MainSys;


void pclCouldCopyTomy(CP& cloud, CloudEntity& ce);
void CouldCopyFrom(CloudEntity& ce, CP& cloud);

void CouldCopy(rxsPointCouldp* cp, unsigned cpNums, CP& cloud, const char* savePath);
void CouldCopy_ZtoY(rxsPointCouldp* cp, unsigned cpNums, CP& cloud, bool isX, const char* savePath);

CP preprocessFunctionV(CP cloud, std::vector<std::string> funcs, std::vector<std::vector<std::string>> paramters);
bool emptyLengthV(CP pc, int rang, float& val);
CP Inflexion(CP cloud);
CP getBoundOfLineV(CP cloud, std::string func, std::vector<std::string> paramter);
float deformOfLaptopV(CP model, CP cloud, float order_deformation);
float lengthOfBoundV(CP cloud, CP cloud_bound, bool consider_y);
void normlizeByBound(CP cloud, CP cloud_bound);
std::vector<float> GapDenoiseV(CP cloud, std::vector<float> xs, std::vector<float> coplanarityThreshold, std::vector<float> skewThreshold, int range);


bool pcdFileRead(std::string fp, CloudEntity& out) {
    CP PC_rf(new CloudT);
    pcl::io::loadPCDFile(fp, *PC_rf);
    out.pNums = PC_rf->points.size();
    out.Entiy = (rxsPointCouldp*)malloc(sizeof(rxsPointCouldp) * out.pNums);
    if (out.Entiy == 0) { return false; }
    for (unsigned x = 0; x < out.pNums; x++) {
        out.Entiy[x].x = PC_rf->points[x].x;
        out.Entiy[x].y = PC_rf->points[x].y;
        out.Entiy[x].z = PC_rf->points[x].z;
    }
    return true;
}


bool pcdFileSave(rxsPointCouldp* cp, unsigned cpNums, const char* path) {
    if (path == 0) { return false; }
    if (cp == 0 || cpNums < 1) { return false; }
    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, path);
    return true;
}

void pclCouldCopyTomy(CP& cloud, CloudEntity& ce) {
    for (unsigned x = 0; x < ce.pNums; x++) {
        ce.Entiy[x].x = cloud->points[x].x;
        ce.Entiy[x].y = cloud->points[x].y;
        ce.Entiy[x].z = cloud->points[x].z;
    }
}

void CouldCopyFrom(CloudEntity& ce, CP& cloud) {
    cloud->width = ce.pNums;
    cloud->height = 1;
    cloud->points.resize(cloud->width * cloud->height);
    for (unsigned x = 0; x < ce.pNums; x++) {
        cloud->points[x].x = ce.Entiy[x].x;
        cloud->points[x].y = ce.Entiy[x].y;
        cloud->points[x].z = ce.Entiy[x].z;
    }
}


void CouldCopy(rxsPointCouldp* cp, unsigned cpNums, CP& cloud, const char* savePath) {
    cloud->width = cpNums;
    cloud->height = 1;
    cloud->points.resize(cloud->width * cloud->height);
    for (unsigned x = 0; x < cpNums; x++) {
        cloud->points[x].x = cp[x].x;
        cloud->points[x].y = cp[x].y;
        cloud->points[x].z = cp[x].z;
    }
    if (savePath != 0) {
        pcl::io::savePCDFileBinary(savePath, *cloud);
    }
}

void CouldCopy_ZtoY(rxsPointCouldp* cp, unsigned cpNums, CP& cloud, bool isX, const char* savePath) {
    cloud->width = cpNums;
    cloud->height = 1;
    cloud->points.resize(cloud->width * cloud->height);
    if (isX) {
        for (unsigned x = 0; x < cpNums; x++) {
            cloud->points[x].x = cp[x].x;
            cloud->points[x].y = cp[x].z;
            //cloud->points[x].z = cp[x].z;
        }
    }
    else {
        for (unsigned x = 0; x < cpNums; x++) {
            cloud->points[x].x = cp[x].y;
            cloud->points[x].y = cp[x].z;
            cloud->points[x].z = 0;
        }
    }
    if (savePath != 0) {
        pcl::io::savePCDFileBinary(savePath, *cloud);
    }
}


CP preprocessFunctionV(CP cloud, std::vector<std::string> funcs, std::vector<std::vector<std::string>> paramters)
{
    typedef CP(*preprocess)(CP, std::vector<std::string>, std::vector<std::vector<std::string>>);
    preprocess func = (preprocess)GetProcAddress(dllHander_v, "preprocessFunction");
    if (func == 0) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return 0;
    }
    return func(cloud, funcs, paramters);
}


bool emptyLengthV(CP pc, int rang, float& val) {
    /// <param name="cloud">输入点云，要保证左右两端点数目都超过十个</param>
    /// <returns></returns>
    typedef float (*emptyLength)(CP cloud, int range);
    emptyLength func = (emptyLength)GetProcAddress(dllHander_v, "emptyLength");
    if (func == 0) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }
    val = func(pc, rang);
    return true;
}


std::vector<float> GapDenoiseV(CP cloud, std::vector<float> xs,
    std::vector<float> coplanarityThreshold, std::vector<float> skewThreshold, int range) {
    typedef std::vector<float>(*GapDenoise)(CP, std::vector<float>, std::vector<float>, std::vector<float>, int);
    GapDenoise gapFunc = (GapDenoise)GetProcAddress(dllHander_v, "GapDenoise");
    if (!gapFunc) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return std::vector<float>();
    }
    return gapFunc(cloud, xs, coplanarityThreshold, skewThreshold, range);
}

/// <summary>
/// 计算锋锐度
/// </summary>
/// <param name="cloud">待计算锋锐度的点云点云， 建议输入未经处理刚采集到的点云， 或者经过preprocessFunction处理过的</param>
/// <param name="order">阶数，建议设置成4， 这个值越大对尖锐信息越敏感，但是grr也会越差</param>
/// <returns></returns>
float sharpnessEstimateV(CP cloud, float order) {

    typedef float(*sharpnessEstimate)(CP, float);
    sharpnessEstimate seFunc = (sharpnessEstimate)GetProcAddress(dllHander_v, "sharpnessEstimate");
    if (!seFunc) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return 0.0;
    }
    return seFunc(cloud, order);
}


/// <summary>
/// 输入点云，寻找两个拐点
/// </summary>
/// <param name="cloud"></param>
/// <returns>输出两个点，第一个点是x较小的左侧点</returns>
CP Inflexion(CP cloud) {
    typedef CP(*inflexion)(CP);
    inflexion seFunc = (inflexion)GetProcAddress(dllHander_v, "inflexion");
    if (!seFunc) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return 0;
    }
    return seFunc(cloud);
}

/// <summary>
/// 输入点云和两个点，切割出两个点中间的点云
/// </summary>
/// <param name="cloud">点云</param>
/// <param name="p1">点一</param>
/// <param name="p2">点二</param>
/// <returns></returns>
CP SliceCloud(CP cloud, PointT p1, PointT p2) {
    typedef CP(*sliceCloud)(CP, PointT, PointT);
    sliceCloud seFunc = (sliceCloud)GetProcAddress(dllHander_v, "sliceCloud");
    if (!seFunc) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return 0;
    }
    return seFunc(cloud, p1, p2);
}


/// <summary>
/// 获取 边界 的两个点
/// </summary>
/// <param name="cloud">必须是经过preprocessFunction处理过的点云</param>
/// <param name="func">函数名
/// 支持如下参数
/// getAnchorJumpReferencePoint   对应两个参数， 
/// 第一个是左跳变的参考值，表示左边边界的跳变起点到其下一个点y方向差值，带符号，如果左边界的跳变起点比其下一个点低，则为负号(左跳变的起点是跳变的左边)
/// 第二个是右跳变的参考值，表示右边边界的跳变起点到其下一个点y方向差值，带符号，如果右边界的跳变起点比其下一个点低，则为负号(右跳变的起点是跳变的右边)
/// </param>
/// <param name="paramter">函数参数</param>
/// <returns>返回两个边界点，第一个点是左边的点，第二个点是右边的点</returns>
CP getBoundOfLineV(CP cloud, std::string func, std::vector<std::string> paramter)
{
    typedef CP(*getLineBound)(CP, std::string, std::vector<std::string>);
    getLineBound fun = (getLineBound)GetProcAddress(dllHander_v, "getBoundOfLine");
    if (fun == 0) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return 0;
    }
    return fun(cloud, func, paramter);
}


/// <summary>
/// 变形度计算
/// </summary>
/// <param name="model">数模点云，点顺序必须是按照x从小到大排列</param>
/// <param name="cloud">实际点云，必须是经过preprocessFunction预处理的</param>
/// <param name="order_deformation">这个值越大，变形度越关注大形变区域（有装配件，或者是其他数模和样品不太一致情况会对变形度的干扰越大），
/// 越小则变形度越关注总体趋势（装配件和其他因素导致的数模与样品不一致情况干扰越小）。 推荐设置为1</param>
/// <returns>输出变形度， 输入点云都会被修改， model被改成上凸最严重的两个点， cloud被改成下凹最严重的两个点</returns>
float deformOfLaptopV(CP model, CP cloud, float order_deformation)
{
    typedef float (*deformLaptop)(CP, CP, float);
    deformLaptop myfun = (deformLaptop)GetProcAddress(dllHander_v, "deformOfLaptop");
    if (myfun == 0) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }
    return myfun(model, cloud, order_deformation);
}


/// <summary>
/// 根据边界点，构建新坐标系
/// </summary>
/// <param name="cloud">点云</param>
/// <param name="cloud_bound">点云的边界，可以由getBoundOfLine获取，或者手动选取，第一个点是左边界点，第二个点是右边界点</param>
void normlizeByBound(CP cloud, CP cloud_bound) {
    typedef void (*nbbfun)(CP, CP);
    nbbfun myfun = (nbbfun)GetProcAddress(dllHander_v, "normlizeByBound");
    if (myfun == 0) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return;
    }
    return myfun(cloud, cloud_bound);
}


/// <summary>
/// 计算内P长宽
/// </summary>
/// <param name="cloud">扫描点云</param>
/// <param name="cloud_bound">点云边界</param>
/// <param name="consider_y">是否考虑y方向落差，(考虑的话 grr容易受边界翘曲影响)</param>
/// <returns>点云边界的距离</returns>
float lengthOfBoundV(CP cloud, CP cloud_bound, bool consider_y)
{
    typedef float (*lengthBound)(CP, CP, bool);
    lengthBound myfun = (lengthBound)GetProcAddress(dllHander_v, "lengthOfBound");
    if (myfun == 0) {
        MainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }
    return myfun(cloud, cloud_bound, consider_y);
}


rxsToolKit::rxsToolKit() {
    isDebug = false;
    notCalculating = false;
    inAutoRun = false;
    dllHander = 0;
    dllHander_v = 0;
    mainSys = 0;
    MainSys = 0;
    memset(&cmp, 0, sizeof(Movepath));
    ToolKitAdd("None", 0);
    ToolKitAdd("fitBSpline_", 1);
    ToolKitAdd("semiconductor", 2);
    ToolKitAdd("PIN", 3);
    ToolKitAdd("registration", 4);
    ToolKitAdd("rtConvert", 5);
    ToolKitAdd("keyboard", 6);
    ToolKitAdd("surfaceProfile", 8);
    ToolKitAdd("computeConvexHull", 9);
    ToolKitAdd("volume", 10);
    ToolKitAdd("dis2dis", 11);
    ToolKitAdd("computeDeformationNake", 12);
    ToolKitAdd("colorReg", 13);
    ToolKitAdd("LaptopCalculation", 14);
}


bool rxsToolKit::Ready(rxsMainFrame* sys) {
    mainSys = sys;
    MainSys = sys;
    //早期的fitBSpline
    dllHander = LoadLibrary(L"czxToolkit.dll");
    //瑞宏专用
    dllHander_vrh = LoadLibrary(L"LaptopDLL-V18.dll");
    dllHander_v = LoadLibrary(L"LaptopDLL.dll");

    return true;
}

void rxsToolKit::SavePathSet(std::string path) {
    if (path.length() < 2) {
        inCouldSavePath.clear();
        return;
    }
    inCouldSavePath = path;
}


void rxsToolKit::ToolKitAdd(std::string toolName, int mid) {
    tookMap[toolName] = mid;
}

std::string* rxsToolKit::ToolsNameGet(unsigned& len) {
    len = ToolsInfos.Names.size();
    if (len < 1) { return 0; }
    return &ToolsInfos.Names[0];
}


void rxsToolKit::ToolsInfosClr() {
    ToolsInfos.Names.clear();
    ToolsInfos.Addrs.clear();
    ToolsInfos.Nos.clear();
}

void rxsToolKit::ForeachMethods() {
    const IMAGE_DOS_HEADER* pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(dllHander);
    const IMAGE_NT_HEADERS* pNtHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<BYTE*>(dllHander) + pDosHeader->e_lfanew);
    const IMAGE_EXPORT_DIRECTORY* pExportDirectory = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(reinterpret_cast<BYTE*>(
        dllHander) + pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    const DWORD* pAddressOfFunctions = reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(dllHander) + pExportDirectory->
        AddressOfFunctions);
    const DWORD* pAddressOfNames = reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(dllHander) + pExportDirectory->AddressOfNames);
    const WORD* pAddressOfNameOrdinals = reinterpret_cast<WORD*>(reinterpret_cast<BYTE*>(dllHander) + pExportDirectory->AddressOfNameOrdinals);
    // 返回格式 [[函数名, 函数地址, 函数序号], ...]
    for (DWORD i = 0; i < pExportDirectory->NumberOfNames; i++)
    {
        const char* pName = reinterpret_cast<char*>(reinterpret_cast<BYTE*>(dllHander) + pAddressOfNames[i]);
        const DWORD dwAddress = pAddressOfFunctions[pAddressOfNameOrdinals[i]];
        const DWORD dwOrdinal = pAddressOfNameOrdinals[i] + pExportDirectory->Base;
        //{ pName, std::to_string(dwAddress), std::to_string(dwOrdinal) }
        ToolsInfos.Names.push_back(pName);
        ToolsInfos.Addrs.push_back(std::to_string(dwAddress));
        ToolsInfos.Nos.push_back(std::to_string(dwOrdinal));
    }
}


bool rxsToolKit::isValidCould(rxsPointCouldp* cp, unsigned cpNums) {
    if (cp == 0 || cpNums < 10) {
        return false;
    }
    return true;
}

rxsToolKit::~rxsToolKit() {
    //FreeLibrary(dllHander);
    FreeLibrary(dllHander_v);
    FreeLibrary(dllHander_vrh);
}


void rxsToolKit::setExparam(void* param) {
    cmp = *(Movepath*)param;
    //RunparamAnasyer(cmp.Flower);
}

void rxsToolKit::RunparamAnasyer(const char* cmd) {
    std::vector<std::string> params = stringSplit(cmd, '|');
    if (params.size() > 0) {
        for (int x = 0; x < params.size(); x++) {
            std::vector<std::string> cmdps = stringSplit(params[x], '*');

        }
    }
}

CloudEntity rxsToolKit::pcdReader(std::string fp) {
    CP rc(new CloudT);
    pcl::io::loadPCDFile(fp, *rc);
    CloudEntity ce = { 0,rc->points.size() };
    ce.Entiy = (rxsPointCouldp*)malloc(ce.pNums * sizeof(rxsPointCouldp));
    pclCouldCopyTomy(rc, ce);
    return ce;
}

short rxsToolKit::ToolKitCaller(std::string toolName, u8 dir, CloudEntity ce, rxsResultReport* result, /*roiArea* Rois*/ RoiArea* Rois, unsigned roiNums, int id, void* extParam) {
    int wich = -1;
    bool ret = false;
    short erc = -3;
    if (!isValidCould(ce.Entiy, ce.pNums)) { return -2; }
    rtkLock.lock();
    std::string msg = "算法处理异常:";
    try {
        wich = tookMap[toolName];
        switch (wich)
        {
        case 0:
            rtkLock.unlock();
            return 0;
        case 1:
            ret = fitBSpline(ce.Entiy, ce.pNums, result, dir);
            break;

        case 2:
            if (Rois != 0 && roiNums > 0) {
                BaseFace bf = { 0 };
                bf.min_x = Rois[0].Area.End.x;
                bf.min_y = Rois[0].Area.End.y;
                bf.max_x = Rois[0].Area.SORC.x;
                bf.max_y = Rois[0].Area.SORC.y;
                ret = semiconductor(ce.Entiy, ce.pNums, bf, result, dir);
            }
            else {
                erc = -102;
            }
            break;

        case 3:
            if (Rois != 0 && roiNums > 0) {
                BaseFace bf = { 0 };
                bf.min_x = Rois[0].Area.End.x;
                bf.min_y = Rois[0].Area.End.y;
                bf.max_x = Rois[0].Area.SORC.x;
                bf.max_y = Rois[0].Area.SORC.y;
                ret = PIN(ce.Entiy, ce.pNums, bf, result, dir);
            }
            else {
                erc = -103;
            }
            break;

        case 4:

            //ret = registration(cp, cpNums, *A, result);
            break;

        case 5:
            //ret = rtConvert(ce.Entiy, ce.pNums, *A, result);
            break;

        case 6:
            ret = keyboard(ce.Entiy, ce.pNums, result, dir);
            break;
        case 8:
            ret = surfaceProfile(ce.Entiy, ce.pNums, result);
            break;
        case 9:
            ret = computeConvexHull(ce.Entiy, ce.pNums, result);
            break;
        case 10:
            ret = volume(ce.Entiy, ce.pNums, result);
            break;
        case 11:
            ret = dis2dis(ce.Entiy, ce.pNums);
            break;
        case 12:
            ret = computeDeformationNake(ce.Entiy, ce.pNums, result, dir, id);
            break;
            //case 13:
            //    ret = initModelNake(ce.Entiy, ce.pNums, "");
            //    break;
        case 14:
            ret = LaptopCalculation(ce.Entiy, ce.pNums, result, dir, id);
            break;

        default:
            erc = -1;
            break;
        }
    }
    catch (std::exception e) {
        msg += e.what();
        erc = -998;
    }
    catch (...) {
        erc = -999;
    }
    rtkLock.unlock();
    if (erc < -900) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)msg.c_str());
    }
    return ret ? 0 : erc;
}

short rxsToolKit::Run(u8 dir, CloudEntity ce, rxsResultReport*,/* roiArea**/ RoiArea* Rois, void* extParam) {
    bool ret = false;
    short erc = -3;
    if (!isValidCould(ce.Entiy, ce.pNums)) { 
        return -2; 
    }
    rtkLock.lock();
    setExparam(extParam);
    std::string msg = "算法处理异常:";
    try {
        //参考 锚定到 LaptopCalculation方法中.....

    }
    catch (std::exception e) {
        msg += e.what();
        erc = -998;
    }
    catch (...) {
        erc = -999;
    }
    rtkLock.unlock();
    if (erc < -900) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)msg.c_str());
    }
    return ret ? 0 : erc;
}


bool rxsToolKit::fitBSpline(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result, u8 dir) {
    /// <summary>
    /// 输入是点云, 是否需要可视化轮廓度
    /// 还要有配置文件conf_curve.czx
    /// </summary>
    typedef std::vector<double>(__cdecl* VCVV)(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, bool);
    VCVV getLength = (VCVV)GetProcAddress(dllHander, "fitBSpline_");
    if (!getLength) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp, cpNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    else {
        CouldCopy_ZtoY(cp, cpNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    if (notCalculating) { return true; }
    std::vector<CP> fitted_x, fitted_y;
    std::vector<double> ret = getLength(cloud, isDebug);
    char buff[10];
    int ID = Result->AddRow();
    int rowIndex = ID - 1;
    sprintf(buff, "%d", ID);
    Result->SetValue("ID", buff, rowIndex);
    sprintf(buff, "%.5f", ret[0]);
    Result->SetValue("弧长(mm)", buff, rowIndex);
    sprintf(buff, "%.5f", ret[1]);
    Result->SetValue("投影长(mm)", buff, rowIndex);
    sprintf(buff, "%.5f", ret[2]);
    Result->SetValue("轮廓度", buff, rowIndex);
    Result->SetValue("PASS", "OK", rowIndex);
    return true;
}

bool rxsToolKit::PIN(rxsPointCouldp* cp, unsigned cpNums, BaseFace bf, rxsResultReport* Result, u8 dir) {
    typedef std::vector<PointT>(*funcFormater)(CP cloud, double min_x, double max_x, double min_y, double max_y);

    funcFormater func = (funcFormater)GetProcAddress(dllHander, "PIN");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    if (notCalculating) { return true; }
    std::vector<PointT> result = func(cloud, bf.min_x, bf.max_x, bf.min_y, bf.max_y);
    if (result.size() > 0) {
        if (Result == 0) {
            return false;
        }
        for (unsigned x = 0; x < result.size(); x++) {
            PointT rp = result[x];
            char buff[10];
            int ID = Result->AddRow();
            int rowIndex = ID - 1;
            sprintf(buff, "%d", ID);
            Result->SetValue("ID", buff, rowIndex);
            sprintf(buff, "%.5f", rp.x);
            Result->SetValue("X(mm)", buff, rowIndex);
            sprintf(buff, "%.5f", rp.y);
            Result->SetValue("Y(mm)", buff, rowIndex);
            sprintf(buff, "%.5f", rp.z);
            Result->SetValue("Z(mm)", buff, rowIndex);
            Result->SetValue("PASS", "OK", rowIndex);
        }
        return true;
    }
    return false;
}


bool rxsToolKit::semiconductor(rxsPointCouldp* cp, unsigned cpNums, BaseFace bf, rxsResultReport* Result, u8 dir) {
    /// <summary>
    /// 输入是点云,和四个确定确定参考平面边框的参数
    /// </summary>
    typedef std::vector<PointT>(*funcFormater)(CP cloud, double min_x, double max_x, double min_y, double max_y);
    funcFormater func = (funcFormater)GetProcAddress(dllHander, "semiconductor");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    if (notCalculating) { return true; }
    std::vector<PointT> result = func(cloud, bf.min_x, bf.max_x, bf.min_y, bf.max_y);
    if (result.size() > 0) {
        char buff[10];
        for (unsigned x = 0; x < result.size(); x++) {
            PointT rp = result[x];
            int ID = Result->AddRow();
            int rowIndex = ID - 1;
            sprintf(buff, "%d", ID);
            Result->SetValue("ID", buff, rowIndex);
            sprintf(buff, "%.5f", rp.x);
            Result->SetValue("X(mm)", buff, rowIndex);
            sprintf(buff, "%.5f", rp.y);
            Result->SetValue("Y(mm)", buff, rowIndex);
            sprintf(buff, "%.5f", rp.z);
            Result->SetValue("Z(mm)", buff, rowIndex);
            Result->SetValue("PASS", "OK", rowIndex);
        }
        return true;
    }
    return false;
}

bool rxsToolKit::registration(rxsPointCouldp* cp, unsigned& cpNums, std::string& rtTx, rxsResultReport* outResult) {
    if (outResult == 0) {
        if (G_Cloud && G_Cloud->size() > 0) {
            G_Cloud->clear();
        }
        CouldCopy(cp, cpNums, G_Cloud, (inCouldSavePath.length() > 1) ? (inCouldSavePath + "\\registration_p1.pcd").c_str() : 0);
        return true;
    }
    /// <summary>
    /// 输入是两幅点云,和两个文件名
    ///这两个文件按行存放着12个数组描述3组孔洞roi, 顺序是double min_x, double max_x, double min_y, double max_y, 需要注意的是两幅点云的三个洞顺序要对应
    ///输入还需要有配置文件conf.czx 和 ransac.czx
    /// </summary>
    typedef Eigen::Matrix4f(__cdecl* funcFormater)(CP c1, CP c2, std::string hole_pos_path_x, std::string hole_pos_path_y);
    funcFormater func = (funcFormater)GetProcAddress(dllHander, "registration");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    CP cloudTrx(new CloudT);

    cloud->width = cpNums;
    cloud->height = 1;
    cloud->points.resize(cloud->width * cloud->height);

    cloudTrx->width = cpNums;
    cloudTrx->height = 1;
    cloudTrx->points.resize(cloudTrx->width * cloudTrx->height);

    for (unsigned x = 0; x < cpNums; x++) {
        cloud->points[x].x = cp[x].x;
        cloud->points[x].y = cp[x].y;
        cloud->points[x].z = cp[x].z;

        cloudTrx->points[x].x = cp[x].x;
        cloudTrx->points[x].y = cp[x].y;
        cloudTrx->points[x].z = cp[x].z;
    }
    if (inCouldSavePath.length() > 1) {
        pcl::io::savePCDFileBinary((inCouldSavePath + "\\registration_p2.pcd").c_str(), *cloud);
    }
    Eigen::Matrix4f EM = func(cloud, G_Cloud, "y_Hole.txt", "x_Hole.txt");

    //对传入参数的第一副成像进行旋转
    pcl::transformPointCloud(*cloudTrx, *cloudTrx, EM);

    for (unsigned m = 0; m < cloudTrx->points.size(); m++) {
        cp[m].x = cloudTrx->points[m].x;
        cp[m].y = cloudTrx->points[m].y;
        cp[m].z = cloudTrx->points[m].z;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(5);

    for (unsigned m = 0; m < 4; m++) {
        for (unsigned n = 0; n < 4; n++) {
            if (m == 3 && n == 3) {
                ss << EM(m, n);
            }
            else {
                ss << EM(m, n) << ',';
            }
        }
    }
    rtTx = ss.str();
    return true;
}

bool rxsToolKit::rtConvert(rxsPointCouldp* cp, unsigned& cpNums, std::string rtTx, rxsResultReport* outResult) {
    if (outResult == 0) {
        return false;
    }

    if (rtTx.length() < 1) {
        //MCR mcr;
        //mcr.PrfCR = { 1,0.025,1 };
        //outResult->push_back(mcr);
        return true;
    }

    std::vector<std::string> Vals = stringSplit(rtTx, ',');
    CP cloudTrx(new CloudT);
    CouldCopy(cp, cpNums, cloudTrx, 0);

    Eigen::Matrix4f EM;
    float txM[16] = { 0 };
    for (unsigned n = 0, m = 0; n < Vals.size(); n++) {
        if (Vals[n].length() > 0) {
            txM[m] = atof(Vals[n].c_str());
            m += 1;
        }
    }
    for (unsigned m = 0; m < 4; m++) {
        for (unsigned n = 0; n < 4; n++) {
            EM(m, n) = txM[m * 4 + n];
        }
    }
    //对传入参数的第一副成像进行旋转
    pcl::transformPointCloud(*cloudTrx, *cloudTrx, EM);

    for (unsigned m = 0; m < cloudTrx->points.size(); m++) {
        cp[m].x = cloudTrx->points[m].x;
        cp[m].y = cloudTrx->points[m].y;
        cp[m].z = cloudTrx->points[m].z;
    }
    return true;
}

bool rxsToolKit::keyboard(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result, u8 dir) {
    /// <summary>
    /// 输入的点云,初始平面roi坐标文件名,验证平面度所需要平面的roi的文件名
    ///输出通过引用导出, 是拟合到的平面, 平面度, 和段差矩阵
    /// </summary>
    typedef void(__cdecl* funcFormater)(
        CP cloud,
        std::string initial_plane_pos,
        std::vector<std::string> verify_path,
        std::vector<CP>& planes,
        std::vector<std::vector<float>>& flatness,
        Eigen::MatrixXf& offset);

    funcFormater func = (funcFormater)GetProcAddress(dllHander, "keyboard");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, "C:\\Users\\Administrator\\Desktop\\keyboard.pcd");
    if (notCalculating) { return true; }

    std::vector<CP> outRoiPlanes;
    std::vector<std::vector<float>> outFlatness;
    Eigen::MatrixXf outDifVal;
    std::vector<std::string> roiFlatness;
    roiFlatness.push_back("roiFlatness1.txt");
    roiFlatness.push_back("roiFlatness2.txt");

    func(cloud, "dis_bf.txt", roiFlatness, outRoiPlanes, outFlatness, outDifVal);

    if (outFlatness.size() > 0) {
        int cLen = 0;
        char buff[10];
        for (int x = 0; x < outFlatness.size(); x++) {
            cLen = outFlatness[x].size();
            for (unsigned y = 0; y < cLen; y++) {
                int ID = Result->AddRow();
                int rowIndex = ID - 1;
                sprintf(buff, "%d", ID);
                Result->SetValue("ID", buff, rowIndex);

                sprintf(buff, "%.5f", outFlatness[x][y]);
                Result->SetValue("平面度", buff, rowIndex);

                sprintf(buff, "%.5f", outDifVal(0, 1));
                Result->SetValue("断差", buff, rowIndex);

                Result->SetValue("PASS", "OK", rowIndex);
            }
        }
        return true;
    }
    return false;
}


bool rxsToolKit::surfaceProfile(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result) {
    typedef double(__cdecl* surfaceProfileApi)(CP cloud, int);
    surfaceProfileApi func = (surfaceProfileApi)GetProcAddress(dllHander, "surfaceProfile");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, (inCouldSavePath.length() > 2) ? inCouldSavePath.c_str() : 0);
    if (notCalculating) { return true; }
    double r = 0.0;
    r = func(cloud, 12);
    char buff[20];
    int ID = Result->AddRow();
    int rowIndex = ID - 1;
    sprintf(buff, "%d", ID);
    Result->SetValue("ID", buff, rowIndex);

    sprintf(buff, "-#%d", rowIndex);
    Result->SetValue("编号", buff, rowIndex);

    sprintf(buff, "%.5f", r);
    Result->SetValue("面轮廓度", buff, rowIndex);
    Result->SetValue("PASS", "OK", rowIndex);
    return true;
}

bool rxsToolKit::computeConvexHull(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*) {
    typedef CP(__cdecl* computeConvexHullApi)(CP cloud);
    computeConvexHullApi func = (computeConvexHullApi)GetProcAddress(dllHander, "computeConvexHull");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    if (notCalculating) { return true; }
    CP r = func(cloud);

    return true;
}

bool rxsToolKit::volume(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result)
{
    typedef double (*funx_volume)(CP cloud, double xyInterval);
    funx_volume func = (funx_volume)GetProcAddress(dllHander, "volume");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    if (notCalculating) { return true; }
    double r = 0.0f;
    int ID = Result->AddRow();
    r = func(cloud, 12);
    char buff[20];
    int rowIndex = ID - 1;
    sprintf(buff, "%d", ID);
    Result->SetValue("ID", buff, rowIndex);
    sprintf(buff, "-#%d", rowIndex);
    Result->SetValue("编号", buff, rowIndex);
    sprintf(buff, "%.5f", r);
    Result->SetValue("体积", buff, rowIndex);
    Result->SetValue("PASS", "OK", rowIndex);
    return true;
}


bool rxsToolKit::dis2dis(rxsPointCouldp* cp, unsigned cpNums) {
    typedef std::vector<float>(*fun_api)(CP model, CP source, float interval_x, float interval_y);
    fun_api func = (fun_api)GetProcAddress(dllHander, "dis2dis");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP model(new CloudT);
    std::string fp;
    pcl::io::loadPCDFile(fp, *model);

    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    if (notCalculating) { return true; }
    float xspace = 20.0f;
    float yspace = 20.0f;
    std::vector<float> r = func(model, cloud, xspace, yspace);

    return true;
}

//计算2D和变形量
bool rxsToolKit::computeDeformationNake(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result, u8 dir, int id) {
    typedef std::vector<float>(*fun_api)(CP model, CP cloud);
    fun_api func = (fun_api)GetProcAddress(dllHander, "computeDeformationNake");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP model(new CloudT);
    //处理后已经能匹配的模型路径
    std::string vmp[2] = { "x","y" };
    std::string fp = "CompareModel\\";
    fp += std::to_string(id) + vmp[dir] + "_model.pcd";
    pcl::io::loadPCDFile(fp, *model);

    CP cloud(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp, cpNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    else {
        CouldCopy_ZtoY(cp, cpNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }

    if (notCalculating) { return true; }
    std::vector<float> r = func(model, cloud);
    char buff[20];
    int ID = Result->AddRow();
    int rowIndex = ID - 1;
    sprintf(buff, "-#%d", rowIndex);
    Result->SetValue("编号", buff, rowIndex);

    sprintf(buff, "%.5f", r[6]);
    Result->SetValue("投影长(mm)", buff, rowIndex);

    float hopbxd = 1.2868 * r[5] + 0.0264;
    float dif = (hopbxd - r[5]);
    float tv = r[6] + dif;
    sprintf(buff, "%.5f", tv);
    Result->SetValue("预测长", buff, rowIndex);

    sprintf(buff, "%.5f", r[0]);
    Result->SetValue("上最大偏差", buff, rowIndex);
    sprintf(buff, "%.5f", r[1]);
    Result->SetValue("下最大偏差", buff, rowIndex);
    sprintf(buff, "%.5f", r[2]);
    Result->SetValue("总最大偏差", buff, rowIndex);
    sprintf(buff, "%.5f", r[3]);
    Result->SetValue("变形度V1", buff, rowIndex);
    sprintf(buff, "%.5f", r[4]);
    Result->SetValue("变形度V2", buff, rowIndex);
    sprintf(buff, "%.5f", r[5]);
    Result->SetValue("锋锐度", buff, rowIndex);
    sprintf(buff, "%.5f", r[7]);
    Result->SetValue("变形类型", buff, rowIndex);
    Result->SetValue("PASS", "OK", rowIndex);
    return true;
}


bool rxsToolKit::computeDeformationNake(rxsPointCouldp* cp, unsigned cpNums, std::string modeFile, rxsResultReport* Result, u8 dir) {
    typedef std::vector<float>(*fun_api)(CP model, CP cloud);
    fun_api func = (fun_api)GetProcAddress(dllHander, "computeDeformationNake");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP model(new CloudT);
    pcl::io::loadPCDFile(modeFile, *model);

    CP cloud(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp, cpNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    else {
        CouldCopy_ZtoY(cp, cpNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }

    if (notCalculating) { return true; }
    std::vector<float> r = func(model, cloud);
    char buff[20];
    int ID = Result->AddRow();
    int rowIndex = ID - 1;
    sprintf(buff, "-#%d", rowIndex);
    Result->SetValue("编号", buff, rowIndex);

    sprintf(buff, "%.5f", r[6]);
    Result->SetValue("投影长(mm)", buff, rowIndex);

    float hopbxd = 1.2868 * r[5] + 0.0264;
    float dif = (hopbxd - r[5]);
    float tv = r[6] + dif;
    sprintf(buff, "%.5f", tv);
    Result->SetValue("预测长", buff, rowIndex);

    sprintf(buff, "%.5f", r[0]);
    Result->SetValue("上最大偏差", buff, rowIndex);
    sprintf(buff, "%.5f", r[1]);
    Result->SetValue("下最大偏差", buff, rowIndex);
    sprintf(buff, "%.5f", r[2]);
    Result->SetValue("总最大偏差", buff, rowIndex);
    sprintf(buff, "%.5f", r[3]);
    Result->SetValue("变形度V1", buff, rowIndex);
    sprintf(buff, "%.5f", r[4]);
    Result->SetValue("变形度V2", buff, rowIndex);
    sprintf(buff, "%.5f", r[5]);
    Result->SetValue("锋锐度", buff, rowIndex);
    sprintf(buff, "%.5f", r[7]);
    Result->SetValue("变形类型", buff, rowIndex);
    Result->SetValue("PASS", "OK", rowIndex);
    return true;
}

bool rxsToolKit::initModelNake(rxsPointCouldp* cp, unsigned cpNums, const char* sfp) {
    typedef void (*fun_api)(CP model);
    fun_api func = (fun_api)GetProcAddress(dllHander, "initModelNake");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }

    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    if (notCalculating) { return true; }

    func(cloud);
    if (sfp != 0) {
        pcl::io::savePCDFileBinary(sfp, *cloud);
    }
    return true;
}

bool rxsToolKit::initModelNake(const char* srcPCDfile, const char* sfp) {
    try {
        CloudT* pce = new CloudT;
        int x = pcl::io::loadPCDFile(srcPCDfile, *pce);
        if (x != 0) { return false; }
        typedef void (*fun_api)(CP model);
        fun_api func = (fun_api)GetProcAddress(dllHander, "initModelNake");
        if (!func) {
            mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
            return false;
        }

        CP cloud(pce);
        func(cloud);
        if (sfp != 0) {
            pcl::io::savePCDFileBinary(sfp, *cloud);
        }
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
    catch (...) {

    }
    return true;
}

//bool rxsToolKit::colorReg(const char* m_root, const char* obj_dir, rxsResultReport* Result)
//{
//    typedef std::string(*colorReg)(std::string model_root, std::string object_directory);
//    colorReg func = (colorReg)GetProcAddress(dllHander, "colorReg");
//    if (!func) {
//        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
//        return false;
//    }
//
//    std::string r = func(m_root, obj_dir);
//    char buff[20];
//
//    int ID = Result->AddRow();
//    int rowIndex = ID - 1;
//    sprintf(buff, "-#%d", rowIndex);
//    Result->SetValue("编号", buff, rowIndex);
//    if (r == "NULL") {
//        r = "蓝色";
//    }
//    Result->SetValue("结果", r.c_str(), rowIndex);
//    return true;
//}


float rxsToolKit::predict2D(float deform, float sharpness, float dis_2D, std::string modeName, std::string id) {
    /// <summary>
    /// 预测2D长度
    /// </summary>
    /// <param name="deform">变形度</param>
    /// <param name="sharpness">锋锐度</param>
    /// <param name="dis_2D">2D长度</param>
    /// <param name="laptop_type">笔记本类型</param>
    /// <param name="line_type">路径类型</param>
    /// <returns></returns>
    typedef float (*predict2D)(float deform, float sharpness, float dis_2D, std::string laptop_type, std::string line_type);
    predict2D func = (predict2D)GetProcAddress(dllHander_v, "predict2D");
    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }
    return func(deform, sharpness, dis_2D, modeName, id);
}


bool emptyLength(CP pc, int rang, float& val) {
    /// <param name="cloud">输入点云，要保证左右两端点数目都超过十个</param>
    /// <returns></returns>
    typedef float (*emptyLength)(CP cloud, int range);
    emptyLength func = (emptyLength)GetProcAddress(dllHander_v, "emptyLength");
    if (func == 0) {
        return false;
    }
    val = func(pc, rang);
    return true;
}

bool rxsToolKit::LaptopCalculation(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result, u8 dir, int id) {
    /// <param name="model">数模</param>
    /// <param name="cloud">点云</param>
    /// <param name="type">笔记本类型
    /// <param name="func_flag">第0个位置表示是否判断辅料有无</param>
    /// <param name="auxiliary_x">辅料检测的x位置</param>
    /// <param name="auxiliary_z">辅料检测的z的标准</param>
    /// <param name="auxiliary_ret">输出参数  辅料检测的结果</param>
    /// <returns>变形度，锋锐度，左间隙，右间隙，上落差，下落差，数值为-1代表没有计算成功</returns>
    typedef std::vector<float>(*LapCalculation)(CP model, CP cloud, std::string type, int func_flag,
        std::vector<float> auxiliary_x, std::vector<float> auxiliary_z, std::vector<bool>& auxiliary_ret);
    LapCalculation func = (LapCalculation)GetProcAddress(dllHander_vrh, "LaptopCalculation");

    if (!func) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return false;
    }
    mainSys->FrontProcessMsgSender(CBC_STR_N_MSG, (void*)"进入计算");
    CP cloud(new CloudT);
    CP cloudDiscontinuity(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp, cpNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
        CouldCopy_ZtoY(cp, cpNums, cloudDiscontinuity, true, 0);
    }
    else {
        CouldCopy_ZtoY(cp, cpNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
        CouldCopy_ZtoY(cp, cpNums, cloudDiscontinuity, false, 0);
    }

    if (notCalculating) { return true; }
    if (cmp.id != id) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"路径ID未能匹配!请确认时序！");
        return false;
    }
    bool isDefmation = true;
    float lowVal = -100.0f;
    float upVal = 100.0f;
    float OffsetCal = 0.0f;
    int GapSelected = 0;
    std::vector<float>  xs;
    std::vector<float>  thrs_Cop;//共面面阈值
    std::vector<float>  thrs_Oop;//异面阈值
    int rang = 5;
    std::vector<float> r;
    std::vector<bool> flChkRet;
    std::vector<std::string> params = stringSplit(cmp.Flower, '|');

    if (params.size() > 8) {
        CP model(new CloudT);
        std::string vmp[2] = { "x","y" };
        std::string fp = "CompareModel\\";
        if (params[0] == "-") {
            isDefmation = false;
        }
        if (isDefmation) {
            //处理后已经能匹配的模型路径
            fp += params[0] + "\\" + std::to_string(id) + vmp[dir] + "_model.pcd";
            pcl::io::loadPCDFile(fp, *model);
        }

        int flChk = 0;
        std::vector<float> Xpos;
        std::vector<float> cmpZpos;
        if (params.size() > 2) {
            flChk = atoi(params[2].c_str());
        }
        lowVal = atof(params[3].c_str());
        upVal = atof(params[4].c_str());
        std::vector<std::string> rrr = stringSplit(params[5], '/');
        for (int x = 0; x < rrr.size(); x++) {
            std::vector<std::string> xs_thrd = stringSplit(rrr[x], '*');
            xs.push_back(atof(xs_thrd[0].c_str()));
            thrs_Cop.push_back(atof(xs_thrd[1].c_str()));
            thrs_Oop.push_back(atof(xs_thrd[2].c_str()));
            rang = atof(xs_thrd[3].c_str());
        }
        GapSelected = atoi(params[6].c_str());
        OffsetCal = atof(params[7].c_str());
        if (flChk == 1) {
            int nums = params.size() - 9;
            for (int n = 0; n < nums; n += 2) {
                Xpos.push_back(atof(params[9 + n].c_str()));
                cmpZpos.push_back(atof(params[9 + n + 1].c_str()));
            }
        }
        if (isDefmation) {//变形度测试
            r = func(model, cloud, params[1], flChk, Xpos, cmpZpos, flChkRet);
        }
        else {//非变形度测试
            if (cloud->points.size() < 20) {
                mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"测空长度-输入点云数量过少！");
                return false;
            }
            float reval = 0;
            //if (!emptyLength(cloud, atoi(params[1].c_str()), reval)) {
            if (!emptyLengthV(cloud, atoi(params[1].c_str()), reval)){
                mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"测空长度计算失败！");
                return false;
            }
            r.push_back(-1.0f);
            r.push_back(-1.0f);
            r.push_back(reval);
            r.push_back(-1.0f);
            r.push_back(-1.0f);
            r.push_back(-1.0f);
            r.push_back(-1.0f);
            r.push_back(-1.0f);
        }
        if (r.size() < 7) {
            mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"计算结束，数量不匹配!!");
            return false;
        }
        std::vector<float>  Dr;
        if (xs.size() > 0 && xs.size() == thrs_Cop.size()) {
            mainSys->FrontProcessMsgSender(CBC_STR_N_MSG, (void*)"进入GAP计算");
            /// <summary>
            /// 计算先下跳变再上跳变的两个落差
            /// </summary>
            /// <param name="cloud">输入点云</param>
            /// <param name="xs">开始判断的x坐标， 从该位置向后查找符合条件的段差， 向后指的是索引向后</param>
            /// <param name="coplanarityThreshold">共面阈值， 低于该阈值的视为同一个面上的点</param>
            /// <param name="skewThreshold">异面阈值， 高于该阈值的视为不同面上的点</param>
            /// <param name="range">一个面最少的点数目</param>
            /// <returns></returns>
            typedef std::vector<float>(*GapDenoise)(CP cloud, std::vector<float> xs, std::vector<float> coplanarityThreshold, std::vector<float> skewThreshold, int range);
            GapDenoise gapFunc = (GapDenoise)GetProcAddress(dllHander_vrh, "GapDenoise");
            if (!gapFunc) {
                mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
                return false;
            }
            Dr = gapFunc(cloudDiscontinuity, xs, thrs_Cop, thrs_Oop, rang);
            mainSys->FrontProcessMsgSender(CBC_STR_N_MSG, (void*)"GAP计算完成");
        }

        float prevResult = -1;
        if (isDefmation && params[8] == "1") {
            std::string modname = params[0];
            std::string dvp[2] = { "X","Y" };
            if (params[0].size() > 3) {
                modname = params[0].substr(0, 3);
            }
            prevResult = predict2D(r[0], r[1], r[2] + OffsetCal, modname, std::to_string(cmp.id) + dvp[dir]);
        }
        mainSys->FrontProcessMsgSender(CBC_STR_N_MSG, (void*)"计算结束，报表输出");
        char buff[20];
        int ID = Result->AddRow();
        int rowIndex = ID - 1;
        std::string rvmp[2] = { "P1内长","P1内宽" };
        sprintf(buff, "%s-#%d", rvmp[dir].c_str(), cmp.id);
        Result->SetValue("编号", buff, rowIndex);
        sprintf(buff, "%.5f", r[0]);
        Result->SetValue("变形度", buff, rowIndex);
        sprintf(buff, "%.5f", r[1]);
        Result->SetValue("锋锐度", buff, rowIndex);
        sprintf(buff, "%.5f", r[2] + OffsetCal);
        Result->SetValue("内P长宽", buff, rowIndex);
        sprintf(buff, "%.5f", (r[3] < 0 ? -r[3] : r[3]));
        Result->SetValue("GAP1", buff, rowIndex);
        sprintf(buff, "%.5f", (r[4] < 0 ? -r[4] : r[4]));
        Result->SetValue("GAP2", buff, rowIndex);

        sprintf(buff, "%.5f", (prevResult < 0 ? -prevResult : prevResult));
        Result->SetValue("预测长", buff, rowIndex);
        if (Dr.size() > 0) {
            sprintf(buff, "%.5f", (Dr[0] - 1.0f));
            Result->SetValue("断差", buff, rowIndex);
        }
        else {
            sprintf(buff, "%d", -1);
            Result->SetValue("断差", buff, rowIndex);
        }

        DeformationType dtp = DeformationType(r[7]);
        switch (dtp)
        {
        case DeformationType::UP:
            Result->SetValue("变形类型", "凸母", rowIndex);
            break;

        case DeformationType::DOWN:
            Result->SetValue("变形类型", "凸公", rowIndex);
            break;

        case DeformationType::OTHER:
            Result->SetValue("变形类型", "其他", rowIndex);
            break;

        case DeformationType::EMPTY:
            Result->SetValue("变形类型", "空", rowIndex);
            Result->SetValue("PASS", "NG", rowIndex);
            return false;

        default:
            Result->SetValue("变形类型", "忽略", rowIndex);
            break;
        }
        if (r[0] > upVal || r[0] < lowVal) {
            Result->SetValue("PASS", "NG", rowIndex);
        }
        else {
            Result->SetValue("PASS", "OK", rowIndex);
        }
        int nums = flChkRet.size();
        if (nums > 0) {
            for (size_t u = 0; u < flChkRet.size(); u++) {
                rowIndex = Result->AddRow();
                rowIndex -= 1;
                sprintf(buff, "%d-#%d", cmp.id, u + 1);
                Result->SetValue("编号", buff, rowIndex);
                Result->SetValue("变形度", "辅料检测", rowIndex);
                if (flChkRet[u]) {
                    Result->SetValue("PASS", "OK", rowIndex);
                }
                else {
                    Result->SetValue("PASS", "NG", rowIndex);
                }
            }
        }
        return true;
    }
    mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"缺少必要参数，请确认已经正确配置了相关参数!");
    return false;
}


CloudEntity rxsToolKit::ModelReaderAndProcess(std::string mod, rxsPointCouldp* modBund) {
    if (notCalculating) { return { 0,0 }; }
    CP Mdl(new CloudT);
    pcl::io::loadPCDFile(mod, *Mdl);

    ColorCloudEntity ce_mod;
    ce_mod.Color = { 255,0,0,0 };

    CP mbund_cloud(new CloudT);
    CloudEntity BCE = { &modBund[0],2 };
    CouldCopyFrom(BCE, mbund_cloud);
    normlizeByBound(Mdl, mbund_cloud);

    ce_mod.PC.pNums = Mdl->width * Mdl->height;
    ce_mod.PC.Entiy = (rxsPointCouldp*)malloc(ce_mod.PC.pNums * sizeof(rxsPointCouldp));
    pclCouldCopyTomy(Mdl, ce_mod.PC);
    if (!inAutoRun) {
        mainSys->FrontProcessMsgSender(CBC_COLORPCUSHOW, &ce_mod);
    }
    return ce_mod.PC;
}

void rxsToolKit::LineContourMatchingGet(std::string pcf, std::string mod, rxsPointCouldp* modBund, std::string spcBundstr, std::string FuncParamsGrp)
{
    try {
        //----模型点云的操作-------
        CloudEntity mr = ModelReaderAndProcess(mod, modBund);
        free(mr.Entiy);

        //----------扫描到的点云操作----
        CP PC_rf(new CloudT);
        pcl::io::loadPCDFile(pcf, *PC_rf);

        ColorCloudEntity ce_src;
        ce_src.Color = { 0,0,255,0 };

        std::vector<std::string> funcs;
        std::vector<std::vector<std::string>> paramters;
        preprocessFunctionParamsConvert(FuncParamsGrp, funcs, paramters);
        CP ret = preprocessFunctionV(PC_rf, funcs, paramters);

        //---开启预处理结果前端显示---
        //CloudEntity prepCe;
        //prepCe.pNums = ret->width * ret->height;
        //prepCe.Entiy = (rxsPointCouldp*)malloc(prepCe.pNums * sizeof(rxsPointCouldp));
        //pclCouldCopyTomy(ret, prepCe);
        //mainSys->FrontProcessMsgSender(CBC_PCUSHOW, &prepCe);
        //free(prepCe.Entiy);

        std::vector<std::string> vp = stringSplit(spcBundstr, ',');
        std::vector<std::string> vpp = stringSplit(vp[1], '|');
        CP cbund_cloud = getBoundOfLineV(ret, vp[0], vpp);
        normlizeByBound(ret, cbund_cloud);

        ce_src.PC.pNums = ret->width * ret->height;
        ce_src.PC.Entiy = (rxsPointCouldp*)malloc(ce_src.PC.pNums * sizeof(rxsPointCouldp));
        pclCouldCopyTomy(ret, ce_src.PC);
        if (!inAutoRun) {
            mainSys->FrontProcessMsgSender(CBC_COLORPCUSHOW, &ce_src);
        }
        free(ce_src.PC.Entiy);
    }
    catch (std::exception exc) {
        std::string msg = "CAD匹配查看异常!";
        msg += exc.what();
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)msg.c_str());
    }
    catch (...) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"CAD匹配查看异常!");
    }
}


CloudEntity myRoiareaGet(CloudEntity& sin, rxsPointCouldp& p1, rxsPointCouldp& p2) {
    if (sin.pNums > 20 && sin.Entiy != 0) {
        bool equX = false;
        bool equY = false;

        for (int x = 0; x < 20; x++) {
            if (sin.Entiy[x].x == sin.Entiy[x + 1].x)
            {
                equX = true;
            }
            else {
                equX = false;
            }
            if (sin.Entiy[1].y == sin.Entiy[0].y) {
                equY = true;
            }
            else {
                equY = false;
            }
        }

        double dx = sin.Entiy[1].x - sin.Entiy[0].x;
        double dy = sin.Entiy[1].y - sin.Entiy[0].y;
        double pdx = p2.x - p1.x;
        double pdy = p2.y - p1.y;

        int si = -1;
        int ei = -1;
        if (equY) {
            if (dx > 0 && pdx < 0) {
                rxsPointCouldp zjp = p1;
                p1 = p2;
                p2 = zjp;
            }
            if (dx > 0) {//从小到大
                for (int i = 0; i < sin.pNums; i++) {
                    if (sin.Entiy[i].x > p2.x) {
                        ei = i;
                        break;
                    }
                    if (sin.Entiy[i].x > p1.x) {
                        if (si < 0) {
                            si = i;
                        }
                        continue;
                    }
                }
            }
            else
            {//从大到小
                for (int i = 0; i < sin.pNums; i++) {
                    if (sin.Entiy[i].x < p2.x) {
                        ei = i;
                        break;
                    }
                    if (sin.Entiy[i].x < p1.x) {
                        if (si < 0) {
                            si = i;
                        }
                        continue;
                    }
                }
            }
        }
        else if (equX) {
            if (dy > 0 && pdy < 0) {
                rxsPointCouldp zjp = p1;
                p1 = p2;
                p2 = zjp;
            }
            if (dy > 0) {
                for (int i = 0; i < sin.pNums; i++) {
                    if (sin.Entiy[i].y > p2.y) {
                        ei = i;
                        break;
                    }
                    if (sin.Entiy[i].y > p1.y) {
                        if (si < 0) {
                            si = i;
                        }
                        continue;
                    }
                }
            }
            else {
                for (int i = 0; i < sin.pNums; i++) {
                    if (sin.Entiy[i].y < p2.y) {
                        ei = i;
                        break;
                    }
                    if (sin.Entiy[i].y < p1.y) {
                        if (si < 0) {
                            si = i;
                        }
                        continue;
                    }
                }
            }
        }
        if (si < 0 || ei < 0) {
            return { 0,0 };
        }

        CloudEntity ce = { 0,(ei - si) };
        ce.Entiy = (rxsPointCouldp*)malloc(ce.pNums * sizeof(rxsPointCouldp));
        memcpy(ce.Entiy, &sin.Entiy[si], ce.pNums * sizeof(rxsPointCouldp));
        return ce;
    }
    return { 0,0 };
}


CloudEntity rxsToolKit::RoiAreaGet(u8 cutMode, CloudEntity& sin, rxsPointCouldp p1, rxsPointCouldp p2) {
    return myRoiareaGet(sin, p1, p2);
    //CP could(new CloudT);
    //CouldCopyFrom(sin, could);
    //PointT pt1;
    //PointT pt2;
    //pt1.x = p1.x;
    //pt1.y = p1.y;
    //pt1.z = p1.z;
    //pt2.x = p2.x;
    //pt2.y = p2.y;
    //pt2.z = p2.z;
    //CP ret = SliceCloud(could, pt1, pt2);
    //if (ret->points.size() > 0) {
    //    CloudEntity ce = { 0,ret->points.size() };
    //    ce.Entiy = (rxsPointCouldp*)malloc(ce.pNums * sizeof(rxsPointCouldp));
    //    pclCouldCopyTomy(ret, ce);
    //    return ce;
    //}
    //else {
    //    mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"roi流程执行未获取到!!");
    //}
    //return { 0,0 };
}

rxsPointCouldp rxsToolKit::roiPC_LRPointGet(CloudEntity& ce, u8 L_or_R) {
    CP could(new CloudT);
    CouldCopyFrom(ce, could);
    CP ret = Inflexion(could);
    if (ret == 0) { return { 0,0,0 }; }
    if (L_or_R == 'L') {
        return { ret->points[0].x,ret->points[0].y,ret->points[0].z };
    }
    else if (L_or_R == 'R') {
        return { ret->points[1].x,ret->points[1].y,ret->points[1].z };
    }
    return { 0,0,0 };
}


bool rxsToolKit::emptyLength(CloudEntity& cp, int rang, float& val, u8 dir) {
    CP cloud(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    else {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }

    if (notCalculating) { return true; }
    bool r = emptyLengthV(cloud, rang, val);
    return r;
}

float rxsToolKit::sharpnessEstimate(CloudEntity& cp, float order, u8 dir) {
    CP cloud(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    else {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }

    if (notCalculating) { return true; }
    return sharpnessEstimateV(cloud, order);
}


void rxsToolKit::preprocessFunctionParamsConvert(std::string& FuncParamsGrp, std::vector<std::string>& funcs, std::vector<std::vector<std::string>>& paramters) {
    std::vector<std::string> ppp = stringSplit(FuncParamsGrp, ',');
    for (unsigned x = 0; x < ppp.size(); x++) {
        std::vector<std::string> fps = stringSplit(ppp[x], '|');
        funcs.push_back(fps[0]);
        fps.erase(fps.begin());
        paramters.push_back(fps);
    }
}

bool rxsToolKit::preprocessFunction(CloudEntity cp, std::vector<std::string> funcs, std::vector<std::vector<std::string>> paramters, u8 dir) {
    CP cloud(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    else {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }

    if (notCalculating) { return true; }
    CP r = preprocessFunctionV(cloud, funcs, paramters);
    return (r->size() > 0);
}

bool rxsToolKit::getBoundOfLine(CloudEntity cp, std::string func, std::vector<std::string> paramter, u8 dir) {
    CP cloud(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    else {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }

    if (notCalculating) { return true; }
    CP r = getBoundOfLineV(cloud, func, paramter);
    return (r->size() > 0);
}

bool rxsToolKit::deformOfLaptop(CloudEntity normdoel, CloudEntity cp, CloudEntity couldbund, float order_deformation, float& result, u8 dir) {
    if (notCalculating) { return true; }
    CP cloud(new CloudT);
    CP model_cloud(new CloudT);

    if (dir == 0) {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
        CouldCopy(normdoel.Entiy, normdoel.pNums, model_cloud, 0);
    }
    else {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
        CouldCopy(normdoel.Entiy, normdoel.pNums, model_cloud, 0);
    }
    CP Cbnd(new CloudT);
    CouldCopyFrom(couldbund, Cbnd);

    float zv1 = Cbnd->points[0].y;
    Cbnd->points[0].y = Cbnd->points[0].z;
    Cbnd->points[0].z = 0;

    zv1 = Cbnd->points[1].y;
    Cbnd->points[1].y = Cbnd->points[1].z;
    Cbnd->points[1].z = 0;

    normlizeByBound(cloud, Cbnd);

    ColorCloudEntity ce_src;
    ce_src.Color = { 0,0,255,0 };
    ce_src.PC.pNums = cloud->width * cloud->height;
    ce_src.PC.Entiy = (rxsPointCouldp*)malloc(ce_src.PC.pNums * sizeof(rxsPointCouldp));
    pclCouldCopyTomy(cloud, ce_src.PC);
    if (!inAutoRun) {
        mainSys->FrontProcessMsgSender(CBC_COLORPCUSHOW, &ce_src);
    }
    float r = deformOfLaptopV(model_cloud, cloud, order_deformation);

    free(ce_src.PC.Entiy);
    return true;
}

bool rxsToolKit::lengthOfBound(CloudEntity cp, CloudEntity cloud_bound, bool consider_y, float& result, u8 dir) {
    CP cloud(new CloudT);
    CP bound(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
        CouldCopy_ZtoY(cloud_bound.Entiy, cloud_bound.pNums, bound, true, 0);
    }
    else {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
        CouldCopy_ZtoY(cloud_bound.Entiy, cloud_bound.pNums, bound, false, 0);
    }
    if (notCalculating) { return true; }
    result = lengthOfBoundV(cloud, bound, consider_y);
    return true;
}

std::vector<float> rxsToolKit::GapDenoise(CloudEntity cp, std::vector<float> xs, std::vector<float> coplanarityThreshold, std::vector<float> skewThreshold, int range, u8 dir) {
    CP cloud(new CloudT);
    if (dir == 0) {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, true, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }
    else {
        CouldCopy_ZtoY(cp.Entiy, cp.pNums, cloud, false, (inCouldSavePath.length() > 1) ? inCouldSavePath.c_str() : 0);
    }

    if (notCalculating) { return std::vector<float>(); }
    return GapDenoiseV(cloud, xs, coplanarityThreshold, skewThreshold, range);
}

void* rxsToolKit::trainModel(const std::vector<std::vector<float>>& X, const std::vector<float>& y)
{
    typedef void* (*trainModel)(const std::vector<std::vector<float>>&, const std::vector<float>& y);
    trainModel myfun = (trainModel)GetProcAddress(dllHander_v, "trainModel");
    if (myfun == 0) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return 0;
    }
    return myfun(X, y);
}

std::vector<float> rxsToolKit::predictLength(const std::vector<std::vector<float>>& X, void* model_in)
{
    typedef std::vector<float>(*predictLength)(const std::vector<std::vector<float>>&, void*);
    predictLength myfun = (predictLength)GetProcAddress(dllHander_v, "predictLength");
    if (myfun == 0) {
        mainSys->FrontProcessMsgSender(CBC_STR_W_MSG, (void*)"算法dll未能正确加载，请确认是否缺少相关第三方引用及位置是否正确");
        return std::vector<float>();
    }
    return myfun(X, model_in);
}


void rxsToolKit::CouldCopy_ZtoXY(rxsPointCouldp* cp, unsigned cpNums, bool isX) {
    if (isX) {
        for (unsigned x = 0; x < cpNums; x++) {
            cp[x].y = cp[x].z;
            cp[x].z = 0.0f;
        }
    }
    else {
        for (unsigned x = 0; x < cpNums; x++) {
            cp[x].x = cp[x].y;
            cp[x].y = cp[x].z;
            cp[x].z = 0;
        }
    }
}

