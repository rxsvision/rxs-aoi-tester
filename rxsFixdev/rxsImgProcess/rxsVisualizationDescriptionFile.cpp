
#include "private.h"
#include <iostream>
#include <fstream>
#include <fstream>



extern HrdWrkSyetemEnv hrdSyetemEnver;


rxsVisualizationDescriptionFile::rxsVisualizationDescriptionFile(imgTaskFlowerProcesser* eptr, LinkLister* ll)
{
    itfptr = eptr;
    Lkr = ll;
}

rxsVisualizationDescriptionFile::~rxsVisualizationDescriptionFile()
{
    Lkr = 0;
    itfptr = 0;
}


bool rxsVisualizationDescriptionFile::reLoader(const char* path) {
    std::ifstream inputFile;
    bool ret = false;
    inputFile.open(path);
    if (inputFile.is_open()) {
        std::string line;
        byte segType = 0;
        int errNums = 0;
        while (std::getline(inputFile, line, '\n')) {
            std::cout << line << std::endl;
            int l = line.length();
            if (l < 1) {
                segType = 0;
                continue;
            }
            if (line[0] == '\x1D') {
                segType = 1;
                continue;
            }
            if (segType == 1) {
                Lkr->LineCfgStringLoader(line);
            }
            else if (!AnasyerAndFillStruct(line.c_str(), l)) {
                hrdSyetemEnver.pSystemer->SysLogRecord((line + " =>Fail").c_str());
                errNums += 1;
            }
        }
        ret = (errNums == 0);
    }
    else {
        std::cout << "Failed to open the file." << std::endl;
    }
    inputFile.close();
    return ret;
}


const char* rxsVisualizationDescriptionFile::saveConvert() {
    int l = itfptr->StepBlockCount();
    stringstream content;
    for (int i = 0; i < l; i++) {
        StepBlockBase* sbb = itfptr->StepBlockGet(i);
        if (sbb == 0) { continue; }
        unsigned btype = sbb->blkType();
        rxsPointI* p = sbb->GetRenderPos();
        content << sbb->ID() << " " << btype << " " << sbb->Name() << " " << std::to_string(p->x) << ',' << std::to_string(p->y) << " " << "from" << " " << "to" << " ";
        content << sbb->InputParamSaveStringGet();
        content << "\x3\n";
    }
    std::string rs = content.str();
    return rs.length() > 0 ? rs.c_str() : 0;
}


void rxsVisualizationDescriptionFile::Save(const char* newfile) {
    if (newfile == 0) { return; }
    std::ofstream outputFile;
    //in	以输入方式打开文件，允许读取文件内容。
    //out	以输出方式打开文件，允许写入文件内容。如果文件不存在，则创建新文件；如果文件已存在，则将其长度截为0。
    //app	在文件末尾附加内容，允许写入文件；如果文件不存在，则创建新文件。
    //ate	打开文件后立即定位到文件末尾。
    //trunc	如果文件已存在，则将截断文件长度为0（清空文件）。
    //binary	以二进制方式打开文件，而非文本模式。文件内容将以二进制格式进行读取和写入，而不是以文本格式。

    outputFile.open(newfile, std::ios_base::out);
    if (outputFile.is_open()) {
        const char* content = saveConvert();
        if (content != 0) {
            outputFile << content << "\n\n\x1D\n" << Lkr->SaveStringGet() << std::endl;
        }
    }
    else {
        std::cout << "Failed to open the file." << std::endl;
    }
    outputFile.close();
}


bool rxsVisualizationDescriptionFile::getValue(const char* content, int sl, int ei, void* outv) {
    int si = 0;
    si = ei - sl;
    if (si < 0) { return false; }
    char* sstr = (char*)malloc(sl + 1);
    if (sstr == 0) { return false; }
    memcpy(sstr, &content[si], sl);
    sstr[sl] = 0;
    (*(std::string*)outv) = sstr;
    free(sstr);
    return true;
}

bool rxsVisualizationDescriptionFile::fillValue(int SNo, void* s, const char* v) {
    FdlInfo* fdli = (FdlInfo*)s;
    if (SNo == 1) {
        fdli->ID = std::atoi(v);
    }
    else if (SNo == 2) {
        int val = std::atoi(v);
        fdli->blkType = (unsigned)val;
    }
    else if (SNo == 3) {
        fdli->name = v;
    }
    else if (SNo == 4) {
        if (fdli->pos.x < 0) {
            fdli->pos.x = atoi(v);
        }
        else {
            fdli->pos.y = atoi(v);
        }
    }
    else if (SNo == 5)
    {
        fdli->from.push_back(std::atoi(v));
    }
    else if (SNo == 6)
    {
        fdli->tolinker.push_back(std::atoi(v));
    }
    else if (SNo == 7)
    {
        fdli->params.push_back(v);
    }
    return true;
}

bool rxsVisualizationDescriptionFile::AnasyerAndFillStruct(const char* content, int len) {
    FdlInfo fdli;
    int sl = 0;
    int ei = 0;
    int SNo = 0;
    u8 isDH_or_Space = 0;
    std::string resultVal;
    for (int x = 0; x < len; x++) {
        char ch = content[x];
        switch (ch)
        {
        case ' ':
            if (isDH_or_Space < 1) {//过滤由','-> ' '的情况
                SNo += 1;
            }
            isDH_or_Space = 0;
            if (sl == 0) { break; }
            ei = x;
            if (!getValue(content, sl, ei, &resultVal)) {
                return false;
            }

            if (!fillValue(SNo, &fdli, resultVal.c_str())) {
                return false;
            }
            sl = 0;
            break;

        case '\r':
        case '\n':
        case '\x3':
        case ',':
            if (isDH_or_Space == 0) {
                // 由' ' -> ','
                SNo += 1;
            }
            if (sl == 0) { break; }
            isDH_or_Space += 1;
            ei = x;
            if (!getValue(content, sl, ei, &resultVal)) {
                return false;
            }
            if (!fillValue(SNo, &fdli, resultVal.c_str())) {
                return false;
            }
            sl = 0;
            break;
        default:
            sl += 1;
            break;
        }
    }
    blkCreater(&fdli);
    return true;
}

void rxsVisualizationDescriptionFile::msgRecord(const char* msg) {
    hrdSyetemEnver.pSystemer->SysLogRecord(msg);
}

void rxsVisualizationDescriptionFile::blkCreater(FdlInfo* fdli) {
    if (itfptr == 0) { return; }
    HrdWrkSyetemEnv* hwe = &hrdSyetemEnver;
    int index = 0;
    itfptr->CanUseID(fdli->ID + 1);
    if (fdli->blkType == StepBlockBase::IMG_IN_SOURCE) {
        StepBlkCreater<ImgSourceStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::IMG_OUT) {
        StepBlkCreater<ImgOutStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::IMG_MERG_PROCESS) {
        StepBlkCreater<ImgMergingStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::IMG_LAMBPHOTOMETRIC) {
        StepBlkCreater<LambertianPhotoMetricstereoStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::THRD_PROCESS) {
        StepBlkCreater<ThridPartyCallBlk>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::IMG_SCANCODE) {
        StepBlkCreater<ScanCodeStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::COLORREG_UCM_PROCESS) {
        StepBlkCreater<ColorRecognitionStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::IMG_PREV_PROCESS)
    {

    }
    else if (fdli->blkType == StepBlockBase::IMG_PROCESS) {

    }
    else if (fdli->blkType == StepBlockBase::IMG_UCUSTOM_PROCESS) {
        StepBlkCreater<UserCustomStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::MOTION_MOV) {
        StepBlkCreater<MotionMovStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_IN_SOURCE) {
        StepBlkCreater<PointCouldSourceStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_OUT) {
        StepBlkCreater<PointCouldOutStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_FILTER) {
        StepBlkCreater<PointCouldFilterStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_ARC) {
        StepBlkCreater<PointCouldArclenStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_POA_SL) {
        StepBlkCreater<PointCouldPoaslStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_DEFORMATION) {
        StepBlkCreater<PointCouldDeformationStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_LENGTH) {
        StepBlkCreater<PointCouldLengthStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_EMPLEN) {
        StepBlkCreater<PointCouldEmplenStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_GAP) {
        StepBlkCreater<PointCouldGapStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::ROIGRPMANGER) {
        StepBlkCreater<roiGrpMangerStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_BOUNDOL) {
        StepBlkCreater<PointCouldBoundolStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::PC_PREPROCESS) {
        StepBlkCreater<PointCouldPreProcessStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::TCP_SERVERRECV) {
        StepBlkCreater<TcpServerRecvStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::HW_FOCUSINGLENS) {
        StepBlkCreater<FocusingLensStep>(fdli);
    }
    else if (fdli->blkType == StepBlockBase::HW_LIGHTSOURCE) {
        StepBlkCreater<LightSourceStep>(fdli);
    }
}
