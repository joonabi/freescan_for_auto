# 配置conan
include(conan)

conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS BUILD never)
include(SetConanPackageDir)

set_package_root_postfix(Qt Qt5_DIR /lib/cmake/Qt5)
set_package_root_postfix(glogc glogc_DIR /cmake)
set_package_root_postfix(ZeroMQ ZeroMQ_DIR /cmake)
set_package_root_postfix(Sn3DSDKWrapper Sn3DSDKWrapper_DIR /cmake)
set_package_root_postfix(Sn3DSDKCore Sn3DSDKCore_DIR /cmake)
set_package_root_postfix(Sn3DSDKPlugin Sn3DSDKPlugin_DIR /cmake)
set_package_root_postfix(Sn3DSDKDataLib Sn3DSDKDataLib_DIR /cmake)
#set_package_root_postfix(gflags gflags_DIR /cmake)
#set_package_root_postfix(glogc glogc_DIR /cmake)
#set_package_root_postfix(mimalloc mimalloc_LIB /lib)
#set_package_root_postfix(mimalloc mimalloc_INCLUDE /include)
#set_package_root_postfix(Sn3DCommon Sn3DCommon_DIR /cmake)
#set_package_root_postfix(EinScan3rdDependencies EinScan3rdDependencies_BIN_DIR /bin/Release)
#set_package_root_postfix(FunctionLayer FunctionLayer_BIN_DIR /bin)
#set_package_root_postfix(FunctionLayer FunctionLayer_INCLUDED_DIR /include)
#set_package_root_postfix(Sn3D.UI.Sn3DFileDialog Sn3DUISn3DFileDialog_DIR /cmake)
#set_package_root_postfix(Sn3D.3DDigit.Sn3DPostProcess Sn3D3DDigitSn3DPostProcess_DIR /cmake)
#set_package_root_postfix(Sn3D.3DDigit.Sn3DPostProcessUI Sn3D3DDigitSn3DPostProcessUI_DIR /cmake)
#set_package_root_postfix(Sn3D.3DDigit.Sn3DMeasurement Sn3D3DDigitSn3DMeasurement_DIR /cmake)
#set_package_root_postfix(Sn3D.3DDigit.Sn3DMeasurementUI Sn3D3DDigitSn3DMeasurementUI_DIR /cmake)
#set_package_root_postfix(ThirdpartySoftwareRequiredComponents ThirdpartySoftwareRequiredComponents_DIR /cmake)
#set_package_root_postfix(QtTunnel QtTunnel_DIR /cmake)
#set_package_root_postfix(nvml nvml_DIR /cmake)
#set_package_root_postfix(Sn3D.Navigation Sn3DNavigation_DIR /cmake)
#set_package_root_postfix(Eigen Eigen_DIR /cmake)
#set_package_root_postfix(Boost Boost_DIR /cmake)

find_package(Qt5 REQUIRED 
    COMPONENTS 
    Core Gui Xml Qml Quick RemoteObjects Sql
    Multimedia OpenGL Concurrent LinguistTools 
    QuickWidgets WebSockets Widgets)

set(BUILD_SHARED_LIBS OFF)
find_package(glogc REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package(Sn3DSDKWrapper REQUIRED)
find_package(Sn3DSDKCore REQUIRED)
find_package(Sn3DSDKPlugin REQUIRED)
find_package(Sn3DSDKDataLib REQUIRED)
#find_package(gflags REQUIRED)
#find_package(glogc REQUIRED)
#find_package(QtTunnel REQUIRED CONFIG)
#find_package(Sn3DUISn3DFileDialog REQUIRED CONFIG)
#find_package(Sn3D3DDigitSn3DPostProcess REQUIRED CONFIG)
#find_package(Sn3D3DDigitSn3DPostProcessUI REQUIRED CONFIG)
#find_package(Sn3D3DDigitSn3DMeasurement REQUIRED CONFIG)
#find_package(Sn3D3DDigitSn3DMeasurementUI REQUIRED CONFIG)
#find_package(ThirdpartySoftwareRequiredComponents REQUIRED CONFIG)
#find_package(Sn3DCommon REQUIRED CONFIG)
#find_package(nvml REQUIRED)
#find_package(Sn3DNavigation REQUIRED CONFIG)
#find_package(Eigen REQUIRED CONFIG)
#find_package(Boost REQUIRED CONFIG COMPONENTS system locale)

###################################################################################
STRING(REGEX MATCH "/[0-9][0-9.]+/" FreeScanSync_version "${CONAN_BIN_DIRS_FREESCANSYNC_RELEASE}")
STRING(REPLACE "/" "" FreeScanSync_version "${FreeScanSync_version}")
message("FreeScanSync_version:${FreeScanSync_version}")
###################################################################################
STRING(REGEX MATCH "/[0-9][0-9.]+/" Sn3DPostProcess_version "${CONAN_BIN_DIRS_SN3D.3DDIGIT.SN3DPOSTPROCESS_RELEASE}")
STRING(REPLACE "/" "" Sn3DPostProcess_version "${Sn3DPostProcess_version}")
message("Sn3DPostProcess_version:${Sn3DPostProcess_version}")
###################################################################################
STRING(REGEX MATCH "/[0-9][0-9.]+/" Sn3DMeasurement_version "${CONAN_BIN_DIRS_SN3D.3DDIGIT.SN3DMEASUREMENT_RELEASE}")
STRING(REPLACE "/" "" Sn3DMeasurement_version "${Sn3DMeasurement_version}")
message("Sn3DMeasurement_version:${Sn3DMeasurement_version}")
###################################################################################