#include "private.h"

extern HrdWrkSyetemEnv hrdSyetemEnver;


LambertianPhotoMetricstereoStep::LambertianPhotoMetricstereoStep(int id) :StepBlockBase(id) {
	_type = StepBlockBase::IMG_LAMBPHOTOMETRIC;
	outResultType = BLKOUTRESULTYPE_IMG;
	flrNum = 0;
	flrGrps = 0;
	MultinGrpsAlloc(3);
}

LambertianPhotoMetricstereoStep::LambertianPhotoMetricstereoStep(int id, const char* name) : StepBlockBase(id, name) {
	_type = StepBlockBase::IMG_LAMBPHOTOMETRIC;
	outResultType = BLKOUTRESULTYPE_IMG;
	flrNum = 0;
	flrGrps = 0;
	MultinGrpsAlloc(3);
}

LambertianPhotoMetricstereoStep::~LambertianPhotoMetricstereoStep() {
	LambertianPhotoMetriParam* imp = ParamGet<LambertianPhotoMetriParam>();
	if (imp != 0 && imp->Lcp != 0) {
		free(imp->Lcp);
		imp->Lcp = 0;
	}
	if (flrGrps != 0) {
		free(flrGrps);
		flrGrps = 0;
	}
}


void LambertianPhotoMetricstereoStep::FromLinkerSet(FromLinker* fls, int n) {
	LambertianPhotoMetriParam* imp = ParamGet<LambertianPhotoMetriParam>();
	if (imp->imMethod == 1 && imp->Nums > 0) {
		if (flrGrps != 0) {
			free(flrGrps);
			flrGrps = 0;
		}
		int sl = sizeof(FromLinker) * imp->Nums;
		flrGrps = malloc(sl);
		if (flrGrps == 0) { return; }
		memcpy(flrGrps, fls, sl);
		imp->Method.srcLinker = (FromLinker*)flrGrps;
	}
}

void LambertianPhotoMetricstereoStep::LbmSet(imLbmParam* ilp, int l) {
	LambertianPhotoMetriParam* imp = ParamGet<LambertianPhotoMetriParam>();
	if (imp->Lcp != 0) {
		free(imp->Lcp);
	}
	imp->Lcp = (imLbmParam*)malloc(sizeof(imLbmParam) * l);
	if (imp->Lcp == 0) { return; }
	imp->Nums = l;
	memcpy(imp->Lcp, ilp, sizeof(imLbmParam) * l);
}

void LambertianPhotoMetricstereoStep::LocDirSet(const char* dirpath) {
	locDir = dirpath;
	LambertianPhotoMetriParam* imp = ParamGet<LambertianPhotoMetriParam>();
	if (imp->imMethod == 0) {
		imp->Method.locDir = locDir.c_str();
	}
}


bool LambertianPhotoMetricstereoStep::InputParamStringCfgSet(std::string* p, int l)
{
	int index = 0;
	if (l < 4) { return false; }
	LambertianPhotoMetriParam* imp = ParamGet<LambertianPhotoMetriParam>();
	imp->isAnsy = false;//true;//注意此处临时强制异步处理
	std::string val = p[index];
	imp->imMethod = atoi(val.c_str());
	val = p[index += 1];
	imp->Nums = atoi(val.c_str());
	val = p[index += 1];
	imp->outFlag = atoi(val.c_str());
	if (imp->imMethod == 0) {
		val = p[index += 1];
		LocDirSet(val.c_str());

		std::vector<imLbmParam> hdrt_adjf_p;
		for (int x = 0; x < imp->Nums; x++) {
			imLbmParam ibp;
			ibp.slant = atof(p[index += 1].c_str());
			ibp.titl = atof(p[index += 1].c_str());
			hdrt_adjf_p.push_back(ibp);
		}
		LbmSet(&hdrt_adjf_p[0], hdrt_adjf_p.size());
	}
	else if (imp->imMethod == 1) {
		std::vector<FromLinker> v;
		std::vector<imLbmParam> hdrt_adjf_p;
		int id = 0, oc = 0;
		float slant = 0, tit = 0;
		for (int x = 0; x < imp->Nums; x++) {
			id = atoi(p[index += 1].c_str());
			oc = atoi(p[index += 1].c_str());
			v.push_back({ id,oc });

			slant = atof(p[index += 1].c_str());
			tit = atof(p[index += 1].c_str());
			hdrt_adjf_p.push_back({ slant ,tit });
		}
		FromLinkerSet(&v[0], v.size());
		LbmSet(&hdrt_adjf_p[0], hdrt_adjf_p.size());
	}
	return true;
}


std::string LambertianPhotoMetricstereoStep::InputParamSaveStringGet() {
	LambertianPhotoMetriParam* imp = (LambertianPhotoMetriParam*)InParamsGet();
	if (imp == 0) { return ""; }
	stringstream result;
	std::string rr;
	int o = imp->imMethod;
	result << std::to_string(o) << "," << std::to_string(imp->Nums) << "," << std::to_string(imp->outFlag) << ",";
	if (o == 0) {
		if (imp->Method.locDir != 0) {
			result << imp->Method.locDir << ",";
			for (int x = 0; x < imp->Nums; x++) {
				result << imp->Lcp[x].slant << "," << imp->Lcp[x].titl << ",";
			}
		}
	}
	else if (o == 1) {
		if (imp->Method.srcLinker != 0) {
			for (int x = 0; x < imp->Nums; x++) {
				result << imp->Method.srcLinker[x].id << "," << imp->Method.srcLinker[x].outChoose << ",";
				if (imp->Lcp != 0) {
					result << imp->Lcp[x].slant << "," << imp->Lcp[x].titl << ",";
				}
			}
		}
	}
	rr = result.str();
	rr.pop_back();
	return rr;
}


short LambertianPhotoMetricstereoStep::LoadDirFiles(const char* dirPath, void* po) {
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


void LambertianPhotoMetricstereoStep::RunReady() {
	MultinClr();
	titls.clear();
	slants.clear();
	img_paths.clear();
}


void LambertianPhotoMetricstereoStep::fer(void * org) {
	try {
		std::string tlfn = ((imgTaskFlowerProcesser*)tfpDependency)->FlowerName();
		cv::Mat outr;
		std::cout << "in_photometricStero_" << std::endl;
		bool rrret = hrdSyetemEnver.rtk2d->photometricStero(MultinGrps, (cv::Mat*)&org, slants, titls, outr, tlfn);
		std::cout << "out_photometricStero_" << std::endl;
		if (!rrret) {
			status &= (~1);
			return;
		}
		rxsImg ro = { outr.channels(),outr.data,0,outr.rows,outr.cols };
		ro.Len = ro.Channels * ro.rows * ro.cols;
		outimgAlloc(&ro);
		status &= (~1);
		return;
	}
	catch (...) {
		hrdSyetemEnver.pSystemer->SysLogRecord("Run--执行异常!");
		status &= (~1);
		return;
	}
	return;
}


short LambertianPhotoMetricstereoStep::Run() {
	LambertianPhotoMetriParam* imp = (LambertianPhotoMetriParam*)InParamsGet();
	if (imp == 0) {
		return -2000;
	}
	status |= 1;
	cv::Mat org;
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
				if (imp->Lcp[x].slant < 0 && imp->Lcp[x].titl < 0) {

				}
				slants.push_back(imp->Lcp[x].slant);
				titls.push_back(imp->Lcp[x].titl);
				continue;
			}
		}
	}
	else if (imp->imMethod == 1) {
		for (int x = 0; x < imp->Nums; x++) {
			StepBlockBase* sbb = fromSrc(imp->Method.srcLinker[x].id);
			const rxsImg* v = 0;
			if (imp->Lcp[x].slant < 0 && imp->Lcp[x].titl < 0) {//两个参数都小于0  原始图
				sbb->GetResult(&v);
				org = cv::Mat(v->rows, v->cols, CV_MAKETYPE(CV_8U, v->Channels), v->Data, v->cols * v->Channels);
				continue;
			}
			slants.push_back(imp->Lcp[x].slant);
			titls.push_back(imp->Lcp[x].titl);
			sbb->GetResult(&v);
			cv::Mat pin(v->rows, v->cols, CV_MAKETYPE(CV_8U, v->Channels), v->Data, v->cols * v->Channels);
			PushIn((void*)&pin);
		}
	}
	else {
		status &= (~1);
		return -2003;
	}


	try {
		std::string tlfn = ((imgTaskFlowerProcesser*)tfpDependency)->FlowerName();
		cv::Mat outr;
		std::cout << "in_photometricStero_" << std::endl;
		bool rrret = hrdSyetemEnver.rtk2d->photometricStero(MultinGrps, &org, slants, titls, outr, tlfn);
		std::cout << "out_photometricStero_" << std::endl;
		if (!rrret) {
			status &= (~1);
			return -2004;
		}
		rxsImg ro = { outr.channels(),outr.data,0,outr.rows,outr.cols };
		ro.Len = ro.Channels * ro.rows * ro.cols;
		outimgAlloc(&ro);
		status &= (~1);
		return 0;
	}
	catch (...) {
		hrdSyetemEnver.pSystemer->SysLogRecord("Run--执行异常!");
		status &= (~1);
		return -2006;
	}

	//fer(&org);
	//if (!imp->isAnsy) {
	//	fer();
	//}
	//else {
	//	std::thread sthr(fer);
	//	sthr.detach();
	//}
	return 0;
}