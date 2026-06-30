# 系统架构总览

## 1. 三大子系统概览

rxs-aoi-tester 仓库由三个相对独立的子系统组成，各自承担不同层次的职责：

| 子系统 | 定位 | 技术栈 | 说明 |
|--------|------|--------|------|
| **AoiTester** | 3D 点云应用（主程序） | Qt5 + CloudCompare + PCL | 基于 CloudCompare 衍生的 3D 测量与可视化平台，提供 GUI 操作界面 |
| **rxsFixdev** | 设备 DAL 框架 | C++ / MSVC (v142) | 四层 DAL 架构，封装硬件驱动、图像处理、设备工作流，是系统的核心后端 |
| **flowchart2d** | 2D 流程图引擎 | Qt5 | 可视化流程编排前端，用于配置和运行 2D/3D 检测流程 |

### 子系统交互关系

```
+----------------------------------------------------------+
|                    flowchart2d (前端)                      |
|   可视化流程编排 -> 生成 StepBlock 流程描述文件             |
+----------------------------+-----------------------------+
                             | 流程文件加载 / UI 回调
                             v
+----------------------------------------------------------+
|                  AoiTester (主程序)                        |
|   Qt5 GUI + 3D 可视化 + 用户交互                          |
|   调用 rxsFixdev 导出的 API 进行设备控制与数据处理          |
+----------------------------+-----------------------------+
                             | 链接 rxsFixdal.dll / rxsImgProcess.dll
                             v
+----------------------------------------------------------+
|                  rxsFixdev (DAL 框架)                      |
|   L4 rxsFixdal -> L3 rxsImgProcess -> L2 rxsHrdwdal      |
|                                     -> L1 SystemFrame      |
+----------------------------------------------------------+
```

**交互机制**：
- **AoiTester** 通过链接 `rxsFixdal.dll` 调用 `rxsSystemReady()` / `WorkProcessDalGet()` 等全局 API 初始化系统
- **flowchart2d** 生成的流程描述文件由 `imgTaskProcessCollection::Load()` 加载，在 rxsImgProcess 层执行
- **UI 回调**：前端通过 `rxsMainFrame::FontMessageProcessCB()` 注册 `uiProcessCallBack` 回调函数，后端通过 `FrontProcessMsgSender()` 向前端推送消息

---

## 2. rxsFixdev 四层 DAL 架构详解

rxsFixdev 是整个系统的后端核心，采用四层分层架构（DAL = Data Access Layer），每层编译为独立的库/DLL，严格单向依赖。

### 构建依赖链

```
L1  SystemFrame (STATIC)         <- 基础类型 + 系统框架
        ^
L2  rxsHrdwdal (SHARED DLL)     <- 硬件驱动实现
        ^
L3  rxsImgProcess (SHARED DLL)  <- 图像处理 + StepBlock 引擎
        ^
L4  rxsFixdal (SHARED DLL)      <- 设备工作流编排
        ^
    Tester (EXE x 4)             <- 测试可执行文件
```

### L1: SystemFrame（静态库）

**目录**: `rxsFixdev/SystemEntry/`
**输出**: `SystemFrame.lib`（静态链接到所有下游模块）

SystemFrame 是整个 DAL 框架的基石，定义了所有基础类型和系统级服务：

#### 基础类型定义

| 类型 | 定义位置 | 说明 |
|------|----------|------|
| `rxsPoint` / `rxsPointI` | `public.h` | 2D 浮点/整数坐标点 |
| `rxsPointCouldp` | `public.h` | 3D 点云坐标 (double x, y, z) |
| `rxsRectI` | `public.h` | 整数矩形区域（含旋转角度） |
| `AxisInfo` | `public.h` | 运动轴完整描述：运动参数 + 限位 + 回原点 + 状态 |
| `AxisStsInfo` | `public.h` | 轴实时状态：规划位置/速度/加速度 + 编码器 + 伺服/到位/原点/限位/报警标志 |
| `TrapPrm` | `public.h` | 梯形运动参数（加速度/减速度/起始速度/平滑时间） |
| `MotionModel` | `public.h` | 运动模式枚举：PP/JOG/GROUP（PT/GEAR/FOLLOW/PVT/插补） |
| `SysParam` | `public.h` | 系统全局参数（速度/无人值守/到位检测时间等） |
| `dioPort` | `public.h` | 数字 IO 端口描述 |
| `MeasurementJudgment` | `public.h` | 测量判定结构（标准值 + 正/负公差 + 使能） |

#### 核心类

| 类名 | 说明 |
|------|------|
| **`MotionUnit`** | 运动控制器抽象基类（虚接口），定义 Open/Close/JogMove/PpMoveAbs/Servon/AxHome 等标准方法 |
| **`MotionGpioSerialize`** | 运动+GPIO 序列化操作层，封装轴名映射、多轴协调运动、IO 信号管理 |
| **`ImageableSensorUnit`** | 成像传感器抽象基类（虚接口），定义 open/close/grabContinuity/DataGet 等标准方法 |
| **`FHWObject`** | 硬件对象基类（Request/Response 通信模型），用于光源、调焦等串行设备 |
| **`FixServerBaser`** | 服务基类，持有 MotionGpioSerialize 指针和系统日志 |
| **`rxsResultReport`** / **`rxsResultReportGrpsManger`** | 检测结果报表系统（模板化报表 + 多表管理 + 合并输出） |

#### 系统状态机 (`SystemInfo`)

```
INIT(0) -> RDY(1) -> RUNING(3)
  ^         |         |
  |     PAUSEING(2)   |
  |         |         |
  +-- RESETING(4) <---+
               |
          ALAMING(5) -> RESETING(4)
```

- **INIT**: 系统初始化中
- **RDY**: 准备完成，等待加工指令
- **PAUSEING**: 加工暂停，可通过 Goon() 恢复
- **RUNING**: 正在执行加工流程
- **RESETING**: 系统复位中（回原点等）
- **ALAMING**: 报警状态（急停/伺服/驱动/安全光栅等）

报警类型 (`ALAMTYPE`): EMG(急停) / SERV(伺服) / DRIVER(驱动) / WORKING(加工) / SECURITY(安全)

#### 用户管理 (`UserManger`)

四级权限模型：
- `NOLOGIN(0)`: 未登录
- `OPTER(1)`: 操作员
- `TECH(2)`: 示教技术员/工程师
- `ADMIN(3)`: 管理员

#### 参数管理

| 类 | 说明 |
|---|------|
| `FactoryConfigManger` | 出厂配置管理：键值对参数 + 运动参数表 |
| `WorkParamManger` | 加工运行参数管理：用户参数/加工参数/移动路径(Movepath)/扫描点表(Scanpoint) + 测量项标准值 |

### L2: rxsHrdwdal（硬件驱动 DLL）

**目录**: `rxsFixdev/rxsHrdwdal/`
**输出**: `rxsHrdwdal.dll` + `rxsHrdwdal.lib`
**依赖**: SystemFrame

rxsHrdwdal 是 L1 定义的抽象接口的**具体实现层**，为每种硬件设备提供驱动实现。

#### 运动控制器驱动（继承 `MotionUnit`）

| 实现类 | 对应硬件 | SDK |
|--------|----------|-----|
| `ZMotionDal` | 正运动控制器 | zmotion SDK |
| `AuboRobotDal` | 遨博协作机器人 | auboi5 SDK |
| `robotMotion_Dal` | 通用机械手接口 | 自定义 |

#### 传感器驱动（继承 `ImageableSensorUnit`）

| 实现类 | 传感器类型 | 品牌/型号 |
|--------|-----------|-----------|
| `LightE_AOCC` | 点光谱/共焦 | 立仪 LightE |
| `KeyenceSpectrum` | 点光谱 | 基恩士 Keyence |
| `KeyenceLJX8000A` | 线激光 | 基恩士 LJ-X8000A |
| `NVTLineLsr` | 线激光 | 翌视 NVT |
| `Precitec` | 点光谱/线光谱 | 普雷斯特 Precitec |
| `SSZN_SR8060` | 线激光 | 深视智能 SSZN |
| `MPSizectorDal` | 结构光 | 盛相 MPSizector |
| `HkCream_ch120` | 2D 工业相机 | 海康 HIKVISION |
| `DialGauge` | 数显千分表 | 串行通信 |

#### 光源与调焦驱动（继承 `FHWObject` + `r232Connection`）

| 实现类 | 说明 |
|--------|------|
| `CST_LightSource` | 多光谱多通道光源（4 分区 x 8 色彩：白/蓝/绿/黄/红730/红850/UV/红） |
| `CST_TZls` | 同轴光源控制器（8 通道亮度 + 常亮/频闪模式） |
| `AdjustFocal` | 电动调焦镜头（RS232 通信） |

### L3: rxsImgProcess（图像处理 DLL）

**目录**: `rxsFixdev/rxsImgProcess/`
**输出**: `rxsImgProcess.dll` + `rxsImgProcess.lib`
**依赖**: SystemFrame + rxsHrdwdal

L3 是系统的**流程编排引擎**，核心是基于 StepBlock 的可视化流程执行框架。

#### StepBlock 流程编排引擎

StepBlock 是整个检测流程的基本执行单元。每个 StepBlock 代表一个处理步骤，多个 StepBlock 通过 Linker 连接形成有向图（流程），由 `imgTaskFlowerProcesser` 编排执行。

**核心类层次**:

```
StepBlockBase                    <- 流程块基类（ID/名称/输入参数/运行/输出）
+-- ImgSourceStep                <- 2D 图像输入（硬件采集/本地读取/外部传入）
+-- ImgPreProcessStep            <- 预处理（灰度化/二值化/归一化）
+-- ImgProcessStep               <- 图像处理（裁剪/缩放/镜像）
+-- ImgMergingStep               <- 图像融合（HDR/多焦距融合）
+-- LambertianPhotoMetricstereoStep  <- 光度立体法
+-- ImgOutStep                   <- 图像输出（叠加渲染/保存）
+-- ScanCodeStep                 <- 扫码识别（条码/二维码/数据矩阵）
+-- ColorRecognitionStep         <- 颜色识别
+-- UserCustomStep               <- 用户自定义算法
+-- ThridPartyCallBlk            <- 第三方工具调用（如海康 VM）
+-- ComparisonStep               <- 条件比较器
+-- FormatConvertStep            <- 格式转换（深度图与点云）
+-- roiGrpMangerStep             <- ROI 区域管理
+-- PointCouldSourceStep         <- 3D 点云输入（硬件采集/本地读取）
+-- PointCouldFilterStep         <- 点云滤波
+-- PointCouldPreProcessStep     <- 点云预处理
+-- PointCouldBoundolStep        <- 边界点提取
+-- PointCouldArclenStep         <- 弧长测量
+-- PointCouldLengthStep         <- 长度测量
+-- PointCouldEmplenStep         <- 轮廓长度/锋锐度
+-- PointCouldGapStep            <- 断差/落差测量
+-- PointCouldPoaslStep          <- 线轮廓度/面轮廓度
+-- PointCouldDeformationStep    <- 变形度测量
+-- PointCouldOutStep            <- 点云输出
+-- MotionMovStep                <- 运动移动步骤
+-- LightSourceStep              <- 光源控制步骤
+-- FocusingLensStep             <- 调焦控制步骤
+-- TcpServerRecvStep            <- TCP 通信步骤
```

#### 流程编排核心类

| 类 | 说明 |
|---|------|
| `imgTaskFlowerProcesser` | 单条流程编排器：管理 StepBlock 集合、Linker 连接关系、顺序执行/异步执行 |
| `imgTaskProcessCollection` | 流程集合管理器：多条流程的加载/保存/切换/批量执行 |
| `imgToolKitBlock` | 算法工具箱：用户可扩展的自定义算法注册与调用 |

#### 图像处理工具集

| 工具类 | 说明 |
|--------|------|
| `imgprocToolbase` | 基础工具（通道提取/灰度/二值化/ROI裁剪/缩放/镜像/旋转） |
| `imgTool_Draw` | 绘图工具（圆/矩形/多边形/文字） |
| `imgTool_Filter` | 滤波工具（高斯/中值/均值） |
| `imgTool_EdgeCheck` | 边缘检测（Canny/Sobel） |
| `imgTool_Template` | 模板匹配 |
| `imgTool_Features` | 特征提取与匹配 |
| `imgTool_CodeDataReader` | 条码/二维码/数据矩阵读取 |
| `imgTool_Contour` | 轮廓提取与筛选 |
| `imgTool_Calibration` | 相机标定与畸变校正 |

### L4: rxsFixdal（设备工作流 DLL）

**目录**: `rxsFixdev/rxsFixdal/`
**输出**: `rxsFixdal.dll` + `rxsFixdal.lib`
**依赖**: SystemFrame + rxsHrdwdal + rxsImgProcess

L4 是最顶层的**设备工作流编排层**，将 L2 的硬件驱动和 L3 的图像处理流程整合为完整的检测设备工作流。

#### 核心类

| 类 | 继承自 | 说明 |
|---|--------|------|
| **`WorkProcessDal`** | `WorkProcess` | 检测设备工作流核心：加工启停、复位、传感器管理、光源控制、结果报表、运动路径执行 |
| **`CompoundGather`** | `ImageableGather` | 复合采集器：运动扫描 + 传感器采集 + 数据过滤转换 + 点云拼接 |
| `imgToolKitBlockExt` | `imgToolKitBlock` | 扩展工具箱：光度检测缺陷检测、颜色识别、QR码处理 |

#### WorkProcessDal 工作流

```
加工开始 (Start)
    |
    v
StartReady()           <- 加工前准备（光源/传感器/运动就位）
    |
    v
WorkProcesser(step)    <- 逐步执行加工逻辑
    |   +-- MoveToStartPoint()    <- 移动到路径起点
    |   +-- MovePathProcess()     <- 运动路径执行
    |   |   +-- XmoveGather()     <- X方向扫描采集
    |   |   +-- YmoveGather()     <- Y方向扫描采集
    |   |   +-- CreamGatherAndProcess()  <- 2D相机采集处理
    |   |   +-- GroupDirScanerProcess()  <- 群组运动扫描
    |   +-- imgTaskProcessCollection::Run()  <- 图像处理流程执行
    |   +-- SaveCheckResultReport()          <- 保存检测报告
    |
    v
StopBatchCompleted()   <- 批次结束（结果判定/出料）
    |
    v
StopJob()              <- 作业结束（清理/复位）
```

#### CompoundGather 采集流程

```
ReadyParamSet()        <- 采集参数设置（路径/传感器/方向）
    |
    v
GatherPrevProcess()    <- 采集前处理（光源开启/运动到起始位/触发模式切换）
    |
    v
Gather()               <- 执行采集（传感器触发 + 数据获取）
    |
    v
GatherEndProcess()     <- 采集后处理（数据过滤/坐标转换/点云拼接）
    |
    v
GatherResultGet()      <- 获取采集结果（点云/图像数据）
```

---

## 3. 依赖链图与层间通信机制

### 完整依赖关系

```
+------------------+
|   Tester (EXE)   |  测试程序入口
+--------+---------+
         | 链接
         v
+------------------+
|  L4: rxsFixdal   |  WorkProcessDal / CompoundGather
|    (DLL)         |  设备工作流编排
+---+------+-------+
    |      | 链接
    |      v
    | +--------------------+
    | | L3: rxsImgProcess  |  StepBlock 引擎 / 图像处理工具
    | |    (DLL)            |
    | +---------+----------+
    |           | 链接
    v           v
+------------------+
|  L2: rxsHrdwdal  |  运动控制器 / 传感器 / 光源驱动
|    (DLL)          |
+--------+---------+
         | 静态链接
         v
+------------------+
|  L1: SystemFrame |  基础类型 / MotionUnit / ImageableSensorUnit
|    (STATIC)      |  状态机 / 用户管理 / 参数管理 / 报表系统
+------------------+
```

### 层间通信机制

| 通信方式 | 使用场景 | 说明 |
|---------|---------|------|
| **C++ 继承 + 虚函数** | L1->L2 | L1 定义 `MotionUnit` / `ImageableSensorUnit` 纯虚接口，L2 实现具体驱动 |
| **DLL 导出符号** | L2->L3->L4 | 通过 `__declspec(dllexport)` + `.def` 文件导出类和函数 |
| **函数指针回调** | 后端->前端 | `uiProcessCallBack` 回调函数 + `CBC_*` 消息代码 |
| **动态加载** | L3 内部 | `protected_rxsToolKit.cpp` 通过 `LoadLibrary` + `GetProcAddress` 调用外部算法 DLL |
| **TCP 通信** | 跨进程 | `devCommunicationsServer` / `TcpServerRecvStep` 实现设备间通信 |

---

## 4. 回调机制（uiProcessCallBack + CBC_* 消息代码）

### 回调注册

前端通过 `rxsMainFrame::FontMessageProcessCB()` 注册回调：

```cpp
typedef int (*uiProcessCallBack)(void*, int CBC_Code, void* self);

// 注册
rxsMainFrame* frame = rxsSystemReady();
frame->FontMessageProcessCB(myCallback, this);
```

### CBC 消息代码表

| 代码 | 宏名 | 含义 |
|------|------|------|
| 0 | `CBC_UPDWRKD` | 加工参数更新 |
| 1 | `CBC_WRKSTOP` | 加工停止 |
| 2 | `CBC_WRKSTART` | 加工开始 |
| 3 | `CBC_WRKPAUSE` | 加工暂停 |
| 9 | `CBC_RELOADPAGE` | 重新刷新页面 |
| 10 | `CBC_PCUSHOW` | 显示点云图 |
| 11 | `CBC_COLORPCUSHOW` | 显示带颜色的点云图 |
| 14 | `CBC_IMGSHOW` | 显示 2D 图片 |
| 20 | `CBC_STR_N_MSG` | 字符串一般消息 |
| 21 | `CBC_STR_W_MSG` | 字符串警告消息 |
| 22 | `CBC_STR_E_MSG` | 字符串错误消息 |
| 100 | `CBC_VISUAL_UPD` | 可视化流程报表结果更新 |
| 101 | `CBC_VISUAL_UPDSSR` | 可视化流程单步报表结果更新 |
| 200 | `CBC_VISUAL_ERR` | 可视化流程运行出错 |
| 201 | `CBC_VISUAL_WARING` | 可视化流程运行警告 |

### 消息推送流程

```
后端事件发生
    |
    v
rxsMainFrame::FrontProcessMsgSender(msgNo, param, Ansy)
    |
    v
uiProParam 结构封装 (msgNo + param + 前端指针)
    |
    v
uiProcessCallBack(front_obj, CBC_Code, self)  -> 前端处理函数
```

**线程安全**：消息推送通过 `std::mutex uipLock` 保护，支持同步/异步两种模式。
