#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;


ImgMergingStep::ImgMergingStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::IMG_MERG_PROCESS;
	outResultType = BLKOUTRESULTYPE_IMG;
	locDir = new std::string;
	flrNum = 0;
	flrGrps = 0;
	MultinGrpsAlloc(3);
}

ImgMergingStep::ImgMergingStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::IMG_MERG_PROCESS;
	outResultType = BLKOUTRESULTYPE_IMG;
	locDir = new std::string;
	flrNum = 0;
	flrGrps = 0;
	MultinGrpsAlloc(3);
}

ImgMergingStep::~ImgMergingStep() {
	ImgMergingParam* imp = ParamGet<ImgMergingParam>();
	free(imp->hf);
	imp->hf = 0;
	delete (std::string*)locDir;
	if (flrGrps != 0) {
		free(flrGrps);
	}
	locDir = 0;
	flrGrps = 0;
}


void ImgMergingStep::FromLinkerSet(FromLinker* fls, int n) {
	ImgMergingParam* imp = ParamGet<ImgMergingParam>();
	if (imp->imMethod == 1 && imp->Nums > 0) {
		if (flrGrps != 0) {
			free(flrGrps);
			flrGrps = 0;
		}
		int sl = sizeof(FromLinker) * imp->Nums;
		flrGrps = malloc(sl);
		if (flrGrps == 0) { return; }
		memcpy(flrGrps, imp->Method.srcLinker, sl);
		imp->Method.srcLinker = (FromLinker*)flrGrps;
	}
}

void ImgMergingStep::HdrFiSet(imHdrParam* s, int len) {
	ImgMergingParam* imp = ParamGet<ImgMergingParam>();
	if (imp->imType == 2 || imp->imType == 1) {
		if (imp->hf != 0) {
			free(imp->hf);
		}
		imp->hf = (imHdrParam*)malloc(sizeof(imHdrParam) * len);
		if (imp->hf == 0) {
			return;
		}
		memcpy(imp->hf, s, sizeof(imHdrParam) * len);
	}
}

void ImgMergingStep::LocDirSet(const char* dirpath) {
	(*(std::string*)locDir) = dirpath;
	ImgMergingParam* imp = ParamGet<ImgMergingParam>();
	if (imp->imMethod == 0) {
		imp->Method.locDir = (*(std::string*)locDir).c_str();
	}
}


bool ImgMergingStep::InputParamStringCfgSet(std::string* p, int l)
{
	int index = 0;
	if (l < 4) { return false; }
	ImgMergingParam* imp = ParamGet<ImgMergingParam>();
	imp->isAnsy = true;//注意此处临时强制异步处理
	std::string val = p[index];
	imp->imType = atoi(val.c_str());
	val = p[index += 1];
	imp->imMethod = atoi(val.c_str());
	val = p[index += 1];
	imp->Nums = atoi(val.c_str());
	val = p[index += 1];
	imp->outFlag = atoi(val.c_str());
	if (imp->imMethod == 0) {
		val = p[index += 1];
		LocDirSet(val.c_str());
		if (imp->imType == 1) {//HDR
			std::vector<imHdrParam> hdrt_adjf_p;
			for (int i = 0; i < imp->Nums; i++) {
				imHdrParam hdrp = {
					atof(p[index += 1].c_str()),
					atof(p[index += 1].c_str()),
					//CST_LightSource::ColorIndex(atoi(p[index += 1].c_str())),
					//atoi(p[index += 1].c_str()) 
				};
				hdrt_adjf_p.push_back(hdrp);
			}
			HdrFiSet(&hdrt_adjf_p[0], hdrt_adjf_p.size());
		}
	}
	else if (imp->imMethod == 1) {
		std::vector<FromLinker> v;
		int id = 0, oc = 0;
		for (int x = 0; x < imp->Nums; x++) {
			id = atoi(p[index += 1].c_str());
			oc = atoi(p[index += 1].c_str());
			v.push_back({ id,oc });
		}
		FromLinkerSet(&v[0], v.size());
	}
	return true;
}


std::string ImgMergingStep::InputParamSaveStringGet() {
	ImgMergingParam* imp = (ImgMergingParam*)InParamsGet();
	if (imp == 0) { return ""; }
	stringstream result;
	std::string rr;
	int imt = imp->imType;
	int o = imp->imMethod;
	result << std::to_string(imt) << "," << std::to_string(o) << "," << std::to_string(imp->Nums) << "," << std::to_string(imp->outFlag) << ",";
	if (o == 0) {
		if (imp->Method.locDir != 0) {
			result << imp->Method.locDir << ",";
			if (imp->imType == 1) {//HDR
				//int v = 0;
				for (int x = 0; x < imp->Nums; x++) {
					//v = imp->hf[x].curVal;
					//result << imp->hf[x].times << "," << imp->hf[x].adjFocus << "," << imp->hf[x].curCi << "," << std::to_string(v) << ",";
					result << imp->hf[x].times << "," << imp->hf[x].adjFocus << ",";
				}
			}
		}
	}
	else if (o == 1) {
		if (imp->Method.srcLinker != 0) {
			for (int x = 0; x < imp->Nums; x++) {
				result << imp->Method.srcLinker[x].id << "," << imp->Method.srcLinker[x].outChoose << ",";
			}
		}
	}
	rr = result.str();
	rr.pop_back();
	return rr;
}


short ImgMergingStep::LoadDirFiles(const char* dirPath, void* po) {
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	int l = 0;
	if ((hFile = _findfirst(p.assign(dirPath).append("\\*").c_str(), &fileinfo)) != -1)//若查找成功，则进入
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				//文件名不等于"."&&文件名不等于".."
				//判断时，两者都要忽略，不然就无限递归跳不出去了！
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					//getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				string fp = (p.assign(dirPath).append("\\").append(fileinfo.name));
				((std::vector<std::string> *)po)->push_back(fp);

			}
			l++;
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return 0;
}


short ImgMergingStep::Run() {
	ImgMergingParam* imp = (ImgMergingParam*)InParamsGet();
	if (imp == 0) {
		return -2000;
	}
	status |= 1;
	MultinClr();
	titls_times.clear();
	slants.clear();
	img_paths.clear();
	if (imp->imMethod == 0) {
		LoadDirFiles(imp->Method.locDir, &img_paths);
		for (int x = 0; x < img_paths.size(); x++) {
			Mat A = imread(img_paths[x], IMREAD_GRAYSCALE);
			PushIn(&A);
			if (img_paths.size() > imp->Nums) {
				status &= (~1);
				return -2011;
			}
			else {
				if (imp->imType == 1) {
					titls_times.push_back(imp->hf[x].times);
					continue;
				}
			}
		}
	}
	else if (imp->imMethod == 1) {
		//----此处需要完善----
	}
	else {
		status &= (~1);
		return -2003;
	}
	auto fun = [=]() {
		try {
			if (imp->imType == 1) {
				rxsImg* ro = hrdSyetemEnver.rtk2d->createHDR_dll(*(std::vector<cv::Mat> *)MultinGrps, titls_times);
				outimgAlloc(ro);
			}
			else if (imp->imType == 2) {
				rxsImg* ro = hrdSyetemEnver.rtk2d->fuseImages_dll(*(std::vector<cv::Mat> *)MultinGrps);
				outimgAlloc(ro);
			}
			else {
				status &= (~1);
				return -2002;
			}
		}
		catch (...) {
			hrdSyetemEnver.pSystemer->SysLogRecord("Run--执行异常!");
			status &= (~1);
			return -2004;
		}
		return 0;
	};

	if (!imp->isAnsy) {
		fun();
	}
	else {
		std::thread sthr(fun);
		sthr.detach();
	}
	return 0;
}