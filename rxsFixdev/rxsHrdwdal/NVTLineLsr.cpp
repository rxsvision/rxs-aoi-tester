#pragma once
#include "rxsHrdwdal.h"
#include "proteced.h"


#ifdef _MYX86

#include "../lvm_sdk/x86/include/lvm_basic.h"
#include "../lvm_sdk/x86/include/lvm_sdk.h"

#pragma comment(lib,"../lvm_sdk/x86/lib/nvt_lvm_sdk.lib")
#else

#include "../lvm_sdk/x64/include/lvm_basic.h"
#include "../lvm_sdk/x64/include/lvm_sdk.h"

#pragma comment(lib,"../lvm_sdk/x64/lib/nvt_lvm_sdk.lib")
#endif


using namespace std;

#pragma warning(disable : 4996)


#define DEVNUM			2
#define CAPTURETMS		30000
#define LINEBUFMAX		30000

#define	LVM2710			1


#ifdef LVM2710

#define ZRATE			0.0001
#define	XRATE			0.008
#define	YRATE			0.008

#define ENC_DIV_R		2
#define ENC_TRG_FREQ	200
#define ENC_RES			0.004

#elif LVM2730

#define	XRATE			0.012
#define	YRATE			0.016
#define ZRATE			0.0005

#define ENC_DIV_R		4
#define ENC_TRG_FREQ	200
#define ENC_RES			0.004	
#endif




typedef struct _DevInfo {
	lvm_dev_t* dev;
	lvm_buf_t* buf;

	int intensity_enable;
	int uniformity_enable;

	/// <summary>
	/// 异步回调采集完成标记
	/// </summary>
	bool cb_GatherFinsh;
	unsigned cb_Lines;

	rxsPointCouldp StartPoint;

}DevInfo;

string reopCmd;
int curSelectedDev;
DevInfo dev[DEVNUM];
vector<rxsPointCouldp>	vpc;


int frame_cb(lvm_dev_t* curDev, void* frame, void* data, int lines)
{
	if (frame == 0) {
		return 0;
	}

	lvm_depth_map_t* depthMap = (lvm_depth_map_t*)frame;
	//stringstream name;
	//name << depthMap->head.fid << "_" << depthMap->head.sid << "__demo_" << dev[curSelectedDev].cb_Lines << ".png";
	//lvm_save_img(name.str().c_str(), depthMap->data, depthMap->head.width, depthMap->head.height, LVM_IMAGE_FORMAT_16BIT_USHORT);

	unsigned long totalPoints = depthMap->head.width * depthMap->head.height;
	double x, y, z;
	int curIndex = 0;
	if (curSelectedDev == 0) {//X轴的坐标
		for (unsigned rows = 0; rows < depthMap->head.height; rows++) {
			for (unsigned cols = 0; cols < depthMap->head.width; cols++)
			{
				curIndex = rows * depthMap->head.width + cols;
				z = ((unsigned short*)(depthMap->data))[curIndex];
				if (z > 1000) {
					x = XRATE * rows + dev[curSelectedDev].StartPoint.x;
					y = YRATE * cols + dev[curSelectedDev].StartPoint.y;
					vpc.push_back({ x,y,z * ZRATE });
				}
			}
		}
		dev[curSelectedDev].cb_Lines += lines;
		dev[curSelectedDev].cb_GatherFinsh = true;
		return 0;
	}

	if (curSelectedDev == 1) {//Y轴的坐标
		for (unsigned rows = 0; rows < depthMap->head.height; rows++) {
			for (unsigned cols = 0; cols < depthMap->head.width; cols++)
			{
				curIndex = rows * depthMap->head.width + cols;
				z = ((unsigned short*)(depthMap->data))[curIndex];
				if (z > 1000) {
					x = XRATE * cols + dev[curSelectedDev].StartPoint.x;
					y = -YRATE * rows + dev[curSelectedDev].StartPoint.y;
					vpc.push_back({ x,y,z * ZRATE });
				}
			}
		}
		dev[curSelectedDev].cb_Lines += lines;
		dev[curSelectedDev].cb_GatherFinsh = true;
		return 0;
	}
	return 0;
}

NVTLineLsr::NVTLineLsr() {
	errCode = lvm_init_sdk(NULL, "./SysLog/NVTLog");
	memset(dev, 0, sizeof(DevInfo) * DEVNUM);
	curSelectedDev = 0;
	CtrInfo = new ControlInfo[2];
	memset(CtrInfo, 0, sizeof(ControlInfo) * DEVNUM);
	Sensorinfo.devType = DevType::LineLsr;
}

NVTLineLsr::~NVTLineLsr() {
	delete[] CtrInfo;
	CtrInfo = 0;
}

short NVTLineLsr::open(const char* tsPath) {
	int connect_dev_type = -1; //设备类型 -1表示sdk自动检测(依赖UDP) 1:2600系列 2:2800系列 3:2500系列 4:2800系列 5:2000系列 6:1800系列 传入其他值默认为1模式
	if (tsPath == 0) {
		return -1;
	}

	reopCmd = tsPath;
	int x = reopCmd.find(',');
	string fpx[DEVNUM];
	if (x == -1) {
		fpx[0] = reopCmd;
		fpx[1] = "";
	}
	else {
		fpx[0] = reopCmd.substr(0, x);
		fpx[1] = reopCmd.substr((x + 1), reopCmd.length() - x - 1);
	}

	for (unsigned x = 0; x < DEVNUM; x++) {
		if (fpx[x].length() > 4) {
			dev[x].dev = lvm_create_dev((char*)fpx[x].c_str(), connect_dev_type);
			if (!dev[x].dev)
			{
				return -1;
			}

			errCode = lvm_connect_dev(dev[x].dev);
			if (DEV_PARAM_DO_NOT_MATCH == errCode)
			{
				return -2;
			}
			else if (errCode != 0)
			{
				return -3;
			}
			baseCfgSet(x);
		}
	}
	return 0;
}

bool NVTLineLsr::ResetConnect() {
	if (reopCmd.length() > 0) {
		errCode = lvm_init_sdk(NULL, "./SysLog/NVTLog");
		Open((char*)reopCmd.c_str());
		return true;
	}
	return false;
}

bool NVTLineLsr::GrabTigger(unsigned int gCount) {
	vpc.clear();
	curSelectedDev = slectedChannelno;
	dev[slectedChannelno].cb_GatherFinsh = false;
	dev[slectedChannelno].cb_Lines = 0;
	inBindBuff(gCount);
	errCode = lvm_trigger_en_ctrl(dev[slectedChannelno].dev, true);
	if (errCode != 0)
	{
		return false;
	}
	return true;
}


/// <summary>
/// 基本必要参数的配置，可以时配置文件路径加载，当配置文件路径为空时候采用默认的固定必要参数
/// </summary>
/// <param name="sw">指定操作的设备的索引</param>
/// <param name="path">配置文件路径</param>
void NVTLineLsr::baseCfgSet(int sw, const char* path) {
	lvm_dev_t* curDev = dev[sw].dev;
	if (path == 0) {

		curDev->capture_param->div_ratio = ENC_DIV_R;//分频比； 分频比*分辨率 == 触发间隔
		curDev->capture_param->time_trigger_freq = ENC_TRG_FREQ;
		curDev->capture_param->trigger_input_type = lvm_trigger_type_t::LVM_TRIGGER_ECODER_AB;
		errCode = lvm_set_param(curDev, LVM_CAPTURE_PARAM);
		if (errCode != 0)
		{
			errCode = curDev->error_code;
			return;
		}

		curDev->io_ctrl_param->encoder_mode = lvm_encoder_mode_t::LVM_ENCODER_CTRL_TWO_WAY_MODE;
		curDev->io_ctrl_param->encoder_type = lvm_encoder_type_t::LVM_ENCODER_MULTI_FREQ_1;
		curDev->io_ctrl_param->encoder_resolution = ENC_RES;//分辨率
		errCode = lvm_set_param(curDev, LVM_IO_CTRL_PARAM);
		if (errCode != 0) {
			errCode = curDev->error_code;
			return;
		}
	}
	else {
		//采用配置方式加载

	}
}


void NVTLineLsr::GetSensorparam() {
	lvm_dev_t* curDev = dev[slectedChannelno].dev;
	CtrInfo[slectedChannelno].gain = curDev->config_param->gain_k;
	CtrInfo[slectedChannelno].expTime = curDev->config_param->expsure_time;
	CtrInfo[slectedChannelno].sampleFreq = curDev->capture_param->time_trigger_freq;

	//采集行数
	CtrInfo[slectedChannelno].lines = curDev->capture_param->trigger_number_per_ctrl;
	CtrInfo[slectedChannelno].TiggerMode = (int)curDev->capture_param->ctrl_type;
	//if (CtrInfo[slectedChannelno].TiggerMode > 1) {

	//	curDev->capture_param->ctrl_mode;
	//}
}

void NVTLineLsr::SetSensorparam() {

	lvm_dev_t* csDev = dev[slectedChannelno].dev;
	////设置触发方式为时间触发
	//csDev->capture_param->ctrl_delay = ctrl_delay;
	//csDev->capture_param->ctrl_delay_number = ctrl_delay_number;
	//csDev->capture_param->ctrl_mode = ctrl_mode;
	//csDev->capture_param->trigger_input_type = trigger_input_type;
	//csDev->capture_param->ctrl_type = ctrl_type;
	//csDev->capture_param->div_ratio = div_ratio;
	//csDev->capture_param->time_trigger_freq = time_trigger_freq;
	//errCode = lvm_set_param(csDev, LVM_CAPTURE_PARAM);
	//if (errCode != 0)
	//{
	//	cout << "capture param set failed! error_code:" << csDev->error_code << endl;
	//	return -2;
	//}

	//if (csDev->connect_dev_type == 5 || csDev->connect_dev_type == 6)
	//{
	//	gain_k = 50.0f;
	//}

	//设置基础配置参数
	csDev->config_param->gain_k = CtrInfo[slectedChannelno].gain;
	csDev->config_param->expsure_time = CtrInfo[slectedChannelno].expTime;
	errCode = lvm_set_param(csDev, LVM_CONFIG_PARAM);
	if (errCode != 0)
	{
		errCode = -2;
		return;
	}



	//设置触发间隔
	//csDev->calib_param->dir[1] = y_scale;
	//errCode = lvm_set_param(csDev, LVM_CALIB_PARAM);
	//if (errCode != 0)
	//{
	//	errCode = -4;
	//	return;
	//}
}

int NVTLineLsr::IsCollectionCompleted(unsigned int gCount) {
	if (isGrabing()) {
		while (!dev[slectedChannelno].cb_GatherFinsh)
		{
			_sleep(100);
		}
		StatusFlagClr(GRABING);
		if (dev[slectedChannelno].buf != 0) {
			//释放缓冲区，同时回自动Stop Grab
			if (lvm_free_buf(dev[slectedChannelno].buf) == 0) {
				dev[slectedChannelno].buf = 0;
			}
		}
		return vpc.size();
	}
	//else {
	//	/// 默认开启强度图存储
	//	int intensity_enable = 1;
	//	/// 默认开启均匀点距转换
	//	int uniformity_enable = 1;
	//	while (isGrabing())
	//	{
	//		lvm_point_cloud_t* pcld = (lvm_point_cloud_t*)lvm_grab_frame(dev[slectedChannelno].dev, CAPTURETMS);
	//		if (pcld)
	//		{
	//			lvm_save_pcld(dev[slectedChannelno].dev, "demo.pcd", pcld);
	//			if (dev[slectedChannelno].intensity_enable)
	//			{
	//				unsigned short* intensity_data = new unsigned short[pcld->head.width * pcld->head.height];
	//				for (int i = 0; i < pcld->head.width * pcld->head.height; i++)
	//				{
	//					intensity_data[i] = (unsigned short)pcld->p[i].reserved;
	//				}
	//				lvm_save_img("demo.png", intensity_data, pcld->head.width, pcld->head.height, LVM_IMAGE_FORMAT_16BIT_USHORT);

	//				delete[] intensity_data;
	//			}
	//			if (dev[slectedChannelno].uniformity_enable)
	//			{
	//				lvm_save_img("pcld_to_dm.png", pcld->dm->data, pcld->dm->head.width, pcld->dm->head.height, LVM_IMAGE_FORMAT_16BIT_USHORT);
	//				if (1 == intensity_enable)
	//				{
	//					lvm_save_img("dm_intensity.png", pcld->insensity_img->data, pcld->insensity_img->head.width, pcld->insensity_img->head.height, LVM_IMAGE_FORMAT_16BIT_USHORT);
	//				}

	//			}
	//			break;
	//		}
	//		else
	//		{
	//			//超时
	//			errCode = lvm_trigger_en_ctrl(dev[slectedChannelno].dev, false);
	//			return -1;
	//		}
	//		_sleep(10);
	//	}
	//	errCode = lvm_trigger_en_ctrl(dev[slectedChannelno].dev, false);
	//}
	return 0;
}

void* NVTLineLsr::DataGet(unsigned& outcount) {
	outcount = vpc.size();
	Sensorinfo.BuffSize = outcount;
	Sensorinfo.ptrGrabResult = &vpc[0];
	return Sensorinfo.ptrGrabResult;
}

void NVTLineLsr::stop() {
	errCode = lvm_trigger_en_ctrl(dev[slectedChannelno].dev, false);
	curSelectedDev = slectedChannelno;
	//if (dev[slectedChannelno].buf != 0) {
	//	//释放缓冲区，同时回自动Stop Grab
	//	if (lvm_free_buf(dev[slectedChannelno].buf) == 0) {
	//		dev[slectedChannelno].buf = 0;
	//	}
	//}
}

bool NVTLineLsr::TiggerTaber(short wich, bool isClose) {
	lvm_dev_t* curDev = dev[slectedChannelno].dev;
	if (curDev == 0) { return false; }
	if (wich == 0) {//内部软触发
		curDev->capture_param->ctrl_type = lvm_ctrl_type_t::SOFTWARE_CTRL;
		curDev->capture_param->ctrl_mode = 2;
	}
	else if (wich == 1 || wich == 2) {//外部
		curDev->capture_param->ctrl_type = lvm_ctrl_type_t::HARDWARE_CTRL;
		curDev->capture_param->ctrl_mode = 0;
	}
	if (wich == 3 || wich == 4) {//X Y 编码器触发
		lvm_reset_encoder_state(curDev, 1);

		curDev->capture_param->ctrl_mode = 2;
		curDev->capture_param->ctrl_type = lvm_ctrl_type_t::ENCODER_CTRL;
	}

	errCode = lvm_set_param(curDev, LVM_CAPTURE_PARAM);
	if (errCode != 0)
	{
		return false;
	}
	return true;
}

bool NVTLineLsr::expActive(short fno, void* param) {
	switch (fno)
	{
	case 1:
		return OpenScanerLine();
		break;

	case 2:

		break;

	default:
		break;
	}
	return false;
}


int NVTLineLsr::inBindBuff(int grab_lines) {
	if (dev[slectedChannelno].buf != 0) {
		if (lvm_free_buf(dev[slectedChannelno].buf) == 0) {
			dev[slectedChannelno].buf = 0;
		}
	}

	//2表示缓冲区数量最多存储2帧数据,根据情况选择 深度图还是点云图
	//dev[slectedChannelno].buf = lvm_alloc_pcld_buf(dev[slectedChannelno].dev, dev[slectedChannelno].intensity_enable, dev[slectedChannelno].uniformity_enable, grab_lines, 2);
	dev[slectedChannelno].buf = lvm_alloc_depth_map_buf(dev[slectedChannelno].dev, 1, 0, grab_lines, 2);
	errCode = lvm_bind_buf(dev[slectedChannelno].dev, dev[slectedChannelno].buf);
	if (errCode != 0)
	{
		return dev[slectedChannelno].dev->error_code;
	}
	return 0;
}

void NVTLineLsr::close() {
	for (unsigned x = 0; x < DEVNUM; x++) {
		if (dev[x].dev == 0) {
			continue;
		}
		lvm_disconnect_dev(dev[x].dev);
		if (dev[x].buf != 0) {
			if (lvm_free_buf(dev[x].buf) == 0) {
				dev[x].buf = 0;
			}
		}
		lvm_destroy_dev(dev[x].dev);
		dev[x].dev = NULL;
	}
	lvm_deinit_sdk();
	Sensorinfo.ptrGrabResult = 0;
}



bool NVTLineLsr::OpenScanerLine() {
	lvm_dev_t* csDev = dev[slectedChannelno].dev;
	csDev->laser_param->laser_enable = 1;
	csDev->laser_param->array_enable = 1;
	errCode = lvm_set_param(csDev, LVM_LASER_PARAM);
	if (errCode != 0)
	{
		return false;
	}
	return true;
}

//量程零点 及 上下边界设置
bool NVTLineLsr::OrgAndRange(float zVal, float upVal, float downVal) {
	lvm_dev_t* csDev = dev[slectedChannelno].dev;
	csDev->config_param->top_edge = upVal;
	csDev->config_param->bottom_edge = downVal;
	errCode = lvm_set_param(csDev, LVM_CONFIG_PARAM);
	if (errCode != 0)
	{
		return false;
	}
	return true;
}


void NVTLineLsr::CoordinateConvert() {


}

bool NVTLineLsr::WorldCoordinateSet(u8 wich, float x, float y, float z) {
	dev[slectedChannelno].StartPoint.x = x;
	dev[slectedChannelno].StartPoint.y = y;
	dev[slectedChannelno].StartPoint.z = z;
	return true;
}

bool NVTLineLsr::grabContinuity(bool isAnsy, int gCount) {
	vpc.clear();
	curSelectedDev = slectedChannelno;
	dev[slectedChannelno].cb_GatherFinsh = false;
	dev[slectedChannelno].cb_Lines = 0;
	inBindBuff((gCount == 1) ? LINEBUFMAX : gCount);
	///异步使能接口调用同时注册异步采集回调函数，使用之前需要先申请好缓冲区
	//0表示普通采集模式完成才进入回调， 1表示高速模式,即收到数据就进入回调
	errCode = lvm_enable_async_mode(dev[slectedChannelno].dev, 0, frame_cb);
	if (errCode != 0)
	{
		errCode = dev[slectedChannelno].dev->error_code;
		return false;
	}
	//触发开始
	errCode = lvm_trigger_en_ctrl(dev[slectedChannelno].dev, true);
	if (errCode != 0)
	{
		errCode = dev[slectedChannelno].dev->error_code;
		return false;
	}
	return true;
}