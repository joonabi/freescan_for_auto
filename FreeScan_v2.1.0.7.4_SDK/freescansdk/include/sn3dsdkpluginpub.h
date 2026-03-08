/**
*@file sn3dsdkpluginpub.h
*@brief define const value or enum for Shining3D SDK
*@author xuzhen
*@data 2023/11/29
*/
#if !defined(SNSDK_PLUGIN_PUBLIC)
#define SNSDK_PLUGIN_PUBLIC
// Error Code

// Error Code
#define SNSDK_PUBLIC
#define SNSDK_ERR_OK    									0x00000000  // �ɹ�								No error
#define SNSDK_ERR_NOTINITIALIZED							0x00000001  // û�г�ʼ��						No initialization
#define SNSDK_ERR_INITIALIZEFIAILZED						0x00000002  // ��ʼ��ʧ��						Initialization failed.
#define SNSDK_ERR_AlREADYINITIALIZED						0x00000003  // �ظ���ʼ��						Initialization repeat.
#define SNSDK_ERR_SAVEFAILED_TYPEERROR						0x00000004  // ����ʧ�ܣ���ʽ����				Format error��failed to save files.
#define SNSDK_ERR_CHECKDEVICEFAILED_PLENOTRIGHT				0x00000005  // �����豸ʧ�ܣ�ple����ȷ			PLE error, failed to connect device.
#define SNSDK_ERR_CHECKDEVICEFAILED_NODEVICEFOUND			0x00000006  // �����豸ʧ�ܣ�δ�����豸			Device no found, connection failed.
#define SNSDK_ERR_OPENORCREATSLNFAILED						0x00000007  // ������򿪹���ʧ��				Failed to create or open project.
#define SNSDK_ERR_ENTERSCANFAILED							0x00000008  // ����ɨ��ʧ��				        Failed to enter scan mode.
#define SNSDK_ERR_SCANFAILED								0x00000009  // ɨ����ʧ��					    Scanning failed.
#define SNSDK_ERR_ENDSCANFAILED								0x0000000A  // ����ʧ��					        Generating failed.
#define SNSDK_ERR_MESHFAILED								0x0000000B  // ����ʧ��					    Meshing failed.
#define SNSDK_ERR_EXITSCANFAILED							0x0000000C  // �˳�ɨ��					        Quitting scan mode failed.
#define SNSDK_ERR_CANCELSCANFAILED							0x0000000D  // ȡ��ɨ��					        Canceling scan failed.
#define SNSDK_ERR_SAVEFAILED								0x0000000E  // ����ʧ��					        Saving failed.
#define SNSDK_ERR_CREATNEWPROJECTFAILED						0x00000010  // �½�����ʧ��				        New project failed.
#define SNSDK_ERR_WORKRANGE									0x00000011	//�������뷶Χ������Ҫ��			    The working range dose not meet the requirements
#define SNSDK_ERR_UNIMPLEMENTED                             0x00000012
#define SNSDK_ERR_NULLFUNCTION                              0x00000013
#define SNSDK_ERR_STARTPLUGIN_FAIL                          0x00000014
#define SNSDK_ERR_NULLDATA                                  0x00000015
#define SNSDK_ERR_NULLSERVICE                               0x00000016
#define SNSDK_ERR_OUTOFMEMORY                               0x00000017
#define SNSDK_ERR_UNKNOWN                                   0x00000018
#define SNSDK_ERR_INVALIDINPUT                              0x00000019
#define SNSDK_ERR_TOOFEW_SCANDATA                           0x0000001A  // The scan data lik point cloud is too few
#define SNSDK_ERR_OPTIMIZE_POINT_FAIL                       0x0000001B  // The point cloud optimization is failed
#define SNSDK_ERR_ALIGNMODERROR								0x0000001C   // ƴ��ģʽ����						Process call error
#define SNSDK_ERR_CALIBOARDDISTINGUISHERROR					0x0000001D   // �궨��ʶ�����
#define SNSDK_ERR_CALICALCULATEERROR						0x0000001E   // �궨�������						


#define SNSDK_ERR_NO_GLOBAL_MARKERS							0x0000001D   // ���ǿ�ܵ��ļ�						
#define SNSDK_ERR_TOOMORE_GLOBAL_MARKERS					0x0000001E   // ��ܵ���������������				    Process call error
#define SNSDK_ERR_TOOLESS_GLOBAL_MARKERS					0x0000001F   // ��ܵ�������3��		
#define SNSDK_ERR_TIMEOUT									0x00000020   // �ӿڷ��س�ʱ
#define SNSDK_ERR_RECONNECT									0x00000021   // �����豸ʧ��
#define SNSDK_ERR_PARAMERROR_TURTABLECHECKED				0x00000022	//ת̨��ת̨�����ƴ�� ��Ҫ���Ƿ�ʹ��ת̨����Ϊtrue
#define SNSDK_ERR_MODPARAMERROR								0x00000023	//��ܵ�ƴ�� ɨ���ܵ�ʱ ���ܿ���HDR ����ɨ����
#define SNSDK_ERR_TURTABLEPARAMERROR						0x00000024	//ת̨����(8-180)��ת��(1-10)��Χ����
#define SNSDK_ERR_TURNTALBE_VERIFY_FAIL						0x00000025   // ת̨У��ʧ��
#define SNSDK_ERR_TURNTALBE_NOT_CONNECT						0x00000026   // ת̨δ����
#define SNSDK_ERR_NEED_DELETE_CUR_PRJ_DATA					0x00000027   // ��Ҫɾ����ǰ��������	
#define SNSDK_ERR_PROCESSERROR								0x10000000   // ���̵��ô���					    Process call error
#define SNSDK_ERR_PARAMERROR								0x10000001	//�������ô���

#define SNSDK_ERR_SCAN_NODATA								0x00000030   // û��ɨ���κ�����
#define SNSDK_ERR_TOOLESS_MARKERS							0x00000031   // ��־����������3��


#define SNSDK_ERR_TOOMORE_POINTS							0x00000032   // ѡ��������������������
#define SNSDK_ERR_TOOLESS_POINTS							0x00000033	 // ѡ����������������3��	
#define SNSDK_ERR_SAME_UUID									0x00000034   // �̶���͸�������ͬ

#define SNSDK_ERR_NEED_CONFIRM								0x00000035	 // ��Ҫ�û�ȷ���Ƿ�����ǰƬ
#define SNSDK_ERR_MARKERS_OUT_LIMIT							0x00000036	 //��֡��־�㳬������
#define SNSDK_ERR_PRJ_MARKERS_OUT_LIMIT						0x00000037	 // ���̱�־�������������
#define SNSDK_ERR_MARKER_ALIGN_ERROR						0x00000038	 // ��Ƭ��־��ƴ��ʧ��
#define SNSDK_ERR_FRAMEMARKER_ALIGN_ERROR					0x00000039	 // ��Ƭɨ���ܵ�ʱƴ��ʧ��
#define SNSDK_ERR_TURN_MARKER_ALIGN_ERROR					0x0000003A	 // ��־��ƴ��ת̨ɨ�裬ĳһƬû��ʶ�𵽱�־�㡢���߱�־��ƴ��ʧ��
#define SNSDK_ERR_FRAMEMARKER_PARAM							0x0000003B	 // ��ܵ�ƴ�� ɨ����ƺ�ɨ���־�㲻��ͬ��false
// Device Type
#define SNSDK_DEVTYPE_UE           0x00000000
#define SNSDK_DEVTYPE_UEPRO        0x00000001
#define SNSDK_DEVTYPE_COMBO        0x00000002
#define SNSDK_DEVTYPE_TRAK         0x00000003
#define SNSDK_DEVTYPE_5M           0x00000004
#define SNSDK_DEVTYPE_9M           0x00000005
#define SNSDK_DEVTYPE_E10          0x00000006
#define SNSDK_DEVTYPE_2X           0x00000007
#define SNSDK_DEVTYPE_TRIO         0x00000008
#define SNSDK_DEVTYPE_UEP2         0x00000009
#define SNSDK_DEVTYPE_COMBOPLUS     0x00000010

// Device Status
#define SNSDK_DEVSTAT_ONLINE        0x00000000
#define SNSDK_DEVSTAT_OFFLINE       0x00000001

// Project Flags
#define SNSDK_PROJFLAG_SOLUTION     0x00000000
#define SNSDK_PROJFLAG_PROJECT      0x00000001
// Create solution and project
#define SNSDK_PROJFLAG_SLNPROJ      0x00000002

// Scan Type
#define SNSDK_SCANTYPE_FIXED        0x00000000
#define SNSDK_SCANTYPE_HD           0x00000001
#define SNSDK_SCANTYPE_RAPID        0x00000002

// Scan Mode
#define SNSDK_SCANMODE_LASER        0x00000000
#define SNSDK_SCANMODE_SPECKLE      0x00000001
#define SNSDK_SCANMODE_INFRARED     0x00000002

// Scan Align Mode
#define SNSDK_SCANALIGN_MARKER      0x00000000

// Project Type
#define SNSDK_PROJTYPE_HUMAN         0x00000000
#define SNSDK_PROJTYPE_THING         0x00000001
#define SNSDK_PROJTYPE_SMALLTHING    0x00000002
#define SNSDK_PROJTYPE_FRAME         0x00000000
#define SNSDK_PROJTYPE_POINTCLOUD    0x00000001
#define SNSDK_PROJTYPE_PHOTOGRAPHIC  0x00000002

// Laser Grade
#define SNSDK_LASERGRADE_OFF         0x00000000
#define SNSDK_LASERGRADE_SINGLE      0x00000001
#define SNSDK_LASERGRADE_26          0x0000001A

// Bright Align Mode
#define SNSDK_BRIGHT_A_POINTCLOUD    0x00000000
#define SNSDK_BRIGHT_A_FRAMEWORK     0x00000001
#define SNSDK_BRIGHT_A_PHOTOGRAPHIC  0x00000002

// Scan Object
#define SNSDK_SCANOBJECT_NORMAL      0x00000000
#define SNSDK_SCANOBJECT_REFLECT     0x00000001
#define SNSDK_SCANOBJECT_BLACK       0x00000002

// Calib Mode
#define SNSDK_CALIBMODE_QUICK        0x00000000
#define SNSDK_CALIBMODE_FACTORY      0x00000001

// Scan State
#define SNSDK_PRE_SCAN               0x00000001
#define SNSDK_PRE_SCANNING           0x00000002
#define SNSDK_SCAN                   0x00000003
#define SNSDK_PAUSED                 0x00000004
#define SNSDK_SCAN_STOPED            0x00000005

// Post Process
#define SNSDK_POST_AUTOFILL_CURVATE  0x00000000
#define SNSDK_POST_AUTOFILL_TANGENT  0x00000001
#define SNSDK_POST_AUTOFILL_PLANE    0x00000002

// Manual align state
#define SNSDK_MA_OK = 0x00000001 | 0x39000000, ///<��ʾ����:����
#define SNSDK_MA_POINT_FEATURE = 0x00000002 | 0x39000000,	///<��ʾ����:������ƴ��ʧ��
#define	SNSDK_MA_POINT_MANUAL = 0x00000003 | 0x39000000,	///<��ʾ����:�ֶ�ѡ��ƴ��ʧ��
#define	SNSDK_MA_MARKER_FEATURE = 0x00000004 | 0x39000000,	///<��ʾ����:��־������ƴ��ʧ��
#define	SNSDK_MA_MARKER_MANUAL = 0x00000005 | 0x39000000,	///<��ʾ����:�ֶ���־��ƴ��ʧ��
#define	SNSDK_MA_INPUT_ERROR = 0x00000006 | 0x39000000,	///<��ʾ����:�������
#define	SNSDK_MA_ALGORITHM_ERROR = 0x00000007 | 0x39000000,	///<��ʾ����:�㷨����ʧ��

// End Post Process
#endif