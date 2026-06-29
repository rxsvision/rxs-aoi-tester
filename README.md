# RXS AOI Tester

> AOI 测试平台 — 基准来源 E:\2_6\Real3DMeasure

## 来源

- **基准**: `E:\2_6\06_QT上位机源码库\Real3Dmetrologyplatform\Real3DMeasure\`
- 含 rxsFixdev 子模块 (rxsImgProcess + rxsFixdal)
- flowchart2d (111 文件: 29 cpp + 33 h + 22 ui + 27 其他)

## 架构

```
rxs-aoi-tester/
├── Real3DMeasure/           # 主程序
│   ├── rxsFixdev/           # 设备模块
│   │   ├── rxsImgProcess/   # 图像处理 (DLL 消费者, GetProcAddress)
│   │   └── rxsFixdal/       # 设备抽象层
│   ├── flowchart2d/         # 2D 流程图 (111 文件)
│   └── ...
└── README.md
```

## 关键说明

- rxsImgProcess/protected_rxsToolKit.cpp 是 **DLL 消费者封装层** (LoadLibrary + GetProcAddress)
- **非** czxTool 副本 — 14 个方法全部通过 GetProcAddress 调用 czxToolkit.dll
- CzxTookit2D.dll (拼写错误) 引用需保持向后兼容

## 许可证

BSL 1.1 (Change Date: 2030-01-01, Change License: GPLv2)

## 公司

苏州锐新视科技有限公司
