cmake_minimum_required(VERSION 3.14)
cmake_policy(VERSION 3.14)

set(TARGET_NAME Sn3DQmlController)

# ── SDK 공유 소스 (SDKCPlusDemo) ────────────────────────────────
set(SDK_DEMO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../SDKCPlusDemo/src")

file(GLOB COMMON_HEADERS ${SDK_DEMO_SRC}/SDKDemoCommon/*.h)
file(GLOB COMMON_SOURCES ${SDK_DEMO_SRC}/SDKDemoCommon/*.cpp)
file(GLOB FREESCAN_HEADERS ${SDK_DEMO_SRC}/SDKDemoFreeScan/*.h)
file(GLOB FREESCAN_SOURCES ${SDK_DEMO_SRC}/SDKDemoFreeScan/*.cpp)
file(GLOB OPTIMSCAN_HEADERS ${SDK_DEMO_SRC}/SDKDemoOptimScan/*.h)
file(GLOB OPTIMSCAN_SOURCES ${SDK_DEMO_SRC}/SDKDemoOptimScan/*.cpp)
file(GLOB EINSCAN_HEADERS ${SDK_DEMO_SRC}/SDKDemoEinScan/*.h)
file(GLOB EINSCAN_SOURCES ${SDK_DEMO_SRC}/SDKDemoEinScan/*.cpp)

# SDKDemoFreeScan 파일 중 UI가 필요한 파일만 제외
# (QDialog, QWidget 상속 파일은 제외하고 순수 비즈니스 로직만 포함)
list(FILTER FREESCAN_SOURCES EXCLUDE REGEX ".*mainview.*\\.cpp")
list(FILTER FREESCAN_SOURCES EXCLUDE REGEX ".*proppanel.*\\.cpp")
list(FILTER FREESCAN_SOURCES EXCLUDE REGEX ".*postprocess.*\\.cpp")
list(FILTER FREESCAN_SOURCES EXCLUDE REGEX ".*choosemode.*\\.cpp")
list(FILTER FREESCAN_SOURCES EXCLUDE REGEX ".*calibpage.*\\.cpp")
list(FILTER FREESCAN_SOURCES EXCLUDE REGEX ".*markpage.*\\.cpp")
list(FILTER FREESCAN_HEADERS EXCLUDE REGEX ".*mainview.*\\.h")
list(FILTER FREESCAN_HEADERS EXCLUDE REGEX ".*proppanel.*\\.h")
list(FILTER FREESCAN_HEADERS EXCLUDE REGEX ".*postprocess.*\\.h")
list(FILTER FREESCAN_HEADERS EXCLUDE REGEX ".*choosemode.*\\.h")
list(FILTER FREESCAN_HEADERS EXCLUDE REGEX ".*calibpage.*\\.h")
list(FILTER FREESCAN_HEADERS EXCLUDE REGEX ".*markpage.*\\.h")

# SDKDemoOptimScan/EinScan: UI-heavy mainview/tabwidget 제외, 순수 래퍼만 포함
list(FILTER OPTIMSCAN_SOURCES EXCLUDE REGEX ".*mainview.*\\.cpp")
list(FILTER OPTIMSCAN_SOURCES EXCLUDE REGEX ".*tabwidget.*\\.cpp")
list(FILTER OPTIMSCAN_HEADERS EXCLUDE REGEX ".*mainview.*\\.h")
list(FILTER OPTIMSCAN_HEADERS EXCLUDE REGEX ".*tabwidget.*\\.h")
list(FILTER EINSCAN_SOURCES EXCLUDE REGEX ".*mainview.*\\.cpp")
list(FILTER EINSCAN_SOURCES EXCLUDE REGEX ".*tabwidget.*\\.cpp")
list(FILTER EINSCAN_SOURCES EXCLUDE REGEX ".*progressbar.*\\.cpp")
list(FILTER EINSCAN_HEADERS EXCLUDE REGEX ".*mainview.*\\.h")
list(FILTER EINSCAN_HEADERS EXCLUDE REGEX ".*tabwidget.*\\.h")
list(FILTER EINSCAN_HEADERS EXCLUDE REGEX ".*progressbar.*\\.h")

# sn3dsdkdemoapp 및 루트 레벨 공통 소스
set(APP_SOURCES
    ${SDK_DEMO_SRC}/sn3dsdkdemoapp.cpp
    ${SDK_DEMO_SRC}/sn3dsdkdemoapp.h
    ${SDK_DEMO_SRC}/calibrator.cpp
    ${SDK_DEMO_SRC}/calibrator.h
    ${SDK_DEMO_SRC}/scanresultfile.cpp
    ${SDK_DEMO_SRC}/scanresultfile.h
    ${SDK_DEMO_SRC}/snjsondocument.cpp
    ${SDK_DEMO_SRC}/snjsondocument.h
    ${SDK_DEMO_SRC}/util.cpp
    ${SDK_DEMO_SRC}/util.h
)

# ── 이 프로젝트 고유 소스 ────────────────────────────────────────
set(QML_SOURCES
    main.cpp
    FreeScanController.cpp
    FreeScanController.h
    DataManager.cpp
    DataManager.h
)

# QML 리소스
set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../qml.qrc)

add_executable(${TARGET_NAME}
    ${QML_SOURCES}
    ${APP_SOURCES}
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
    ${FREESCAN_SOURCES}
    ${FREESCAN_HEADERS}
    ${OPTIMSCAN_SOURCES}
    ${OPTIMSCAN_HEADERS}
    ${EINSCAN_SOURCES}
    ${EINSCAN_HEADERS}
    ${RESOURCES}
)

# ── Qt 컴포넌트 ──────────────────────────────────────────────────
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Quick QuickControls2 Qml Network)

# ── 인클루드 경로 ────────────────────────────────────────────────
target_include_directories(${TARGET_NAME} PUBLIC
    ${SDK_DEMO_SRC}
    ${SDK_DEMO_SRC}/SDKDemoCommon
    ${SDK_DEMO_SRC}/SDKDemoFreeScan
    ${SDK_DEMO_SRC}/SDKDemoOptimScan
    ${SDK_DEMO_SRC}/SDKDemoEinScan
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# ── 라이브러리 링크 ──────────────────────────────────────────────
target_link_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib
)

target_link_libraries(${TARGET_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Qml
    Qt5::Network
    glogc.lib
    SDKCWrapper.lib
    SDKSocketWrapper.lib
    SDKModBusWrapper.lib
    Version
)

# ── 컴파일 정의 ──────────────────────────────────────────────────
target_compile_definitions(${TARGET_NAME} PRIVATE
    GLOG_NO_ABBREVIATED_SEVERITIES
    WIN32_LEAN_AND_MEAN
    UNICODE
    _UNICODE
    QT_MESSAGELOGCONTEXT
)

target_compile_options(${TARGET_NAME} PRIVATE /utf-8 /wd4828 /wd5208)

set_target_properties(${TARGET_NAME} PROPERTIES
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)

set_project_compile_definitions(${TARGET_NAME} "/MP")
copy_debug_file(${TARGET_NAME} ${COPY_PATH})

SOURCE_GROUP(SDKDemoCommon FILES ${COMMON_HEADERS} ${COMMON_SOURCES})
SOURCE_GROUP(SDKDemoFreeScan FILES ${FREESCAN_HEADERS} ${FREESCAN_SOURCES})
SOURCE_GROUP(SDKDemoOptimScan FILES ${OPTIMSCAN_HEADERS} ${OPTIMSCAN_SOURCES})
SOURCE_GROUP(SDKDemoEinScan FILES ${EINSCAN_HEADERS} ${EINSCAN_SOURCES})
SOURCE_GROUP(QmlController FILES ${QML_SOURCES})
