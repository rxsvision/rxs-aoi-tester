# 硬件集成指南

## 1. 支持的硬件设备列表

rxsFixdev 的 rxsHrdwdal 层（L2）为各类工业硬件提供了统一的驱动实现。所有驱动均通过继承 L1 (SystemFrame) 中定义的抽象基类来实现。

### 1.1 运动控制器

运动控制器驱动继承自 `MotionUnit` 抽象基类，提供轴运动、IO 控制、回原点等统一接口。

| 实现类 | 硬件设备 | SDK 目录 | 通信方式 | 说明 |
|--------|----------|----------|----------|------|
| `ZMotionDal` | 正运动控制器 (ZMotion) | `rxsFixdev/zmotion/x86/` | 以太网 | 支持 BAS 脚本上传、自动搜索 IP、圆弧插补 |
| `AuboRobotDal` | 遨博协作机器人 (Aubo i5) | `rxsFixdev/auboi5/inc/` | 以太网 | 六轴协作机械臂，支持关节/笛卡尔运动 |
| `robotMotion_Dal` | 通用机械手接口 | 自定义 | 以太网/串口 | 标准化的机械手控制接口 |

### 1.2 点光谱/共焦传感器

点光谱传感器继承自 `ImageableSensorUnit`，用于单点高度/厚度测量。

| 实现类 | 硬件设备 | SDK 目录 | 通信方式 | 说明 |
|--------|----------|----------|----------|------|
| `LightE_AOCC` | 立仪点光谱 | `rxsFixdev/LightE/inc/` | USB/以太网 | 支持内触发/外触发、高度数据/厚度数据模式、暗校正 |
| `KeyenceSpectrum` | 基恩士点光谱 | `rxsFixdev/KeyenceLib/include/` | 以太网 | 自定义协议通信，支持数据存储对象读写 |
| `Precitec` | 普雷斯特传感器 | `rxsFixdev/Precitec/include/` | 以太网 | 支持起始/终止点扫描模式 |

### 1.3 线激光传感器

线激光传感器用于轮廓扫描和 3D 重建，通过运动轴联动生成点云数据。

| 实现类 | 硬件设备 | SDK 目录 | 通信方式 | 说明 |
|--------|----------|----------|----------|------|
| `KeyenceLJX8000A` | 基恩士 LJ-X8000A | `rxsFixdev/KeyenceLib/include/` | 以太网 | 高速线激光轮廓仪，直接输出点云 |
| `NVTLineLsr` | 翌视线激光 | `rxsFixdev/lvm_sdk/x86/include/` | 以太网 | 支持扫描激光线开关、量程配置 |
| `SSZN_SR8060` | 深视智能 SR8060 | `rxsFixdev/sszn/SR_SdkDll/` | 以太网 | 支持双相机、编码器触发、强度数据输出 |
| `Precitec` | 普雷斯特线光谱 | `rxsFixdev/Precitec/include/` | 以太网 | 同时支持点/线模式 |

### 1.4 结构光传感器

| 实现类 | 硬件设备 | SDK 目录 | 通信方式 | 说明 |
|--------|----------|----------|----------|------|
| `MPSizectorDal` | 盛相 MPSizector | `rxsFixdev/SizectorAPI/` | 以太网 | 结构光 3D 相机，支持多设备、直接输出点云 |

### 1.5 2D 工业相机

| 实现类 | 硬件设备 | SDK 依赖 | 通信方式 | 说明 |
|--------|----------|----------|----------|------|
| `HkCream_ch120` | 海康工业相机 | MVS SDK (HIKVISION) | USB3/GigE | 支持曝光/增益调节、软触发/硬件触发、实时预览窗口 |

### 1.6 光源控制器

光源设备继承自 `FHWObject` + `r232Connection`，通过 RS232 串口通信。

| 实现类 | 硬件设备 | 通信方式 | 说明 |
|--------|----------|----------|------|
| `CST_LightSource` | 多光谱多通道光源 | RS232 | 4 分区，每区支持 8 种色彩（White/Blue/Green/Yellow/Red730/Red850/UV/Red），独立亮度调节 |
| `CST_TZls` | 同轴光源控制器 | RS232 | 8 通道（A-H），0-255 级亮度，支持常亮/常灭/脉冲频闪模式 |

### 1.7 其他设备

| 实现类 | 硬件设备 | 通信方式 | 说明 |
|--------|----------|----------|------|
| `AdjustFocal` | 电动调焦镜头 | RS232 | 支持焦距范围查询、绝对值设定 |
| `DialGauge` | 数显千分表 | RS232 | 数值读取 |

---

## 2. MotionUnit 抽象接口说明

`MotionUnit` 是所有运动控制器驱动的抽象基类，定义于 `public.h`。

### 类定义概要

```cpp
class CLASSEXPORT MotionUnit
{
protected:
    unsigned Serno;      // 序列号
    int errCode;         // 错误码
    void* EsData;        // 扩展数据
    unsigned AxNum;      // 轴数量
    unsigned DiNum;      // 数字输入数量
    unsigned DoNum;      // 数字输出数量

public:
    typedef enum { DIO = 1, AIO = 2, MOTION, MOTIONCRD, UNKNOW } muType;
    muType UnitType;     // 单元类型标识

    // --- 生命周期 ---
    virtual short Open(char* CfgPath);   // 打开设备，传入配置文件路径
    virtual void Close();                 // 关闭设备

    // --- 轴运动控制 ---
    virtual void JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi);   // 点动运动
    virtual void PpMoveAbs(unsigned AxisNo, AxisInfo& Axi);           // 绝对点位运动
    virtual void PpMoveRel(unsigned AxisNo, AxisInfo& Axi);           // 相对点位运动
    virtual void GroupMove(unsigned AxisNo, AxisInfo& Axi);           // 群组运动
    virtual void AxHome(unsigned AxisNo, AxisInfo& Axi);              // 回原点
    virtual void AxStop(unsigned AxisNo, bool isSmooth);              // 轴停止
    virtual bool Servon(unsigned AxisNo, bool OnOff);                 // 伺服使能

    // --- 状态与位置 ---
    virtual void GetAxStatus(unsigned AxisNo, AxisInfo& Axi);         // 获取轴状态（IO/到位/限位/报警）
    virtual void GetAxVelPos(unsigned AxisNo, AxisInfo& Axi);         // 获取轴当前位置和速度
    virtual void ClearPos(unsigned AxisNo);                            // 清除位置
    virtual bool ClearStatusError(u8 SOE);                             // 清除状态/错误

    // --- IO 控制 ---
    virtual bool SetOutput(unsigned oSernum, bool Value);              // 设置数字输出
    virtual int GetInput(unsigned index);                               // 读取数字输入
    virtual int GetOutput(unsigned index);                              // 读取数字输出

    // --- 高级功能 ---
    virtual void SetSoftLimit(unsigned AxisNo, long Ppos, long Npos);  // 设置软限位
    virtual void PosCompareSet(...);                                     // 位置比较输出
    virtual void GroupParamSet(unsigned mAxisNo, AxisInfo& Axi);       // 群组参数设置
    virtual void GroupMoveClose(unsigned mAxisNo, AxisInfo& Axi);      // 关闭群组运动
};
```

### 运动模式 (`MotionModel`)

| 模式 | 说明 |
|------|------|
| `PP` | 点位运动（Point-to-Point） |
| `JOG` | 点动运动（按住持续运动） |
| `GROUPSALVER` | 群组从轴 |
| `GROUP_PT_MASTER` | PT 群组主轴 |
| `GROUP_GEAR_MASTER` | 齿轮群组主轴 |
| `GROUP_FOLLOW_MASTER` | 跟随群组主轴 |
| `GROUP_PVT_MASTER` | PVT 群组主轴 |
| `GROUP_LINE_INTERPOLATION_MASTER` | 直线插补主轴 |
| `GROUP_ARC_INTERPOLATION_MASTER` | 圆弧插补主轴 |

### 使用方式（通过 MotionGpioSerialize）

实际使用中，`MotionUnit` 不直接操作，而是通过 `MotionGpioSerialize` 进行序列化访问：

```cpp
// 由 rxsMainFrame 自动创建和管理
MotionGpioSerialize* motionGpio = pSystem->MotionGpio;

// 按轴名运动
motionGpio->AxMove("X轴", 'A', 100.0f, 50.0f);  // 绝对移动
motionGpio->AxMove("Y轴", '+', 50.0f, 0);         // Jog 正方向

// IO 操作
motionGpio->SetDoutput("气缸1", true);
u8 val = motionGpio->GetDinput("传感器1");
```

---

## 3. ImageableSensorUnit 抽象接口说明

`ImageableSensorUnit` 是所有成像传感器驱动的抽象基类，定义于 `public.h`。

### 传感器类型枚举

```cpp
typedef enum {
    Cream = 0,      // 2D 相机
    PointLsr = 1,   // 点激光/点光谱
    LineLsr = 2,    // 线激光
    Confocal = 3,   // 共焦
    LineConfocal = 4 // 线共焦
} DevType;
```

### 连接方式

```cpp
typedef union connMeathod {
    EarthConn earth;   // 以太网 (IP + Port)
    UsbConn   usb;     // USB
    Rs232Conn rs232;   // 串口 (端口名 + 波特率 + 停止位)
} ConnMeathod;
```

### 采集模式

- **GatherMethod**: `Signal`(单帧) / `Continuous`(连续)
- **TiggerMethod**: `Encoder`(编码器) / `External`(外部输入) / `Software`(软触发)

### 核心虚方法

| 方法 | 说明 |
|------|------|
| `open(const char* tsPath)` | 打开设备连接 |
| `close()` | 关闭设备 |
| `stop()` | 停止采集 |
| `grabContinuity(bool isAnsy, int ccNums)` | 连续触发采集（异步/同步） |
| `DataGet(unsigned& outcount)` | 获取采集数据（通用） |
| `DataGetPointCould(unsigned& outcount)` | 获取点云数据 |
| `TiggerTaber(short wich, bool isClose)` | 触发模式切换 |
| `CoordinateConvert()` | 坐标转换 |
| `WorldCoordinateSet(u8 wich, float x, float y, float z)` | 世界坐标设定 |
| `IsCollectionCompleted(unsigned int gCount)` | 采集完成判断 |
| `SpecParamSet(const char* pkey, float/int v)` | 特定参数设置（如曝光时间、增益） |

---

## 4. 如何添加新的硬件驱动

### 4.1 添加新的运动控制器

**步骤 1**: 在 `rxsFixdev/rxsHrdwdal/` 目录下创建新文件，例如 `MyMotion_Dal.cpp` / `MyMotion_Dal.h`。

**步骤 2**: 继承 `MotionUnit` 并实现所有虚方法：

```cpp
// MyMotion_Dal.h
#pragma once
#include "rxsHrdwdal.h"

class CLASSEXPORT MyMotion_Dal : public MotionUnit {
public:
    MyMotion_Dal();

    short Open(char* CfgPath) override;
    void Close() override;

    void JogMove(unsigned AxisNo, byte dir, AxisInfo& Axi) override;
    void PpMoveAbs(unsigned AxisNo, AxisInfo& Axi) override;
    void PpMoveRel(unsigned AxisNo, AxisInfo& Axi) override;
    void GroupMove(unsigned AxisNo, AxisInfo& Axi) override;
    bool Servon(unsigned AxisNo, bool OnOff) override;
    void AxStop(unsigned AxisNo, bool isSmooth) override;

    bool SetOutput(unsigned oSernum, bool Value) override;
    int GetInput(unsigned index) override;
    int GetOutput(unsigned index) override;

    void AxHome(unsigned AxisNo, AxisInfo& Axi) override;
    void GetAxStatus(unsigned AxisNo, AxisInfo& Axi) override;
    void GetAxVelPos(unsigned AxisNo, AxisInfo& Axi) override;
    // ... 其余虚方法
};
```

**步骤 3**: 在 `rxsHrdwdal/CMakeLists.txt` 中确保新文件被包含（如果使用 `file(GLOB)` 则自动包含）。

**步骤 4**: 在系统配置文件（`FactoryConfigManger` 加载的配置文件）中注册新的运动控制器，使 `rxsMainFrame::Ready()` 在初始化时实例化对应驱动。

### 4.2 添加新的传感器驱动

**步骤 1**: 在 `rxsFixdev/rxsHrdwdal/` 目录下创建新文件。

**步骤 2**: 继承 `ImageableSensorUnit` 并实现核心虚方法：

```cpp
// MySensor_Dal.h
#pragma once
#include "rxsHrdwdal.h"

class CLASSEXPORT MySensor_Dal : public ImageableSensorUnit {
protected:
    short open(const char* tsPath = 0) override;    // 设备连接
    void close() override;                            // 设备关闭
    void stop() override;                             // 停止采集
    bool grabContinuity(bool isAnsy, int gCount) override;  // 采集

public:
    MySensor_Dal();
    ~MySensor_Dal();

    bool ResetConnect() override;                     // 重连
    void GetSensorparam() override;                   // 读取参数
    void SetSensorparam() override;                   // 设置参数
    int IsCollectionCompleted(unsigned int gCount) override;
    void* DataGet(unsigned& outcount) override;        // 获取数据
    rxsPointCouldp* DataGetPointCould(unsigned& num) override;  // 获取点云（如适用）
    void CoordinateConvert() override;                // 坐标转换
    bool WorldCoordinateSet(u8 wich, float x, float y, float z) override;
};
```

**步骤 3**: 实现关键点：
- `open()`: 初始化 SDK、建立通信连接、读取配置
- `grabContinuity()`: 实现单次/连续采集逻辑，将原始数据存入内部缓冲区
- `DataGet()` / `DataGetPointCould()`: 将缓冲区数据转换为 `rxsPointCouldp` 格式输出
- `CoordinateConvert()`: 实现传感器坐标到世界坐标的转换

**步骤 4**: 将传感器 SDK 的头文件和库文件放入 `rxsFixdev/` 对应子目录，并在 `CMakeLists.txt` 的 `RXS_FIXDEV_INCLUDE_DIRS` 中添加 include 路径。

**步骤 5**: 在系统配置中注册传感器别名，使 `ImageableGather::Selected(name)` 能够找到并实例化对应驱动。

### 4.3 添加新的光源/串行设备

继承 `FHWObject` 和 `r232Connection`：

```cpp
class CLASSEXPORT MyLightSource : public FHWObject, r232Connection {
protected:
    short Link(const char* cmdline) override;   // 建立串口连接
    short unLink() override;                      // 断开连接
    void* Reader(unsigned& len) override;         // 读取响应
    int Writer(void*, unsigned len) override;     // 发送指令
public:
    MyLightSource();
    // 自定义控制方法...
};
```

### 4.4 通用注意事项

1. **DLL 导出**: 所有新增驱动类需使用 `CLASSEXPORT`（即 `__declspec(dllexport)`）宏标记，确保可从 DLL 外部访问
2. **配置文件驱动**: 硬件实例化由 `FactoryConfigManger` 加载的配置文件控制，新驱动需在配置文件中注册
3. **线程安全**: 传感器采集可能涉及异步操作，注意使用 `ThreadSafeBuffer` 等机制保证数据安全
4. **坐标系统一**: 所有传感器的 `CoordinateConvert()` 和 `WorldCoordinateSet()` 需遵循系统统一的坐标系约定
5. **SDK 依赖**: 新增硬件的 SDK DLL 需随部署包一同分发，参见 `deployment.md`
