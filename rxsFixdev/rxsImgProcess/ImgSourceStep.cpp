#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;



ImgSourceStep::ImgSourceStep(int id)
	:StepBlockBase(id)
{
	_type = StepBlockBase::IMG_IN_SOURCE;
	outResultType = BLKOUTRESULTYPE_IMG;
}

ImgSourceStep::ImgSourceStep(int id, const char* name)
	: StepBlockBase(id, name)
{
	_type = StepBlockBase::IMG_IN_SOURCE;
	outResultType = BLKOUTRESULTYPE_IMG;
}

bool ImgSourceStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	ImgSourceParam* isp = ParamGet<ImgSourceParam>();//ImgSourceInputParamGet();
	isp->sType = atoi(p[index].c_str());
	isp->ssFlag = atoi(p[index += 1].c_str());
	if (isp->sType == 0) {
		for (int xi = 0; xi < 4; xi++) {
			isp->inParam.lsp.Lcp[xi].isOn = (atoi(p[index += 1].c_str()) == 1) ? true : false;
			isp->inParam.lsp.Lcp[xi].curCi = (CST_LightSource::ColorIndex)(atoi(p[index += 1].c_str()));
			isp->inParam.lsp.Lcp[xi].curVal = atoi(p[index += 1].c_str());
		}
		std::string sc = p[index += 1];
		memset(isp->inParam.lsp.selectedCream, 0, 20);
		memcpy(isp->inParam.lsp.selectedCream, sc.c_str(), sc.length());
		if (index < (l - 1)) {
			isp->inParam.lsp.ExposureTime = atof(p[index += 1].c_str());
		}
		if (index < (l - 1)) {
			isp->inParam.lsp.tzVal = atoi(p[index += 1].c_str());
		}
	}
	else if (isp->sType == 1) {
		index += 1;
		memset(isp->inParam.locReadPath, 0, 256);
		memcpy(isp->inParam.locReadPath, p[index].c_str(), p[index].length());
	}
	if ((isp->ssFlag & 1) == 1) {
		index += 1;
		if (index < l) {
			outSavePathSet(p[index].c_str());
		}
	}
	return true;
}

std::string ImgSourceStep::InputParamSaveStringGet() {
	ImgSourceParam* isp = ParamGet<ImgSourceParam>(); //(ImgSourceParam*)InParamsGet();
	if (isp == 0) { return ""; }
	std::string result;
	unsigned x = isp->ssFlag;
	result += (isp->sType + 0x30);
	result += ("," + std::to_string(x) + ",");
	if (isp->sType == 0) {
		for (int x = 0; x < 4; x++) {
			result += isp->inParam.lsp.Lcp[x].isOn ? "1" : "0";
			result += ",";
			result += std::to_string((int)isp->inParam.lsp.Lcp[x].curCi);
			result += ",";
			result += std::to_string((short)isp->inParam.lsp.Lcp[x].curVal);
			result += ",";
		}
		result += isp->inParam.lsp.selectedCream;
		int v = isp->inParam.lsp.tzVal;
		result += "," + std::to_string(isp->inParam.lsp.ExposureTime) + "," + std::to_string(v);
		result += ",";
	}
	else if (isp->sType == 1) {
		if (isp->inParam.locReadPath[0] == 0) {
			result += "_";
		}
		else {
			result += isp->inParam.locReadPath;
		}
		result += ",";
	}
	else if (isp->sType == 2) {

	}

	if ((isp->ssFlag & 1) == 1) {
		if (osfp.length() > 1) {
			result += ",";
			result += osfp;
		}
		else {
			result += ",\x09";
		}
	}

	return result;
}


void ImgSourceStep::outSavePathSet(const char* fp) {
	if (fp == 0) {
		osfp = "";
	}
	else if (fp[0] == '\x09') {
		osfp = "";
	}
	else {
		osfp = fp;
		if (osfp.length() > 0) {
			char ec = osfp[osfp.length() - 1];
			if (ec != '\\' && ec != '/') {
				osfp = osfp.append("/");
			}
		}
	}
	ImgSourceParam* ip = (ImgSourceParam*)InParamsGet();
	ip->savePath = osfp.c_str();
}


short ImgSourceStep::Run() {
	ImgSourceParam* ip = (ImgSourceParam*)InParamsGet();
	if (ip == 0) {
		return -101;
	}
	status |= 1;
	if (ip->sType == 0) {
		status |= 8;
		ImageableSensorUnit* hrdSensor = hrdSyetemEnver.pSystemer->Wrkpd->iabGather->Selected(ip->inParam.lsp.selectedCream);
		if (hrdSensor == 0) {
			status &= (~8);
			status &= (~1);
			return -102;
		}
		hrdSensor->GetSensorparam();
		float difv = hrdSensor->CtrInfo[0].expTime - ip->inParam.lsp.ExposureTime;
		if (difv < 0) {
			difv = 0 - difv;
		}
		if (difv > 1.0f) {
			hrdSensor->SpecParamSet("ExposureTime", ip->inParam.lsp.ExposureTime);
		}
		if ((ip->ssFlag & 2) == 2) {
			CST_TZls* hrd_tzLs = (CST_TZls*)hrdSyetemEnver.pSystemer->Wrkpd->HrdWareObjectGet(1);
			hrd_tzLs->brightnessDSet('A', ip->inParam.lsp.tzVal);
			_sleep(50);
			hrd_tzLs->brightnessDSet('B', ip->inParam.lsp.Lcp[0].curVal);
			_sleep(70);
		}


		if (difv > 75000) {
			_sleep(1000);
		}
		else if (difv > 25000) {
			_sleep(800);
		}
		else if (difv > 1000) {
			_sleep(400);
		}
		else {
			_sleep(100);
		}
		rxsImg rr = { 0,0,0,0,0 };
		hrdSensor->Grab(false, true);
		rr.Data = hrdSensor->DataGet(rr.Len);
		rr.cols = hrdSensor->CtrInfo[0].Width;
		rr.rows = hrdSensor->CtrInfo[0].Height;
		rr.Channels = 1;
		outimgAlloc(&rr);
		goto OK_END;
	}
	else if (ip->sType == 1) {
		if ((ip->ssFlag & 16) == 16) {
			cv::Mat in = cv::imread(ip->inParam.locReadPath, IMREAD_GRAYSCALE);
			if (in.empty()) { 
				return false; 
			}
			rxsImg rr = { in.channels(),in.data,in.cols * in.rows * in.channels(),in.rows,in.cols };
			outimgAlloc(&rr);
		}
		else {
			cv::Mat in = cv::imread(ip->inParam.locReadPath);
			if (in.empty()) 
			{ 
				return false; 
			}
			rxsImg rr = { in.channels(),in.data,in.cols * in.rows * in.channels(),in.rows,in.cols };
			outimgAlloc(&rr);
		}
		goto OK_END;
	}
	else if (ip->sType == 2) {
		outimgAlloc(ip->inParam.imgBase);
	}
	else {
		status &= (~8);
		status &= (~1);
		return -103;
	}
OK_END:
	status &= (~8);
	status &= (~1);
	if ((ip->ssFlag & 1) == 1) {
		Mat img(outImg.rows, outImg.cols, CV_MAKETYPE(CV_8U, outImg.Channels), outImg.Data, outImg.cols * outImg.Channels);
		std::string fname = sbName;
		imwrite(fname + ".bmp", img);
	}
	return 0;
}
