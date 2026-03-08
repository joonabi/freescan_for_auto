from scan_common import *
from instructions import *

'''
runtime environment:
	python 3.6(64-bit)
	pip 9.0.3
	setuptools 39.0.1
	pyzmq 25.1.0
'''

while True:
    user_input = input("""
====================================================================>
Please enter a string to execute command, or 'q' to quit:
init: run FreeScan and connectToFreeScan
enterScan: chooseScanMode for Combo(only laser by now)
newProject: new project
openProject: open project
setScanType: chooseScanSubMode(scan markers or scan mesh)
importMarkers: importGlobalMarkersFile
scanSettings: change scan settings
startScan: start scan
pauseScan: pause scan
clearScan: clear scan(cancel scan)
saveScan: save your scan
enterPostProcess: enter PostProcess
q: quit
<====================================================================
your command:""")

    if user_input.lower() == 'q':
        print("Exiting the program.")    
        disconnectToFreeScan()
        cmd_command="taskkill /F /im FreeScan.exe"
        subprocess.Popen(cmd_command,shell=True)		
        exit()
    elif user_input == 'init':
        working_dir=service_dir
        runFreeScanCommand=[working_dir+"/FreeScan.exe"]
        waitInitEvent.clear()
        freescanProcess=subprocess.Popen(runFreeScanCommand,cwd=working_dir,close_fds=True)
        connectToFreeScan()
        waitInitEvent.wait()
        newProject()

    elif user_input == 'enterScan':
        chooseScanMode()

    elif user_input == 'newProject':
        newProject()
    
    elif user_input == 'openProject':
        openProject()

    elif user_input == 'setScanType':
        scanType=input("enter 0 for scan markers,1 for scan mesh")
        chooseScanType(int(scanType))

    elif user_input == 'importMarkers':
        importGlobalMarkersFile()

    elif user_input == 'startScan':
        startScan()
    
    elif user_input == 'pauseScan':
        waitPauseScanEvent.clear()
        pauseScan()
        waitPauseScanEvent.wait()
        waitSaveP3Event.clear()
        saveScan(saveAscFile=False,
                 saveP3File=True,
                 saveStlFile=False,
                 saveObjFile=False,
                 savePlyFile=False,
                 save3MfFile=False
                )
        waitSaveP3Event.wait()
        waitClipCloudEvent.clear()
        clipCloudWithBottomPlane()
        waitClipCloudEvent.wait()
        waitSaveAscEvent.clear()
        saveScan(saveAscFile=True,
                 saveP3File=False,
                 saveStlFile=False,
                 saveObjFile=False,
                 savePlyFile=False,
                 save3MfFile=False
                )    
        waitSaveAscEvent.wait()

    elif user_input == 'clearScan':
        clearScan()

    elif user_input == 'saveScan':
        saveAsc=input("save your scan as asc? input y/n : ")
        saveP3=input("save your scan as P3? input y/n : ")
        saveStl=input("save your scan as stl? input y/n : ")
        saveObj=input("save your scan as Obj? input y/n : ")
        savePly=input("save your scan as Ply? input y/n : ")
        save3Mf=input("save your scan as 3Mf? input y/n : ")
        saveScan(saveAscFile=(saveAsc=='y'),
                 saveP3File=(saveP3=='y'),
                 saveStlFile=(saveStl=='y'),
                 saveObjFile=(saveObj=='y'),
                 savePlyFile=(savePly=='y'),
                 save3MfFile=(save3Mf=='y')
                )
    
    elif user_input == 'enterPostProcess':
        needOptimizeBeforePostProcess=input("do you need optimize before postProces? input y/n : ")
        enterPostProcess(needOptimize=(needOptimizeBeforePostProcess=='y'))
        while True :
            postProcess_user_input=input("""
====================================================================>
Please enter a string to execute command, or 'exit' to return to scan:
apply: applyPostProcess
undo: undoPostProcess
confirm: confirmPostProcess
save: savePostProcess
exit: return to scan
<====================================================================
your command:""")
            if postProcess_user_input=="exit":
                returnToScan()
                break
            elif postProcess_user_input=="apply":
                applyPostProcess()
            elif postProcess_user_input=="undo":
                undoPostProcess()
            elif postProcess_user_input=="confirm":
                confirmProstProcess()
            elif postProcess_user_input=="save":
                savePostProcess()
            else:
                print("Invalid input.")
            os.system('pause')

    elif user_input=='scanSettings':
        while True :
            scanSettings_user_input=input("""
====================================================================>
Please enter a string to execute command, or 'exit' to exit scanSettings:
pointDis: changePointDistance
laserGrade: changeLaserGrade
objectType: changeScanObjectType
brightness1: changeBrightnessWhenScanMesh
brightness2: changeBrightnessWhenScanFramwork
outdoor:changeOutdoorMode
increaseMarkers:changeAddIncreaseMarkersMode
<====================================================================
your command:""")
            if scanSettings_user_input=="exit":
                break
            elif scanSettings_user_input=="pointDis":
                pointDis_input=input("""enter pointDistance you want to change to: 
dis=0.05 index={0,1,2,3,4}
0.06<=dis<=0.1(0.06 0.07 0.08...) index=(dis-0.06)/0.01+5
0.2<=dis<=2.0(0.2 0.3 0.4...) index=(dis-0.2)/0.1+10
2.5<=dis<=10(2.5 3 3.5...) index=(dis-2.5)/0.5+29          
your pointDistance:""")
                changePointDistance(float(pointDis_input))
            elif scanSettings_user_input=="laserGrade":
                laserGrade_input=input("""enter laserGrade you want to change to: 
0:cross 1:parallel 2:single          
your laserGrade:""")
                changeLaserGrade(int(laserGrade_input))
            elif scanSettings_user_input=="objectType":
                objectType_input=input("""enter objectType you want to change to: 
0:normal 1:reflect          
your objectType:""")
                changeScanObjectType(int(objectType_input))
            elif scanSettings_user_input=="brightness1":
                brightness_input=input("""enter brightness you want to change to: 
0<=index<=17          
your brightness:""")
                changeBrightnessWhenScanMesh(int(brightness_input))
            elif scanSettings_user_input=="brigntness2":
                brightness_input=input("""enter brightness you want to change to: 
0:low 1:medium 2:high          
your brightness:""")
                changeBrightnessWhenScanFramwork(int(brightness_input))
            elif scanSettings_user_input=="outdoor":
                outdoor_input=input("""enter a number to open or close outdoor mode: 
0:close outdoor mode 1:open outdoor mode          
your number:""")
                changeOutdoorMode(outdoor_input=='1')
            elif scanSettings_user_input=="increaseMarkers":
                increaseMarkers_input=input("""enter a number to open or close addIncreaseMarkers mode: 
0:close addIncreaseMarkers mode 1:open addIncreaseMarkers mode          
your number:""")
                changeAddIncreaseMarkersMode(increaseMarkers_input=='1')
            else:
                print("Invalid input.")
            os.system('pause')   

    else:
        print("Invalid input.")


    os.system('pause')

