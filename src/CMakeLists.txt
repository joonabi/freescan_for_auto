cmake_minimum_required(VERSION 3.14)
cmake_policy(VERSION 3.14)

set(TARGET_NAME Sn3DSDKDemo)
file(GLOB COMMON_HEADERS SDKDemoCommon/*.h)
file(GLOB FREESCAN_HEADERS SDKDemoFreeScan/*.h)
file(GLOB OPTIMSCAN_HEADERS SDKDemoOptimScan/*.h)
file(GLOB EINSCAN_HEADERS SDKDemoEinScan/*.h)
file(GLOB OPERATOR_HEADERS SDKDemoOperator/*.h)
file(GLOB HEADERS *.h subcontrol/*.h ${COMMON_HEADERS} ${FREESCAN_HEADERS} ${OPTIMSCAN_HEADERS} ${EINSCAN_HEADERS} ${OPERATOR_HEADERS})
file(GLOB COMMON_SOURCES SDKDemoCommon/*.cpp)
file(GLOB FREESCAN_SOURCES SDKDemoFreeScan/*.cpp)
file(GLOB OPTIMSCAN_SOURCES SDKDemoOptimScan/*.cpp)
file(GLOB EINSCAN_SOURCES SDKDemoEinScan/*.cpp)
file(GLOB OPERATOR_SOURCES SDKDemoOperator/*.cpp)
file(GLOB SOURCES *.cpp subcontrol/*.cpp ${COMMON_SOURCES} ${FREESCAN_SOURCES} ${OPTIMSCAN_SOURCES} ${EINSCAN_SOURCES} ${OPERATOR_SOURCES})
file(GLOB COMMON_UIS SDKDemoCommon/*.ui)
file(GLOB FREESCAN_UIS SDKDemoFreeScan/*.ui)
file(GLOB OPTIMSCAN_UIS SDKDemoOptimScan/*.ui)
file(GLOB EINSCAN_UIS SDKDemoEinScan/*.ui)
file(GLOB OPERATOR_UIS SDKDemoOperator/*.ui)
file(GLOB UIS *.ui subcontrol/*.ui ${COMMON_UIS} ${FREESCAN_UIS} ${OPTIMSCAN_UIS} ${EINSCAN_UIS} ${OPERATOR_UIS})
file(GLOB RESOURCES *.qrc)
add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES} ${UIS} ${RESOURCES})
find_package(Qt5 COMPONENTS OpenGL REQUIRED)
if(CUSTOMER_MODE)
	target_include_directories(${TARGET_NAME} PUBLIC
		subcontrol
		SDKDemoCommon
		SDKDemoOperator
		${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
else()
get_target_property(_ZeroMQ_LIB libzmq IMPORTED_IMPLIB_RELEASE)
target_include_directories(${TARGET_NAME} PUBLIC
	subcontrol
	${_sn3dsdkcore_install_prefix}/include
	${_sn3dsdkdatalib_install_prefix}/include
)
get_target_property(PluginInclueDir Sn3DSDKPlugin INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(${TARGET_NAME} PRIVATE ${PluginInclueDir} SDKDemoCommon SDKDemoOperator ${ZeroMQ_INCLUDE_DIR})
endif()

set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Gui Qt5::Widgets glogc Boost::headers Boost::system Boost::locale SDKCWrapper)
target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Gui Qt5::Widgets glogc SDKCWrapper 	${_ZeroMQ_LIB})
if(CUSTOMER_MODE)
	target_link_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib)
	target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL glogc.lib SDKCWrapper.lib SDKCWrapper.lib SDKSocketWrapper.lib SDKModBusWrapper.lib)
else()
	target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL glogc SDKCWrapper)
endif()
target_compile_definitions(${TARGET_NAME}
    PRIVATE
	GLOG_NO_ABBREVIATED_SEVERITIES
	WIN32_LEAN_AND_MEAN
	UNICODE
	_UNICODE
)

set_project_compile_definitions(${TARGET_NAME} "/MP")
copy_debug_file(${TARGET_NAME} ${COPY_PATH})

SOURCE_GROUP(SDKDemoCommon FILES ${COMMON_HEADERS} ${COMMON_SOURCES} ${COMMON_UIS})
SOURCE_GROUP(SDKDemoFreeScan FILES ${FREESCAN_HEADERS} ${FREESCAN_SOURCES} ${FREESCAN_UIS})
SOURCE_GROUP(SDKDemoOptimScan FILES ${OPTIMSCAN_HEADERS} ${OPTIMSCAN_SOURCES} ${OPTIMSCAN_UIS})
SOURCE_GROUP(SDKDemoEinScan FILES ${EINSCAN_HEADERS} ${EINSCAN_SOURCES} ${EINSCAN_UIS})
SOURCE_GROUP(SDKDemoOperator FILES ${OPERATOR_HEADERS} ${OPERATOR_SOURCES} ${OPERATOR_UIS})
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")
message("-------------${CUSTOMER_MODE}")
if(CUSTOMER_MODE)
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/../bin/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
else()
	add_custom_target(DeployDemo
		# COMMAND ${DEPLOYQT_EXECUTABLE} $<TARGET_FILE:${TARGET_NAME}>
		COMMAND ${DEPLOYQT_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/demo_runtime/${TARGET_NAME}.exe
	)
    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/demo_runtime)
	install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${SDKCWrapper_BIN_DIR}/SDKCWrapper.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${SDKCWrapper_BIN_DIR}/SDKSocketWrapper.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${SDKCWrapper_BIN_DIR}/SDKModbusWrapper.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${SDKCWrapper_BIN_DIR}/sn3dmodbus.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${Sn3DSDKCore_BIN_DIR}/Sn3DSDKCore.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	message("=================${Sn3DSDKCore_BIN_DIR}")
	install(FILES ${Sn3DSDKDataLib_Dll} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${Sn3DSDKPlguin_BIN_DIR}/Sn3DSDKConfig.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

	
    #get_target_property(_glogc_DLL glogc IMPORTED_LOCATION_RELEASE)
    message("glogc_DIR = ${glogc_DIR}")
    install(FILES ${glogc_DIR}/../bin/glogc.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(FILES ${glogc_DIR}/../bin/glogc.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/demo_runtime)
    message("Qt5_DIR = ${Qt5_DIR}")
    install(FILES ${Qt5_DIR}/../../../bin/Qt5SerialBus.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(FILES ${Qt5_DIR}/../../../bin/Qt5SerialPort.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    # 找到zmq的dll名字
	get_target_property(_ZeroMQ_DLL libzmq IMPORTED_LOCATION_RELEASE)
	install(FILES ${_ZeroMQ_DLL} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/zmq_sdk_config.ini DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${Sn3DSDKPlguin_Dll} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${Sn3DSDKPlguin_Exe} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	#install(FILES ${Sn3DSDKPlguin_PMXml} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(FILES ${Sn3DSDKPlguin_BIN_DIR}/processManager.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${Sn3DSDKPlguin_BIN_DIR}/processManagerApp.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${Sn3DSDKPlguin_BIN_DIR}/processManagerCalib.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${Sn3DSDKPlguin_BIN_DIR}/processManagerSDK.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	install(FILES ${Sn3DSDKPlguin_BIN_DIR}/scansoftwaresdk.ini DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

	install(FILES ${Sn3DSDKPlguin_SDKXml} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

    message("SDKCWrapper_BIN_DIR = ${SDKCWrapper_BIN_DIR}")
    message("SDKCWrapper_LIB_DIR = ${SDKCWrapper_LIB_DIR}")
    message("SDKCWrapper_INC_DIR = ${SDKCWrapper_INC_DIR}")
    install(DIRECTORY ${SDKCWrapper_BIN_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
    install(DIRECTORY ${SDKCWrapper_LIB_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)
    install(DIRECTORY ${SDKCWrapper_INC_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)
endif()
