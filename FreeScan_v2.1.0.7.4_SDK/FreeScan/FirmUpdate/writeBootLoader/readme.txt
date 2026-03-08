1. 先运行express switch\testHID.exe,擦除App,Fx3进入厂家Bootloader模式。
2. 运行USB Control Center烧写我们的二级Bootloader文件(Fx3BootAppGcc.img).
3. 打开Windows终端窗口输入命令：烧写应用镜像文件(newprousb.img)
   .\Fx3Bootloader.exe -i newprousb.img -t I2C
等待烧写完成，最后会提示
Info : Found FX3 Flash Programmer
Info : FX3 firmware programming to I2C started. Please wait...
Info : Programming to I2C completed
说明烧写成功。



