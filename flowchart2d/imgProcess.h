#pragma once

#if RXSFIXDAL_EXPORTS ||RXSHRDWDAL_EXPORTS|| _CONSOLE
#include "../rxsHrdwdal/rxsHrdwdal.h"
#else
#include "rxsHrdwdal.h"
#endif // 0

#define	BLKOUTRESULTYPE_NONE			0L

#define BLKOUTRESULTYPE_IMG				1U
#define BLKOUTRESULTYPE_SHAPE_CIRCLE	2U
#define BLKOUTRESULTYPE_SHAPE_RECT		4U
#define BLKOUTRESULTYPE_SHAPE_PLOY		8U
#define BLKOUTRESULTYPE_POINTSET		16U
#define BLKOUTRESULTYPE_CONTOUR			32U

#define BLKOUTRESULTYPE_PC				128U
#define BLKOUTRESULTYPE_DATASET			256U


typedef short (*imgTaskProcessEnvt)(rxsImg*);
class imgTaskFlowerProcesser;


class CLASSEXPORT imgprocToolbase
{
public:
	imgprocToolbase(rxsImg*);
	imgprocToolbase(const char* path, int flag);
	~imgprocToolbase();
	enum ChannelTYPE
	{
		R,
		G,
		B
	};

	rxsImg ChannelGet(ChannelTYPE ctpe);
	rxsImg Gray();
	rxsImg GrayToRGB();
	/// <summary>
	/// ThresType二值化操作类型 -1,0,1,2,3,4,7,8,16=>自适应,THRESH_BINARY（二值化）,二值化反，截断二值化...
	/// </summary>
	rxsImg Thresholding(short ThresType = -1, double thrv = 0.0f, double maxv = 255);
	rxsImg RoiClipping(rxsPointI s, rxsPointI wh);
	rxsImg RoiClipping(rxsPointI center, rxsPointI wh, double deg, rxsRectI& rect);
	rxsImg RoiClipping(rxsRectI rin, rxsRectI& rect);
	rxsImg Scaling(float xRate, float yRate);
	rxsImg xMirror();
	rxsImg yMirror();
	rxsImg Rotation();
	void WndShowPicture(rxsImg* ri, const char* wndName, bool watk = false);
	void Save(const char* path, rxsImg* ri = 0);
	rxsImg* OutHistoryGet(unsigned no);
protected:
	rxsImg OutDataSet(void* d, u8 channels, unsigned rows, unsigned cols);
	unsigned OutCount();
	rxsImg* outSelected();
	rxsImg* SinImg;
private:
	void* result;
};

class CLASSEXPORT imgTool_Draw :public imgprocToolbase
{
public:
	using imgprocToolbase::imgprocToolbase;

	typedef struct _idpen {
		u8 RGB[3] = { 0,255,0 };
		int thickness = 1;
		int lineType = 8;
		double fontSize = 0.5;
	}idrawPen;

	void SetPen(idrawPen pen);
	idrawPen* GetPen();
	void Circle(rxsPointI center, int raduis);
	void Rectangle(rxsPointI s, rxsPointI e);
	void Rectangle(rxsRectI rect);
	void Rectangle(rxsPointI center, rxsPointI wh, float deg);
	void Polygon(rxsPointI* pGrps, unsigned pNums, rxsPointI offset);
	void Shape(rxsPointI* pGrps, unsigned pNums, rxsPointI offset);
	void Elliptical();
	void Text(const char* text, rxsPointI* org);
private:
	idrawPen pen;
};


class CLASSEXPORT imgTool_Features {
public:
	imgTool_Features();
	~imgTool_Features();

	void Extract(rxsImg*);
	void Extract(const char*, int flag);

	float Match(const char*, int flag);
	float Match(rxsImg*);
private:
	void* p;
};

class CLASSEXPORT imgTool_Filter :public imgprocToolbase
{
public:
	using imgprocToolbase::imgprocToolbase;
	rxsImg Filter();
	rxsImg Gassus();
	rxsImg Middle();
	rxsImg Average();
};

class CLASSEXPORT imgTool_EdgeCheck :public imgprocToolbase {
public:
	using imgprocToolbase::imgprocToolbase;
	rxsImg Canndy();
	rxsImg Sobel();
};


class CLASSEXPORT imgTool_Template {
public:
	imgTool_Template(const char* tempPath, int flag);
	imgTool_Template(rxsImg* tempData);
	~imgTool_Template();

	void Match(const char* path, int flag);
	void Match(rxsImg*);
	unsigned Count();
	rxsPointI Read(unsigned index);
	void Save(const char* path);
private:
	void* mr;
	void* temp;
	void* sin;
	void* P[2];
	char fb;
	void* rcps;
};


class CLASSEXPORT imgTool_CodeDataReader :public imgprocToolbase
{
public:
	using imgprocToolbase::imgprocToolbase;
	~imgTool_CodeDataReader();
	enum CODETYPE
	{
		QR,
		DM,
		BARCODE
	};

	void Reader(CODETYPE, unsigned& Count);
	unsigned Count();
	const char* Code(unsigned index, rxsPointI** pos);
private:
	void* codes;
	void qrCodeReader(rxsImg*, unsigned& nums);
	void barCodeReader(rxsImg*, unsigned& nums);
	void dmCodeReader(rxsImg*, unsigned& nums);
};

class CLASSEXPORT imgTool_Contour :public imgprocToolbase
{
public:
	using imgprocToolbase::imgprocToolbase;
	~imgTool_Contour();
	typedef struct limt {
		float max;
		float min;
	}Limt;

	typedef struct _filterCondition {
		Limt Area;
		Limt PointNum;
		int thrvMethod;//0 = THRESH_BINARY,1 = THRESH_BINARY_INV,
		int thrvAdj;
		int GassKd;
	}FilterCondition;

	int Get(FilterCondition fc);
	int Count();
	rxsPointI* Reader(unsigned i, unsigned& outnums);
	rxsRectI ReaderBoundBox(unsigned i);
	rxsRectI ReaderMinAreaRect(unsigned i);
private:
	void* filterContour = 0;
	void* curConvertrpip = 0;
};


class CLASSEXPORT imgTool_Calibration
{
public:
	imgTool_Calibration(const char* path);
	~imgTool_Calibration();
	void DistortionTest(const char* files, int rowCorrnums, int colCorrnums);
	void TransformTest();
	rxsImg Calibrate(rxsImg*);
	void Save();
private:
	char* curpath;
	int plen;
	void* Params[4] = { 0,0,0,0 };
	bool SaveParam(const char* path);
	bool ReadParam();
	rxsImg out;
};


class CLASSEXPORT  imgToolKitBlock {
public:
	imgToolKitBlock();
	~imgToolKitBlock();
	int MethodNum();
	bool MethodExist(const char* methodName);
	const char* MethodNameForeach(bool& isStart);
	void inputClr();
	void inputPush(rxsImg* img, int nums);
	//用户扩展算法调用
	bool RunUser(const char*);
	//自身内容集成算法调用
	bool Run(const char*);
	void RunEnvironmentSet(rxsImg* outResult, rxsResultReport* rreport);
protected:
	virtual short runProcessActiver(rxsImg* inimg, int nums, rxsResultReport* rr, const char* Fname);
	rxsImg* Photometric(rxsImg*);
	bool ColorRecognition(rxsImg*);
	rxsImg* GetInImg(int& Num);
	CST_LightSource* lightSource;
	rxsMainFrame* pSystemer;
	rxsImg* imgResult;
	rxsResultReport* outResultReport;
private:
	void* nmpm;
	void* nameMapIter;
	void AddmethodNamemap(const char* name, int no);
	void* inGrps;
};


typedef struct _imgfilterparam
{
	//滤波类型，
	//0: 均值滤波
	//1: 高斯滤波
	//2: 中值滤波
	//3: 最大值滤波
	//4: 最小值滤波
	//5: 方差滤波
	unsigned char fType;
	std::string fparam;
}ImgFilterParam;


typedef struct _fromlinker {
	int id;
	int outChoose;
}FromLinker;


class CLASSEXPORT StepBlockBase {
public:
	typedef enum _mytype {
		CONDCMP_PROCESS = 0,//条件比较器
		COLORREG_UCM_PROCESS = 1,//用户自定义颜色识别
		IMG_PREV_PROCESS = 2,//归一化、二值化、灰度图处理
		IMG_PROCESS = 3,//裁剪，缩放，镜像等操作
		HDR_UCM_PROCESS = 1,//hdr
		LBPM_UCM_PROCESS = 1,//光度立体法
		FILTER_PROCESS = 6,//2D图像滤波处理
		THRD_PROCESS = 16,//海康VM 等第三方的流程块
		IMG_IN_SOURCE = 32,
		IMG_OUT = 64,
		IMG_UCUSTOM_PROCESS = 65,//用户自定义
		ROIGRPMANGER = 70,

		IMG_SCANCODE = 128,
		PC_IN_SOURCE = 129,//点云输入
		PC_OUT = 130,//点云输出		
		PC_FILTER = 131,//点云滤波
		PC_ARC = 132,//弧长
		PC_POA_SL = 133,//profile of a line 线轮廓度, profile of  a surface 面轮廓度
		CONVERT_DEPIMG_PC = 898,
		MOTION_MOV = 1000,
		DSIGNAL_OUT = 1001
	}SBTYPE;
	StepBlockBase(int id);
	StepBlockBase(int id, const char* name);
	StepBlockBase(int id, unsigned bsType, const char* name);
	~StepBlockBase();
	int ID() { return _id; }
	const char* Name();
	void Name(const char*);
	unsigned blkType();
	void UpdateRenderPos(rxsPointI pos);
	rxsPointI* GetRenderPos();

	void PushImg(rxsImg img);
	void PushImg(const char* imgPath, int flag = 1);

	virtual bool InputParamStringCfgSet(std::string* p, int l);
	virtual std::string InputParamSaveStringGet();
	void RunReady();
	virtual short Run();
	unsigned  OutType() { return outResultType; }

	//-----以下两个方法需要优化--一旦内部传出的指针 在外部修改变化之后，就容易出现各种BUG----
	bool GetResult(rxsResultReport**);
	bool GetResult(const rxsImg**);
	bool GetResult(const CloudEntity**);
	//-------以上两个方法需要优化--一旦内部传出的指针 在外部修改变化之后，就容易出现各种BUG-----
	StepBlockBase* self();
	void operator()(StepBlockBase*);
	bool operator ==(StepBlockBase*);
	void DependencyMount(void *);
	void* InParamsGet();
	virtual void Release();
protected:
	void InParamAlloc(void*, int len);
	rxsResultReport rr;
	rxsImg outImg;
	CloudEntity outCe;
	char* sbName;
	//StepBlockBase* prev;
	void outimgAlloc(rxsImg*);
	unsigned _type;
	void inmgClr();
	void ResultClr();
	rxsPointI RenderPos;
	int _id;
	unsigned outResultType;
	StepBlockBase* fromSrc(int id);
private:
	void* tfpDependency;
	void* inparams;
	void* inmgGrps;
	unsigned inplen;
};



typedef struct _prevprocessparam {
	/// <summary>
	/// 0 灰度图-多通道图转化，1 二值化，2 归一化
	/// </summary>
	u8 pType;
	union param
	{
		//pType == 0 时候使用指定多通道图片转化类型
		int GCCType;

		//pType == 1 二值化时候使用的参数
		struct thrvParam {
			// 0 自适应-放法0，1 自适应-放法1,2 自适应-放法2,....0xFF 指定阈值
			u8 type;
			//指定值, 或自适应偏移量
			short s_offset;
			u8 maxVal;
			u8 minVal;
		};
	}Params;
}PrevProcessParam;


class CLASSEXPORT ImgPreProcessStep : public StepBlockBase {
public:
	ImgPreProcessStep(int id);
	ImgPreProcessStep(int id, const char* name);
	~ImgPreProcessStep();

	void PrevProcessInputParamSet(PrevProcessParam);
	PrevProcessParam* PrevProcessInputParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;

};



class CLASSEXPORT ImgProcessStep : public StepBlockBase {

};



typedef struct _locsensorparam {
	char selectedCream[20];
	CST_LightSource::Partition Lcp[4];
}LocSensorParam;

typedef struct _imgsourceparam
{
	//源类型，
	//0: 设备硬件采集
	//1: 本地路径读取
	//2: 外部传入
	unsigned char sType;
	//bit 0 保存原图（源类型为0 生效），
	//bit 1 启动光源开启控制 （源类型为0 生效）,
	//bit 2 是否采用拷贝输入 （源类型为2 生效）,
	//bit 3 如果存在处理结果图片是否推送到前端显示
	//bit 4 输出图片类型-灰度图
	unsigned char ssFlag;
	union InParam
	{
		LocSensorParam	lsp;
		char locReadPath[256];
		rxsImg* imgBase;
	}inParam;
	std::string savePath;
}ImgSourceParam;


class CLASSEXPORT ImgSourceStep : public StepBlockBase {
public:
	ImgSourceStep(int id);
	ImgSourceStep(int id, const char* name);

	void ImgSourceInputParamSet(ImgSourceParam);
	ImgSourceParam* ImgSourceInputParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
};


typedef struct _imgoutparamer
{
	FromLinker srcLinker;
	//需要融合的内容来源块ID
	int renderingNums;
	FromLinker** renderingIds;
	std::string savePath;//输出保存路径，0为不开启，开启为具体路径
	std::string saveName;//输出保存文件名
}ImgOutParamer;

class CLASSEXPORT ImgOutStep : public StepBlockBase {
public:
	ImgOutStep(int id);
	ImgOutStep(int id, const char* name);
	~ImgOutStep();
	void ImgOutInputParamSet(ImgOutParamer);
	ImgOutParamer* ImgOutInputParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
	void Release() override;

	void RenderDataClr();
	void RenderDataPush(FromLinker *data);
private:
	void* dyinpdata;
	void ConvertToPoint(const char* str, void* p);
};


typedef struct _thridblkparam {
	FromLinker srcLinker;
	char path[512];
	char inImgName[30];
	char outDataName[30];
	char outImgName[30];
}ThridPartyCallBlkParam;

class CLASSEXPORT ThridPartyCallBlk : public StepBlockBase {
public:
	ThridPartyCallBlk(int id);
	ThridPartyCallBlk(int id, const char* name);

	void ThridPartyInputParamSet(ThridPartyCallBlkParam);
	ThridPartyCallBlkParam* ThridPartyInputParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
};


typedef struct _scancodeparam {
	FromLinker srcLinker;
	//扫码类型 0 条码， 1 二维码 ，2 数据矩阵
	u8 codeType;
	bool roiEnable;
	rxsRectI roiArea;
}ScanCodeParam;

class CLASSEXPORT ScanCodeStep : public StepBlockBase {
public:
	ScanCodeStep(int id);
	ScanCodeStep(int id, const char* name);
	void ScanCodeInputParamSet(ScanCodeParam scp);
	ScanCodeParam* ScanCodeInputParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
};

typedef struct _ucparam {
	FromLinker srcLinker;
	short wichMethod;
	const char* params;
}UseCustomParam;

class CLASSEXPORT UserCustomStep : public StepBlockBase {
public:
	UserCustomStep(int id);
	UserCustomStep(int id, const char* name);
	void UserCustomParamSet(UseCustomParam);
	UseCustomParam* UserCustomParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
	void Release() override;
	const char *foreachMethod(int &index);
private:
	void* p;
};

typedef struct _colorrecogparam {
	FromLinker srcLinker;
	rxsRectI roiArea;
	bool roiEnable;
	u8 lightVal;
	char selectedCream[20];
	char rootDir[256];
	char objDir[256];
}ColorRecogParam;


class CLASSEXPORT ColorRecognitionStep : public StepBlockBase {
public:
	ColorRecognitionStep(int id);
	ColorRecognitionStep(int id, const char* name);
	void ColorRecognitionParamSet(ColorRecogParam);
	ColorRecogParam* ColorRecognitionParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
	void rpicGather(const char* savedir);
};


typedef struct _formatconvert {


}FormatConvertParam;

class CLASSEXPORT FormatConvertStep : public StepBlockBase {
public:
	FormatConvertStep(int id);
	FormatConvertStep(int id, const char* name);
	void FormatConvertParamSet(FormatConvertParam);
	FormatConvertParam* FormatConvertParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};




typedef struct _motionmovparam {
	int			AxNum;
	AxMoveparam* AxisLister;
}MotionMovParam;

class CLASSEXPORT MotionMovStep : public StepBlockBase {
public:
	MotionMovStep(int id);
	MotionMovStep(int id, const char* name);
	void MotionMovParamSet(MotionMovParam);
	MotionMovParam* MotionMovParamParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
	void Release() override;
	void AxListCreater(int AxNums, AxMoveparam *amps);
private:
	void* names;
};




typedef struct _pclocsensorparam {
	const char* selectedSensor;
	/// <summary>
	/// 方式：0 编码器，1 硬件IO 触发， 2 连续
	/// </summary>
	u8 method;
	//相关配置参数
	const char* cfgPath;
	MotionMovParam mmp;
}LocPcSensorParam;

typedef struct _pointcouldsrcparam {
	//源类型，
	//0: 设备硬件采集
	//1: 本地路径读取
	//2: 外部传入
	unsigned char sType;
	union _PCParams
	{
		LocPcSensorParam Lpcsp;
		CloudEntity pce;
		const char* locPath;
	}pcParams;
	//bit 0 点云图，bit 1 深度图,bit 2 数据保存
	u8 outFlag;
}PointCouldSourceParam;


class CLASSEXPORT PointCouldSourceStep : public StepBlockBase {
public:
	PointCouldSourceStep(int id);
	PointCouldSourceStep(int id, const char* name);

	void PointCouldSourceParamSet(PointCouldSourceParam);
	PointCouldSourceParam* PointCouldSourceParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
	void Release() override;
	void InParamUpdateRefush();
	void AxListCreater(int AxNums, AxMoveparam* amps);
private:
	void* sn;
	void* path;
	void* names;
	void locSensorCfgPathSet(const char* path);
	void locSensorSelectedSet(const char* name);
	void LocalPathSet(const char* path);
};


typedef struct _pointcouldoutparam
{


}PointCouldOutParam;

class CLASSEXPORT PointCouldOutStep : public StepBlockBase {
public:
	PointCouldOutStep(int id);
	PointCouldOutStep(int id, const char* name);
	void PointCouldOutParamSet(PointCouldOutParam);
	PointCouldOutParam* PointCouldOutParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};


typedef struct _pointcouldfilteparam
{


}PointCouldFilterParam;

class CLASSEXPORT PointCouldFilterStep : public StepBlockBase {
public:
	PointCouldFilterStep(int id);
	PointCouldFilterStep(int id, const char* name);
	void PointCouldFilterParamSet(PointCouldFilterParam);
	PointCouldFilterParam* PointCouldFilterParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};



typedef struct _pointcouldarcparam {



}PointCouldArclenParam;

class CLASSEXPORT PointCouldArclenStep : public StepBlockBase {
public:
	PointCouldArclenStep(int id);
	PointCouldArclenStep(int id, const char* name);
	void PointCouldArclenParamSet(PointCouldArclenParam);
	PointCouldArclenParam* PointCouldArclenParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};


typedef struct _pointcouldpoaslparam {
	//面轮廓度,线轮廓度
	u8 tp_s_or_l;

}PointCouldPoaslParam;

class CLASSEXPORT PointCouldPoaslStep : public StepBlockBase {
public:
	PointCouldPoaslStep(int id);
	PointCouldPoaslStep(int id, const char* name);
	void PointCouldPoaslParamSet(PointCouldPoaslParam);
	PointCouldPoaslParam* PointCouldPoaslParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};

typedef struct _roigrpmanger {


}roiMangeParam;


class CLASSEXPORT roiGrpMangerStep : public StepBlockBase {
public:
	roiGrpMangerStep(int id);
	roiGrpMangerStep(int id, const char* name);
	void roiGrpParamSet(roiMangeParam);
	roiMangeParam* roiGrpParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};


class CLASSEXPORT ComparisonStep : public StepBlockBase {
public:
	ComparisonStep(int id) :StepBlockBase(id) { _type = StepBlockBase::CONDCMP_PROCESS; }
	ComparisonStep(int id, const char* name) :StepBlockBase(id, name) { _type = StepBlockBase::CONDCMP_PROCESS; }

	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
};


typedef struct _canuseoutlinker {
	int id;
	const char* Name;
	unsigned outType;
}cuOutLinker;

class CLASSEXPORT imgTaskFlowerProcesser {
public:
	imgTaskFlowerProcesser(const char* flowerName);
	~imgTaskFlowerProcesser();
	const char* FlowerName();
	void ResultReportGrpSet(rxsResultReportGrpsManger* rrpm, rxsMainFrame* fSystemer);
	void ProcessEnvtSet(const char* ctkName, imgTaskProcessEnvt);

	template <typename T>
	void StepBlockPush(T* sbb, int nums = 1)
	{
		std::vector<StepBlockBase*>* tkc = (std::vector<StepBlockBase*>*)tkCollection;
		for (int i = 0; i < nums; i++) {
			T* newSbb = new T(totalID);
			(sbb[i])((StepBlockBase*)newSbb);
			newSbb->DependencyMount(this);
			tkc->push_back(newSbb);
			totalID += 1;
			linkInsert(newSbb);
		}
	}

	void StepBlockRemove(StepBlockBase*);
	void StepBlockRemove(const char* blkName, int id);
	int StepBlockCount();
	short Run();
	short RunAnsy();
	bool operator ==(imgTaskFlowerProcesser*);
	StepBlockBase* operator[](const int id);
	StepBlockBase* StepBlockGet(int index);
	StepBlockBase* StepBlockGet(const char* sbName, int id);
	imgTaskFlowerProcesser* self();
	void CopyTo(imgTaskFlowerProcesser*);

	void Save(const char*);
	bool Load(const char*);
	void Clear();
	short SignalBlkTester(StepBlockBase*);
	short SignalBlkTester(int id, const char* path, int flag);
	int CanUseID();
	void CanUseID(int setVal);
	cuOutLinker* ForeachCanUseOutLinker(int index);
private:
	short Ready();
	void linkInsert(StepBlockBase *sbb);
	rxsResultReportGrpsManger* rrpmg;
	rxsMainFrame* pSystemer;
	void* tkCollection;
	void* pemaps;
	char* locFlowerName;
	void* linkLister;
	int totalID;
	void* outlinkTable;
	bool getValer(int cid, u8 outType);
};

class CLASSEXPORT imgTaskProcessCollection {
public:
	imgTaskProcessCollection(rxsResultReportGrpsManger* ras, rxsMainFrame* fSystemer, CST_LightSource* ls, void* rtk);
	~imgTaskProcessCollection();
	const char* Foreach(bool& isStart);

	bool Run(bool isAnsy = false);
	int Count();
	void Clear();
	void TaskFlowerProcessPush(const char* taskName);
	void TaskFlowerProcessPush(imgTaskFlowerProcesser*);
	bool TaskFlowerProcessRemove(imgTaskFlowerProcesser*);
	imgTaskFlowerProcesser* Selected(int index);
	imgTaskFlowerProcesser* Selected(const char* tskName);
	imgTaskFlowerProcesser* operator[](int index);
	imgTaskFlowerProcesser* operator[](const char* tskName);

	/// <summary>
	/// 保存流程集合，path 为路径 文件夹，不是文件名
	/// </summary>
	short Save(const char* dirPath);
	/// <summary>
	/// 导入流程集合，path 为流程集合文件路径 文件夹，不是文件名
	/// </summary>
	/// <param name="dirPath"></param>
	/// <returns></returns>
	short Load(const char* dirPath);
private:
	void anasyloader(int cui,const char* fp, int len);
	void* imgTaskGrps;
	imgTaskFlowerProcesser* curItp;
	void linCreater(const char *name, const char* fp);
};


RXSAPIS int LoadThridSolutionAndExer(const char* solPath, const char* passworld, rxsImg* inImg, rxsResultReport* rpp, rxsImg* out);
