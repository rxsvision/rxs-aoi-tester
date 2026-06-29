#include "private.h"


ScanCodeStep::ScanCodeStep(int id) :StepBlockBase(id)
{
	_type = StepBlockBase::IMG_SCANCODE;
	outResultType = (BLKOUTRESULTYPE_SHAPE_RECT | BLKOUTRESULTYPE_DATASET);
}

ScanCodeStep::ScanCodeStep(int id, const char* name) :StepBlockBase(id, name)
{
	_type = StepBlockBase::IMG_SCANCODE;
	outResultType = (BLKOUTRESULTYPE_SHAPE_RECT | BLKOUTRESULTYPE_DATASET);
}

bool ScanCodeStep::InputParamStringCfgSet(std::string* p, int l) {
	int index = 0;
	//ScanCodeParam* scp = ScanCodeInputParamGet();
	ScanCodeParam* scp = ParamGet<ScanCodeParam>();
	//memset(&scp, 0, sizeof(ScanCodeParam));
	std::string val = p[index];
	scp->srcLinker.id = atoi(val.c_str());
	val = p[index += 1];
	scp->srcLinker.outChoose = atoi(val.c_str());
	val = p[index += 1];
	scp->codeType = atoi(val.c_str());
	val = p[index += 1];
	scp->roiEnable = (val == "1") ? true : false;
	val = p[index += 1];
	scp->roiArea.x = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.y = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.w = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.h = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.Cx = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.Cy = atoi(val.c_str());
	val = p[index += 1];
	scp->roiArea.Angle = atof(val.c_str());
	val = p[index += 1];
	scp->scale = atof(val.c_str());
	val = p[index += 1];
	scp->tageNum = atoi(val.c_str());
	return true;
}

std::string ScanCodeStep::InputParamSaveStringGet() {
	ScanCodeParam* scp = (ScanCodeParam*)InParamsGet();
	int ct = scp->codeType;
	std::string result = std::to_string(scp->srcLinker.id) + "," + std::to_string(scp->srcLinker.outChoose) + ",";
	result += (std::to_string(ct) + ",");
	result += (scp->roiEnable ? "1," : "0,");
	result += (std::to_string(scp->roiArea.x) + ",");
	result += (std::to_string(scp->roiArea.y) + ",");
	result += (std::to_string(scp->roiArea.w) + ",");
	result += (std::to_string(scp->roiArea.h) + ",");
	result += (std::to_string(scp->roiArea.Cx) + ",");
	result += (std::to_string(scp->roiArea.Cy) + ",");
	result += std::to_string(scp->roiArea.Angle) + ",";
	result += std::to_string(scp->scale) + ",";
	result += std::to_string(scp->tageNum);
	return result;
}

//void ScanCodeStep::ScanCodeInputParamSet(ScanCodeParam scp) {
//	int len = sizeof(ImgSourceParam);
//	InParamAlloc(&scp, len);
//}
//
//ScanCodeParam* ScanCodeStep::ScanCodeInputParamGet() {
//	ScanCodeParam* ipg = (ScanCodeParam*)InParamsGet();
//	if (ipg == 0) {
//		ScanCodeParam scp;
//		memset(&scp, 0, sizeof(ScanCodeParam));
//		scp.scale = 1.0;
//		scp.tageNum = 1;
//		ScanCodeInputParamSet(scp);
//		ipg = (ScanCodeParam*)InParamsGet();
//		if (ipg == 0) { return 0; }
//	}
//	return ipg;
//}


short ScanCodeStep::Run() {
	ScanCodeParam* scp = (ScanCodeParam*)InParamsGet();
	if (scp == 0) {
		return -101;
	}
	std::vector<SCResult> ret;
	StepBlockBase* prev = fromSrc(scp->srcLinker.id);
	if (prev != 0) {
		const rxsImg* inImg = 0;
		prev->GetResult(&inImg);
		if (scp->codeType == 1)
		{
			ret = cv_wechat_qrdecode((rxsImg*)inImg, scp->tageNum, scp->roiEnable, scp->roiArea, scp->scale);
		}
		else if (scp->codeType == 2) {
			ret = dmtx_dmdecode((rxsImg*)inImg, scp->tageNum, scp->roiEnable, scp->roiArea, scp->scale);
		}
		else {
			//barCodeReader(ir, Count);
		}
		int Len = ret.size();
		if (Len > 0) {
			std::string filed[6] = { "ID","条码","P1","P2","P3","P4" };
			rr.SetFiled(filed, 6, "条码内容");
			char buff[50];
			int row = 0;
			for (unsigned x = 0; x < Len; x++) {
				rxsPointI* pos = ret[x].pos;
				const char* v = ret[x].code.c_str();
				memset(buff, 0, 50);
				if (v != 0) {
					row = rr.AddRow();
					sprintf_s(buff, "%d", row);
					row -= 1;
					rr.SetValue("ID", buff, row);
					rr.SetValue("条码", v, row);
					sprintf_s(buff, "%d,%d", pos[0].x, pos[0].y);
					rr.SetValue("P1", buff, row);
					sprintf_s(buff, "%d,%d", pos[1].x, pos[1].y);
					rr.SetValue("P2", buff, row);
					sprintf_s(buff, "%d,%d", pos[2].x, pos[2].y);
					rr.SetValue("P3", buff, row);
					sprintf_s(buff, "%d,%d", pos[3].x, pos[3].y);
					rr.SetValue("P4", buff, row);
				}
			}
		}
		return 0;
	}
	return -1;
}
