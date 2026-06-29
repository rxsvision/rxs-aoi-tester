#pragma once
#include "PointCloudProcess.h"

//#define GLOG_NO_ABBREVIATED_SEVERITIES
#define NOMINMAX
#define BOOST_USE_WINDOWS_H
#define _CRT_SECURE_NO_WARNINGS


	
typedef enum _LaptopType
{
	带网孔蓝色笔记本 = 0,
	蓝色笔记本 = 1,
	键盘 = 2,
	装配键盘 = 3,
	装配背板 = 4,
	装配背板_普雷茨特 = 5,
	装配键盘_普雷茨特 = 6,
	NB6767D_GYG9D = 7,
	NB6837A = 8,
	IDC40 = 9,
	IDC401X = 10,
	S7 = 11,
	S7_8Y = 12,
	S7_EDO
}LaptopType;

//变形类型
typedef enum _DeformationType {
	UP,
	DOWN,
	OTHER,
	EMPTY
}DeformationType;


class CLASSEXPORT rxsToolKit {
public:
	bool isDebug;
	bool notCalculating;
	bool inAutoRun;

	rxsToolKit();
	~rxsToolKit();

	bool Ready(rxsMainFrame* sys);
	void SavePathSet(std::string path);
	void setExparam(void* param);

	CloudEntity pcdReader(std::string fp);
	short ToolKitCaller(std::string toolName, u8 dir, CloudEntity, rxsResultReport*, /*roiArea* */ RoiArea *, unsigned, int id, void* extParam);
	void ToolKitAdd(std::string toolName, int mid);
	std::string* ToolsNameGet(unsigned& len);
	short Run(u8 dir, CloudEntity, rxsResultReport*, /*roiArea* */ RoiArea*, void* extParam);


	CloudEntity ModelReaderAndProcess(std::string mod, rxsPointCouldp* modBund);
	void LineContourMatchingGet(std::string pcf, std::string mod, rxsPointCouldp* modBund, std::string spcBundstr, std::string FuncParamsGrp);
	rxsPointCouldp roiPC_LRPointGet(CloudEntity& ce, u8 L_or_R);
	CloudEntity RoiAreaGet(u8 cutMode, CloudEntity& sin, rxsPointCouldp p1, rxsPointCouldp p2);

	//bool colorReg(const char* m_root, const char* obj_dir, rxsResultReport* Result);
	//辅助功能-为了使computeDeformationNake中使用的模型与采集的模型 空间上对齐
	bool initModelNake(rxsPointCouldp* cp, unsigned cpNums, const char* sfp);
	bool initModelNake(const char* srcPCDfile, const char* sfp);

	//以下计算算子来自于算法dll
	bool fitBSpline(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result, u8 dir);
	bool semiconductor(rxsPointCouldp* cp, unsigned cpNums, BaseFace bf, rxsResultReport*, u8 dir);
	bool PIN(rxsPointCouldp* cp, unsigned cpNums, BaseFace bf, rxsResultReport*, u8 dir);
	/// <summary>
	/// 配准,当outResult 为空时候为不满足两幅图效果不需要进行计算，不为空时候则说明有两幅需要进行计算
	/// </summary>
	bool registration(rxsPointCouldp* cp, unsigned& cpNums, std::string& rtTx, rxsResultReport* outResult = 0);
	bool rtConvert(rxsPointCouldp* cp, unsigned& cpNums, std::string rtTx, rxsResultReport* outResult = 0);
	bool keyboard(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*, u8 dir);
	bool surfaceProfile(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*);
	bool computeConvexHull(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*);
	bool volume(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport*);
	bool dis2dis(rxsPointCouldp* cp, unsigned cpNums);
	//计算2D和变形量
	bool computeDeformationNake(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result, u8 dir,int id);
	bool computeDeformationNake(rxsPointCouldp* cp, unsigned cpNums, std::string modeFile, rxsResultReport* Result, u8 dir);
	bool LaptopCalculation(rxsPointCouldp* cp, unsigned cpNums, rxsResultReport* Result, u8 dir, int id);




	float predict2D(float deform, float sharpness, float dis_2D, std::string modeName, std::string id);
	/// <summary>
	/// 训练展平尺寸模型
	/// </summary>
	/// <param name="X">输入数据， 多组未加压数据， 每组数据所包含元素数目要一致</param>
	/// <param name="y">输入数据， 多个加压后的2D尺寸</param>
	/// <param name="model_name">预留参数，无实际用途</param>
	/// <returns>返回训练后的模型指针</returns>
	void* trainModel(const std::vector<std::vector<float>>& X, const std::vector<float>& y);
	/// <summary>
	/// 预测展平尺寸
	/// </summary>
	/// <param name="X">输入多组待预测数据，   每组数据所包含元素数目要一致，且与训练时长度一样 </param>
	/// <param name="model_in">训练好的模型</param>
	/// <param name="model_name">预留参数，无实际用途</param>
	/// <returns>返回每组数据所预测的展平尺寸</returns>
	std::vector<float> predictLength(const std::vector<std::vector<float>>& X, void* model_in);

	/// <summary>
	/// 计算锋锐度
	/// </summary>
	/// <param name="cloud">待计算锋锐度的点云点云， 建议输入未经处理刚采集到的点云， 或者经过preprocessFunction处理过的</param>
	/// <param name="order">阶数，建议设置成4， 这个值越大对尖锐信息越敏感，但是grr也会越差</param>
	/// <returns></returns>
	float sharpnessEstimate(CloudEntity& cloud, float order, u8 dir);


	bool emptyLength(CloudEntity& cp, int rang, float& val, u8 dir);

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
	bool getBoundOfLine(CloudEntity cloud, std::string func, std::vector<std::string> paramter, u8 dir);

	/// <summary>
	/// 变形度计算
	/// </summary>
	/// <param name="model">数模点云，点顺序必须是按照x从小到大排列</param>
	/// <param name="model_bound">数模边界，第一个点是左边的点，第二个点是右边的点</param>
	/// <param name="cloud">实际点云，必须是经过preprocessFunction预处理的</param>
	/// <param name="cloud_bound">点云边界，第一个点是左边的点，第二个点是右边的点-----getBoundOfLine()函数处理得来</param>
	/// <param name="order_deformation">这个值越大，变形度越关注大形变区域（有装配件，或者是其他数模和样品不太一致情况会对变形度的干扰越大），
	/// 越小则变形度越关注总体趋势（装配件和其他因素导致的数模与样品不一致情况干扰越小）。 推荐设置为1</param>
	/// <returns>输出变形度， 四幅输入点云都会被修改， model_bound被改成上凸最严重的两个点， cloud_bound被改成下凹最严重的点</returns>
	bool deformOfLaptop(CloudEntity normdoel, CloudEntity cloud, CloudEntity couldbund, float order_deformation, float& result, u8 dir);

	/// <summary>
	/// 计算内P长宽
	/// </summary>
	/// <param name="cloud">扫描点云</param>
	/// <param name="cloud_bound">点云边界</param>
	/// <param name="consider_y">是否考虑y方向落差，(考虑的话 grr容易受边界翘曲影响)</param>
	/// <returns>点云边界的距离</returns>
	bool lengthOfBound(CloudEntity cloud, CloudEntity cloud_bound, bool consider_y, float& result, u8 dir);

	/// <summary>
	/// 计算先下跳变再上跳变的两个落差
	/// </summary>
	/// <param name="cloud">输入点云</param>
	/// <param name="xs">开始判断的x坐标， 从该位置向后查找符合条件的段差， 向后指的是索引向后</param>
	/// <param name="coplanarityThreshold">共面阈值， 低于该阈值的视为同一个面上的点</param>
	/// <param name="skewThreshold">异面阈值， 高于该阈值的视为不同面上的点</param>
	/// <param name="range">一个面最少的点数目</param>
	/// <returns></returns>
	std::vector<float> GapDenoise(CloudEntity ce, std::vector<float> xs, std::vector<float> coplanarityThreshold, std::vector<float> skewThreshold, int range, u8 dir);

	/// <summary>
	/// 执行内置预处理函数，和一些用户给定的预处理，
	/// </summary>
	/// <param name="cloud">输入点云</param>
	/// <param name="funcs">预处理函数名， 可以为空，如果为空则仅执行内置的预处理函数
	///支持如下函数
	/// filterInterval  它用于滤掉x方向离群的噪点   参数是float threshold, int range；；； threshold越大会删掉越多的点，range聚集在一起的噪点最多能有多少个，  这两个参数推荐设置成0.1 和 5
	/// 
	/// filterGap  它用于滤掉y方向离群的噪点  参数是float threshold, int range；；； threshold越大会删掉越多的点，range聚集在一起的噪点最多能有多少个，  这两个参数推荐设置成0.1 和 3
	/// </param>
	/// <param name="paramters">预处理函数对应的参数</param>
	/// <returns></returns>
	bool preprocessFunction(CloudEntity cloud, std::vector<std::string> funcs, std::vector<std::vector<std::string>> paramters, u8 dir);
	void preprocessFunctionParamsConvert(std::string&, std::vector<std::string>&, std::vector<std::vector<std::string>>&);




	void CouldCopy_ZtoXY(rxsPointCouldp* cp, unsigned cpNums, bool isX);
private:
	rxsMainFrame *mainSys;
	Movepath cmp;
	std::string inCouldSavePath;
	std::map<std::string, int> tookMap;
	void ForeachMethods();
	struct _apiInfo {
		std::vector<std::string> Names;
		std::vector<std::string> Addrs;
		std::vector<std::string> Nos;
	}ToolsInfos;
	bool isValidCould(rxsPointCouldp* cp, unsigned cpNums);
	void ToolsInfosClr();
	void RunparamAnasyer(const char*);
};
