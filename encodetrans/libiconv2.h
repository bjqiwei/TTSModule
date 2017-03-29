
#ifndef __LIBICONV2_H__
#define __LIBICONV2_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma comment(lib,"libiconv2.lib")

/* Identifier for conversion method from one codeset to another.  */
typedef void *iconv_t;

/*创建字符集转换句柄*/
extern iconv_t libiconv_open(const char *tocode, const char *fromcode);

/*字符集转换函数*/
/*返回值：0表示成功，其它表示失败*/
/*inbuf、inbytesleft: 指向转换结束后所停留未转换完的字符开始处*/
/*outbuf、outbytesleft: 指向转换结束后，空闲的位置处*/
extern int libiconv(iconv_t handle, char **inbuf, int *inbytesleft,
             char **outbuf, int *outbytesleft);

/*释放字符集转换句柄*/
extern int libiconv_close (iconv_t handle);


/*****有关code page的说明******
 *European languages:
 *ASCII, ISO-8859-{1,2,3,4,5,7,9,10,13,14,15,16}, KOI8-R, KOI8-U, KOI8-RU, CP{1250,1251,1252,1253,1254,1257}, 
 *CP{850,866,1131}, Mac{Roman,CentralEurope,Iceland,Croatian,Romania}, Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh
 *
 *Japanese:
 *EUC-JP, SHIFT_JIS, CP932, ISO-2022-JP, ISO-2022-JP-2, ISO-2022-JP-1
 *
 *Chinese:
 *EUC-CN, HZ, GBK, CP936, GB18030, EUC-TW, BIG5, CP950, BIG5-HKSCS, BIG5-HKSCS:2001, BIG5-HKSCS:1999, 
 *ISO-2022-CN, ISO-2022-CN-EXT
 *
 *Korean:
 *EUC-KR, CP949, ISO-2022-KR, JOHAB
 *
 *Full Unicode:
 *UTF-8
 *UCS-2, UCS-2BE, UCS-2LE
 *UCS-4, UCS-4BE, UCS-4LE
 *UTF-16, UTF-16BE, UTF-16LE
 *UTF-32, UTF-32BE, UTF-32LE
 *UTF-7
 *C99, JAVA
 *
 *******************************/

#ifdef __cplusplus
}
#endif


#endif /* libiconv2.h */
