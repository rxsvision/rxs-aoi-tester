#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/wechat_qrcode.hpp>

#include "protected_rxsToolKit.h"
#include "protected_rxsToolKit_2D.h"

#pragma warning(disable : 4996)

using namespace cv;


typedef struct _SystemEnvObj {
	rxsMainFrame* pSystemer;
	rxsResultReportGrpsManger* rrpmg;
	rxsToolKit* rtk;
	rxsToolKit2D* rtk2d;
}HrdWrkSyetemEnv;


typedef struct _linknode {
	int sbbID = -1;
	StepBlockBase* curSbbptr = 0;
	std::vector<_linknode*>nextGrps;
	std::vector<_linknode*>prevGrps;
}LinkNoder;


typedef struct _linker {
	int Sid;
	int Eid;
}Linker;


class LinkLister
{
public:
	LinkLister();
	~LinkLister();

	bool Insert(StepBlockBase* v, int index);
	bool InsertAttend(int Aid,StepBlockBase* v, int index);
	LinkNoder *Find(int id);

	std::string SaveStringGet();
	bool LineCfgStringLoader(std::string str);
	void isolated(std::vector<StepBlockBase*>&);
	void LinkerPush(int sid, int eid);
	bool Remove(int id);
	int* NextStepID(int curid, unsigned& Nums);
	void Clear();
private:
	std::map<int, std::vector<int>> linkGrp;

	bool fillRoot(StepBlockBase* v);
	void releaseLister(LinkNoder*);
	void prevnullsetUpdate(LinkNoder*);
	LinkNoder* root;
	LinkNoder* end;
	LinkNoder* curptr;
};

typedef struct _fdlinfo {
	int ID = 0;
	unsigned blkType = 0;
	rxsPointI pos = { -1,-1 };
	std::string name;
	std::vector<int> from;
	std::vector<int> tolinker;
	std::vector<std::string> params;
}FdlInfo;

class rxsVisualizationDescriptionFile
{
public:
	rxsVisualizationDescriptionFile(imgTaskFlowerProcesser*, LinkLister*);
	~rxsVisualizationDescriptionFile();

	bool reLoader(const char* path);
	void Save(const char* newfile);

	template <typename T>
	void StepBlkCreater(FdlInfo* fdli) {
		T* sbb = new T(fdli->ID, fdli->name.c_str());
		sbb->UpdateRenderPos(fdli->pos);
		if (fdli->params.size() == 0) {
			msgRecord((fdli->name + " 导入解析->块无参数!需设置!").c_str());
			return;
		}
		//注意此处位置顺序不能调整,也千万不要优化这个操作
		itfptr->StepBlockPush<T>(sbb, 1);
		T* issNew = (T*)(*itfptr)[fdli->ID];
		bool icfret = issNew->InputParamStringCfgSet(&fdli->params[0], fdli->params.size());
		if (!icfret) {
			msgRecord((fdli->name + " 参数导入解析失败!").c_str());
			return;
		}
	}

private:
	bool fillValue(int, void*, const char*);
	bool getValue(const char*, int, int, void*);
	bool AnasyerAndFillStruct(const char* content, int len);
	const char* saveConvert();
	imgTaskFlowerProcesser* itfptr;
	LinkLister* Lkr;
	void blkCreater(FdlInfo* v);
	void msgRecord(const char* msg);
};


typedef struct _scancodedresult {
	std::string code;
	rxsPointI	pos[4];
}SCResult;


std::vector<SCResult>	cv_wechat_qrdecode(rxsImg*, unsigned num, bool roiEnb, rxsRectI roiArea, float scale);
std::vector<SCResult>	dmtx_dmdecode(rxsImg*, unsigned num, bool roiEnb, rxsRectI roiArea, float scale);
void leastSquaresMethod(const Mat& A, const Mat& B, Mat& X);



std::string GetFileNameFormPath(const  char *path);

bool pcdFileSave(rxsPointCouldp* cp, unsigned cpNums, const char* path);
bool pcdFileRead(std::string fp, CloudEntity& out);


#ifdef HK_VMASTER



#include "IVmSolution.h"
#include "IVmProcedure.h"
#include "IVmImageSource.h"
#include "IVmSolution.h"
#include "VMException.h"
#include "IVmGlobalVariable.h"

using namespace VisionMasterSDK;
using namespace VisionMasterSDK::VmSolution;
using namespace VisionMasterSDK::ImageSourceModule;
using namespace VisionMasterSDK::GlobalVariableModule;

class HkVmProcesser {
public:
	HkVmProcesser(std::string flowerName, std::string solPath, std::string passworld);
	~HkVmProcesser();

	bool InputSet(std::string inModName, rxsImg* img);
	int Run();
	int GetResult(std::string flowObjName, rxsResultReport* rpp);
	int GetResultImg(std::string flowObjName, rxsImg*);

private:
	std::string fname;
	IVmSolution* m_pVmSol;
};


#endif // HK_VMASTER
