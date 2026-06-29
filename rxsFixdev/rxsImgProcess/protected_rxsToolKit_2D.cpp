#include "protected_rxsToolKit_2D.h"



#include <pcl/io/pcd_io.h>



HMODULE	_dllHand_;
HMODULE	_dllHand_N2;
HMODULE	_dllHand_N3;
HMODULE	crd;
HMODULE psd;


rxsToolKit2D::rxsToolKit2D() {
    memset(&rimg, 0, sizeof(rxsImg));
    _dllHand_ = LoadLibrary(L"czxToolkit2D.dll");
    _dllHand_N2 = LoadLibrary(L"HideGlueDLL.dll");
    _dllHand_N3 = LoadLibrary(L"SlotFitDLL.dll");

    crd = LoadLibrary(L"colorRecognitionDLL.dll");
    psd = LoadLibrary(L"photometricSteroDLL.dll");
}


rxsToolKit2D::~rxsToolKit2D() {
    FreeLibrary(crd);
    FreeLibrary(psd);

    FreeLibrary(_dllHand_);
    FreeLibrary(_dllHand_N2);
    FreeLibrary(_dllHand_N3);
    if (rimg.Data != 0) {
        free(rimg.Data);
    }
    memset(&rimg, 0, sizeof(rxsImg));
}

rxsImg* rxsToolKit2D::resultImgSet(rxsImg* r) {
    if (rimg.Data == 0) {
        rimg = *r;
        rimg.Data = malloc(r->Len);
        memcpy(rimg.Data, r->Data, r->Len);
        return &rimg;
    }
    if (rimg.Len < r->Len) {
        free(rimg.Data);
        rimg = *r;
        rimg.Data = malloc(r->Len);
        memcpy(rimg.Data, r->Data, r->Len);
        return &rimg;
    }
    else {
        memcpy(rimg.Data, r->Data, r->Len);
        rimg.Channels = r->Channels;
        rimg.rows = r->rows;
        rimg.cols = r->cols;
        return &rimg;
    }
}

void rxsToolKit2D::dimple(cv::Mat& img, int kernel_size, int alpha) {
    typedef void(*dimple)(cv::Mat& img, int kernel_size, int alpha);
    dimple func = (dimple)GetProcAddress(_dllHand_, "dimple");
    if (!func)
        return;
    func(img, kernel_size, alpha);
}

rxsImg* rxsToolKit2D::createHDR_dll(const std::vector<cv::Mat>& images, std::vector<float>& times) {
    typedef cv::Mat(*createHDR)(const std::vector<cv::Mat>&, std::vector<float>&);
    createHDR func = (createHDR)GetProcAddress(_dllHand_, "createHDR_dll");
    if (!func)
        return 0;
    //for (int i = 0; i < images.size(); i++) {
    //    int v = images[i].data[500 * images[i].cols + 500];
    //    std::cout << std::to_string(images[i].type()) << std::to_string(v) << std::endl;
    //}
    cv::Mat r = func(images, times);
    if (r.empty()) {
        return 0;
    }
    //float a = r.at<float>(500,500);
    //std::cout << std::to_string(r.type()) <<a << std::endl;
    r.convertTo(r, CV_8U, 255);
    //cv::imwrite("HDR.jpg", r);
    int dl = r.channels() * r.rows * r.cols;
    rxsImg ri = { r.channels(),r.data,dl,r.rows,r.cols };
    return resultImgSet(&ri);
}


rxsImg* rxsToolKit2D::fuseImages_dll(const std::vector<cv::Mat>& images) {
    typedef cv::Mat(*fuseImages)(const std::vector<cv::Mat>&);
    fuseImages func = (fuseImages)GetProcAddress(_dllHand_, "fuseImages_dll");
    if (!func)
        return 0;

    cv::Mat r = func(images);
    if (r.empty()) {
        return 0;
    }
    r.convertTo(r, CV_8U, 255);
    //cv::imwrite("fuseImages.jpg", r);
    int dl = r.channels() * r.rows * r.cols;
    rxsImg ri = { r.channels(),r.data,dl,r.rows,r.cols };
    return resultImgSet(&ri);
}




std::vector<cv::Point2f> rxsToolKit2D::slotFit(cv::Mat& img, std::string id) {
    std::vector<cv::Point2f> r;
    typedef std::vector<cv::Point2f>(*slotfit)(cv::Mat&, std::string);
    slotfit func = (slotfit)GetProcAddress(_dllHand_N3, "slotFit");
    if (!func)
        return r;

    r = func(img, id);
    return r;
}


rxsImg* rxsToolKit2D::mergeByROI(std::vector<cv::Mat>& imgs, const std::string& id)
{
    /// <summary>
    /// 把patch_img中ROI部分复制到main_img
    /// </summary>
    /// <param name="main_img">主图形-----不亮</param>
    /// <param name="patch_img">补丁图像，----亮--ROI(工行)内部分会保留</param>
    /// <param name="id">id名，每个id首次会启动ROI框选</param>
    /// <returns>返回打好补丁的图片</returns>
    typedef cv::Mat(*mergebyroi)(cv::Mat& main_img, cv::Mat patch_img, const std::string& id);
    mergebyroi func = (mergebyroi)GetProcAddress(_dllHand_N2, "mergeByROI");
    if (!func && imgs.size() != 2)
        return 0;

    cv::Mat r = func(imgs[0], imgs[1], id);
    if (r.empty()) {
        return 0;
    }

    cv::imwrite(id + "ps.bmp", r);
    int dl = r.channels() * r.rows * r.cols;
    rxsImg ri = { r.channels(),r.data,dl,r.rows,r.cols };
    return resultImgSet(&ri);
}



rxsImg* rxsToolKit2D::HideGlue(std::vector<cv::Mat>& imgs, const std::string& id) {
    typedef cv::Mat(*hideglue)(const std::vector<cv::Mat>&, cv::Mat, const std::string&);
    hideglue func = (hideglue)GetProcAddress(_dllHand_N2, "HideGlue");
    if (!func)
        return 0;

    cv::Mat org;
    imgs[imgs.size() - 1].copyTo(org);
    imgs.pop_back();
    cv::Mat r = func(imgs, org, id);
    if (r.empty()) {
        return 0;
    }

    if (id == "B1C1C5" || id == "C6C2B2" ) {
        cv::Mat ror;
        cv::rotate(r, ror, cv::ROTATE_90_CLOCKWISE);
        slotFit(ror, id);
        cv::rotate(ror, r, cv::ROTATE_90_COUNTERCLOCKWISE);
    }
    else if (id == "B6") {
        slotFit(r, id);
    }

    cv::imwrite(id + "_photometricStero.bmp", r);
    int dl = r.channels() * r.rows * r.cols;
    rxsImg ri = { r.channels(),r.data,dl,r.rows,r.cols };
    return resultImgSet(&ri);
}


bool rxsToolKit2D::photometricStero(void* imgs, cv::Mat* ori, std::vector<float> slants, std::vector<float> tilts, cv::Mat& outmat, std::string tlfn)
{
    typedef cv::Mat(*photometricSteroFunc)(const std::vector<cv::Mat>& imgs, cv::Mat, const std::vector<float>&, const std::vector<float>&, const std::string& id);
    photometricSteroFunc func = (photometricSteroFunc)GetProcAddress(psd, "photometricStero");
    if (!func) {
        return false;
    }
    std::vector<cv::Mat>* Imgs = (std::vector<cv::Mat>*)imgs;
    cv::Mat r = func(*Imgs, *ori, slants, tilts, tlfn);
    //r.convertTo(r, CV_8U, 255);
    r.copyTo(outmat);
    cv::imwrite(tlfn + "_photometricStero.jpg", r);
    return true;
}


std::string rxsToolKit2D::useModel_ColorReg(std::string model_path, std::string pic_root) {
    typedef std::string(__cdecl* useModelType)(std::string model_path, std::string pic_root);
    useModelType useModel = (useModelType)GetProcAddress(crd, "useModel");
    if (!useModel) {
        return "dll为空!";
    }
    return useModel(model_path, pic_root);
}


bool rxsToolKit2D::trainModel_ColorReg(std::string model_root, std::string save_path) {
    typedef void(__cdecl* trainModelType)(std::string model_root, std::string save_path);
    trainModelType trainModel = (trainModelType)GetProcAddress(crd, "trainModel");
    if (trainModel == 0) {
        return false;
    }
    trainModel(model_root, save_path);
}


