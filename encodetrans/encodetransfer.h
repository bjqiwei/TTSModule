#ifndef HEADER_ENCODETRANSFER_H
#define HEADER_ENCODETRANSFER_H
//#include "libiconv2.h"
#include "iconv.h"


char* charset_table[]=
{
	{"GB2312"},         //0 
	{"UTF-8"},          //1 
	{"GB18030"},        //2 
	{"GBK"},            //3 
	{"UTF-16"},         //4 
	{"UTF-32"},         //5 
	{"ASCII"},          //6 
	{"ISO-8859-1"},     //7 
	{"ISO-8859-2"},     //8 
	{"ISO-8859-3"},     //9 
	{"ISO-8859-4"},     //10
	{"ISO-8859-5"},     //11
	{"ISO-8859-7"},     //12
	{"ISO-8859-9"},     //13
	{"ISO-8859-10"},    //14
	{"ISO-8859-13"},    //15
	{"ISO-8859-14"},    //16
	{"ISO-8859-15"},    //17
	{"ISO-8859-16"},    //18
	{"EUC-CN"},         //19
	{"HZ"},             //20
	{"EUC-TW"},         //21
	{"BIG5"},           //22
	{"CP950"},          //23
	{"BIG5-HKSCS"},     //24
	{"ISO-2022-CN"},    //25
	{"ISO-2022-CN-EXT"},//26
	{"UCS-2"},          //27
	{"UCS-2BE"},        //28
	{"UCS-2LE"},        //29
	{"UCS-4"},          //30
	{"UCS-4BE"},        //31
	{"UCS-4LE"},        //32
	{"UTF-16BE"},       //33
	{"UTF-16LE"},       //34
	{"UTF-32BE"},       //35
	{"UTF-32LE"},       //36
	{"UTF-7"},          //37
	{"C99"},            //38
	{"JAVA"},           //39
};

#ifdef  __cplusplus
extern "C" {
#endif

int EncodeTrans(const char* inbuf,int unsigned inlen, char* outbuf,unsigned int& outlen, const char* fromencode="UTF-8", const char* toencode="GB2312")
{
	int ret;
	iconv_t cd;
//	cd = libiconv_open(toencode,fromencode); 
	cd = iconv_open(toencode,fromencode); 
//	outlen = 1000;//3*inlen+1;

	if(cd==NULL)
		return 1;
//	ret = libiconv(cd,&inbuf,&inlen,&outbuf,&outlen);
	//size_t iconv (iconv_t cd, const char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft)
	ret = iconv(cd,&inbuf,&inlen,&outbuf,&outlen);
	int i = errno;
//	libiconv_close(cd);
	iconv_close(cd);
	return ret;
	
}





#ifdef  __cplusplus
}
#endif


#endif