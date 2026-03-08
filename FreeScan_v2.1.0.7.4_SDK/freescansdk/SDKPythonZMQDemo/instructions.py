##\file
##这是相关DEMO
from scan_common import *

##\brief
#**选择扫描模式** \n
#**chooseScanMode**
##@details
##<pre><b>ZMQ</b>:envelope  v2.0/invoke/scanservice/other
##payload	{"cmd":"setScanMode2","ScanMode": ScanMode}</pre>
##\param
#ScanMode 扫描模式, 0 扫人, 1 扫物, 2 扫小物. \n
#Scan mode, 0 portrait, 1 object, 2 smallobject. Reference: ScanMode
def chooseScanMode():
	if(deviceType=="combo"):
		resultFromService=sendMessageToService(
			"{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
			,'{{\"cmd\":\"setScanMode2\",\"ScanMode\":{0}}}'.format(str(scanType))
		)
		logging.info("set scanType result: "+resultFromService)

##@brief
##**新建工程** \n
##**New project**
##@details
##<pre><b>ZMQ</b>:envelope  v2.0/invoke/scanservice/other
##payload	{"cmd":"new",
##		 "fileDir":slnDirPath,
##		 "mode":0
##		}</pre>
##@param slnDirPath 解决方案路径	Solution Path
##@param mode 扫描模式, 0 portrait, 1 object, 2 smallobject. Reference: ScanMode
##Scan mode, 0 portrait, 1 object, 2 smallobject. Reference: ScanMode
def newProject():
    paramsList={"cmd":"new","fileDir":slnDirPath,"mode":0}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("newProject result: "+resultFromService)

##\brief
##**打开工程** \n
##**Open project**
##\details
##<pre><b>ZMQ</b>:envelope  v2.0/invoke/scanservice/other
##payload	{"cmd":"openProject",
##		 "fileDir":openProjectDir,
##		 "fileName":openProjectFile,
##		 "ScanType":"LASER"
##		 }</pre>
##\param fileDir openProjectDir-打开工程的所在文件夹路径(例如D:/desktop/Data)
##\param fileName openProjectFile-打开工程的文件名
##\param ScanType 工程类型
def openProject():
    paramsList={"cmd":"openProject",
                "fileDir":openProjectDir,
                "fileName":openProjectFile,
                "ScanType":"LASER"
                }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("openProject result: "+resultFromService)

##\brief
##**选择扫描类型** \n
##**chooseScanType**
##@details
##<pre><b>ZMQ</b>:envelope  v2.0/invoke/scanservice/other
##payload	{"cmd":"setProjectType2",
##		 "type":scanType
##		}</pre>
##\param type 0:scan framework 1:scan mesh \n 0:扫框架点 1:网格扫描
#0:scan framework 1:scan mesh
def chooseScanType(scanType):
    paramsList={"cmd":"setProjectType2",
                "type":scanType
                }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("chooseScanType result: "+resultFromService)




##\brief
##**导入全局标志点文件** \n
##**import Global Markers File**
##@details
##<pre><b>ZMQ</b>:envelope  v2.0/invoke/scanservice/other
##payload	{"FileName":globalMarkersFile,
##		 "cmd": "loadFramework"
##		 }</pre>
##\param FileName 全局标志点文件路径 \n Global Markers File
def importGlobalMarkersFile():
    paramsList={"FileName":globalMarkersFile,
                "cmd": "loadFramework"
               }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("importGlobalMarkersFile result: "+resultFromService)



##\brief
##**设置是否增加标志点模式** \n
##**change Add Increase Markers Mode**
##@details
##<pre><b>ZMQ</b>:envelope  v2.0/invoke/scanservice/other
##payload	{"cmd":"setIsIncFramework2",
##		 "increase":isIncrease
##		}</pre>
##\param isIncrease 是否增加 \n isIncrease
def changeAddIncreaseMarkersMode(isIncrease):
    paramsList={"cmd":"setIsIncFramework2",
                "increase":isIncrease
               }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("changeAddIncreaseMarkersMode result: "+resultFromService)


##\brief
##**开始扫描** \n
##**startScan**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"scan",
##		 "method":"scanCtrl",
##		 "target":"Sn3DLaserScanController"
##		}</pre>
def startScan():
    paramsList={"cmd":"scan",
                "method":"scanCtrl",
                "target":"Sn3DLaserScanController"
                }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("start scan result: "+resultFromService)


##\brief
##**停止扫描** \n
##**pause Scan**
##@details
##<pre><b>ZMQ</b>: envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"pause",
##		 "method":"scanCtrl",
##		 "target":"Sn3DLaserScanController"
##		 }</pre>
def pauseScan():
    paramsList={"cmd":"pause",
                "method":"scanCtrl",
                "target":"Sn3DLaserScanController"
                }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("pause scan result: "+resultFromService)
##\brief
##**清除扫描** \n
##**pause Scan**
##@details
##<pre><b>ZMQ</b>:envelope  v2.0/invoke/scanservice/other
##payload	{"cmd":"delete",
##		 "method":"scanCtrl",
##		 "target":"Sn3DLaserScanController"
##		}</pre>
def clearScan():
    paramsList={"cmd":"delete",
                "method":"scanCtrl",
                "target":"Sn3DLaserScanController"
                }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("clear scan result: "+resultFromService)

##\brief
##**保存扫描** \n
##**save Scan**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"saveScanDataToSrvCmd",
##		 "prjNameList":["Project1"],
##		 "saveFolder":saveScanFileName,
##		 "saveAscFile":saveAscFile,
##		 "saveP3File":saveP3File,
##		 "saveStlFile":saveStlFile,
##		 "saveObjFile":saveObjFile,
##		 "savePlyFile":savePlyFile,
##		 "save3MfFile":save3MfFile
##		 }</pre>
##\param prjNameList 项目列表
##\param saveXXXFile 保存XXX类型文件 \n save XXX type Scan File
def saveScan(saveAscFile,saveP3File,saveStlFile,saveObjFile,savePlyFile,save3MfFile):
    paramsList={"cmd":"saveScanDataToSrvCmd",
                "prjNameList":["Project1"],
                "saveFolder":saveScanFileName,
                "saveAscFile":saveAscFile,
                "saveP3File":saveP3File,
                "saveStlFile":saveStlFile,
                "saveObjFile":saveObjFile,
                "savePlyFile":savePlyFile,
                "save3MfFile":save3MfFile
                }

    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("save scan result: "+resultFromService)

##\brief
##**进入后处理** \n
##**enter Post Process**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"params":{"value":needOptimize}}</pre>
##\param needOptimize 是否需要优化 \n need Optimize
def enterPostProcess(needOptimize):
    paramsList={"params":{"value":needOptimize}}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,"meshPostProcess")
        ,paramsJsonStr
    )
    logging.info("enterPostProcess result: "+resultFromService)

##@brief
##**网格处理** \n
##**freescan mesh**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other 
##payload {"cmd":"apply", 
##		 "method":"apply",
##		 "fileter_level":0, 
##		 "remove_small_floating_parts":0, 
##		 "max_trianlges_ischeck":True, 
##		 "max_trianlges_value":20000000,
##		 "fill_small_hole_ischeck":True, 
##		 "fill_small_hole_value":10, 
##		 "remove_spike_ischeck":True, 
##		 "marker_hole_filling_ischeck":True, 
##		 "isRememberChoice":False 
##		}</pre>
##@param fileter_level 封闭选项
##**0-非封闭模型 1-半封闭模型 2-封闭模型**
##@param remove_small_floating_parts  去孤立面
##@param max_trianlges_ischeck 是否选择最大面片数
##@param max_trianlges_value 最大面片数值
##@param fill_small_hole_ischeck 是否补小洞
##@param fill_small_hole_value  补小洞周长
##@param remove_spike_ischeckrue 是否去尖刺
##@param marker_hole_filling_ischeck 标志点补洞
##@param isRememberChoice 是否使用推荐参数
##             }
def applyPostProcess():
    paramsList={"cmd":"apply",
                "method":"apply",
                "fileter_level":0,
                "remove_small_floating_parts":0,
                "max_trianlges_ischeck":True,
                "max_trianlges_value":20000000,
                "fill_small_hole_ischeck":True,
                "fill_small_hole_value":10,
                "remove_spike_ischeck":True,
                "marker_hole_filling_ischeck":True,
                "isRememberChoice":False
                }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("applyPostProcess result: "+resultFromService)
##\brief
##**回滚后处理** \n
##**undo Post Process**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"undoMesh"}</pre>
def undoPostProcess():
    paramsList={"cmd":"undoMesh"}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("undoPostProcess result: "+resultFromService)
##\brief
##**确认后处理** \n
##**confirm Post Process**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"confirmMesh"}</pre>
def confirmProstProcess():
    paramsList={"cmd":"confirmMesh"}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("confirmPostProcess result: "+resultFromService)    
## @brief
## **保存网格处理结果** \n
## **freescan save mesh**
## @details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other 
##payload	{"fileName":fileName ,
##		 "folderPath":folderPath ,
##		 "format":"001000000" ,
##		 "method":"save" ,
##		 "postpageVisible":1 ,
##		 "saveAscFile":0 ,
##		 "saveStlFile":1 ,
##		 "saveObjFile":0 ,
##		 "savePlyFile":0 ,
##		 "saveP3File":0,
##		 "save3MfFile":0 ,
##		 "saveTxtFile":0 ,
##		 "saveCsvFile":0 
##		}</pre>
## @param format 前置补0，后面7位与saveXXXFile一致
## @param postpageVisible 默认置1
## @param saveXXXFile 是否保存XXX格式的文件
## 
def savePostProcess():
    last_slash_index = saveScanFileName.rfind("/")
    fileName = saveScanFileName[last_slash_index + 1:]
    folderPath = saveScanFileName[:last_slash_index + 1]
    paramsList={"fileName":fileName
                ,"folderPath":folderPath
                ,"format":"001000000"
                ,"method":"save"
                ,"postpageVisible":1
                ,"saveAscFile":0
                ,"saveStlFile":1
                ,"saveObjFile":0
                ,"savePlyFile":0
                ,"saveP3File":0
                ,"save3MfFile":0
                ,"saveTxtFile":0
                ,"saveCsvFile":0
                }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("savePostProcess result: "+resultFromService)
##\brief
##**返回到扫描界面** \n
##**return To Scan**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"enterScanPage2"}</pre>
def returnToScan():
    paramsList={"cmd":"enterScanPage2"}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("returnToScan: "+resultFromService)

##\brief
##**改变点距** \n
##**change Point Distance**
##\brief
##**返回到扫描界面** \n
##**return To Scan**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"enterScanPage2","index": index}</pre>
##\param newPointDis 新点距 \n new Point Distance
'''
    0<=index<5 dis=0.05
    5<=index<10 dis=0.06+(index-5)*0.01
    10<=index<29 dis=0.2+(index-10)*0.1
    29<=index<45   dis=2.5+(index-29)*0.5


    dis=0.05 index={0,1,2,3,4}
    0.06<=dis<=0.1(0.06 0.07 0.08...) index=(dis-0.06)/0.01+5
    0.2<=dis<=2.0(0.2 0.3 0.4...) index=(dis-0.2)/0.1+10
    2.5<=dis<=10(2.5 3 3.5...) index=(dis-2.5)/0.5+29
'''
def changePointDistance(newPointDis):
    logging.info("newPointDis: "+str(newPointDis))
    index=13
    if newPointDis == 0.05 :
        index=4
    elif newPointDis>=0.06 and newPointDis<=0.1 :
        index=round((newPointDis-0.06)/0.01)+5
    elif newPointDis>=0.2 and newPointDis<=2.0 :
        index=round((newPointDis-0.2)/0.1)+10
    elif newPointDis>=2.5 and newPointDis<=10 :
        index=round((newPointDis-2.5)/0.5)+29
    paramsList={    "cmd": "setPointDist2",    "index": index}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("changePointDistance: "+resultFromService)
##\brief
##**改变激光模式** \n
##**change Laser Grade**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"setLaserGrade2","index": index}</pre>
##\param index 0:网格 1:平行 2:单线 \n 0:cross 1:parallel 2:single
'''
0:cross 1:parallel 2:single
'''
def changeLaserGrade(index):
    paramsList={"cmd":"setLaserGrade2", "index":index}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("changeLaserGrade: "+resultFromService)
##\brief
##**改变扫面物体模式** \n
##**change Scan Objec tType**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"setScanObject2","index": index}</pre>
##\param index 0:普通 1:反光 \n 0:normal 1:reflect
'''
0:normal 1:reflect
'''
def changeScanObjectType(index):
    paramsList={"cmd":"setScanObject2", "index":index}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("changeScanObjectType: "+resultFromService)
##\brief
##**网格扫描时改变亮度** \n
##**change Brightness When Scan Mesh**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"changeBrightStep2","index": index}</pre>
##\param index 
##0<=index<=17
def changeBrightnessWhenScanMesh(index):
    paramsList={"cmd":"changeBrightStep2", "index":index}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("changeBrightnessWhenScanMesh: "+resultFromService)

##\brief
##**框架点扫描时改变亮度** \n
##**change Brightness When Scan Framwork**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"changeBrightRadio2","index": index}</pre>
##\param index 
##0:低 1:中 2:高 \n
##0:low 1:medium 2:high
def changeBrightnessWhenScanFramwork(index):
    paramsList={"cmd":"changeBrightRadio2","index":index}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("changeBrightnessWhenScanFramwork: "+resultFromService)

##\brief
##**设置是否为户外模式** \n
##**change Outdoor Mode**
##@details
##<pre><b>ZMQ</b>:envelope v2.0/invoke/scanservice/other
##payload	{"cmd":"setIsStrongLight2","strong": isOutdoorMode}</pre>
def changeOutdoorMode(isOutdoorMode):
    paramsList={"cmd":"setIsStrongLight2","strong":isOutdoorMode}
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("changeOutdoorMode: "+resultFromService)

def clipCloudWithClipPlane(centerX,centerY,centerZ,normalX,normalY,normalZ):
    paramsList={"cmd":"clipCloudWithClipPlane",
                "clipPlaneCenterX":centerX,
                "clipPlaneCenterY":centerY,
                "clipPlaneCenterZ":centerZ,
                "clipPlaneNormalX":normalX,
                "clipPlaneNormalY":normalY,
                "clipPlaneNormalZ":normalZ,
                }
    paramsJsonStr=json.dumps(paramsList)
    logging.info("paramsJsonStr: "+paramsJsonStr)
    resultFromService=sendMessageToService(
        "{0}/{1}/{2}/{3}".format(C_zmqVersion,C_msgTypeInvoke,C_zmqObjectName,C_msgTopicOther)
        ,paramsJsonStr
    )
    logging.info("clipCloudWithClipPlane: "+resultFromService)

#根据底面上的标志点确定底面然后进行裁剪，需要预先确保标志点都贴在底面上
def clipCloudWithBottomPlane():
    file_path=saveScanFileName+"_Project1.p3"
    line_index=10
    offset=4.0
    print("file_path:"+file_path)
    with open(file_path, 'r') as file:
        lines = file.readlines()
        if line_index < 0 or line_index >= len(lines):
            print("Invalid line number.")
            return
        line = lines[line_index].strip().split()
        if len(line) != 7:
            print("Invalid line format.")
            return

        # 提取点和法向的坐标
        point_coords = list(map(float, line[:3]))
        normal_coords = list(map(float, line[3:6]))

        # 计算新平面的点和法向
        new_point_coords = [point_coords[i] + offset * normal_coords[i] for i in range(3)]
        new_normal_coords = list(normal_coords)

        # 打印新平面的点法式表示
        print("New Plane:")
        print("Point: ({:.2f}, {:.2f}, {:.2f})".format(*new_point_coords))
        print("Normal: ({:.2f}, {:.2f}, {:.2f})".format(*new_normal_coords))
        clipCloudWithClipPlane(new_point_coords[0],new_point_coords[1],new_point_coords[2],
                               new_normal_coords[0],new_normal_coords[1],new_normal_coords[2])

