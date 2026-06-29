#include "private.h"
#include <mutex>

extern HrdWrkSyetemEnv hrdSyetemEnver;

ThridPartyCallBlk::ThridPartyCallBlk(int id) :StepBlockBase(id) {
	_type = StepBlockBase::THRD_PROCESS;
	outResultType = (BLKOUTRESULTYPE_IMG | BLKOUTRESULTYPE_DATASET);
}

ThridPartyCallBlk::ThridPartyCallBlk(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::THRD_PROCESS;
	outResultType = (BLKOUTRESULTYPE_IMG | BLKOUTRESULTYPE_DATASET);
}


void ThridPartyCallBlk::ThridPartyInputParamSet(ThridPartyCallBlkParam tpcbp) {
	if (InParamsGet() != 0) { return; }
	int len = sizeof(ThridPartyCallBlkParam);
	InParamAlloc(&tpcbp, len);
}

ThridPartyCallBlkParam* ThridPartyCallBlk::ThridPartyInputParamGet() {
	ThridPartyCallBlkParam* tpcbp = (ThridPartyCallBlkParam*)InParamsGet();
	if (tpcbp == 0) {
		ThridPartyCallBlkParam bp;
		memset(&bp, 0, sizeof(ThridPartyCallBlkParam));
		ThridPartyInputParamSet(bp);
		tpcbp = (ThridPartyCallBlkParam*)InParamsGet();
		if (tpcbp == 0) { return 0; }
	}
	return tpcbp;
}

bool ThridPartyCallBlk::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	ThridPartyCallBlkParam* tpcbp = ThridPartyInputParamGet();
	std::string val = p[index];
	tpcbp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	tpcbp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	if (val.length() > 1) {
		memcpy(tpcbp->path, val.c_str(), val.length());
	}
	val = p[index += 1];
	if (val.length() > 1) {
		memcpy(tpcbp->inImgName, val.c_str(), val.length());
	}
	val = p[index += 1];
	if (val.length() > 1) {
		memcpy(tpcbp->outDataName, val.c_str(), val.length());
	}
	val = p[index += 1];
	if (val.length() > 1) {
		memcpy(tpcbp->outImgName, val.c_str(), val.length());
	}
	return true;
}

std::string ThridPartyCallBlk::InputParamSaveStringGet() {
	ThridPartyCallBlkParam* inp = (ThridPartyCallBlkParam*)InParamsGet();
	if (inp == 0) { return ""; }
	stringstream ss;
	ss << std::to_string(inp->srcLinker.id) << "," << std::to_string(inp->srcLinker.outChoose) << ",";
	ss << (inp->path[0] == 0 ? "_" : inp->path) << "," << (inp->inImgName[0] == 0 ? "_" : inp->inImgName);
	ss << "," << (inp->outDataName[0] == 0 ? "_" : inp->outDataName) << "," << (inp->outImgName[0] == 0 ? "_" : inp->outImgName);
	return ss.str();
}



short ThridPartyCallBlk::Run() {
	ThridPartyCallBlkParam* inp = (ThridPartyCallBlkParam*)InParamsGet();
	if (inp == 0) {
		return -101;
	}
	StepBlockBase* prev = fromSrc(inp->srcLinker.id);
	if (prev != 0) {
		auto fun = [=]() {
			const char* inBlkName = 0;
			const char* outBlkName = 0;
			const char* outImgBlkName = 0;
			const rxsImg* inImg = 0;
			if (inp->inImgName != 0 && inp->inImgName[0] != 0) {
				prev->GetResult(&inImg);
				inBlkName = inp->inImgName;
			}
			if (inp->outDataName != 0 && inp->outDataName[0] != 0) {
				outBlkName = inp->outDataName;
			}
			if (inp->outImgName != 0 && inp->outImgName[0] != 0) {
				outImgBlkName = inp->outImgName;
			}
			//仅在作为最后一个调用块的时候才能生效
			hrdSyetemEnver.pSystemer->SysLogRecord((std::string(inp->path) + ":三方调用启用-等待合成完成!").c_str());
			while (!((imgTaskFlowerProcesser*)tfpDependency)->HadFinshed()) {
				_sleep(10);
				if (hrdSyetemEnver.pSystemer->sysStatus->CurFixStatus() != SystemInfo::FixStaus::RUNING) {
					return;
				}
			}
			status |= 1;
			hrdSyetemEnver.pSystemer->SysLogRecord((std::string(inp->path) + ":三方调用启用-开始调用!").c_str());
			int er = LoadThridSolutionAndExer(inp->path, "", (rxsImg*)inImg, &rr, &outImg, inBlkName, outBlkName, outImgBlkName);
			status &= (~1);
			if (er != 0) {
				hrdSyetemEnver.pSystemer->SysLogRecord((std::to_string(er) + ":三方调用失败!").c_str());
			}
			else {
				hrdSyetemEnver.pSystemer->SysLogRecord((std::string(inp->path) + ":三方调用完成!").c_str());
			}
		};
		std::thread sthr(fun);
		sthr.detach();
		return 0;
	}
	return -3;
}

