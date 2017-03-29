一、安装ffmpeg
	安装ffmpeg是为了将wav格式语音转换成mp3格式，如果不使用此功能不需要安装。
	解压安装lame-3.99.5.tar.gz。
	解压ffmpeg-2.5.3.tar.gz, ffmpeg要使用lame库，所以configure参数添加 --enable-libmp3lame
	安装ffmpeg。
二、运行程序
1、科大讯飞的TTS引擎需要加密狗才能运行，所以此程序只能在有加密狗的机子上运行。

2、编译完成后的执行文件在./work/bin目录下，tts.exe是release版本，ttsD.exe是Debug版本。

3、运行方式 ./tts.exe start。停止方式 ./tts.exe stop。./tts.exe loadconfig 重新加载AllowIp配置。

4、日志配置 日志的配置文件是 ./work/etc/log4cplus.properties，修改配置文件后程序会重新读取配置，不需要重启就可生效，修改日志文件的路径../log/log4tts.log，此参数是程序的日志文件写入目标，注意日志的创建并不会创建目录，需要相应的目录存在并且有写权限才能生成日志文件。其他参数不需要修改，如需要修改请参考log4cplus的官方说明。

5、tts 配置 tts的配置文件是 ./work/etc/config.xml。System项：配置tts接收http消息的监听地址和端口，录音文件存放路径，ip：制定监听的IP,一般是0.0.0.0监听多个网卡，如果本机上有多个网卡只想监听其中一个地址，需要写入对应的IP。port：需要监听的端口，FileServerPath：录音文件将按照一定的规则创建在此目录下，此目录必须有写的权限，如果目录不存在程序会自动创建目录。
TTS项：配置TTS模块的线程数量、授权号、文本编码，LicenseNumber：授权数量，表示TTS引擎支持多少路并发，程序会启动对应的TTS转换线程数量。SerialNumber：授权序列号,Code:指定要解析的文本编码，0:自动判断,1:GB2312,2:GBK,3:BIG5,4:UNICODE,5:GB18030,6:UTF-8。
HttpFileServer项：tts程序会提供对合成语音文件的http浏览和下载，此模块是配置下载的端口和根目录。enable:是否启用自身的http下载功能，如果不启用需要另外部署http下载模块，0为不使用用，非0为使用。ip：指定监听的IP,一般是0.0.0.0监听多个网卡，如果本机上有多个网卡只想监听其中一个地址，需要写入对应的IP。port：需要监听的端口。documentroot：文档的根目录，一般和FileServerPath相同。
AllowIp项，在此配置中的ip地址才允许使用TTS,ip:允许使用的ip,如果一个不在此配置中的ip连接tts，TTS返回认证失败信息。

三、Http消息
消息示例：
1. TTS模块接口
1.1. 内部接口
1.1.1. 转换文本接口(CM->TM)
1、请求URL：　POST /txttransfer HTTP/1.1
2、属性：
属性名 属性说明 属性值
serviceid 云平台上的业务唯一标识符 数字、字符串
sessionid 请求的sessionid，callid_socketid 字符串
txt 转换的文本内容。 文本
vid 发音人编号,现在只支持2:女声、4:男声
speed 发音速度，-500～+500，0为原速，数值大则语速快，对应于0.5-1.5倍线性调整关系。
volume 音量-20~+20 （0为缺省音量）
pitch 音调-500 -- +500 0为原调，数值大则音调高，对应于0.5-1.5倍线性调整关系。
bgsound 背景音编号，指定范围内。　目前可取的值有0、1
audiofmt 语音编码，目前可取的值有0,28,30。0:默认格式,28:8kHz单声道mp3格式，30:16kHz单声道mp3格式。其他值按默认方式处理。
resptype 结果反回方式：　0　在响应中反回　1发新的请求反回结果。 0或1　
respurl 回复请求业务url 相对url
respip 回复请求的ip地址 ip
respport 回复请求的端口 端口
3、包体示例
　　请求：

<Request>
<serviceid>4001234561</serviceid>
<sessionid>0001000000000009</sessionid>
<txt>您好，这里是云通讯文本转语音功能</txt>
<vid>2</vid>
<speed>0</speed>
<volume>0</volume>
<pitch>0</pitch>
<resptype>0</resptype>
<bgsound>0</bgsound>
<respurl>playttsfile</respurl>
<respip>192.168.110.8</respip>
<respport>8080</respport>
</Request>
响应：

<Response>
　　<RespPlaytts>
<statuscode>000000</statuscode> //0成功 非0失败
<sessionid>0001000000000009</sessionid>
<file> http://192.168.110.8:8080/4001234561/0001000000000009_130312135722.wav</file>
</RespPlayts>
</Response>

2.1.2. 播放文本语音接口(TM->CM)
1、请求URL：POST http:<ip>:<port>/<respurl>?sessionid=&file= HTTP/1.1
2、属性：
参数名 说明
sessionid 呼叫sessionid
file http://192.168.110.8:8080/4001234561/0001000000000009_9584858484.wav

file格式: http://ip:port/serviceid/sessionid_yyyyMMddhhmmss.wav

3、包体：无