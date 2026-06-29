vector<double> fitBSpline_(CP cloud, bool visual=false)
输入是点云, 是否需要可视化轮廓度
还要有配置文件conf_curve.czx

vector<PointT> semiconductor(CP cloud, double min_x, double max_x, double min_y, double max_y)
输入是点云,和四个确定确定参考平面边框的参数

vector<PointT> PIN(CP cloud, double min_x, double max_x, double min_y, double max_y)
输入是点云,和四个确定确定参考平面边框的参数

Eigen::Matrix4f registration(CP c1, CP c2, string hole_pos_path_x, string hole_pos_path_y)
输入是两幅点云,和两个文件名
这两个文件按行存放着12个数组描述3组孔洞roi,顺序是double min_x, double max_x, double min_y, double max_y,需要注意的是两幅点云的三个洞顺序要对应
输入还需要有配置文件conf.czx 和 ransac.czx
输出把c1旋转到c2的变换矩阵

void keyboard(CP cloud, string initial_plane_pos, vector<string> verify_path, vector<CP>& planes, vector<vector<float>>& flatness, MatrixXf& offset)
输入的点云,初始平面roi坐标文件名,验证平面度所需要平面的roi的文件名
输出通过引用导出,是拟合到的平面,平面度,和段差矩阵


一下两个函数依赖deform_conf.czx
void initModelNake(CP model)
初始化模型，输入是从数模割下来的模型，一个模型只需也只能初始化一次

vector<float> computeDeformationNake(CP model, CP cloud)
计算变形量和2D长度，
输入是初始化后的点云，和待计算的点云
输出模型在上最大偏差量，模型在下最大偏差量，总最大偏差量，变形量V1，变形度V2，锋锐度，2D长度，类型（1表示下凹，-1表示上凸，0表示波形，666表示贴合）

/// <summary>
/// 
/// </summary>
/// <param name="model">模型点云，必须无遮挡</param>
/// <param name="source">待测点云，无要求</param>
/// <param name="interval_x">模型点云的x间距</param>
/// <param name="interval_y">模型点云的y间距</param>
/// <returns>输出与待测点云等长的数组，每个值代表对应待测点云中的点到模型的距离</returns>
vector<float> dis2dis(CP model, CP source, float interval_x, float interval_y)

/// <summary>
/// 
/// </summary>
/// <param name="model_root">输入模型的路径，路径下有各色文件夹，各色文件夹下以颜色命名，文件夹内是对应颜色的图片，图片以打光颜色命名</param>
/// <param name="object_directory">物体文件夹路径，文件夹内各图片以打光颜色命名</param>
/// <returns>返回预测的颜色，颜色范围取决于模型文件夹下的各文件</returns>
__declspec(dllexport) string colorReg(string model_root, string object_directory)