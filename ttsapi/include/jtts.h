#ifndef _JTTS_H_
#define _JTTS_H_

#pragma pack( push, jtts_h )
#pragma pack( 8 )

#ifdef __cplusplus
extern "C"
{
#endif

#define TTSAPI __stdcall
/*
函数一览: 

	jTTSPreInit			初始化之前设置系统将读取的音库
	jTTSInit			初始化jTTS核心，读取音库
	jTTSEnd				释放jTTS核心占用内存
	jTTSSetLogFile		设置系统的记录日志文件

	jTTSSetPlay			确定放音设备和放音过程中的回调机制
	jTTSPlay			利用放音设备阅读指定的文本
	jTTSStop			中止当前的阅读
	jTTSPause			暂停当前的阅读
	jTTSResume			恢复当前的阅读
	jTTSSetParam		设置参数
	jTTSGetParam		得到参数
	jTTSSetRemoteParam	设置远程合成参数
	jTTSGetRemoteParam	得到远程合成参数
	jTTSSet2			通过结构的方式一次性设置语音合成的所有参数
	jTTSGet2			通过结构的方式一次性获得语音合成的所有参数
	jTTSSetRemote		通过结构的方式一次性设置远程语音合成的所有参数
	jTTSGetRemote		通过结构的方式一次性获得远程语音合成的所有参数
	jTTSPlayToFileEx	对文本进行语音合成，将结果写入语音文件，并且使用传入的合成参数，
	                    使用新的回调函数，可以回传用户指定的数据

	jTTSSessionStartEx	开始直接得到语音流的语音合成过程
	jTTSSessionGetDataEx	进行语音合成，直接得到语音流，使用内部缓冲区
	jTTSSessionRemoteConfig	设置远程调用时的参数
	jTTSSessionStop			中止直接得到语音流的语音合成过程
	jTTSSessionGetReadBytes 获得已合成完毕的字节数

	jTTSSessionStart	开始直接得到语音流的语音合成过程
	jTTSSessionConfig2	设置语音合成过程的参数，使用新的结构
*/

//ERRCODE
typedef enum 
{
	ERR_NONE,			//	0
	ERR_ALREADYINIT,	//	1
	ERR_NOTINIT,		//	2 
	ERR_MEMORY,			//	3
	ERR_INVALIDHWND,	//	4
	ERR_INVALIDFUNC,	//	5
	ERR_OPENLIB,		//	6
	ERR_READLIB,		//	7
	ERR_PLAYING,		//	8
	ERR_DONOTHING,		//	9
	ERR_INVALIDTEXT,	//	10
	ERR_CREATEFILE,		//	11
	ERR_WRITEFILE,		//	12
	ERR_FORMAT,			//	13
	ERR_INVALIDSESSION,	//	14
	ERR_TOOMANYSESSION,	//	15
	ERR_MORETEXT,		//	16
	ERR_CONFIG,			//	17
	ERR_OPENDEVICE,		//	18
	ERR_RESETDEVICE,	//	19
	ERR_PAUSEDEVICE,	//	20
	ERR_RESTARTDEVICE,	//	21
	ERR_STARTTHREAD,	//	22
	ERR_BEGINOLE,		//	23
	ERR_NOTSUPPORT,		//	24
	ERR_SECURITY,		//	25
	ERR_CONVERT,		//	26
	ERR_PARAM,			//	27
	ERR_INPROGRESS,		//  28
	ERR_INITSOCK,		//	29
	ERR_CREATESOCK,		//	30
	ERR_CONNECTSOCK,	//	31
	ERR_TOOMANYCON,		//	32
	ERR_CONREFUSED,		//	33
	ERR_SEND,			//	34
	ERR_RECEIVE,		//	35
	ERR_SERVERSHUTDOWN,	//	36
	ERR_OUTOFTIME,		//	37
	ERR_CONFIGTTS,		//	38
	ERR_SYNTHTEXT,		//	39
	ERR_CONFIGVERSION,	//	40
	ERR_EXPIRED,		//	41
	ERR_NEEDRESTART,	//	42
	ERR_CODEPAGE,		//	43
	ERR_ENGINE,			//	44
	ERR_CREATEEVENT,	//	45
	ERR_PLAYMODE,		//	46
	ERR_OPENFILE,		//	47
	ERR_USERABORT,		//  48
	ERR_LICENSEFULL,	//	49
	ERR_LICENSEFILEINVALID,	//	50
	ERR_LICENSE,		//	51
	ERR_LICENSEEXPIRED, //  52
}ERRCODE;

// Log File Flag
#define LOG_TEXT	0x01	// 是否在日志文件中记录合成的文本
#define LOG_SESSION	0x02	// 是否记录每个Session过程的详细调用情况

#define WM_JTTS_NOTIFY	(WM_USER + 0x4999)

// wParam of WM_JTTS_NOTIFY or JTTSCALLBACKPROC
#define NOTIFY_BEGIN		0	// lParam: not use 
#define NOTIFY_END			1	// lParam: not use
#define NOTIFY_PROGRESS		2	// lParam: bytes of content have been read
#define NOTIFY_PROGRESS2	3	// lParam: end postion of content will be read in next session
#define NOTIFY_MARK			4	// lParam: 标记Mark串的位置和长度, 参考宏GETMARKOFFSET和GETMARKLEN
#define NOTIFY_VISEME		5	// lParam: 标记唇型信息，现版本中未使用

#define STATUS_NOTINIT	0
#define STATUS_READING	1
#define STATUS_PAUSE	2
#define STATUS_IDLE		3


#define VID_MALE1			0
#define VID_FEMALE1			1

//BACKAUDIO
#define BACKAUDIO_NONE			0		// 没有背景音乐
#define BACKAUDIO_MIN			1		// 最小的背景音乐序号
#define BACKAUDIO_MAX			10000	// 最大的背景音乐序号

//VOICESYTLE
#define VOICESTYLE_CADENCE		0		// 抑扬顿挫 jTTS 3.0风格的。
#define VOICESTYLE_FLAT			1		// 平稳庄重

//VOICEBUFSIZE
#define VOICEBUFSIZE_DEFAULT	0		// Default: 128K
#define VOICEBUFSIZE_MIN		16		// 16K
#define VOICEBUFSIZE_MAX		8192	// 8M

//INSERTINFOSIZE
#define INSERTINFOSIZE_DEFAULT	0		// Default: 100
#define INSERTINFOSIZE_MIN		1
#define INSERTINFOSIZE_MAX		1000

// BackAudio Flag
#define BACKAUDIOFLAG_REPEAT	0x01

// BackAudio Volume
#define AUDIOVOLUME_MIN		0
#define AUDIOVOLUME_MAX		100

// Volume, Speed, Pitch
#define VOLUME_MIN	0
#define VOLUME_MAX	9
#define SPEED_MIN	0
#define SPEED_MAX	9
#define PITCH_MIN	0
#define PITCH_MAX	9

// PuncMode
#define PUNC_OFF		0	/* 不读符号，自动判断回车换行是否分隔符*/
#define PUNC_ON			1	/* 读符号，  自动判断回车换行是否分隔符*/
#define PUNC_OFF_RTN	2	/* 不读符号，强制将回车换行作为分隔符*/
#define PUNC_ON_RTN		3	/* 读符号，  强制将回车换行作为分隔符*/


// DigitMode，在此版本中，增加了DIGIT_AUTO_TELEGRAM，无法自动判断的数字串将用数字来读
#define DIGIT_AUTO_NUMBER	0
#define DIGIT_TELEGRAM		1
#define DIGIT_NUMBER		2
#define DIGIT_AUTO_TELEGRAM	3

#define DIGIT_AUTO			DIGIT_AUTO_NUMBER

// EngMode
#define ENG_AUTO			0	/* 自动方式 */
#define ENG_ENGENGINE		1	/* 强制使用英文引擎 */
#define ENG_LETTER			2	/* 强制单字母方式 */
#define ENG_LETTER_PHRASE	3	/* 单字母＋自录音词方式 */

#define ENG_SAPI			1	/* 强制SAPI 与jTTS3.0兼容*/

//支持TAG
#define TAG_AUTO				0x00	// 自动判断
#define TAG_JTTS				0x01	// 仅处理含有jTTS 3.0支持的TAG: \read=\  
#define TAG_SSML				0x02	// 仅处理含有SSML 的TAG: <voice gender="female" />
#define TAG_NONE				0x03	// 没有TAG

// Domain
//在jTTS 4.0专业版中支持多领域
// 
//这里列出的是系统内建的领域定义，需要安装相应音库的资源包才能真正支持。
//
//对于这里没有列出的领域，将来也可能会发布相应的资源包，同时会分配一个数值，
//只要安装此资源包后，就可以使用。对于没有列出的领域，如果想使用，可以直接使用数值
//
//可以通过Domain系列函数得到所有系统中定义的（包括将来扩展的）领域数值及其描述的信息

//为了支持Domain，需要使用JTTSCONFIG4。

#define DOMAIN_COMMON			0		// 通用领域，新闻
#define DOMAIN_FINANCE			1		// 金融证券
#define DOMAIN_WEATHER			2		// 天气预报
#define DOMAIN_SPORTS			3		// 体育赛事
#define DOMAIN_TRAFFIC			4		// 公交信息
#define DOMAIN_TRAVEL			5		// 旅游餐饮
#define DOMAIN_CARBORNE			6		//汽运
#define DOMAIN_QUEUE			7		//排队
#define DOMAIN_SONG				8		//点歌
#define DOMAIN_INSURANCE		9		//保险
#define DOMAIN_VOYAGE			10		//航空
#define DOMAIN_REVENUE			11		//税务
#define DOMAIN_ELECPOWER		12		//电力
#define DOMAIN_MESSAGE			13		//短信

#define DOMAIN_CUSTOM			30		// 一般定制领域

#define DOMAIN_MIN				0
#define DOMAIN_MAX				31

// CodePage，在此版本中，CP_GB2312和CP_GBK等效  
// CP_xxx 只用于兼容3.0，新的程序应该使用CODEPAGE_xxx等值
#define CP_GB2312		0	
#define CP_GBK			1	
#define CP_BIG5			2
#define CP_UNICODE		3

//支持的CODEPAGE 4.0 的新的标准
#define CODEPAGE_GB		        936		// 包括GB18030, GBK, GB2312
#define CODEPAGE_BIG5		    950
#define CODEPAGE_SHIFTJIS	    932
#define CODEPAGE_KCS			949
#define CODEPAGE_ISO8859_1		1252
#define CODEPAGE_UNICODE		1200
#define CODEPAGE_UNICODE_BIGE	1201	// BIG Endian
#define CODEPAGE_UTF8 			65001

typedef enum 
{
	PARAM_CODEPAGE,			// CP_xxx
	PARAM_VOICEID,			// VID_xxx
	PARAM_PITCH,			// PITCH_MIN - PITCH_MAX
	PARAM_VOLUME,			// VOLUME_MIN - VOLUME_MAX
	PARAM_SPEED,			// SPEED_MIN - SPEED_MAX
	PARAM_PUNCMODE,			// PUNCMODE_xxx
	PARAM_DIGITMODE,		// DIGITMODE_xxx
	PARAM_ENGMODE,			// ENGMODE_xxx
	PARAM_TAGMODE,			// TAG_xxx
	PARAM_DOMAIN,		    // DOMAIN_xxx
	PARAM_VOICESTYLE,		// VOICESTYLE_xxx
	PARAM_BACKAUDIO,		// 0: no backaudio, from 1: backaudio order
	PARAM_BACKAUDIOVOLUME,	// BACKAUDIOVOLUME_MIN - BACKAUDIOVOLUME_MAX
	PARAM_BACKAUDIOFLAG,	// BACKAUDIOFLAG_xxx
	PARAM_VOICEBUFSIZE,		// VOICEBUFSIZE_DEFAULT, or n (K)
	PARAM_INSERTINFOSIZE,	// INSERTINFOSIZE_DEFAULT, or n
}JTTSPARAM;

typedef enum 
{
	PARAM_REMOTE_TRYTIMES,			// 合法值：1 - 100, 缺省值：10
	PARAM_REMOTE_LOADBALANCE,		// 合法值：TRUE, FALSE, 缺省值：FALSE
}JTTSREMOTEPARAM;

#define FORMAT_WAV			0	// PCM Native (目前为16KHz, 16Bit)
#define FORMAT_VOX_6K		1	// OKI ADPCM, 6KHz, 4bit (Dialogic Vox)
#define FORMAT_VOX_8K		2	// OKI ADPCM, 8KHz, 4bit (Dialogic Vox)
#define FORMAT_ALAW_8K		3	// A律, 8KHz, 8Bit
#define FORMAT_uLAW_8K		4	// u律, 8KHz, 8Bit
#define FORMAT_WAV_8K8B		5	// PCM, 8KHz, 8Bit
#define FORMAT_WAV_8K16B	6	// PCM, 8KHz, 16Bit
#define FORMAT_WAV_16K8B	7	// PCM, 16KHz, 8Bit
#define FORMAT_WAV_16K16B	8	// PCM, 16KHz, 16Bit
#define FORMAT_WAV_11K8B	9	// PCM, 11.025KHz, 8Bit
#define FORMAT_WAV_11K16B	10	// PCM, 11.025KHz, 16Bit
#define FORMAT_MP3			11	// PCM, 40KBit/s

#define FORMAT_FIRST		0
#define FORMAT_LAST			11

//jTTSPlayToFileEx函数中参数dwFlag设置值定义
#define PLAYTOFILE_DEFAULT	0	//默认值,写文件时只增加FORMAT_WAV_...格式的文件头
#define PLAYTOFILE_NOHEAD	1	//所有的格式都不增加文件头
#define PLAYTOFILE_ADDHEAD	2	//增加FORMAT_WAV_...格式和FORMAT_ALAW_8K,FORMAT_uLAW_8K格式的文件头

//JTTSCONFIG2 主要用于与jTTS3.0兼容，但不支持jTTS4.0的新功能，
//如果需要jTTS4.0的新功能，请使用JTTSCONFIG4结构。
#define JTTS_VERSION	0x0002	// version 2.0 

typedef struct 
{
	WORD wVersion;
	UINT nCodePage;
	UINT nVoiceID;
	int nPitch;
	int nVolume;
	int nSpeed;
	int nPuncMode;
	int nDigitMode;
	int nEngMode;
}JTTSCONFIG2;

//新的JTTSCONFIG4结构用于jTTS 4.0专业版，
//所有需要传入JTTSCONFIG2指针的情况，都可以使用JTTSCONFIG4指针代替。
#define JTTS_VERSION4		0x0004	//用于jTTS 4.0专业版
typedef struct JTTSCONFIG_4
{
	WORD	wVersion;				// JTTS_VERSION4
	UINT 	nCodePage;
	UINT 	nVoiceID;
	short	nDomain;	
	short	nPitch;
	short	nVolume;
	short	nSpeed;
	short	nPuncMode;
	short	nDigitMode;
	short	nEngMode;
	short	nTagMode;
	short	nVoiceStyle;
	short	nBackAudio;
	short	nBackAudioVolume;
	WORD	wBackAudioFlag;
	short	nVoiceBufSize;		// 设置内部缓冲区的大小
	short	nInsertInfoSize;	// 缓冲区中插入信息的个数
	short	nReserved[6];		// 保留
}
JTTSCONFIG4;


typedef struct 
{
	WORD wVersion;				// 应该使用JTTS_VERSION
	int nTryTimes;
	BOOL bLoadBalance;
}JTTSREMOTECONFIG;

typedef BOOL (* JTTSCALLBACKPROC)(WORD wParam, LONG lParam);
typedef BOOL (* JTTSCALLBACKPROCEX)(WORD wParam, LONG lParam, DWORD dwUserData);

//------------------------------------------------------------------------
// 系统函数

ERRCODE TTSAPI jTTSSetSerialNo(const char * pszSerialNo);
ERRCODE TTSAPI jTTSPreInit(int * arrayVoiceID, int nCount);
ERRCODE TTSAPI jTTSInit(const char * pcszLibPath);
ERRCODE TTSAPI jTTSEnd();
void    TTSAPI jTTSSetLogFile(const char* pcszLogFile, DWORD dwLogFlag);

//------------------------------------------------------------------------
// 播放函数 

// 标准版本中仅支持uDeviceID == WAVE_MAPPER(-1), 即缺省放音音设备
ERRCODE TTSAPI jTTSSetPlay(UINT uDeviceID, HWND hwnd, JTTSCALLBACKPROC lpfnCallback);
ERRCODE TTSAPI jTTSPlay(const char * pcszText);
ERRCODE TTSAPI jTTSStop();
ERRCODE TTSAPI jTTSPause();
ERRCODE TTSAPI jTTSResume();
int		TTSAPI jTTSGetStatus();

//------------------------------------------------------------------------
// 播放到文件函数 

// 标准版本中仅支持nFormat = FORMAT_WAV, 即写成WAV文件格式
// pConfig == NULL时，使用系统的Config
// dwFlag, 为 PLAYTOFILE_xxxx，现主要用于标识是否写文件头
ERRCODE TTSAPI jTTSPlayToFileEx(const char *pcszText, const char * pcszFileName, UINT nFormat, const JTTSCONFIG2 * pConfig, 
						 DWORD dwFlag, JTTSCALLBACKPROCEX lpfnCallbackEx, DWORD dwUserData);

//------------------------------------------------------------------------
// 设置函数 
ERRCODE TTSAPI jTTSSet2(const JTTSCONFIG2 * pConfig);
ERRCODE TTSAPI jTTSGet2(JTTSCONFIG2 * pConfig);
ERRCODE TTSAPI jTTSSetParam(JTTSPARAM nParam, DWORD dwValue);
ERRCODE TTSAPI jTTSGetParam(JTTSPARAM nParam, DWORD *pdwValue);

//-------------------------------------------------------------
// 远程合成参数设置
ERRCODE TTSAPI jTTSSetRemote(const JTTSREMOTECONFIG * pConfig);
ERRCODE TTSAPI jTTSGetRemote(JTTSREMOTECONFIG * pConfig);
ERRCODE TTSAPI jTTSSetRemoteParam(JTTSREMOTEPARAM nParam, DWORD dwValue);
ERRCODE TTSAPI jTTSGetRemoteParam(JTTSREMOTEPARAM nParam, DWORD* pdwValue);

//------------------------------------------------------------------------
// 合成过程底层函数 

ERRCODE TTSAPI jTTSSessionStartEx(const char * pcszText, DWORD * pdwSessionID, UINT nFormat, const JTTSCONFIG2 *pConfig, 
								  int * pnBitsPerSample, int * pnSamplesPerSec);
ERRCODE TTSAPI jTTSSessionRemoteConfig(DWORD dwSessionID, const JTTSREMOTECONFIG *pConfig);
ERRCODE TTSAPI jTTSSessionGetDataEx(DWORD dwSessionID, int nIndex, unsigned char ** ppVoiceData, DWORD * pdwLen, int nReserveLen);
ERRCODE TTSAPI jTTSSessionStop(DWORD dwSessionID);
ERRCODE TTSAPI jTTSSessionGetReadBytes(DWORD dwSessionID, DWORD * pdwBytes);

//负载均衡加密验证用
ERRCODE TTSAPI jTTSLoadBalanceInit(char* pszIPnPort, BOOL bMainServer);
ERRCODE TTSAPI jTTSLoadBalanceEnd();

ERRCODE TTSAPI jTTSSessionStart(const char * pcszText, DWORD * pdwSessionID);
ERRCODE TTSAPI jTTSSessionConfig2(DWORD dwSessionID, const JTTSCONFIG2 *pConfig, int nFormat, int * pnBitsPerSample, int * pnSamplesPerSec);
#ifdef __cplusplus
}
#endif

#pragma pack( pop, jtts_h )

#endif	// _JTTS_H_
