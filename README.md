# 宿舍氛围灯工程

## 0. 关于本项目
主控：STC8G1K08A    
芯片选型思路：该项目主要需要一个按键用作功能切换 一个电位器读取电压值控制亮度和速度 一个IO口用做控制WS2812  所以八个引脚足以，
再考虑到成本方面    STC8G系列是最好的选择   4K + 1K 的存储刚好达到续需求 再加上STC8G1K08A有ADC功能  SOP8的封装批发价0.75元  十分适合该项目

下载电路为STC官方提供的原理图   但需要自行安装驱动

灯带为WS2812    淘宝链接        https://item.taobao.com/item.htm?spm=a1z09.2.0.0.fc352e8d9EufZA&id=599554346846&_u=b33h9g5d29cb         7.6块钱一根

## 1. 硬件打样说明

硬件部分下有打板文件可以直接在嘉立创下单    （每人每月两次5元打样，手动滑稽）

## 2. 固件编译说明
自行安装Keil软件配置环境（想自己做的话这部分必须要有）

软件结构包括  
    
模式操作  

    常亮模式 
    纯色模式  
    单色万花筒  
    流动万花筒  
    梦幻模式  
    随机颜色流水灯模式  
    流星拖尾灯模式  
    随机颜色呼吸灯模式  
    随机颜色模式  
    检测模式是否切换  
系统操作  

    读取电位器电压  
    系统初始化  
WS2812操作  

    写1码  
    写0码  
    写重置码  
    写一个字节  
    写24个比特  
    显示小灯数据  
    清空小灯数据  
延时操作  

    延时1ms  
    延迟单位毫秒  
Flash操作  

    关闭IAP功能  
    EEPROM读一字节子程序  
    EEPROM字节编程子程序  
    EEPROM擦除扇区子程序  
    


SW2818驱动教程网上大把  这里不再赘述
如果想自行开发
直接对小灯数据进行处理在调用显示函数即可

## 3. 如果觉得不错的话记得加个星哦