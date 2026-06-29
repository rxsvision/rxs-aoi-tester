#include "private.h"


extern HrdWrkSyetemEnv hrdSyetemEnver;

void leastSquaresMethod(const Mat &A, const Mat &B, Mat &X) {
    // 最小二乘法求解 Ax = B 中的 x
    Mat AtA = A.t() * A; // A 的转置乘以 A
    Mat AtB = A.t() * B; // A 的转置乘以 B
    X = AtA.inv() * AtB; // 求解 x = (A^T * A)^(-1) * (A^T * B)
}


imgToolKitBlock::imgToolKitBlock() {
    nmpm = new std::map<std::string, int>;
    nameMapIter = new std::map<std::string, int>::iterator;
    inGrps = new std::vector<rxsImg>;
    imgResult = 0;
    pSystemer = hrdSyetemEnver.pSystemer;
    outResultReport = 0;
    lightSource = (CST_LightSource*)hrdSyetemEnver.pSystemer->Wrkpd->HrdWareObjectGet(2);
    AddmethodNamemap("光度立体法", 0);
    AddmethodNamemap("颜色识别", 1);
}

imgToolKitBlock::~imgToolKitBlock() {
    if (nameMapIter != 0) {
        delete (std::map<std::string, int>::iterator*)nameMapIter;
        nameMapIter = 0;
    }
    if (nmpm != 0) {
        delete (std::map<std::string, int> *)nmpm;
        nmpm = 0;
    }
    if (inGrps != 0) {
        delete (std::vector<rxsImg>*)inGrps;
        inGrps = 0;
    }
    imgResult = 0;
    outResultReport = 0;
}

void imgToolKitBlock::AddmethodNamemap(const char* name, int no) {
    std::map<std::string, int>* nameMap = (std::map<std::string, int> *)nmpm;
    (*nameMap)[name] = no;
}

int imgToolKitBlock::MethodNum() {
    return ((std::map<std::string, int> *)nmpm)->size();
}

bool imgToolKitBlock::MethodExist(const char* methodName) {
    std::string mn = methodName;
    std::map<std::string, int>::iterator miter = ((std::map<std::string, int> *)nmpm)->find(mn);
    return (miter != ((std::map<std::string, int> *)nmpm)->end());
}

const char* imgToolKitBlock::MethodNameForeach(bool& isStart) {
    if (isStart) {
        (*(std::map<std::string, int>::iterator*)nameMapIter) = ((std::map<std::string, int> *)nmpm)->begin();
        isStart = false;
        std::map<std::string, int>::iterator* nmIter = (std::map<std::string, int>::iterator*)nameMapIter;
        return (*nmIter)->first.c_str();
    }
    std::map<std::string, int>::iterator* nmIter = (std::map<std::string, int>::iterator*)nameMapIter;
    (*nmIter)++;
    if ((*nmIter) == ((std::map<std::string, int> *)nmpm)->end()) { return 0; }
    return (*nmIter)->first.c_str();
}

rxsImg* imgToolKitBlock::GetInImg(int& Num) {
    std::vector<rxsImg>* A = ((std::vector<rxsImg>*)inGrps);
    Num = A->size();
    if (Num < 1) {
        return 0;
    }
    return &(*A)[0];
}

void imgToolKitBlock::inputClr() {
    ((std::vector<rxsImg>*)inGrps)->clear();
}

void imgToolKitBlock::inputPush(rxsImg* inv, int nums) {
    if (nums > 0) {
        for (int i = 0; i < nums; i++)
            (*(std::vector<rxsImg>*)inGrps).push_back(inv[i]);
    }
}

void imgToolKitBlock::RunEnvironmentSet(rxsImg* outImgResult, rxsResultReport* rreport) {
    imgResult = outImgResult;
    outResultReport = rreport;
}

short imgToolKitBlock::runProcessActiver(rxsImg* inimg, int nums, rxsResultReport* rr, const char* Fname) { return false; }

bool imgToolKitBlock::RunUser(const char* methodName) {

    std::vector<rxsImg>* A = (std::vector<rxsImg>*)inGrps;
    short v = runProcessActiver(&(*A)[0], A->size(), outResultReport,methodName);
    return (v == 0);
}

bool imgToolKitBlock::Run(const char* methodName) {
    std::vector<rxsImg>* A = (std::vector<rxsImg>*)inGrps;
    int inLen = A->size();
    if (inLen < 1) {
        return false;
    }
    std::map<std::string, int>* nameMap = (std::map<std::string, int> *)nmpm;
    int i = (*nameMap)[methodName];
    switch (i)
    {
    case 0:
        if (inLen != 4) { return false; }
        return (Photometric(&(*A)[0]) != 0);
    case 1:
        return ColorRecognition(&(*A)[0]);
    default:
        break;
    }
    return false;
}


rxsImg* imgToolKitBlock::Photometric(rxsImg* SinImg) {
    Mat grayImage1(SinImg[0].rows, SinImg[0].cols, CV_MAKETYPE(CV_8U, SinImg[0].Channels), SinImg[0].Data, SinImg[0].cols * SinImg[0].Channels);
    Mat grayImage2(SinImg[1].rows, SinImg[1].cols, CV_MAKETYPE(CV_8U, SinImg[1].Channels), SinImg[1].Data, SinImg[1].cols * SinImg[1].Channels);
    Mat grayImage3(SinImg[2].rows, SinImg[2].cols, CV_MAKETYPE(CV_8U, SinImg[2].Channels), SinImg[2].Data, SinImg[2].cols * SinImg[2].Channels);
    Mat grayImage4(SinImg[3].rows, SinImg[3].cols, CV_MAKETYPE(CV_8U, SinImg[3].Channels), SinImg[3].Data, SinImg[3].cols * SinImg[3].Channels);

    imwrite("inGray1.bmp", grayImage1);
    imwrite("inGray2.bmp", grayImage2);
    imwrite("inGray3.bmp", grayImage3);
    imwrite("inGray4.bmp", grayImage4);
    //Mat image1 = imread("D:\\ruixinTV_photometric_stereo\\1\\0.bmp");
    //Mat image2 = imread("D:\\ruixinTV_photometric_stereo\\1\\1.bmp");
    //Mat image3 = imread("D:\\ruixinTV_photometric_stereo\\1\\2.bmp");
    //Mat image4 = imread("D:\\ruixinTV_photometric_stereo\\1\\3.bmp");

    //cv::Mat grayImage1, grayImage2, grayImage3, grayImage4;
    //cv::cvtColor(image1, grayImage1, cv::COLOR_BGR2GRAY);
    //cv::cvtColor(image2, grayImage2, cv::COLOR_BGR2GRAY);
    //cv::cvtColor(image3, grayImage3, cv::COLOR_BGR2GRAY);
    //cv::cvtColor(image4, grayImage4, cv::COLOR_BGR2GRAY);
    // 将灰度图像转换为 CV_32FC1 格式
    grayImage1.convertTo(grayImage1, CV_32FC1);
    grayImage2.convertTo(grayImage2, CV_32FC1);
    grayImage3.convertTo(grayImage3, CV_32FC1);
    grayImage4.convertTo(grayImage4, CV_32FC1);

    // 获取图像的行数和列数
    int rows = grayImage1.rows;
    int cols = grayImage1.cols;
    //QImage img(image1.data, image1.cols, image1.rows, image1.step, QImage::Format_RGB888);
    //QPixmap pixmap222 = QPixmap::fromImage(img);
    //ui->label->setPixmap(pixmap222);
    //ui->label->setScaledContents(true);
    int a = rows * cols;

    // 创建四个大小为 rows x cols 的全为零的矩阵
    Mat M1 = Mat::zeros(1, a, CV_32FC1);
    Mat M2 = Mat::zeros(1, a, CV_32FC1);
    Mat M3 = Mat::zeros(1, a, CV_32FC1);
    Mat M4 = Mat::zeros(1, a, CV_32FC1);
    // 将图像的灰度值存入相应的矩阵中
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            float pixel_value1 = grayImage1.at<float>(y, x);
            float pixel_value2 = grayImage2.at<float>(y, x);
            float pixel_value3 = grayImage3.at<float>(y, x);
            float pixel_value4 = grayImage4.at<float>(y, x);

            // 如果灰度小于10，则存入0
            if (pixel_value1 < 40) pixel_value1 = 0;
            if (pixel_value2 < 40) pixel_value2 = 0;
            if (pixel_value3 < 40) pixel_value3 = 0;
            if (pixel_value4 < 40) pixel_value4 = 0;

            M1.at<float>(0, y * cols + x) = pixel_value1;
            M2.at<float>(0, y * cols + x) = pixel_value2;
            M3.at<float>(0, y * cols + x) = pixel_value3;
            M4.at<float>(0, y * cols + x) = pixel_value4;
        }
    }
    Mat MM = Mat::zeros(4, a, CV_32F);

    // 将四个矩阵的信息依次存入 MM 中
    for (int i = 0; i < a; ++i) {
        MM.at<float>(0, i) = M1.at<float>(0, i);
        MM.at<float>(1, i) = M2.at<float>(0, i);
        MM.at<float>(2, i) = M3.at<float>(0, i);
        MM.at<float>(3, i) = M4.at<float>(0, i);
    }

    // 创建 4x3 的矩阵 L
    Mat L = (Mat_<float>(4, 3) <<
        1, 0, 1,
        -1, 0, 1,
        0, 1, 1,
        0, -1, 1);
    // 创建矩阵 N，大小为 3x(rows*cols)
    Mat N;
    leastSquaresMethod(L, MM, N);

    // 创建矩阵 averaged_normals，大小为 rows x cols
    cv::Mat averaged_normals = cv::Mat::zeros(1, rows * cols, CV_32F);
    std::cout << "Matrix N:" << std::endl;

    // 创建三个大小为 rows x cols 的矩阵
    cv::Mat N_row1 = cv::Mat::zeros(rows, cols, CV_32F);
    cv::Mat N_row2 = cv::Mat::zeros(rows, cols, CV_32F);
    cv::Mat N_row3 = cv::Mat::zeros(rows, cols, CV_32F);

    // 将矩阵 N 的数据按照要求存入这三个矩阵中
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int index = i * cols + j;
            N_row1.at<float>(i, j) = N.at<float>(0, index); // 第一行元素
            N_row2.at<float>(i, j) = N.at<float>(1, index); // 第二行元素
            N_row3.at<float>(i, j) = N.at<float>(2, index); // 第三行元素
        }
    }

    // 显示归一化后的三个矩阵
    //cv::imshow("Normalized Row 1", N_row1);
    //cv::imshow("Normalized Row 2", N_row2);
    //cv::imshow("Normalized Row 3", N_row3);

    // 缩放法向量的每个分量到 [0, 255] 范围内
    cv::normalize(N_row1, N_row1, 0, 255, cv::NORM_MINMAX);
    cv::normalize(N_row2, N_row2, 0, 255, cv::NORM_MINMAX);
    cv::normalize(N_row3, N_row3, 0, 255, cv::NORM_MINMAX);

    // 将每个分量作为RGB通道的值赋给图像
    cv::Mat rgbImage(rows, cols, CV_8UC3);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            uchar b = N_row1.at<float>(i, j);
            uchar g = N_row2.at<float>(i, j);
            uchar r = N_row3.at<float>(i, j);
            rgbImage.at<cv::Vec3b>(i, j) = cv::Vec3b(b, g, r);
        }
    }
    // 将矩阵 N 的数据转换为梯度图
    cv::Mat gradientImage(rows, cols, CV_32FC3);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float x = N.at<float>(0, i * cols + j);
            float y = N.at<float>(1, i * cols + j);
            float z = N.at<float>(2, i * cols + j);

            // 计算梯度强度
            float magnitude = sqrt(x * x + y * y + z * z);
            float normalizedX = 0;
            float normalizedY = 0;
            float normalizedZ = 0;
            // 归一化梯度分量，并将它们映射到[0, 255]
            // float normalizedX = (x / magnitude) * 127.5 + 127.5;
            // float normalizedY = (y / magnitude) * 127.5 + 127.5;
            // float normalizedZ = (z / magnitude) * 127.5 + 127.5;
            // 添加亮度低于40的判断
            if (x > 150 && y > 150 && z > 150) {
                // 如果亮度低于40，将像素设置为全黑
                normalizedX = 0;
                normalizedY = 0;
                normalizedZ = 0;
            }
            else {
                // 否则将像素设置为读取到的值
                normalizedX = (x / magnitude) * 127.5 + 127.5;
                normalizedY = (y / magnitude) * 127.5 + 127.5;
                normalizedZ = (z / magnitude) * 127.5 + 127.5;
            }

            // 将梯度分量写入梯度图像的相应位置
            gradientImage.at<cv::Vec3f>(i, j) = cv::Vec3f(normalizedX, normalizedY, normalizedZ);
        }
    }
    // 将梯度图像转换为8位无符号整数格式，并转换为可显示的图像
    cv::Mat gradientImage8bit;
    gradientImage.convertTo(gradientImage8bit, CV_8U);
    imwrite("gradient.bmp", gradientImage8bit);

    if (imgResult->Data != 0) {
        free(imgResult->Data);
    }
    imgResult->Channels = (u8)gradientImage8bit.channels();
    imgResult->rows = (unsigned)gradientImage8bit.rows;
    imgResult->cols = (unsigned)gradientImage8bit.cols;

    imgResult->Len = imgResult->Channels * imgResult->cols * imgResult->rows;
    imgResult->Data = malloc(imgResult->Len);
    memcpy(imgResult->Data, gradientImage8bit.data, imgResult->Len);
    return imgResult;
}


bool imgToolKitBlock::ColorRecognition(rxsImg* Sin) {
    Mat image(Sin->rows, Sin->cols, CV_MAKETYPE(CV_8U, Sin->Channels), Sin->Data, Sin->cols * Sin->Channels);
    // 灰度总和
    long long totalGray = 0;
    // 获取图像的通道数
    int channels = image.channels();
    // 遍历图像的每个像素
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            // 获取当前像素的指针
            const uchar* pixel = image.ptr<uchar>(i, j);

            // 计算灰度值并累加到总和
            if (channels == 1) {  // 灰度图像
                totalGray += pixel[0];
            }
            else if (channels == 3) {  // 彩色图像
                totalGray += 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];
            }
        }
    }

    // 计算平均灰度值
    int averageGray = totalGray / (image.rows * image.cols);
    int r = outResultReport->AddRow();
    int rowIndex = r - 1;
    std::string fileds[3] = { "序号","结果值","颜色" };
    outResultReport->SetFiled(fileds, 3, "颜色识别");
    outResultReport->SetValue("序号", std::to_string(r), rowIndex);
    outResultReport->SetValue("结果值", std::to_string(averageGray), rowIndex);
    // Change the text color based on the value of 'a'
    if (averageGray >= 0 && averageGray <= 10) {
        //ui->label_2->setText("ultraviolet");
    }
    else if (averageGray >= 20 && averageGray <= 30) {
        // Set label text color to red
        //ui->label_2->setText("yellow");
    }
    else if (averageGray >= 60 && averageGray <= 70) {
        // Set label text color to black
        //ui->label_2->setText("red;");
    }
    else if (averageGray >= 70 && averageGray <= 80) {
        // Set label text color to black
        //ui->label_2->setText("infrared light II");
    }
    else if (averageGray >= 90 && averageGray <= 100) {
        outResultReport->SetValue("颜色", "green", rowIndex);
    }
    else if (averageGray >= 130 && averageGray <= 140) {
        outResultReport->SetValue("颜色", "white", rowIndex);
    }
    else if (averageGray >= 150 && averageGray <= 160) {
        outResultReport->SetValue("颜色", "blue", rowIndex);
    }
    else {
        outResultReport->SetValue("颜色", "infrared light I", rowIndex);
    }
    return true;
}

