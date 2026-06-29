# RXS AOI Tester

> AOI 测试平台 — 基于 CloudCompare + Qt5 的 3D 测量与设备 DAL 框架

## 来源

- **基准**: `E:\2_6\06_QT上位机源码库\Real3Dmetrologyplatform\Real3DMeasure\`
- 含 rxsFixdev 子模块 (SystemEntry + rxsHrdwdal + rxsImgProcess + rxsFixdal + Tester)
- 含 AoiTester (CloudCompare 衍生的 3D 点云应用)
- flowchart2d (111 文件: 29 cpp + 33 h + 22 ui + 27 其他)

## 构建系统 (CMake)

### 依赖

| 依赖 | 版本 | 说明 |
|------|------|------|
| CMake | >= 3.16 | |
| MSVC | v142 (VS2019) | /utf-8 /MP /permissive- |
| Qt5 | 5.15.2 (msvc2019_64) | AoiTester 主程序 |
| OpenCV | 4.6.0 | rxsImgProcess 图像处理 |
| PCL | 1.10.1 | AoiTester 点云处理 |
| CloudCompare | 内置 | AoiTester/libs/ 下完整源码 |

### 构建方法

```bash
# 配置 (可选择性构建子系统)
cmake -B build -S . \
    -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64" \
    -DRXS_BUILD_AOI_TESTER=ON \
    -DRXS_BUILD_FIXDEV=ON \
    -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build build --config Release --parallel

# 输出
#   build/bin/             — DLL 和可执行文件
#   build/lib/             — 静态库和导入库
```

### 构建选项

| 选项 | 默认 | 说明 |
|------|------|------|
| `RXS_BUILD_AOI_TESTER` | ON | 构建 CloudCompare-based AoiTester |
| `RXS_BUILD_FIXDEV` | ON | 构建 rxsFixdev DAL 框架 |
| `RXS_BUILD_FLOWCHART2D` | OFF | 构建 flowchart2d 引擎 |

### 项目结构

```
rxs-aoi-tester/
├── CMakeLists.txt                    # 顶层 CMake (子系统选择)
├── AoiTester/                        # CloudCompare 3D 点云应用
│   ├── CMakeLists.txt                # (已有, CloudCompare 原始 CMake)
│   ├── libs/                         # qCC_db, qCC_io, qCC_glWindow 等
│   ├── plugins/                      # IO, Standard, Example 插件
│   └── Aoi_ui/                       # 主程序 UI
├── rxsFixdev/                        # 设备 DAL 框架
│   ├── CMakeLists.txt                # 顶层 (依赖链定义)
│   ├── SystemEntry/                  # 1. SystemFrame (静态库)
│   ├── rxsHrdwdal/                   # 2. 硬件 DAL (DLL)
│   ├── rxsImgProcess/                # 3. 图像处理 (DLL)
│   ├── rxsFixdal/                    # 4. 设备抽象层 (DLL)
│   ├── Tester/                       # 5. 测试程序 (EXE)
│   ├── Precitec/                     # Precitec 传感器 SDK
│   ├── KeyenceLib/                   # Keyence 激光 SDK
│   ├── LightE/                       # LightE 运动控制 SDK
│   ├── auboi5/                       # Aubo 机器人 SDK
│   ├── sszn/                         # SSZN 传感器 SDK
│   ├── zmotion/                      # 正运动控制器 SDK
│   └── 3trd/                         # 第三方库 (libredwg)
└── flowchart2d/                      # 2D 流程图引擎
    └── CMakeLists.txt                # (已有)
```

### rxsFixdev 依赖链

```
SystemFrame (STATIC)
    ↑
rxsHrdwdal (SHARED DLL)
    ↑
rxsImgProcess (SHARED DLL)
    ↑
rxsFixdal (SHARED DLL)
    ↑
Tester_* (EXE × 4)
```

| 项目 | 类型 | 输出 | 依赖 |
|------|------|------|------|
| SystemFrame | STATIC | SystemFrame.lib | — |
| rxsHrdwdal | SHARED | rxsHrdwdal.dll + .lib | SystemFrame |
| rxsImgProcess | SHARED | rxsImgProcess.dll + .lib | SystemFrame, rxsHrdwdal |
| rxsFixdal | SHARED | rxsFixdal.dll + .lib | SystemFrame, rxsHrdwdal, rxsImgProcess |
| Tester_FixDalTest | EXE | Tester_FixDalTest.exe | 全部 |
| Tester_HrdwDalTest | EXE | Tester_HrdwDalTest.exe | 全部 |
| Tester_ImgProcTest | EXE | Tester_ImgProcTest.exe | 全部 |
| Tester_mainTester | EXE | Tester_mainTester.exe | 全部 |

## 关键说明

- rxsImgProcess/protected_rxsToolKit.cpp 是 **DLL 消费者封装层** (LoadLibrary + GetProcAddress)
- **非** czxTool 副本 — 14 个方法全部通过 GetProcAddress 调用 czxToolkit.dll
- CzxTookit2D.dll (拼写错误) 引用需保持向后兼容
- rxsImgProcess 和 rxsFixdal 原始 vcxproj 标记为 Application 但实际无 main()，
  CMake 中正确映射为 SHARED library
- 每个测试文件各有独立的 main()，CMake 中创建为独立可执行文件

## 许可证

BSL 1.1 (Change Date: 2030-01-01, Change License: GPLv2)

## 公司

苏州锐新视科技有限公司
