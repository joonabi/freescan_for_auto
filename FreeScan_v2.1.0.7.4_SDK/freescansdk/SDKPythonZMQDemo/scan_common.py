import zmq
import os
import subprocess
import configparser
import time
from threading import Thread
import threading
import logging
import json

logging.basicConfig(filename="pylog.txt"
                    ,format = '%(asctime)s - %(name)s - %(levelname)s -%(funcName)s - %(message)s'
                    ,filemode='w'
                    ,level=logging.DEBUG)

common_config_parser=configparser.ConfigParser()
common_config_parser.read("python_scan_config.ini",encoding="utf-8")
#scan config
#设备类型 device type
deviceType=str(common_config_parser['General']['deviceType'])
logging.info('deviceType='+deviceType)
#scan type
scanType=str(common_config_parser['General']['scanType'])
logging.info('scanType='+scanType)
#where is the solution path D:\comboTest\testPY1
slnDirPath=str(common_config_parser['General']['slnDirPath'])
logging.info('slnDirPath='+slnDirPath)
#open project dir path
openProjectDir=str(common_config_parser['General']['openProjectDir'])
logging.info('openProjectDir='+openProjectDir)
#open project file name
openProjectFile=str(common_config_parser['General']['openProjectFile'])
logging.info('openProjectFile='+openProjectFile)
#saveScanFolder
saveScanFileName=str(common_config_parser['General']['saveScanFileName'])
logging.info('saveScanFileName='+saveScanFileName)
#FreeScaPath where is the FreeScan.exe
service_dir=str(common_config_parser['General']['service_dir'])
if(service_dir[-1]!='/'):service_dir=service_dir+'/'
logging.info('service_dir='+service_dir)
#globalMarkersFile
globalMarkersFile=str(common_config_parser['General']['globalMarkersFile'])
logging.info('globalMarkersFile='+globalMarkersFile)

#wait Events
waitInitEvent=threading.Event()#wait for initializing
waitPauseScanEvent=threading.Event()#wait for pausing scan
waitSaveP3Event=threading.Event()#wait for saving p3
waitClipCloudEvent=threading.Event()#wait for clipping cloud
waitSaveAscEvent=threading.Event()#wait for saving asc
def awakenEvent(payloadStr):
    if(not waitInitEvent.is_set() and "showMaximized" in payloadStr):
        waitInitEvent.set()
    elif(not waitPauseScanEvent.is_set() and "hideProgress" in payloadStr):
        waitPauseScanEvent.set()
    elif(not waitSaveP3Event.is_set() and "hideProgress" in payloadStr):
        waitSaveP3Event.set()
    elif(not waitClipCloudEvent.is_set() and "endClipCloudWithClipPlane" in payloadStr):
        waitClipCloudEvent.set()
    elif(not waitSaveAscEvent.is_set() and "hideProgress" in payloadStr):
        waitSaveAscEvent.set()

#sn3d sdk 
C_zmqVersion="v2.0"
C_zmqObjectName = "scanservice"
C_msgTypeInvoke = "invoke"
C_msgTypePropGet = "propGet"
C_msgTypePropSet = "propSet"
C_msgTypePropChanged = "propChanged"
C_msgTypeRequest="request"
C_msgTypeEvent = "event"
C_msgTopicOther="other"
#zmq port config
logging.info("zmq.__version__:"+zmq.__version__)
zmq_config_parser=configparser.ConfigParser()
zmq_config_parser.read(service_dir+"zmq_sdk_config.ini",encoding="utf-8")
demo_zmq_req_port=str(zmq_config_parser["Ports"]["Pub"])
demo_zmq_rep_port=str(zmq_config_parser["Ports"]["Rep"])
logging.info("demo_zmq_req_port: "+"".join(demo_zmq_req_port))
logging.info("demo_zmq_rep_port: "+"".join(demo_zmq_rep_port))
#zmq initial
zmqContext=zmq.Context()
zmqReqSocket=zmqContext.socket(zmq.REQ)
zmqRepSocket=zmqContext.socket(zmq.REP)

def sendMessageToService(envelope,payload):
    logging.info("sendMessageToService envelope:{0} payload:{1}".format(envelope,payload))
    zmqReqSocket.send_string(envelope,zmq.SNDMORE)
    zmqReqSocket.send_string(payload)
    message=zmqReqSocket.recv_string()
    return message

#receice message in thread
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
        
        awakenEvent(payloadStr)

        envelopeTokens=envelopeStr.split('/')
        if(len(envelopeTokens)!=4):continue
        api_version = envelopeTokens[0]
        msg_type = envelopeTokens[1]
        object_name = envelopeTokens[2]
        topic_name = envelopeTokens[3]
    return
receiverThread=Thread(target=receiverFunc,daemon=False)

#ping service
'''
for index in range(4):
    logging.info("client send ping")
    zmqReqSocket.send_string("v2.0/ping")
    message=zmqReqSocket.recv_string()
    logging.info(f"client receive {message}")
    time.sleep(1)
'''

def connectToFreeScan():
    zmqReqSocket.connect("tcp://127.0.0.1:"+demo_zmq_req_port)
    zmqRepSocket.bind("tcp://127.0.0.1:"+demo_zmq_rep_port)
    receiverThread.start()
    return
def disconnectToFreeScan():
    zmqReqSocket.disconnect("tcp://127.0.0.1:"+demo_zmq_req_port)
    global receiverThreadRunning
    receiverThreadRunning=False
    receiverThread.join()
    zmqRepSocket.unbind("tcp://127.0.0.1:"+demo_zmq_rep_port)
    return