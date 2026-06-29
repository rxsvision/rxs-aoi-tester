/// <summary>
/// 找出图像中异于背景的区域
/// </summary>
/// <param name="img">输入图像</param>
/// <param name="kernel_size">缺陷越大个这个值就需要越大</param>
/// <param name="alpha">输出图像灰度值线性放大，图像太暗可以设置成2亮度值放大两倍</param>
__declspec(dllexport) void dimple(cv::Mat& img, int kernel_size, int alpha)