@echo off

:: AM2的IP地址
set AM2IP=10.80.128.220
:: 保存日志的位置
set downLoadPath=C:/Users/Administrator/Desktop/

set syncPath=/home/zz/scanService/install/syncservice/bin/log/*.txt
set servicePath=/home/zz/scanService/install/res/log/*.log

scp -r zz@%AM2IP%:%syncPath% %downLoadPath%
scp -r zz@%AM2IP%:%servicePath% %downLoadPath%