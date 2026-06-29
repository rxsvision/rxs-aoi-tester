#include "private.h"
#include "IVmCnnCodeRecg.h"
#include "IVMDataSet.h"
#include "IVmSaveImage.h"

#include "IVmImageBuffer.h"
#include "IVmGraphicsSet.h"

#include "IVmBlobFind.h"
#include "IVmCircleFind.h"


#ifdef HK_VMASTER

#pragma warning(disable:4819)

HkVmProcesser::HkVmProcesser(std::string flowerName, std::string solPath, std::string passworld) {
	try
	{
		fname = flowerName + ".";
		m_pVmSol = LoadSolution(solPath.c_str(), passworld.c_str());
		if (NULL == m_pVmSol)
		{
			return;
		}
	}
	catch (CVmException e)
	{
		//strReMsg.Format(_T("%x"), e.GetErrorCode());
		//strReMsg = _T("0x") + strReMsg + _T(" == LoadSolutionFromFile()");
		//m_listMsgBox.InsertString(0, strReMsg);
		return;
	}
	catch (...)
	{
		//strReMsg = _T("Interface Exception!");
		//m_listMsgBox.InsertString(0, strReMsg);
	}
}



HkVmProcesser::~HkVmProcesser() {
	if (NULL != m_pVmSol)
	{
		DestroySolutionInstance(m_pVmSol);
		m_pVmSol = NULL;
	}
}


bool HkVmProcesser::InputSet(std::string inModName,rxsImg* img) {
	try {
		ImageSourceModuleTool* pObject = (ImageSourceModuleTool*)(*m_pVmSol)[(fname + inModName).c_str()];
		if (NULL == pObject) return false;
		VisionMasterSDK::ImageBaseData ibd;
		ibd.DataLen = img->Len;
		ibd.Height = img->rows;
		ibd.Width = img->cols;
		ibd.ImageData = img->Data;
		if (img->Channels == 1) {
			ibd.Pixelformat = MvdPixelFormat::MVD_PIXEL_MONO_08;
		}
		else {
			ibd.Pixelformat = MvdPixelFormat::MVD_PIXEL_RGB_RGB24_C3;
		}
		pObject->SetImageData(&ibd);
		return true;
	}
	catch (CVmException e)
	{
	}
	return false;
}

int HkVmProcesser::Run() {
	try
	{
		if (NULL == m_pVmSol) return -1;
		m_pVmSol->Run();
		return 0;
	}
	catch (CVmException e)
	{
		//strReMsg.Format(_T("%x"), e.GetErrorCode());
		//strReMsg = _T("0x") + strReMsg + _T(" == Run()");
		//m_listMsgBox.InsertString(0, strReMsg);
		return e.GetErrorCode();
	}
}


int HkVmProcesser::GetResult(std::string flowObjName, rxsResultReport* rpp) {
	try {

		VisionMasterSDK::DataSetModule::DataSetModuleTool* pdsObject =
			(VisionMasterSDK::DataSetModule::DataSetModuleTool*)(*m_pVmSol)[(fname + flowObjName).c_str()];
		if (pdsObject == 0) { return -100; }

		VisionMasterSDK::DataSetModule::DataSetResults* pdsResult = pdsObject->GetResult();
		if (pdsResult == 0) { return -101; }

		int rnum = pdsResult->GetResultNum();
		std::string fv[3] = { "NO","名","值" };
		std::string rn = "三方结果:";
		rpp->SetFiled(fv, 3, (rn + flowObjName));
		int r = 0;
		for (int n = 0; n < rnum; n++) {
			VisionMasterSDK::OutputResultInfo* ori = pdsResult->GetResult(n);
			std::string spn = ori->strParamName;
			if (spn == "ModuStatus") { continue; }

			r = rpp->AddRow();
			rpp->SetValue("NO", std::to_string(r), r - 1);
			rpp->SetValue("名", ori->strParamName, r - 1);

			if (ori->nParamType == 3) {
				char* V = ori->pstStringValue->strValue;
				if (V != 0) {
					rpp->SetValue("值", V, r - 1);
				}
				else {
					rpp->SetValue("值", "null", r - 1);
				}
			}
			else if (ori->nParamType == 2) {
				std::string v = std::to_string(*ori->pFloatValue);
				rpp->SetValue("值", v, r - 1);
			}
			else if (ori->nParamType == 1) {
				std::string v = std::to_string(*ori->pIntValue);
				rpp->SetValue("值", v, r - 1);
			}
			else {
			}
		}
		return 0;
	}
	catch (CVmException e)
	{
		return e.GetErrorCode();
	}
}



int HkVmProcesser::GetResultImg(std::string flowObjName, rxsImg* outimg) {
	try {

		VisionMasterSDK::SaveImage::SaveImageTool* pdsObject =
			(VisionMasterSDK::SaveImage::SaveImageTool *)(*m_pVmSol)[(fname + flowObjName).c_str()];
		if (pdsObject == 0) { return -1; }

		VisionMasterSDK::SaveImage::SaveImageResults* pResult = pdsObject->GetResult();
		if (NULL == pResult) return -2;
		VisionMasterSDK::ImageBaseData idb = pResult->GetOutputImage();

		if (idb.Pixelformat == VisionMasterSDK::MvdPixelFormat::MVD_PIXEL_MONO_08) {
			outimg->Channels = 1;
		}
		else if (idb.Pixelformat == VisionMasterSDK::MvdPixelFormat::MVD_PIXEL_RGB_RGB24_C3) {
			outimg->Channels = 3;
		}
		else {
			return -2;
		}
		outimg->cols = idb.Width;
		outimg->rows = idb.Height;
		outimg->Len = idb.DataLen;
		outimg->Data = idb.ImageData;
		return 0;
	}
	catch (CVmException e) {
		return e.GetErrorCode();
	}
}









#endif




std::string GetFileNameFormPath(const  char* path) {
	int si_n = -1;
	int si_f = -1;
	int ei = 0;
	for (int x = 0; path[x] != '\0'; x++) {
		if (path[x] == '\\') {
			si_n = x;
			continue;
		}
		if (path[x] == '/') {
			si_f = x;
			continue;
		}
		if (path[x] == '.') {
			ei = x;
			break;
		}
	}
	int si = 0;
	if (si_n * ei > 0) {
		si = si_n + 1;
		goto f_end;
	}
	if (si_f * ei > 0) {
		si = si_f + 1;
		goto f_end;
	}

f_end:
	int l = ei - si;
	if (l < 1) { return ""; }
	char* v = (char*)malloc(l + 1);
	if (v == 0) { return ""; }
	v[l] = 0;
	memcpy(v, &path[si], l);
	std::string fv = v;
	free(v);
	return fv;
}


int mylock = 0;


int LoadThridSolutionAndExer(const char* solPath, const char* passworld, rxsImg* inImg, rxsResultReport* rpp,rxsImg *outimg,
	const char* srcinBlkName, const char* outdataBlkName, const char* outimgBlkName) 
{
	int errcode = 0;
	while (mylock != 0) {
		_sleep(50);
	}
	mylock = 1;
#ifdef HK_VMASTER
	std::string flname = GetFileNameFormPath(solPath);
	HkVmProcesser hkvm(flname, solPath, passworld);
	if (srcinBlkName != 0 && inImg != 0) {
		hkvm.InputSet(srcinBlkName, inImg);
	}
	if ((errcode = hkvm.Run()) == 0) {
		if (outdataBlkName != 0 && rpp != 0) {
			if ((errcode = hkvm.GetResult(outdataBlkName, rpp)) == 0) {

			}
		}
		if (outimgBlkName != 0 && outimg != 0) {
			rxsImg reult = { 0,0,0,0,0 };
			errcode = hkvm.GetResultImg(outimgBlkName, &reult);
			if (reult.Len > 0 && reult.Data) {
				*outimg = reult;
				outimg->Data = malloc(reult.Len);
				if (outimg->Data == 0) { 

					return errcode; 
				}
				memcpy(outimg->Data, reult.Data, reult.Len);
				Mat img(outimg->rows, outimg->cols, CV_MAKETYPE(CV_8U, outimg->Channels), outimg->Data, outimg->cols * outimg->Channels);
				imwrite("testResult.jpg", img);
			}
		}
	}

#else


#endif // HK_VMASTER

	mylock = 0;
	return errcode;
}