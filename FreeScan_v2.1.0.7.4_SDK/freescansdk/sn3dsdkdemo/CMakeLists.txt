cmake_minimum_required(VERSION 3.14)
cmake_policy(VERSION 3.14)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/application.properties" curVersion OFFSET 8)
project(FreeScanSDK VERSION ${curVersion})

string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
message("Build timeStamp is ${BUILD_TIMESTAMP}")
option(CUSTOMER_MODE "是否用户环境编译" ON)

#DEBUG模式开启
function(set_project_compile_definitions targetName compileFlagsInRelease)
	option(_using_debug_compile_definitions_${targetName} "对项目 ${targetName} 开启DEBUG模式 " OFF)
	if(_using_debug_compile_definitions_${targetName})
		message("project: ${targetName} debug open.")
		set_target_properties(${targetName} PROPERTIES COMPILE_FLAGS "/ZI /Od /MP /openmp")
		set_target_properties(${targetName} PROPERTIES LINK_FLAGS "/DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF")
	else()
		set_target_properties(${targetName} PROPERTIES COMPILE_FLAGS ${compileFlagsInRelease})
		set_target_properties(${targetName} PROPERTIES LINK_FLAGS "/DEBUG")
		#set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/INCLUDE:__tcmalloc")
	endif()
endfunction(set_project_compile_definitions)

#每次编译后拷贝.pdb 和 .dll
function(copy_debug_file targetName  dst)
    if(_using_debug_compile_definitions_${targetName})
        if(EXISTS "${dst}")
            add_custom_command(TARGET ${targetName}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${targetName}> ${dst}
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${targetName}> ${dst}
            )
endif()
endif()
endfunction(copy_debug_file)

#VS分组
function(group)
    foreach(f IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${f}")
            file(RELATIVE_PATH f_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${f}")
        else()
            set(f_rel "${f}")
        endif()
        get_filename_component(f_path "${f_rel}" PATH)
        string(REPLACE "/" "\\" f_path_msvc "${f_path}")
        source_group("${f_path_msvc}" FILES "${f}")
    endforeach()
endfunction(group)

#每次编译后拷贝.pdb 和 .dll
function(copy_debug_file targetName  dst)
    if(_using_debug_compile_definitions_${targetName})
        if(NOT ${dst} STREQUAL " ")
    add_custom_command(TARGET ${targetName}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${targetName}> ${dst}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${targetName}> ${dst}
        )
endif()
endif()
endfunction(copy_debug_file)
set(COPY_PATH " " CACHE PATH "要拷贝到的目录")

add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DQT_MESSAGELOGCONTEXT)
add_definitions(-DGLOG_NO_ABBREVIATED_SEVERITIES)
add_definitions(-DGLOG_CUSTOM_PREFIX_SUPPORT)
list(APPEND DEVICES E3_HX E3_HC)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INSTALL_ALWAYS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(COPY_PATH " " CACHE PATH "要拷贝到的目录")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "" FORCE)
set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD ON)

file(GLOB TS_FILES "language_ts/*.ts")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if(CUSTOMER_MODE)
	find_package(Qt5 COMPONENTS Core Gui Widgets)
else()
	include(${CMAKE_CURRENT_SOURCE_DIR}/ConfigConanPackages.cmake)
endif()

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")

if(NOT CUSTOMER_MODE)
	include(GetGitRevisionDescription)
	get_git_head_revision(GIT_REFSPEC GIT_SHA1)
endif()

if(MSVC)
    set(VERSIONINFO_RC "${CMAKE_BINARY_DIR}/VersionInfo.rc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc.in"
                   "${VERSIONINFO_RC}")
endif()

if(NOT CUSTOMER_MODE)
	configure_file(conanfile.py.in ${CMAKE_INSTALL_PREFIX}/conanfile.py)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/FreeScanSDK-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDKPythonZMQDemo DESTINATION ${CMAKE_INSTALL_PREFIX}/)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDKCPlusDemo DESTINATION ${CMAKE_INSTALL_PREFIX}/sn3dsdkdemo
        FILES_MATCHING PATTERN "*" 
        PATTERN "doc" EXCLUDE)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/application.properties DESTINATION ${CMAKE_INSTALL_PREFIX}/sn3dsdkdemo)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/applicationVersionDef.in DESTINATION ${CMAKE_INSTALL_PREFIX}/sn3dsdkdemo)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/sn3dsdkdemo)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ConfigConanPackages.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/sn3dsdkdemo)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md DESTINATION ${CMAKE_INSTALL_PREFIX}/sn3dsdkdemo)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc.in DESTINATION ${CMAKE_INSTALL_PREFIX}/sn3dsdkdemo)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDKCPlusDemo/doc DESTINATION ${CMAKE_INSTALL_PREFIX}/
        FILES_MATCHING PATTERN "*" 
        PATTERN "发送报文流程.eddx" EXCLUDE
        PATTERN "回调解析流程.eddx" EXCLUDE
        PATTERN "自动化C接口对应.xlsx" EXCLUDE
        PATTERN "XL-QER-RD-090_[FreeScan2.0 SDK]_软件需求规格说明书20230924.docx" EXCLUDE
        PATTERN "XL-QER-RD-012_[SDK 2.0]_概要设计说明书.docx" EXCLUDE
        )

    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/branch.properties" DIST_NAME)
    message("DIST_NAME show ${DIST_NAME}")
    message(" PROJECT_NAME = ${PROJECT_NAME}")
    add_custom_target(conan_clean_release
        COMMAND ${CONAN_CMD} remove ${PROJECT_NAME}/${PROJECT_VERSION}@shining3d/${DIST_NAME} -f
    )
    set_target_properties(conan_clean_release PROPERTIES FOLDER helpers)
    
    add_custom_target(conan_export_release
        COMMAND ${CONAN_CMD} export-pkg . ${PROJECT_NAME}/${PROJECT_VERSION}@shining3d/${DIST_NAME} -s compiler=\"Visual Studio\"
            -s compiler.version=$<IF:$<EQUAL:141,${MSVC_TOOLSET_VERSION}>,15,12> -f
        WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
    )
    set_target_properties(conan_export_release PROPERTIES FOLDER helpers)
    
    add_custom_target(conan_upload_release
        COMMAND ${CONAN_CMD} upload ${PROJECT_NAME}/${PROJECT_VERSION}@shining3d/${DIST_NAME} --all --force
    )
    set_target_properties(conan_upload_release PROPERTIES FOLDER helpers)
endif()
#configure_file(${CMAKE_SOURCE_DIR}/applicationVersionDef.in ${CMAKE_BINARY_DIR}/applicationVersionDef.h)

## copy version_sn3dCommon.txt  fixbug记录
#configure_file(${Sn3DCommon_INCLUDE_DIR}/../version_Sn3DCommon.txt  ${CMAKE_INSTALL_PREFIX}/version_Sn3DCommon.txt)
## copy version_functionlayer.txt   fixbug记录
#configure_file(${FunctionLayer_INCLUDED_DIR}/../version_FunctionLayer.txt  ${CMAKE_INSTALL_PREFIX}/version_FunctionLayer.txt)

#get_cmake_property(_varList VARIABLES)
#list(SORT _varList)
#foreach(_var ${_varList})
#	message(STATUS "--${_var}=${${_var}}")
#endforeach()

add_subdirectory(SDKCPlusDemo)
add_subdirectory(SDKQmlController)

if(NOT CUSTOMER_MODE)
    add_subdirectory(SDKSocketServer)
endif()



if(NOT CUSTOMER_MODE)

    #-------------------------------------------------------------------------------------------------|
    #  1. 先自动生成app单独的ts，路径在build\i18n\language_ts下                                |
    #  2. 自动拷贝其他俩仓库的ts到本地路径在build\i18n\before_merge_ts下							  |
    #  3. 自动合并三者的ts在build\i18n\after_merge_ts下												  |
    #  4. 手动合并loke平台上的和本地的到applications\i18n\final_ts下   			←←←←←←←←必须手动※	  |
    #  5. 自动生成了qm文件在applications\i18n\final_qm下											  |
    #-------------------------------------------------------------------------------------------------|
    # 翻译相关
    # 需要翻译的语言
    set(LANG_LIST
        "zh_CN"
        "zh_TW"
        "en_US"
        "fr_FR"
        "de_DE"
        "ja_JP"
        "ko_KR"
    #    "pt"
    #    "pt_BR"
        "pl_PL"
        "ru_RU"
        "es_ES"
        "tr_TR"
    )
    # 生成ts
	# @todo 沈波自动化工程会出现generate_ts生成失败，后续得查一下
    add_custom_target(generate_ts ALL)
    # 定义生成ts文件的命令
    function(generate_ts_file lang outputTs)
        # 调用Qt自带的lupdate.exe, 遍历源码目录, 生成对应语言的ts文件
        add_custom_command(TARGET generate_ts COMMAND Qt${QT_VERSION_MAJOR}::lupdate
            -extensions h,cpp,ui,qml,qrc # 用户手册中的js会导致build报错，过滤掉
            -locations none # 不记录具体在文件的第几行
            -target-language ${lang} # 指定目标语言
            ${CMAKE_SOURCE_DIR} # 指定搜索目录
            -ts ${outputTs} # 指定输出的文件
        )
    endfunction(generate_ts_file)
    # 创建生成的文件夹
    set(OUTPUT_TS_PATH "${CMAKE_BINARY_DIR}/i18n/language_ts/")
    file(MAKE_DIRECTORY ${OUTPUT_TS_PATH})
    # 遍历生成ts文件
    foreach(LANG ${LANG_LIST})
        set(OUTPUT_TS "${OUTPUT_TS_PATH}/${PROJECT_NAME}_${LANG}.ts")
        generate_ts_file(${LANG} ${OUTPUT_TS})
    endforeach(LANG)

    # 合并ts
    add_custom_target(merge_ts ALL)
    # 定义合并ts文件的命令
    function(merge_ts_file outputTs lang)
        # 调用Qt自带的lconvert.exe进行合并, ARGN代表传入函数的声明的参数之后的所有参数
        add_custom_command(TARGET merge_ts
            COMMAND Qt${QT_VERSION_MAJOR}::lconvert
            -target-language ${lang}
            -i ${ARGN}
            -o ${outputTs}
        )
    endfunction(merge_ts_file)
    # 创建生成的文件夹
    set(BEFORE_MERGE_TS_PATH "${CMAKE_BINARY_DIR}/i18n/before_merge_ts/")
    file(MAKE_DIRECTORY ${BEFORE_MERGE_TS_PATH})
    set(AFTER_MERGE_TS_PATH "${CMAKE_BINARY_DIR}/i18n/after_merge_ts/")
    file(MAKE_DIRECTORY ${AFTER_MERGE_TS_PATH})
    # 拷贝其他需要合并的ts文件
    file(GLOB needCopyTsFiles
        "${OUTPUT_TS_PATH}*.ts"
        "${Sn3DCommon_BIN_DIR}/language_ts/*.ts"
        "${FunctionLayer_BIN_DIR}/language_ts/*.ts"
    )
    file(COPY ${needCopyTsFiles} DESTINATION ${BEFORE_MERGE_TS_PATH})

    # 合并ts文件
    foreach(LANG ${LANG_LIST})
        file(GLOB NEED_MERGE "${BEFORE_MERGE_TS_PATH}*${LANG}.ts")
        set(MERGED_TS_FILE "${AFTER_MERGE_TS_PATH}Language_loka_${LANG}.ts")
        if(NEED_MERGE)
            merge_ts_file(${MERGED_TS_FILE} ${LANG} ${NEED_MERGE})
        endif()
    endforeach(LANG)
endif()
# 生成qm
#add_custom_target(generate_qm ALL)
# 定义生成qm的命令
#function(generate_qm_file tsFile qmFile)
#    add_custom_command(TARGET generate_qm
#        COMMAND Qt${QT_VERSION_MAJOR}::lrelease
#        ${tsFile}
#        -qm ${qmFile}
#    )
#endfunction(generate_qm_file)
# 设置输入输出路径
#set(FINAL_TS_PATH "${CMAKE_SOURCE_DIR}/i18n/final_ts/")
#set(FINAL_QM_PATH "${CMAKE_SOURCE_DIR}/FreeScanApp/src/final_qm/")
#set(FINAL_PREFIX "Language_loka_")
# 生成qm文件
#foreach(LANG ${LANG_LIST})
#    set(TS_FILE "${FINAL_TS_PATH}${FINAL_PREFIX}${LANG}.ts")
#    set(QM_FILE "${FINAL_QM_PATH}${FINAL_PREFIX}${LANG}.qm")
#    generate_qm_file(${TS_FILE} ${QM_FILE})
#endforeach(LANG)
# 设置install目录
#file(GLOB QM_FILES "${FINAL_QM_PATH}*.qm")
#install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/language_qm)


#从common获取数据库密码
#file(READ ${Sn3DCommon_INCLUDE_DIR}/Sn3DSqlitePassword.prop PSW_FILE_CONTENT)
#string(REGEX MATCH "(^|\n)[^\n]*" PSW_FILE_LINES "${PSW_FILE_CONTENT}")
#list(GET PSW_FILE_LINES 0 SQL_PSW)

# file(GLOB XML_PATH_LIST "${CMAKE_CURRENT_SOURCE_DIR}/FreeScanApp/Resource/res/*.xml") 
# foreach(XML_FILE ${XML_PATH_LIST}) 
# 	get_filename_component(BASE_NAME "${XML_FILE}" NAME_WE)
# 	set(BIN_PATH "${CMAKE_INSTALL_PREFIX}/bin")
# 	set(CONFIG_EXE "${BIN_PATH}/ConfigEditor.exe")
# 	install(CODE "
# 	execute_process(COMMAND ${CONFIG_EXE} -g ${XML_FILE} -p ${SQL_PSW} -d ${BIN_PATH}/res/)
# 	configure_file(${BIN_PATH}/res/${BASE_NAME}.db ${BIN_PATH}/res/default_${BASE_NAME}.db COPYONLY)
# 	message(\"database ${BASE_NAME} generated\")
# 	")
# endforeach()

# install(FILES ${_glog_install_prefix}/bin/glog.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)