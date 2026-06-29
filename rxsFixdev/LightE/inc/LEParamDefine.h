#pragma once
/*
DLL版本：V1.80.1.X
更新时间: 2023-06-02
*/
//base command
#define GET_BASE_ADDR 0x00000000
#define SET_BASE_ADDR 0x80000000
#define CMD_BASE_ADDR 0x10000000

//Device parameter base
#define DEVICE_FACTORY_BASE_ADDR 0x01000000
#define DEVICE_NORMAL_BASE_ADDR  0x01000100
#define DEVICE_ENCODER_BASE_ADDR 0x01000300
//channel parameter base
#define CHANNEL_PARM_BASE_ADDR  0x02000000
#define CHANNEL_CALI_BASE_ADDR 0x02000200
//commixture base
#define MEASURE_BASE_ADDR 0x03000000
#define USER_BASE_ADDR    0x04000000

//get and set base
#define G_DEVICE_FACTORY(x) GET_BASE_ADDR + DEVICE_FACTORY_BASE_ADDR + x
#define S_DEVICE_FACTORY(x) SET_BASE_ADDR + DEVICE_FACTORY_BASE_ADDR + x

#define G_DEVICE_NORMAL(x) GET_BASE_ADDR + DEVICE_NORMAL_BASE_ADDR + x
#define S_DEVICE_NORMAL(x) SET_BASE_ADDR + DEVICE_NORMAL_BASE_ADDR + x

#define G_DEVICE_ENCODER(x) GET_BASE_ADDR + DEVICE_ENCODER_BASE_ADDR + x
#define S_DEVICE_ENCODER(x) SET_BASE_ADDR + DEVICE_ENCODER_BASE_ADDR + x

#define G_CH_PARAM(x) GET_BASE_ADDR + CHANNEL_PARM_BASE_ADDR + x
#define S_CH_PARAM(x) SET_BASE_ADDR + CHANNEL_PARM_BASE_ADDR + x

#define G_CH_CALI(x) GET_BASE_ADDR + CHANNEL_CALI_BASE_ADDR + x
#define S_CH_CALI(x) SET_BASE_ADDR + CHANNEL_CALI_BASE_ADDR + x

//cmd base
#define C_MEASURE_COM(x) CMD_BASE_ADDR + MEASURE_BASE_ADDR + x
#define C_USER_COM(x) CMD_BASE_ADDR + USER_BASE_ADDR + x
/**********************************************************************
*                      Device normal parameter - 控制器常规参数		  *
**********************************************************************/
#define CM_UI_DCN_EIP_EN        	G_DEVICE_NORMAL(0x17)          //EIP通讯是否启用
#define CM_UI_DCN_MODULE_EN      G_DEVICE_NORMAL(0x18)			//ARM可选扩展模块及FPGA可选扩展模块功能选择
#define CM_US_DCN_MODBUS_ADDR_TABLE		    G_DEVICE_NORMAL(0x19)		  //modbus地址表
#define CM_C_DCN_DEVICE_IP                 G_DEVICE_NORMAL(0x1A)      //本机IP设置
#define CM_US_DCN_DHCP_EN                   G_DEVICE_NORMAL(0x1B)     //本机IP地址模式
#define CM_UI_THICK_CALIBRATION        	    G_DEVICE_NORMAL(0x1C)          //对射测厚度标定值，单位nm
#define CM_UI_DAC_OUT_MODE        	    G_DEVICE_NORMAL(0x1D)          //dac输出模式
#define CM_D_VOTAGE_SCALE        	    G_DEVICE_NORMAL(0x1E)          //高度模式电压比例因子
#define CM_D_DCN_ANALOG_THICK_RANGE         G_DEVICE_NORMAL(0x20)          //对射测厚模拟量电压输出对应的量程范围，单位mm
#define CM_UI_FILTER_CNT        	    G_DEVICE_NORMAL(0x1F)          //模拟量输出滑动平均窗口大小（目前仅支持高度模式）
/**********************************************************************
*                      Channel parameter - 通道参数         		  *
**********************************************************************/
#define CM_US_CHN_INTENSITY_THD				G_CH_PARAM(0x06)			//波峰亮度阈值，寻峰时波峰亮度低于此值的波峰会被过滤掉
#define CM_D_CHN_ANALOG_V_RANGE				G_CH_PARAM(0x0C)        //模拟量电压输出范围
#define CM_UI_CHN_NUM_SINGTHKNESS			G_CH_PARAM(0x16)				//单边测厚层数 可设置值范围为1~5
#define CM_D_CHN_DISTANCE_OFFSET		    G_CH_PARAM(0x28)		  // 重置高度使用量程范围起始点时的偏移量，单位μm
#define CM_I_CHN_DISTANCE_BASE		        G_CH_PARAM(0x29)		  // 对射测厚模式高度基准值，单位nm
/**********************************************************************
*                      command - 命令操作         		  *
**********************************************************************/
//GPIO输出状态读写,端口序号可输入范围：(端子9)第一组0x0001~0x0005；（端子4）第二组0xFF01~0xFF05。状态值：0-断开；1-闭合。
#define CM_CMD_US_OUTPUT_GPIO							C_MEASURE_COM(0x1A)	
//读取GPIO输入状态读取,端口序号可输入范围：（端子10）第一组0x0001~0x0005；（端子3）第二组0xFF01~0xFF05。状态值：0-断开；1-闭合。
#define CM_CMD_US_INPUT_GPIO							C_MEASURE_COM(0x1C)
#define CM_CMD_DISTANCE_ZERO                                C_MEASURE_COM(0x1E)     //重置高度测量结果的零参考点对应的高度值(目前只用E系列和G系列支持)
#define CM_CMD_THICKNESS_MEASUREMENT	                    C_MEASURE_COM(0x1F)     //对射测厚标定 










