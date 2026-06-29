#pragma once

#include "rxsFixdal.h"
#include "protected_rxsToolKit.h"
#include <pcl/io/pcd_io.h>
#include <pcl/common/transforms.h>


typedef pcl::PointXYZ PointT;
typedef pcl::PointCloud<PointT> CloudT;
typedef CloudT::Ptr CP;

CP G_Cloud(new CloudT);

HMODULE	dllHander;


void CouldCopy(rxsPointCouldp* cp, unsigned cpNums, CP &cloud,const char *savePath);
void CouldCopy_ZtoY(rxsPointCouldp* cp, unsigned cpNums, CP& cloud, bool isX, const char* savePath);


bool pcdFileSave(rxsPointCouldp* cp, unsigned cpNums, const char* path) {
    if (path == 0) { return false; }
    if (cp == 0 || cpNums < 1) { return false; }
    CP cloud(new CloudT);
    CouldCopy(cp, cpNums, cloud, path);
    return true;
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


formatConvert::formatConvert()
{
}

formatConvert::~formatConvert()
{
}


bool rxsToolKit::Ready() {
    dllHander = LoadLibrary(L"czxToolkit.dll");
    //for (std::map<std::string, int>::iterator itr = tookMap.begin(); itr != tookMap.end(); itr++) {
    //    toolNames.push_back(itr->first);
    //}
    if (dllHander == 0)
        return false;
    ForeachMethods();
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
}

rxsToolKit::rxsToolKit() {
    isDebug = false;
    notCalculating = false;
    dllHander = 0;
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
    ToolKitAdd("YOGA变形度1", 11);
    ToolKitAdd("YOGA变形度2", 12);
}

short rxsToolKit::ToolKitCaller(std::string toolName, u8 dir, CloudEntity ce, rxsResultReport* result, roiArea* Rois, unsigned roiNums, void* extParam) {
    int wich = -1;
    bool ret = false;
    short erc = -3;
    if (!isValidCould(ce.Entiy, ce.pNums)) { return -2; }
    try {
        wich = tookMap[toolName];
        switch (wich)
        {
        case 0:
            return 0;
        case 1:
            ret = fitBSpline(ce.Entiy, ce.pNums, result, dir);
            break;

        case 2:
            if (Rois != 0 && roiNums > 0) {
                BaseFace bf = { 0 };
                bf.min_x = Rois[0].P[1].x;
                bf.min_y = Rois[0].P[1].y;
                bf.max_x = Rois[0].P[0].x;
                bf.max_y = Rois[0].P[0].y;
                ret = semiconductor(ce.Entiy, ce.pNums, bf, result, dir);
            }
            else {
                erc = -102;
            }
            break;

        case 3:
            if (Rois != 0 && roiNums > 0) {
                BaseFace bf = { 0 };
                bf.min_x = Rois[0].P[1].x;
                bf.min_y = Rois[0].P[1].y;
                bf.max_x = Rois[0].P[0].x;
                bf.max_y = Rois[0].P[0].y;
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
            //ret = keyboard(cp, cpNums, result, dir);
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
        default:
            erc = -1;
            break;
        }
    }
    catch (...) {
        std::cout << "算法处理异常" << std::endl;
        erc = -999;
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
    if (!getLength)
        return false;

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
    if (!func)
        return false;

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
    if (!func)
        return false;

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
    if (!func)
        return false;

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

    //MCR mcr;
    //mcr.PrfCR = { 2,0.025,1 };
    //outResult->push_back(mcr);
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

    if (!func)
        return false;

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
        for (int x = 0; x < outFlatness.size(); x++) {
            cLen = outFlatness[x].size();
            for (unsigned y = 0; y < cLen; y++) {
                //MCR mcr;
                //mcr.DisCR = { x,outDifVal(0, 1) ,outFlatness[x][y],1 };
                //Result->push_back(mcr);
            }
        }
        return true;
    }
    return false;
}


bool rxsToolKit::surfaceProfile(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result) {
    typedef double(__cdecl* surfaceProfileApi)(CP cloud, int);
    surfaceProfileApi func = (surfaceProfileApi)GetProcAddress(dllHander, "surfaceProfile");
    if (!func)
        return false;

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
    if (!func)
        return false;

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
    if (!func)
        return false;

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