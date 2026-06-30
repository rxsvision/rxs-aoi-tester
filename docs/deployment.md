# 部署与运行指南

## 1. 运行时 DLL 依赖清单

rxs-aoi-tester 部署时需要确保以下所有 DLL 文件存在于运行时搜索路径中。

### 1.1 自有 DLL（项目构建产物）

| DLL 文件 | 来源模块 | 说明 |
|----------|----------|------|
| `SystemFrame.lib` | SystemEntry (L1) | 静态库，已链接进下游 DLL，无需单独部署 |
| `rxsHrdwdal.dll` | rxsHrdwdal (L2) | 硬件驱动层 |
| `rxsImgProcess.dll` | rxsImgProcess (L3) | 图像处理 + StepBlock 引擎 |
| `rxsFixdal.dll` | rxsFixdal (L4) | 设备工作流编排 |
| `czxToolkit.dll` | 外部算法 DLL | rxsImgProcess 通过 LoadLibrary 动态加载（非链接依赖） |
| `CzxTookit2D.dll` | 外部算法 DLL | 2D 算法工具包（注意原始拼写错误需保持兼容） |

### 1.2 第三方核心依赖

| DLL 文件 | 来源 | 说明 |
|----------|------|------|
| `opencv_world460.dll` | OpenCV 4.6.0 | rxsImgProcess 图像处理核心 |
| Qt5 系列 DLL | Qt 5.15.2 | AoiTester 主程序 GUI 框架 |
| `Qt5Core.dll` | Qt | 核心库 |
| `Qt5Gui.dll` | Qt | GUI 库 |
| `Qt5Widgets.dll` | Qt | 控件库 |
| `Qt5OpenGL.dll` | Qt | OpenGL 支持 |
| `Qt5Network.dll` | Qt | 网络通信 |
| `Qt5Xml.dll` | Qt | XML 解析 |
| `Qt5SerialPort.dll` | Qt | 串口通信（如使用） |
| PCL 系列 DLL | PCL 1.10.1 | AoiTester 点云处理 |

### 1.3 传感器 SDK DLL

| DLL 文件 | 来源 | 说明 |
|----------|------|------|
| LightE SDK DLL | 立仪 | 点光谱传感器通信 |
| Keyence SDK DLL | 基恩士 | LJ-X8000A / 点光谱通信 |
| Precitec SDK DLL | 普雷斯特 | 传感器通信 |
| SSZN SR SDK DLL | 深视智能 `sszn/SR_SdkDll/` | SR8060 线激光通信 |
| SizectorAPI DLL | 盛相 `SizectorAPI/` | MPSizector 结构光通信 |
| MVS SDK DLL | 海康 (HIKVISION) | HkCream_ch120 工业相机 |

### 1.4 运动控制 SDK DLL

| DLL 文件 | 来源 | 说明 |
|----------|------|------|
| ZMotion SDK DLL | 正运动 `zmotion/x86/` | ZMotionDal 运动控制器 |
| Aubo SDK DLL | 遨博 `auboi5/` | AuboRobotDal 协作机器人 |

### 1.5 其他依赖

| DLL 文件 | 来源 | 说明 |
|----------|------|------|
| `libredwg.dll` | libredwg 0.13.3 | DWG 文件解析（DesignFilesImport 功能） |
| MSVC 运行时 | Visual Studio 2019 (v142) | `vcruntime140.dll`, `msvcp140.dll` 等 |

---

## 2. 部署目录结构建议

```
deploy/
|-- AoiTester.exe                      # 主程序
|
|-- rxsHrdwdal.dll                     # 自有 DLL (L2)
|-- rxsImgProcess.dll                  # 自有 DLL (L3)
|-- rxsFixdal.dll                      # 自有 DLL (L4)
|-- czxToolkit.dll                     # 外部算法 DLL
|-- CzxTookit2D.dll                    # 2D 算法 DLL
|
|-- opencv_world460.dll                # OpenCV
|
|-- Qt5Core.dll                        # Qt5 运行时
|-- Qt5Gui.dll
|-- Qt5Widgets.dll
|-- Qt5OpenGL.dll
|-- Qt5Network.dll
|-- Qt5Xml.dll
|-- platforms/                         # Qt 平台插件
|   |-- qwindows.dll
|
|-- config/                            # 配置文件目录
|   |-- sysconfig.ini                  # 系统配置（SysConfigManger 加载）
|   |-- factory.ini                    # 出厂配置（FactoryConfigManger 加载）
|   |-- workparam.ini                  # 加工参数（WorkParamManger 加载）
|
|-- flow/                              # 流程文件目录
|   |-- (imgTaskProcessCollection 加载的流程描述文件)
|
|-- sensor_sdk/                        # 传感器 SDK DLL
|   |-- (LightE / Keyence / Precitec / SSZN / Sizector / MVS 各 SDK DLL)
|
|-- motion_sdk/                        # 运动控制 SDK DLL
|   |-- (ZMotion / Aubo 各 SDK DLL)
|
|-- libredwg.dll                       # DWG 解析
|
|-- data/                              # 运行数据目录
|   |-- results/                       # 检测报告输出
|   |-- images/                        # 采集图片保存
|   |-- pointclouds/                   # 点云数据保存
|   |-- models/                        # 模型文件（变形度示教等）
|
|-- logs/                              # 日志目录
|   |-- (inkType::inkLog 生成的系统日志)
```

### 目录说明

- **主程序目录**: `AoiTester.exe` 和所有自有 DLL 应位于同一目录，确保 DLL 搜索路径正常
- **SDK DLL**: 传感器和运动控制的 SDK DLL 可放在主程序目录或子目录中，但需确保在系统 PATH 或 DLL 搜索路径中可达
- **配置文件**: 由 `SysConfigManger`、`FactoryConfigManger`、`WorkParamManger` 加载，路径通常在系统配置中指定
- **流程文件**: 由 `imgTaskProcessCollection::Load(dirPath)` 加载，指向包含流程描述文件的目录
- **数据输出**: 检测报告通过 `ResultReportGrpAssemble::Save()` 保存，点云通过 `pcdFileSave()` 保存

---

## 3. 构建与部署步骤

### 3.1 构建

```bash
# 配置（指定 Qt 路径）
cmake -B build -S . ^
    -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64" ^
    -DRXS_BUILD_AOI_TESTER=ON ^
    -DRXS_BUILD_FIXDEV=ON ^
    -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build build --config Release --parallel
```

构建产物位置：
- `build/bin/` -- DLL 和可执行文件
- `build/lib/` -- 静态库和导入库

### 3.2 部署清单生成

构建完成后，按以下步骤收集部署文件：

1. **复制构建产物**: 将 `build/bin/` 下的所有 `.dll` 和 `.exe` 复制到部署目录
2. **收集 Qt 运行时**: 使用 `windeployqt` 工具自动收集：
   ```bash
   windeployqt --release deploy/AoiTester.exe
   ```
3. **收集 OpenCV**: 复制 `opencv_world460.dll` 到部署目录
4. **收集传感器 SDK**: 根据实际使用的传感器，从各 SDK 安装目录复制对应 DLL
5. **收集运动控制 SDK**: 根据实际使用的运动控制器，复制对应 DLL
6. **复制配置文件**: 将开发环境中的配置文件复制到 `config/` 目录
7. **MSVC 运行时**: 确保目标机器已安装 VS2019 运行时，或将 `vcredist_x64.exe` 随包分发

---

## 4. 常见部署问题排查

### 4.1 启动失败：找不到 DLL

**症状**: 启动 AoiTester.exe 时提示"找不到 xxx.dll"或错误码 0xc000007b。

**排查步骤**:
1. 使用 [Dependencies](https://github.com/lucasg/Dependencies) 工具检查 AoiTester.exe 的 DLL 依赖树
2. 确认所有自有 DLL (rxsHrdwdal / rxsImgProcess / rxsFixdal) 与主程序在同一目录
3. 确认 MSVC 运行时版本匹配（v142 = VS2019）
4. 确认 32/64 位一致（本项目全部为 64 位）

### 4.2 czxToolkit.dll 加载失败

**症状**: 图像处理流程执行时部分算法功能不可用。

**原因**: `rxsImgProcess/protected_rxsToolKit.cpp` 通过 `LoadLibrary` + `GetProcAddress` 动态加载 `czxToolkit.dll`，该 DLL 不通过链接器依赖，需手动确保存在。

**解决**: 将 `czxToolkit.dll`（和 `CzxTookit2D.dll`）放入主程序目录或系统 PATH 可达的位置。

### 4.3 传感器连接失败

**症状**: `ImageableSensorUnit::Open()` 返回非零错误码。

**排查**:
- **以太网传感器**: 检查 IP 地址配置、网络连通性、防火墙规则
- **USB 传感器**（如海康相机）: 确认 MVS 驱动已安装，USB3.0 端口可用
- **串口设备**（光源/调焦）: 确认 COM 端口号正确、波特率匹配、端口未被其他程序占用
- **SDK 版本**: 确认部署的 SDK DLL 版本与编译时使用的头文件版本匹配

### 4.4 运动控制器连接失败

**症状**: `MotionUnit::Open()` 失败，`errCode` 非零。

**排查**:
- **ZMotion**: 确认控制器 IP 可达，ZMotion SDK DLL 已部署
- **Aubo**: 确认机器人控制器 IP 和端口配置正确，auboi5 SDK DLL 已部署
- **通用**: 检查配置文件中轴数量、轴名称映射是否正确

### 4.5 加工流程执行异常

**症状**: 加工开始后立即停止或流程步骤执行错误。

**排查**:
1. 检查 `CBC_VISUAL_ERR` (200) 回调消息获取具体错误信息
2. 确认流程文件（由 flowchart2d 生成）完整且与当前 rxsImgProcess 版本兼容
3. 检查运动路径（Movepath）和扫描点表（Scanpoint）参数是否合理
4. 确认传感器已正确打开（`SensorOpen()` 返回成功）
5. 检查系统状态机是否处于 `RDY` 状态（非 `ALAMING` 或 `RESETING`）

### 4.6 报警状态无法清除

**症状**: 系统处于 `ALAMING` 状态，无法开始加工。

**排查**:
1. 检查急停按钮是否释放（EMG 报警）
2. 检查伺服驱动器是否有报警（SERV/DRIVER 报警）
3. 检查安全光栅/安全门状态（SECURITY 报警）
4. 调用 `SysReset()` 执行系统复位
5. 检查 DI 输入信号映射是否正确（配置文件中的 IO 映射表）

### 4.7 报表输出异常

**症状**: 检测结果未正确保存或报表字段缺失。

**排查**:
1. 确认 `WorkParamManger` 中已配置测量项标准值（`MeasurementStandSet`）
2. 确认报表模板已正确创建（`ResultReportCreater`）
3. 检查 `CheckResultReportSave(true)` 是否已启用自动保存
4. 确认输出目录可写且磁盘空间充足
