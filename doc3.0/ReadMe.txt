һ����װffmpeg
	��װffmpeg��Ϊ�˽�wav��ʽ����ת����mp3��ʽ�������ʹ�ô˹��ܲ���Ҫ��װ��
	��ѹ��װlame-3.99.5.tar.gz��
	��ѹffmpeg-2.5.3.tar.gz, ffmpegҪʹ��lame�⣬����configure������� --enable-libmp3lame
	��װffmpeg��
�������г���
1���ƴ�Ѷ�ɵ�TTS������Ҫ���ܹ��������У����Դ˳���ֻ�����м��ܹ��Ļ��������С�

2��������ɺ��ִ���ļ���./work/binĿ¼�£�tts.exe��release�汾��ttsD.exe��Debug�汾��

3�����з�ʽ ./tts.exe start��ֹͣ��ʽ ./tts.exe stop��./tts.exe loadconfig ���¼���AllowIp���á�

4����־���� ��־�������ļ��� ./work/etc/log4cplus.properties���޸������ļ����������¶�ȡ���ã�����Ҫ�����Ϳ���Ч���޸���־�ļ���·��../log/log4tts.log���˲����ǳ������־�ļ�д��Ŀ�꣬ע����־�Ĵ��������ᴴ��Ŀ¼����Ҫ��Ӧ��Ŀ¼���ڲ�����дȨ�޲���������־�ļ���������������Ҫ�޸ģ�����Ҫ�޸���ο�log4cplus�Ĺٷ�˵����

5��tts ���� tts�������ļ��� ./work/etc/config.xml��System�����tts����http��Ϣ�ļ�����ַ�Ͷ˿ڣ�¼���ļ����·����ip���ƶ�������IP,һ����0.0.0.0�����������������������ж������ֻ���������һ����ַ����Ҫд���Ӧ��IP��port����Ҫ�����Ķ˿ڣ�FileServerPath��¼���ļ�������һ���Ĺ��򴴽��ڴ�Ŀ¼�£���Ŀ¼������д��Ȩ�ޣ����Ŀ¼�����ڳ�����Զ�����Ŀ¼��
TTS�����TTSģ����߳���������Ȩ�š��ı����룬LicenseNumber����Ȩ��������ʾTTS����֧�ֶ���·�����������������Ӧ��TTSת���߳�������SerialNumber����Ȩ���к�,Code:ָ��Ҫ�������ı����룬0:�Զ��ж�,1:GB2312,2:GBK,3:BIG5,4:UNICODE,5:GB18030,6:UTF-8��
HttpFileServer�tts������ṩ�Ժϳ������ļ���http��������أ���ģ�����������صĶ˿ں͸�Ŀ¼��enable:�Ƿ����������http���ع��ܣ������������Ҫ���ⲿ��http����ģ�飬0Ϊ��ʹ���ã���0Ϊʹ�á�ip��ָ��������IP,һ����0.0.0.0�����������������������ж������ֻ���������һ����ַ����Ҫд���Ӧ��IP��port����Ҫ�����Ķ˿ڡ�documentroot���ĵ��ĸ�Ŀ¼��һ���FileServerPath��ͬ��
AllowIp��ڴ������е�ip��ַ������ʹ��TTS,ip:����ʹ�õ�ip,���һ�����ڴ������е�ip����tts��TTS������֤ʧ����Ϣ��

����Http��Ϣ
��Ϣʾ����
1. TTSģ��ӿ�
1.1. �ڲ��ӿ�
1.1.1. ת���ı��ӿ�(CM->TM)
1������URL����POST /txttransfer HTTP/1.1
2�����ԣ�
������ ����˵�� ����ֵ
serviceid ��ƽ̨�ϵ�ҵ��Ψһ��ʶ�� ���֡��ַ���
sessionid �����sessionid��callid_socketid �ַ���
txt ת�����ı����ݡ� �ı�
vid �����˱��,����ֻ֧��2:Ů����4:����
speed �����ٶȣ�-500��+500��0Ϊԭ�٣���ֵ�������ٿ죬��Ӧ��0.5-1.5�����Ե�����ϵ��
volume ����-20~+20 ��0Ϊȱʡ������
pitch ����-500 -- +500 0Ϊԭ������ֵ���������ߣ���Ӧ��0.5-1.5�����Ե�����ϵ��
bgsound ��������ţ�ָ����Χ�ڡ���Ŀǰ��ȡ��ֵ��0��1
audiofmt �������룬Ŀǰ��ȡ��ֵ��0,28,30��0:Ĭ�ϸ�ʽ,28:8kHz������mp3��ʽ��30:16kHz������mp3��ʽ������ֵ��Ĭ�Ϸ�ʽ����
resptype ������ط�ʽ����0������Ӧ�з��ء�1���µ����󷴻ؽ���� 0��1��
respurl �ظ�����ҵ��url ���url
respip �ظ������ip��ַ ip
respport �ظ�����Ķ˿� �˿�
3������ʾ��
��������

<Request>
<serviceid>4001234561</serviceid>
<sessionid>0001000000000009</sessionid>
<txt>���ã���������ͨѶ�ı�ת��������</txt>
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
��Ӧ��

<Response>
����<RespPlaytts>
<statuscode>000000</statuscode> //0�ɹ� ��0ʧ��
<sessionid>0001000000000009</sessionid>
<file> http://192.168.110.8:8080/4001234561/0001000000000009_130312135722.wav</file>
</RespPlayts>
</Response>

2.1.2. �����ı������ӿ�(TM->CM)
1������URL��POST http:<ip>:<port>/<respurl>?sessionid=&file= HTTP/1.1
2�����ԣ�
������ ˵��
sessionid ����sessionid
file http://192.168.110.8:8080/4001234561/0001000000000009_9584858484.wav

file��ʽ: http://ip:port/serviceid/sessionid_yyyyMMddhhmmss.wav

3�����壺��