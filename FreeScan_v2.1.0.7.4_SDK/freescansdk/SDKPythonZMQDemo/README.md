@mainpage
# python demo runtime environment:
*python 3.6(64-bit)*

*pip 9.0.3*

*setuptools 39.0.1*

*pyzmq 25.1.0*

# customize python_scan_config.ini
```ini
[General]
devicetype = combo #device type
slndirpath = D:/desktop/Data #when create a new projects group,select a folder path to place it
openProjectDir = D:/desktop/Data #when open a new projects group,select a folder where it at
openProjectFile = Data.sln_laser_combo #when open a new projects group,select the .sln file
scanType =0 #0 for laser scan mode(only laser scan mode is supported by sdk now)
service_dir = D:/Sn3DSDK/release_sdk_freescan_v2.0.1_3553_f615ef9c/bin #where is your FreeScan.exe(sdk mode)
saveScanFileName= D:/desktop/Data1/saveMyScan #where save scan,select scan file name and path
globalMarkersFile=D:/desktop/freescansdk2.0.1-python-zmq/globalMarkersTest_Project1.p3 #where import global markers file,select a global markers file
```
# run demo

```
python.exe .\pyZmqDemo.py
```

# connect to FreeScan with zmq

two zmq connetion is required:reply(demo)-request(FreeScan) which is aimed at sending command to FreeScan,request(demo)-reply(FreeScan) which is aimed at receiving message from FreeScan

### init context

```python
zmqContext=zmq.Context()
zmqReqSocket=zmqContext.socket(zmq.REQ)
zmqRepSocket=zmqContext.socket(zmq.REP)
```

### connect to FreeScan

**NOTE: When FreeScan is working in sdk mode,it will keep waiting until zmq connection established correctly**

```python
zmqReqSocket.connect("tcp://127.0.0.1:"+demo_zmq_req_port)
zmqRepSocket.bind("tcp://127.0.0.1:"+demo_zmq_rep_port)
```

# send command to FreeScan

```python
def sendMessageToService(envelope,payload):
    logging.info("sendMessageToService envelope:{0} payload:{1}".format(envelope,payload))
    zmqReqSocket.send_string(envelope,zmq.SNDMORE)
    zmqReqSocket.send_string(payload)
    message=zmqReqSocket.recv_string()
    return message
```
**Note:The detailed explanation can be found in the instructions.py**

# receive payload from FreeScan

```python
receiverThreadRunning=True
def receiverFunc():
    global receiverThreadRunning
    while receiverThreadRunning:
        responseList=[]
        envelope=zmqRepSocket.recv()
        payload=""
        while zmqRepSocket.getsockopt(zmq.RCVMORE):
            payload=zmqRepSocket.recv()
            responseList.append(payload)

        zmqRepSocket.send(b'\x02')
        envelopeStr=envelope.decode('utf-8')
        payloadStr=b''.join(responseList).decode('utf-8')
        logging.info("subscriber get response: envelopeStr="+envelopeStr+" payloadStr="+payloadStr)
        
        envelopeTokens=envelopeStr.split('/')
        if(len(envelopeTokens)!=4):continue
        api_version = envelopeTokens[0]
        msg_type = envelopeTokens[1]
        object_name = envelopeTokens[2]
        topic_name = envelopeTokens[3]
    return
receiverThread=Thread(target=receiverFunc,daemon=False)
```
**NOTE: Like `zmqRepSocket.send(b'\x02')` You must send `b'\x02'` everytime you received a envelope-payload from FreeScan**

## payload explanation
the `payloadStr` in the code above is a json contains messages from FreeScan,the following is a detailed explanation of it

### FreeScan start finished 
```json
{    
    "cmd": "showMaximized",
    "expiredDays": 10,//ignore
    "permanent": false//ignore
}
```

### device online/offline status
```json
{    
    "cmd": "deviceStatus",
    "deviceStatus": false //false:device online true:device offline
}
```

### current frame RT
```json
{   
    "cmd": "currentRTcmd",
    "rot0": 0.9998932697307745,//the rotation matrix
    "rot1": 0.0005109950566514646,
    "rot2": -0.014600959939422009,
    "rot3": -0.00042146231094371294,
    "rot4": 0.9999810955996901,
    "rot5": 0.006134395876036174,
    "rot6": 0.014603818562998604,
    "rot7": -0.006127587395994724,
    "rot8": 0.9998745827132939,
    "trans0": -10.64091953867159,//the transform vector
    "trans1": 23.70755333971141,
    "trans2": 68.76225517449166
}
```

### device temperature
```json
{   
    "cmd": "updateDeviceData",
    "data": 
        {        
            "Temperature": 36.56//device temperature
        },
    "name": "DeviceTemperature",
    "type": null,//ignore
    "value": 36.56
}
```

### new projects group create finished
```json
{
    "cmd":"newProjectFinish"
}
```

### new project create finished
```json
{
    "cmd": "addPrj",
    "prjInfo":  
        {
            "prjPath": "Project1.laser_prj_combo",//the project file name
            "rot": [1,0,0,0,1,0,0,0,1],//ignore
            "tran": [0,0,0]//ignore
        },
    "scanMode": null//ignore
}
```

### information after create/load project finished
```json
{   
    "cmd": "createOrLoadCurProjectFinished",
    "ADVANCED_MODE": "0",
    "BRIGHTNESS_LEVEL": "0",
    "CALIBTIME": "",
    "DEF_OPTIMIAL": "1",
    "DIS": "3.5",
    "FIRMWARE": "",
    "FRAMES": "0",
    "GENERATE_CLOUD": "0",
    "LASER_GRADE": "7",
    "MARKERS": "0",
    "MATERIAL_TYPE": "0",
    "NAME": "Project1",
    "NEEDAPPEDIT": 0,
    "POINTS": "0",
    "PROJECT_TYPE": "1",
    "SCANNER": "COMBO",
    "SERIALCODE": "",
    "SHAPECHANGE_MODE": "0",
    "SOFTWARE": "2.0.1",
    "SOPHISTICATED_DIS": "0",
    "STRONG_LIGHT_MODE": "0",
    "TRIANGLES": "0",
    "deviceNum": "",
    "frameCount": 0,
    "isCreate": true,
    "isCreateBySln": true,
    "isCurrentProj": true,
    "isEmptyProj": true,
    "isOpenFromSln": true,
    "markerCount": 0,
    "markerUuid": "d49faff8-62d0-43b5-a6f6-b70de97382d8",//ignore
    "normalCloudUuid": "b26ffe2d-6fad-4df8-9e9c-38dcc0bf4a0d",//ignore
    "parentName": "Data",
    "points": 0,
    "prjSuffix": ".laser_prj_combo",
    "projectPath": "Project1",
    "skeletonMarkerCount": 0,
    "slnDir": "D:\\desktop\\Data",//the folder of this projects group
    "sophisticatedCloudUuid": "dfa9b438-61e8-4386-a680-6745f7c25bcd",//ignore
    "target": "ProjectListUI",//ignore
    "uuid": "0b23892d-f58c-4af4-a87c-133595454ab3"//ignore
}
```

### progress show information
```json
{    
    "cmd": "showProgress",
    "data": 
        {        
            "info": 7,
            "title": "Project Loading",//the title of the progress
            "titleAddtitionStr": "",
            "type": 0,//ignore
            "value": 0,//the value of the progress
            "visible": true//the visiblity of the proress  
        }
}
```

### progress hide information
```json
{    
    "cmd": "hideProgress",
    "data": 
        {        
            "info": -1,
            "title": "",//the title of the progress
            "titleAddtitionStr": "",
            "type": -1,//ignore
            "value": 0,//the value of the progress
            "visible": false//the visiblity of the proress    
        }
}
```

### pause scan finished
```json
{    
    "cmd": "updateStateWhenStopFusion",
    "val": "pause"
}
```

### save scan finished
```json
{   
    "cmd": "saveScanDataReturn",
    "successfulUuids": ["a8e6e497-2cd5-43e3-9735-15f1f1d340b2"]//ignore
}
```

### apply postprocess finished
```json
{   
    "code": 0,
    "method": "meshResult",
    "success": true//true:apply success,false:apply failed
}
```

### save postprocess finished
```json
{   
    "code": 0,
    "finish": true,//true:save success,false:save failed
    "method": "saveMeshFinished"
}
```

### return to scan from postprocess finished
```json
{    
    "cmd": "sdkScanPageSwitched"
}
```

### clear/cancel scan finished
```json
{   
    "cmd": "cancelScanFinish",
    "deleteClipPlane": false//ignore
}
```

### update scanning information
center=(centerX,centerY,centerZ),
up=(upX,upY,upZ),
eye=(eyeX,eyeY,eyeZ) 
This is perspective information in a three-dimensional scene

When the Angle of view is not turned on, the local Angle of view is enlarged and reduced, which can be approximately considered to be the position and direction of the scanner
```json
{   
    "centerX": 0,
    "centerY": -47.397151947021484,
    "centerZ": 296.232177734375,
    "cloudValidFacesNum": 1,
    "cloudValidNum": 3,
    "cmd": "updateScanningInfo",
    "codingPointNum": 0,
    "containCameraPos": true,
    "curFrameDis": 202,
    "disStep": 5,
    "eyeX": 0,
    "eyeY": 78.99524688720703,
    "eyeZ": -493.7203063964844,
    "frameCount": 0,//current frames count
    "frameRate": 0,//current frame rate
    "hasOneCompleteScale": false,
    "markerRecognize": true,
    "serviceTimeStamp": 411336975,
    "trackLost": false,//true:current track success,false:current track lost
    "upX": 0,
    "upY": -1,
    "upZ": 0,
    "wholeMarkerNum": 0
}
```