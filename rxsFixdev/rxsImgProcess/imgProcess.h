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
rxsPointCouldp getBp(void*, int);

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
		IMG_MERG_PROCESS = 4,//hdr,
		IMG_LAMBPHOTOMETRIC = 5, //光度立体法
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
		PC_DEFORMATION = 134,//变形度
		PC_LENGTH = 135,//长度
		PC_EMPLEN = 136,//轮廓长度
		PC_GAP = 137,//断/落差
		PC_PREPROCESS = 138,//预处理
		PC_BOUNDOL = 139,//边界点获取

		HW_LIGHTSOURCE = 450,
		HW_FOCUSINGLENS = 451,
		TCP_SERVERRECV = 500,
		CONVERT_DEPIMG_PC = 898,
		MOTION_MOV = 1000,
		DSIGNAL_OUT = 1001
	}SBTYPE;
	StepBlockBase(int id);
	StepBlockBase(int id, const char* name);
	StepBlockBase(int id, unsigned bsType, const char* name);
	~StepBlockBase();
	int ID();
	const char* Name();
	void Name(const char*);
	unsigned blkType();
	void UpdateRenderPos(rxsPointI pos);
	rxsPointI* GetRenderPos();

	virtual bool InputParamStringCfgSet(std::string* p, int l);
	virtual std::string InputParamSaveStringGet();
	virtual void RunReady();
	virtual short Run();
	unsigned  OutType();
	virtual const char* GetCanOutSelecter(int& index);
	//-----以下两个方法需要优化--一旦内部传出的指针 在外部修改变化之后，就容易出现各种BUG----
	bool GetResult(rxsResultReport**);
	bool GetResult(const rxsImg**, int index = -2);
	bool GetResult(const CloudEntity**, int index = -2);
	//-------以上两个方法需要优化--一旦内部传出的指针 在外部修改变化之后，就容易出现各种BUG-----
	StepBlockBase* self();
	void operator()(StepBlockBase*);
	bool operator ==(StepBlockBase*);
	void DependencyMount(void *);

	template <typename T>
	void ParamSet(T p) {
		int len = sizeof(T);
		InParamAlloc(&p, len);
		//if (inparams == 0) {
		//	inparams = new T;
		//}
		//(*(T*)inparams) = p;
	}

	template <typename T>
	T* ParamGet() {
		T* p = (T*)InParamsGet();
		if (p == 0) {
			T isp;
			memset(&isp, 0, sizeof(T));
			ParamSet(isp);
			p = (T*)InParamsGet();
			if (p == 0) { return 0; }
		}
		return p;
	}

	void* InParamsGet();
	virtual void Release();
	bool HadCompleted();
	void PushIn(rxsImg img);
	void PushIn(const char* imgPath, int flag = 1);
	void PushIn(CloudEntity ce);
protected:
	virtual void* MultResultOut(int index);
	u8 status;
	void InParamAlloc(void*, int len);
	rxsResultReport rr;
	rxsImg outImg;
	CloudEntity outCe;
	char* sbName;
	void outimgAlloc(rxsImg*);
	unsigned _type;
	void MultinClr();
	void ResultClr();
	rxsPointI RenderPos;
	int _id;
	unsigned outResultType;
	StepBlockBase* fromSrc(int id);
	/// <summary>
	/// 多输入分配
	/// </summary>
	/// <param name="isPC_or_img">1-rxsImg,2-点云，3-cv::Mat</param>
	void MultinGrpsAlloc(u8 isPC_or_img);
	void* MultinGet(int index);
	void PushIn(void* p);
	void* MultinGrps;
	void* tfpDependency;
private:
	void* inparams;
	unsigned inplen;
	u8 inGrpsType;
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
	float ExposureTime;
	short tzVal;
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
	const char* savePath;
}ImgSourceParam;


class CLASSEXPORT ImgSourceStep : public StepBlockBase {
public:
	ImgSourceStep(int id);
	ImgSourceStep(int id, const char* name);
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
	void outSavePathSet(const char* p);
private:
	std::string osfp;
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
	//void ImgOutInputParamSet(ImgOutParamer);
	//ImgOutParamer* ImgOutInputParamGet();
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
	float scale;
	int tageNum;
}ScanCodeParam;

class CLASSEXPORT ScanCodeStep : public StepBlockBase {
public:
	ScanCodeStep(int id);
	ScanCodeStep(int id, const char* name);
	//void ScanCodeInputParamSet(ScanCodeParam scp);
	//ScanCodeParam* ScanCodeInputParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
};

typedef struct _ucparam {
	FromLinker srcLinker;
	const char* wichMethod;
	const char* params;
}UseCustomParam;

class CLASSEXPORT UserCustomStep : public StepBlockBase {
public:
	UserCustomStep(int id);
	UserCustomStep(int id, const char* name);
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
	void Release() override;
	const char* foreachMethod(int& index);
	void CallSet(const char* mname, const char* paramlist);
private:
	void* p;
};

typedef struct _colorrecogparam {
	FromLinker srcLinker;
	rxsRectI roiArea;
	bool roiEnable;
	const char* samplePath;
	const char* modelPath;
	const char* gatherPath;
}ColorRecogParam;


class CLASSEXPORT ColorRecognitionStep : public StepBlockBase {
public:
	ColorRecognitionStep(int id);
	ColorRecognitionStep(int id, const char* name);
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
	bool modeTrain(const char* sample, const char* savPath);
	void pathSet(const char* sample, const char* savPath, const char* gatPath);
private:
	std::string modPath;
	std::string sampPath;
	std::string gatherPath;
};



typedef struct _imlbmparam {
	float slant;
	float titl;
}imLbmParam;

typedef struct _imhdrparam {
	/// <summary>
	/// 曝光时间，单位s
	/// </summary>
	float times;
	/// <summary>
	/// 焦距融合对应多图不同焦距
	/// </summary>
	int adjFocus;
}imHdrParam;

typedef struct {
	/// <summary>
	/// 融合类型,1-HDR,2-fuse（多幅不同焦距的图片融合）
	/// </summary>
	u8 imType;
	/// <summary>
	/// 融合数量
	/// </summary>
	int Nums;
	/// <summary>
	/// 方式来源  0-本地目录，1-fromLinker,
	/// </summary>
	u8 imMethod;
	union _methond
	{
		FromLinker* srcLinker;
		const char* locDir;
	} Method;
	imHdrParam* hf;
	bool isAnsy;
	/// <summary>
	/// bit 0 灰度图输出，
	/// </summary>
	unsigned outFlag;
}ImgMergingParam;

class CLASSEXPORT ImgMergingStep : public StepBlockBase {
public:
	ImgMergingStep(int id);
	ImgMergingStep(int id, const char* name);
	~ImgMergingStep();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;

	void FromLinkerSet(FromLinker*, int);
	void HdrFiSet(imHdrParam*, int len);
	void LocDirSet(const char* dirpath);
private:
	short LoadDirFiles(const char* dirPath,void *);
	void* locDir;
	int flrNum;
	void* flrGrps;
	std::vector<std::string> img_paths;
	std::vector<float> slants;
	std::vector<float> titls_times;
};


typedef struct {
	/// <summary>
	/// 融合数量
	/// </summary>
	int Nums;
	/// <summary>
	/// 方式来源  0-本地目录，1-fromLinker
	/// </summary>
	u8 imMethod;
	union _methond
	{
		FromLinker* srcLinker;
		const char* locDir;
	} Method;
	imLbmParam* Lcp;
	bool isAnsy;
	/// <summary>
	/// bit 0 灰度图输出，
	/// </summary>
	unsigned outFlag;
}LambertianPhotoMetriParam;



class CLASSEXPORT LambertianPhotoMetricstereoStep : public StepBlockBase {
public:
	LambertianPhotoMetricstereoStep(int id);
	LambertianPhotoMetricstereoStep(int id, const char* name);
	~LambertianPhotoMetricstereoStep();
	void RunReady() override;
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
	void FromLinkerSet(FromLinker*, int);
	void LbmSet(imLbmParam*, int l);
	void LocDirSet(const char* dirpath);
private:
	short LoadDirFiles(const char* dirPath, void*);
	std::string locDir;
	int flrNum;
	void* flrGrps;
	std::vector<std::string> img_paths;
	std::vector<float> slants;
	std::vector<float> titls;
	void fer(void* org);
};


typedef struct _formatconvert {
	FromLinker srcLinker;
	bool isPicToPC;
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
		const char* locPath;
	}pcParams;
	//bit 0 点云图，bit 1 深度图,bit 2 数据保存
	u8 outFlag;
	const char* outSavePath;
}PointCouldSourceParam;


class CLASSEXPORT PointCouldSourceStep : public StepBlockBase {
public:
	PointCouldSourceStep(int id);
	PointCouldSourceStep(int id, const char* name);
	~PointCouldSourceStep();

	void PointCouldSourceParamSet(PointCouldSourceParam);
	PointCouldSourceParam* PointCouldSourceParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
	void InParamUpdateRefush();
	void AxListCreater(int AxNums, AxMoveparam* amps);
	void outSavePathSet(const char* fp);
private:

	void* curRunIab;
	void* mGpio;
	std::string osfp;
	std::string sn;
	std::string path;
	std::vector<std::string> names;
	void locSensorCfgPathSet(const char* path);
	void locSensorSelectedSet(const char* name);
	void LocalPathSet(const char* path);
	void PointDataComputer(rxsPointCouldp*, unsigned, float, rxsPoint Spos, u8);
	bool PointGatherDataFilterConvert(rxsPointCouldp* rpc, rxsPointCouldp* srcPs, unsigned long& PointNums, double space, u8 dir, rxsPoint spos);
};


typedef struct _pointcouldoutparam
{
	FromLinker srcLinker;

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
	FromLinker srcLinker;

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


typedef struct _xscst {
	float xs;
	float coplanarityThreshold;
	float skewThreshold;
}Xscstp;

typedef struct _pointcouldgapparam
{
	FromLinker srcLinker;
	u8 dir;
	int range;
	int pNum;
	Xscstp* p;
}PointCouldGapParam;

class CLASSEXPORT PointCouldGapStep : public StepBlockBase {
public:
	PointCouldGapStep(int id);
	PointCouldGapStep(int id, const char* name);
	~PointCouldGapStep();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;

	void XscstpPush(Xscstp* p);
	void XscstpClr();
private:
	void* ps;
};

typedef struct _predlenparam {
	int pNum;
	float* LenGrp;
	void* pModelin;
}PredLenParam;

typedef struct _lenparam {
	bool fixedBound;
	FromLinker boundLinker;
	u8 dir;
	int rang;
	bool consider_y;
	rxsPointCouldp Bound[2];
}measureLenParam;

typedef struct _pointcouldlenparam
{
	//是为预测长,否 为内P长框
	bool ispredict;
	FromLinker srcLinker;
	union _pomParam
	{
		measureLenParam Mlp;
		PredLenParam	Plp;
	} Params;
}PointCouldLenParam;

class CLASSEXPORT PointCouldLengthStep : public StepBlockBase {
public:
	PointCouldLengthStep(int id);
	PointCouldLengthStep(int id, const char* name);
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};

typedef struct _pointcouldemplenparam
{
	FromLinker srcLinker;
	/// <summary>
	/// 是否为锋锐度测试
	/// </summary>
	bool isShapnesmate;
	u8 dir;
	int rang;//间隙参数
	float spm_order;//锋锐度参数
}PointCouldEmplenParam;

class CLASSEXPORT PointCouldEmplenStep : public StepBlockBase {
public:
	PointCouldEmplenStep(int id);
	PointCouldEmplenStep(int id, const char* name);
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};


typedef struct _pointcouldarcparam {
	FromLinker srcLinker;
	u8 dir;
	bool isDebug;
}PointCouldArclenParam;

class CLASSEXPORT PointCouldArclenStep : public StepBlockBase {
public:
	PointCouldArclenStep(int id);
	PointCouldArclenStep(int id, const char* name);
	//void PointCouldArclenParamSet(PointCouldArclenParam);
	//PointCouldArclenParam* PointCouldArclenParamGet();
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};

typedef struct _prepropcparam {
	/// <summary>
	/// 输入方式 0 sdkin, 1- 输入源,2- 外部文件
	/// </summary>
	u8 SinMethhod;
	FromLinker srcLinker;
	u8 dir;
	int funcNum;
}PointCouldPreProParam;


class CLASSEXPORT PointCouldPreProcessStep : public StepBlockBase {
public:
	PointCouldPreProcessStep(int id);
	PointCouldPreProcessStep(int id, const char* name);
	~PointCouldPreProcessStep();

	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;

	/// <summary>
	/// 压入预处理方法名称及参数
	/// </summary>
	/// <param name="n">方法名</param>
	/// <param name="ps">方法参数，以英文字符'|'分隔的参数</param>
	void funNameParamSet(const char* n, const char* ps);
	void funNameParamGet(int i, std::string& name, std::string& ps);
	void funNameParamClr();
private:
	void* insp;
};


typedef struct _pcboundofline {
	FromLinker srcLinker;
	u8 isRoifp;
	u8 dir;
	const char* funName;
	/// <summary>
	/// 以英文'|'分隔的参数
	/// </summary>
	const char* funParams;
}PointCouldBoundolParam;

class CLASSEXPORT PointCouldBoundolStep : public StepBlockBase {
public:
	PointCouldBoundolStep(int id);
	PointCouldBoundolStep(int id, const char* name);
	~PointCouldBoundolStep();

	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;

	void FunNameSet(const char* name);
	void FunParamsSet(const char* params);

private:
	void* insp;
	void funnSet(const char**, void*);
	void funpSet(const char**, void*);
};


typedef struct _pointcoulddeformationparam {
	FromLinker srcLinker;
	FromLinker boundLinker;
	u8 dir;
	float order_deformation;
	const char* modefile;
	rxsPointCouldp ModeBound[2];

}PointCouldDeformationParam;


class CLASSEXPORT PointCouldDeformationStep : public StepBlockBase {
public:
	PointCouldDeformationStep(int id);
	PointCouldDeformationStep(int id, const char* name);
	~PointCouldDeformationStep();

	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;

	void ModelFilePathSet(const char* fp);
private:
	void* modefn;
};


typedef struct _pointcouldpoaslparam {
	FromLinker srcLinker;
	//==1线轮廓度,==2面轮廓度,
	u8 tp_s_or_l;
	bool roiEnable;
	rxsRectI roiArea;

}PointCouldPoaslParam;

class CLASSEXPORT PointCouldPoaslStep : public StepBlockBase {
public:
	PointCouldPoaslStep(int id);
	PointCouldPoaslStep(int id, const char* name);
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
};

typedef struct _roirect {
	//bit 0 copy 标志，bit 1 相对标志，(bit 2,bit 3)=>0 起点终点模式 1 起点偏移模式 2中心长宽高模式 3保留
	u8 Flag;
	//起点或中心点
	rxsPointCouldp SORC;
	rxsPointCouldp End;
	//长宽高(也可以是相对roi时候相对参考的偏移量)或对角结束点
	rxsPointCouldp LWH;
	//相对偏移的参考roi 名称
	std::string forRi;
	std::string rName;
}roiRect;

typedef struct _roigrpmanger {
	bool isThreeD;
	FromLinker srcLinker;
	int roiNums;
	roiRect* roiArea;
}roiMangeParam;

RXSAPIS	short RoiConvertAbsToRel(roiRect& ref, roiRect& src, roiRect& outTage);
RXSAPIS short RoiConvertRelToAbs(roiRect& ref, roiRect& src, roiRect& outTage);

class CLASSEXPORT roiGrpMangerStep : public StepBlockBase {
public:
	roiGrpMangerStep(int id);
	roiGrpMangerStep(int id, const char* name);
	~roiGrpMangerStep();
	const char* GetCanOutSelecter(int& nums) override;
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;
	void roiAreaSet(roiRect*, int num);
	void roiAreaSet(roiRect&);
protected:
	void* MultResultOut(int index) override;
private:
	typedef struct _isrmp {
		std::string path;
		std::vector<roiRect> roiAreas;
		std::vector<CloudEntity> outRoiPC;
	}Isrmp;
	Isrmp ep;
	void outClr();
};


class CLASSEXPORT ComparisonStep : public StepBlockBase {
public:
	ComparisonStep(int id) :StepBlockBase(id) { _type = StepBlockBase::CONDCMP_PROCESS; }
	ComparisonStep(int id, const char* name) :StepBlockBase(id, name) { _type = StepBlockBase::CONDCMP_PROCESS; }

	short Run() override;
	bool InputParamStringCfgSet(std::string *p,int l) override;
	std::string InputParamSaveStringGet() override;
};

class CLASSEXPORT TcpServerRecvStep : public StepBlockBase {
public:
	TcpServerRecvStep(int id);
	TcpServerRecvStep(int id, const char* name);
	~TcpServerRecvStep();

	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;


	void ipprConfig(const char* ip, int port);
	const char* ipprConfig(int& port);
	void sendTxt(const char*);
	const char* sendTxt();
private:
	void* p;
};


class CLASSEXPORT FocusingLensStep : public StepBlockBase {
public:
	FocusingLensStep(int id);
	FocusingLensStep(int id, const char* name);

	unsigned short AdjValue();
	void AdjValue(unsigned short);
	unsigned DmsValue();
	void DmsValue(unsigned);
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;

private:
	unsigned short adjVal;
	unsigned dms;
};




typedef struct {
	/// <summary>
	/// 通道号或者对应多光谱扇区号
	/// </summary>
	u8 ChannelNo;
	unsigned short Value;
	/// <summary>
	/// 多光谱颜色选择
	/// </summary>
	CST_LightSource::ColorIndex curCi;
}lsValueSet;

typedef struct {
	/// <summary>
	/// 对应设定的数量
	/// </summary>
	int setNums;
	lsValueSet* lsVs;
	int dms;
}msclspParam;

class CLASSEXPORT LightSourceStep : public StepBlockBase {
public:
	LightSourceStep(int id);
	LightSourceStep(int id, const char* name);
	short Run() override;
	bool InputParamStringCfgSet(std::string* p, int l) override;
	std::string InputParamSaveStringGet() override;

	/// <summary>
	/// 光源类型，  0-多通道光源， 1-多光谱光源
	/// </summary>
	u8 lsType();
	void lsType(u8);
	void LSParamClr();
	void mscSet(lsValueSet);
private:
	std::vector<lsValueSet> msclsp;
	u8 lstype;
};




typedef struct {
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
	short Ready();
	int Run();
	short RunAnsy();
	bool operator ==(imgTaskFlowerProcesser*);
	StepBlockBase* operator[](const int id);
	int index(const int id);
	StepBlockBase* StepBlockGet(int index);
	StepBlockBase* StepBlockGet(const char* sbName);
	StepBlockBase* StepBlockGet(const char* sbName, int id);
	StepBlockBase* StepTypeFind(StepBlockBase::SBTYPE);
	imgTaskFlowerProcesser* self();
	void CopyTo(imgTaskFlowerProcesser*);

	void Save(const char*);
	bool Load(const char*);
	void Clear();
	void Linker(int sid, int TageID);
	int* Linker(int curid, unsigned& nums);
	short SignalBlkTester(StepBlockBase*);
	short SignalBlkTester(int id, const char* path, int flag);
	int CanUseID();
	void CanUseID(int setVal);
	cuOutLinker* ForeachCanUseOutLinker(int index);

	bool HadFinshed();
	void ResultReportSummary();
	void RunResourceRelease();
protected:
	void ProcessEnvtSet(const char* ctkName, imgTaskProcessEnvt);
private:
	void RunResourceEndProcess();
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
	imgTaskProcessCollection(rxsResultReportGrpsManger* ras, rxsMainFrame* fSystemer, void* rtk);
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
	bool HadFinshed();
	void ResultReportAssember();
private:
	void anasyloader(int cui,const char* fp, int len);
	void* imgTaskGrps;
	imgTaskFlowerProcesser* curItp;
	void linCreater(const char *name, const char* fp);
};


RXSAPIS int LoadThridSolutionAndExer(const char* solPath, const char* passworld, rxsImg* inImg, rxsResultReport* rpp, rxsImg* out,
	const char* srcinBlkName, const char* outdataBlkName, const char* outimgBlkName);



template <typename T>
void imgTaskFlowerStepBlockCreater(imgTaskFlowerProcesser* itfp, rxsPointI pi, const char* name) {
	int canUsedID = itfp->CanUseID();
	T isp(canUsedID, name);
	isp.UpdateRenderPos(pi);
	itfp->StepBlockPush(&isp);
}