#pragma once

#if RXSFIXDAL_EXPORTS ||RXSHRDWDAL_EXPORTS|| _CONSOLE
#include "../rxsImgProcess/imgProcess.h"
#else
#include "imgProcess.h"
#endif // 0


//运控实列开关
//#define			MOTIONDAL_OFF			1



class imgToolKitBlockExt :public imgToolKitBlock
{
public:
	using imgToolKitBlock::imgToolKitBlock;
protected:
	short runProcessActiver(rxsImg*, int nums, rxsResultReport* rr, const char* locFname) override;
	short QRCodeProcess(rxsImg*, rxsResultReport* rr);
	/// <summary>
	/// 光度检测法缺陷检测
	/// </summary>
	int PhotometricDefectDetection(const char* ctkName, rxsResultReport* rr);
	int ColorIdentify(const char* ctkName, rxsResultReport* rr);
};

class  CLASSEXPORT CompoundGather : public ImageableGather {
public:
	u8 dir;
	Movepath* mp;
	typedef struct _crcd {
		unsigned getPointNums;
		rxsPointCouldp* pointResult;
		bool isFinshed;
		float Sx;
		float Sy;
		float Len;
		int ID;
	}Crcdm;
	Crcdm crcd;

	CompoundGather();
	short EncTiggerTaber(ImageableSensorUnit*, u8 dir);
	bool GatherDataFilterConvert(rxsPointCouldp* rpc, rxsPointCouldp* srcPs, unsigned& PointNums, double space, u8 dir, rxsPoint spos);

	bool Gather(ImageableSensorUnit*) override;
	void Close() override;

	void ResultReportGrpSet(void*);
protected:
	bool GatherPrevProcess(ImageableSensorUnit*) override;
	bool GatherEndProcess(ImageableSensorUnit*) override;
	bool SensorCreater() override;
	void* resultCollection;
private:
	bool creteSensorDal(const char* key);
};


typedef struct _prodInfo {
	std::string Name;
	int No;
	std::string qrCode;
}ProductInfo;


class  CLASSEXPORT WorkProcessDal :public WorkProcess {
public:
	WorkProcessDal();
	typedef enum IgnoreType { ZVALFILTER = 1, CHKRR_SAVE = 2, ONlYMMOVE = 4, AUTOEXIT = 8, TDGSRC_SAVE = 16, TD_CAL = 32 };
	DialGauge	DigitalDG;

	void DialGaugeOpen(bool on = true);
	float DialGaugeReadVal();

	const char* ResultReportFieldForech(unsigned index);
	const char* ResultReportTempNameForech(unsigned index);
	const char* ResultReportTempFieldForech(const char* tpName, unsigned index);
	void ResultReportTempFieldAppends(const char* tpName, const char* Filed);
	void ResultReportCreater(const char* tempName, const char* rrName);
	const char* ResultReportForech(unsigned index);
	const char* ResultReportFiledsGet(const char* rrName, unsigned index);
	unsigned ResultReportRowCount(const char* rrName);
	const char* ResultReportFiledValGet(const char* rrName, unsigned rowIndex, unsigned index);
	const char* ResultReportFiledValGet(const char* rrName, unsigned rowIndex, const char* filedName);

	const char* ToolKitForech(unsigned index);
	const char* RoiAreaForech(unsigned index, unsigned& id, u8& Type, rxsPointCouldp[3]);
	void RoiAreaSet(const char* roiName, u8 Type, rxsPointCouldp[3], rxsPointCouldp* pdEnt, unsigned nums);
	void RoiReomve(const char* roiName);

	typedef struct _ComputerTx {
		const char* Name;
		unsigned roiNums;
		int* roiIDS;
		const char* toolKitName;
		const char* rrName;
		int rrID;
	}ComputerTx;

	ComputerTx CompTranForech(unsigned index);
	void CompTranSet(const char* comName, const char* roiName, const char* ToolKitName, const char* rrName);
	void CompTranCfgSave();
	short CompTranTester(const char* name, rxsPointCouldp* pdEnt, unsigned nums);
	short CompTranTester(unsigned, rxsPointCouldp* pdEnt, unsigned nums);
	short SensorOpen(bool isReopen = false);
	ImageableSensorUnit* ImageableSensorDefaultGet();
	/// <summary>
	/// 触发方式切换
	/// </summary>
	/// <param name="wich">需要切换到的触发方式</param>
	/// <returns></returns>
	short TiggerTaber(int wich);
	void GRR_ValGet(bool saveResult);
	bool SaveTestdata(int pNums, rxsPointCouldp* pcs, double* srcZdatas, u8 x_or_y, const char* fn);

	/// <summary>
	/// 带过滤转换，去除无效数据，needFilter = true 默认开启过滤，false 不过滤，只是转换
	/// </summary>
	/// <param name="PointNums">in/out ; in 原始点数， out 过滤后的有效点数</param>
	bool CalibrationPC_DataGet(rxsPointCouldp* rpc, rxsPointCouldp* srcPs, unsigned& PointNums, double space, u8 dir, rxsPoint spos);
	/// <summary>
	/// 清除掉所有结果测试
	/// </summary>
	void CheckResultClr();
	/// <summary>
	/// 保存运行完毕后的测试结果报告
	/// </summary>
	/// <param name="isAssemble">是否为汇总结果报告</param>
	void SaveCheckResultReport(bool isAssemble = false);
	/// <summary>
	/// 返回 已经检测出的结果数量
	/// </summary>
	short HasCheckResult(const char* rrName);
	/// <summary>
	/// 开启是否加工完成后测试结果报告的自动保存
	/// </summary>
	/// <param name="on">true 开启，false 关闭</param>
	void CheckResultReportSave(bool on = true);
	/// <summary>
	/// 开启是否加工完成后自动退出到出料位置
	/// </summary>
	/// <param name="on"></param>
	void AutoOutMatterOpen(bool on);
	/// <summary>
	/// 开启是否在2D采集中保存原图
	/// </summary>
	/// <param name="on"></param>
	void GatherSrcPicSave(bool on);

	/// <summary>
	/// Z轴安全位置
	/// </summary>
	/// <param name="chkDone">指示是否需要检测运动到位完成</param>
	void Zsaftypos(bool chkDone);

	/// <summary>
	/// 流程控制，算法计算时候是否需要开启可视化
	/// </summary>
	/// <param name="on"></param>
	void CheckVisualOpen(bool on = true);

	/// <summary>
	/// 采集数据Z值过滤开启
	/// </summary>
	void GatherZValueFilter(bool on = true);

	/// <summary>
	/// 流程测量中是否执行测量计算
	/// </summary>
	/// <param name="on"></param>
	void OnlyShowNoCalculation(bool on = true);
	void OnlyMoveNoScan(bool on = true);
	void InMatterPos_Move();
	void OutMatterPos_Move();
	void NoCalculationTwo(bool isOn = true);

	//光源控制
	void LightOpen(int Cno, CST_LightSource::ColorIndex ci);
	void LightClose(int Cno, CST_LightSource::ColorIndex ci);
	void LightInfoGet(short& Anum, u8* vals);
	void LightRegulate(int Cno, CST_LightSource::ColorIndex ci, u8 value);
	int AutoAllLightTakephoto(int starti = 1);
	int AutoLightIntensityTakephoto(const char* vals, CST_LightSource::ColorIndex ci);
	int AutoSevenLightColorTakephoto(u8 liv);

	void AlamRed(bool on = true) override;
	void AlamGreen(bool on = true) override;
	void AlamYellow(bool on = true) override;

	bool ResetPrevProcess(void* Axng, unsigned& num) override;
	bool ResetFinsh(bool isResting, unsigned& cnt) override;
	bool FacParamLoadSet(AxisInfo* axiGrps, unsigned axiNum) override;
	/// <summary>
	/// 加工暂停,暂停后可用Goon继续加工
	/// </summary>
	/// <returns>暂停成功 true,否则 false</returns>
	bool Pause() override;
	/// <summary>
	/// 加工继续
	/// </summary>
	/// <param name="cmdLine">in  一些需要传递的额外的内容</param>
	/// <returns>成功返回true,否则返回 false</returns>
	bool Goon(char* cmdLine) override;
	void Close();

	const char* imgToolsForeach(bool& isStrat);
	short imgProcessTester(const char* tkName);
	short ImgProcessFlowerSave(const char* dirPath = 0);
	short ImgProcessFlowerLoad(const char* dirPath = 0);
	ProductInfo product;

	void modelConvert(const char* inpcd, const char* outpcd);
	long getCycleTime();

	AdjustFocal adjF;
protected:
	/// <summary>
	/// 全局输入处理，可以是DI等硬件输入信号，也可以是其他一些软处理信号
	/// </summary>
	void InSignlProcess() override;
	void EmgStopdeal() override;
	bool AutoClearAlam() override;
	/// <summary>
	/// 加工逻辑处理，即加工开始后的具体作业流程逻辑。
	/// </summary>
	/// <param name="wrkStep"></param>
	/// <returns></returns>
	int WorkProcesser(int wrkStep);
	int WorkProcess();
	bool StartReady(const char* cmdLine) override;
	/// <summary>
	/// 结束批次（作业流程）-将检测当前加工中的状态，直到这个流程做完，才会结束
	/// </summary>
	/// <returns></returns>
	bool StopBatchCompleted() override;
	/// <summary>
	/// 结束作业流程-不关心当前的状态情况
	/// </summary>
	/// <returns></returns>
	bool StopJob() override;

	short FunctionNumCaller(int funcNo, const char* paramStr) override;
	/// <summary>
	/// 初始化加载方法
	/// </summary>
	/// <returns></returns>
	short InitLoader() override;
	bool WorkProParamConditionCheck(bool isLoad_or_Save, bool be_or_after) override;

private:
	typedef struct _runRecordParam {
		unsigned MmpRecordIndex;
		unsigned ScpRecordIndex;
		bool EndGatherInFac;

		/// <summary>
		/// 重复运行次数;
		/// </summary>
		int repRunCnt;
		/// <summary>
		/// 重复运行时稳定延时时间ms
		/// </summary>
		int repStableDms;
		unsigned hs_tiger;

	}RuningRecordParam;
	RuningRecordParam runRecordp;

	typedef struct _stdp
	{
		double k;
		double b;
		double pDiff;
		double nDiff;
		double fDiffVal;
	}_STDP;

	typedef struct 	_RunGatherRecord {
		unsigned curHadGetIndex;
		bool curHadGetOk;
	}RunGatherRecord;
	RunGatherRecord RunGr;

	/// <summary>
	/// 移动到路径的起点位置 
	/// </summary>
	short MoveToStartPoint(Movepath*, bool);

	/// <summary>
	/// 移动路径处理
	/// </summary>
	short MovePathProcess(Movepath*);
	short moveGatherPrevDeal(bool inRunning = true);
	short XmoveGather(Movepath* mp, Scanpoint* scp, bool inRunning = true);
	short YmoveGather(Movepath* mp, Scanpoint* scp, bool inRunning = true);
	short CreamGatherAndProcess(Movepath* mp, Scanpoint* scp);
	short CreamFlowerProcess(Movepath* mp, ImageableSensorUnit* isu);
	short onlyMove(Movepath* mp, Scanpoint* scp);
	void GetMoveMethodAndSet(u8, bool& isOnlyMove, bool& isSegm, MotionModel& mm, void*, Movepath*);

	/// <summary>
	/// 群组运动时候处理
	/// </summary>
	/// <param name="">in 运动移动点位</param>
	/// <param name="">in 扫描点位</param>
	/// <returns>成功返回0</returns>
	short GroupDirScanerProcess(Movepath*, Scanpoint*, MotionModel, const char* AxLabelName);

	/// <summary>
	/// 分割式运动组合处理
	/// </summary>
	short SegmentedScanerProcess(Movepath*, Scanpoint*);

	bool HpcmpCfgopen(Scanpoint* scp);
	void HpcmpCfgclose();
	static int HSpeedTiggerEnvent(const char*, unsigned);

	/// <summary>
	/// 切换运动模式
	/// </summary>
	/// <param name="">in 对应需要切换的轴的名称</param>
	/// <param name="Mode">in 要切换到的目标模式</param>
	void TabMotionMode(const char*, u8 Mode);

	void runGatherDataCalculatingAndSave(CompoundGather::Crcdm cdm, u8 dir);
	unsigned calScpNums;
	void flushResult(const char* path, const char* contData, unsigned cdLen);
	bool StraightnessGet(unsigned pointNums, double* X, double* Y, _STDP* outResult);

	void OutSaveFileNameSet(int, u8);
	void CreamSrcPicSave(int crID, rxsImg* img);
	void PointDataComputerAndSave(CompoundGather::Crcdm cdm, u8 dir);
	void LineDataComputerAndSave(CompoundGather::Crcdm cdm, u8 dir);
	void* rxsTK;
	void* rxsCT;
	void* rxsRoiGrps;
	void* rxsImgTk;
	rxsResultReportGrpsManger* rxsReportGrps;
	ResultReportGrpAssemble* rxsRRGA;
	imgTaskProcessCollection* rxsImgTPC;
	CST_LightSource lightSource;
};


/*
下面一些全局函数，方便在UI中使用
*/

RXSAPIS rxsMainFrame* rxsSystemReady();
RXSAPIS rxsMainFrame* rxsMainFrameGet();
RXSAPIS WorkProcessDal* WorkProcessDalGet();
RXSAPIS void rxsSystemRelease();
RXSAPIS short rxsCreateRandomPointCould(rxsPointCouldp* pcGrps, unsigned pcNum);
RXSAPIS bool pcdFileSave(rxsPointCouldp* cp, unsigned cpNums, const char* path);
