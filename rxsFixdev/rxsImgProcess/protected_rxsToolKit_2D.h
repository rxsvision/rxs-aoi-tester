#pragma once
#include "imgProcess.h"
#include <opencv2/opencv.hpp>


#pragma warning(disable : 4996)


#define NOMINMAX
#define BOOST_USE_WINDOWS_H
#define _CRT_SECURE_NO_WARNINGS




class rxsToolKit2D {
public:
	rxsToolKit2D();
	~rxsToolKit2D();


	/// <summary>
	/// 找出图像中异于背景的区域
	/// </summary>
	/// <param name="img">输入图像</param>
	/// <param name="kernel_size">缺陷越大个这个值就需要越大</param>
	/// <param name="alpha">输出图像灰度值线性放大，图像太暗可以设置成2亮度值放大两倍</param>
	void dimple(cv::Mat& img, int kernel_size, int alpha);


	/// <summary>
	/// 合并多幅不同曝光的图像
	/// </summary>
	/// <param name="images">图像</param>
	/// <param name="times">曝光时间，单位s</param>
	/// <returns></returns>
	rxsImg* createHDR_dll(const std::vector<cv::Mat>& images, std::vector<float>& times);


	/// <summary>
	/// 多幅不同焦距的图像融合
	/// </summary>
	/// <param name="images">多幅不同焦距的图</param>
	/// <returns></returns>
	rxsImg* fuseImages_dll(const std::vector<cv::Mat>& images);

	/// <param name="model_path">模型配置文件的路径</param>
	/// <param name="pic_root">多光谱照片的根目录，内有各光谱照片，命名要与光谱颜色一致，命名有误会导致无法识别</param>
	std::string useModel_ColorReg(std::string model_path, std::string pic_root);

	/// <param name="model_root">模型根目录，根目录内有各颜色文件夹，各颜色文件夹内是多光谱照片，
	/// 多光谱照片命名需与光谱颜色一致，命名有误会导致无法识别</param>
	/// <param name="save_path">配置文件保存路径</param>
	bool trainModel_ColorReg(std::string model_root, std::string save_path);


	/// <param name="imgs">各个方向光照的图片</param>
	/// <param name="origin_img">原始图</param>
	/// <param name="slants">倾斜角，侧视图中光源和相机的夹角，顺序与图片对应</param>
	/// <param name="tilts">俯仰角，俯视图中光源和x轴的夹角，顺序与图片对应</param>
	/// <param name="id">唯一标志符，与ROI相关</param>
	/// <returns>合成后的图片</returns>
	bool photometricStero(void* images, cv::Mat* ori, std::vector<float> slants, std::vector<float> tilts, cv::Mat& outmat, std::string id);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="four_img">四个扇区的照片</param>
	/// <param name="origin_img">四个扇区全亮的照片</param>
	/// <param name="id">id</param>
	/// <returns></returns>
	rxsImg* HideGlue(std::vector<cv::Mat>& fiveImg, const std::string& id);


	/// <summary>
	/// 某个id第一次运行，会启用roi框选，有三层SLOT，中间的一层尽可能贴近图像的SLOT，外层和内层分别是SLOT允许的波动范围
	/// </summary>
	/// <param name="img"></param>
	/// <param name="id"></param>
	/// <returns>输出一个点集合，实际上只有一个点，代表着SLOT所在的点,另外img中的SLOT会被替换成白色的圆，便于vm查找</returns>
	std::vector<cv::Point2f> slotFit(cv::Mat& img, std::string id);
	
	rxsImg* mergeByROI(std::vector<cv::Mat>& imgs, const std::string& id);
private:
	rxsImg rimg;
	rxsImg* resultImgSet(rxsImg *);
};