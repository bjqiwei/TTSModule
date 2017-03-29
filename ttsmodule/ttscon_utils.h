/** 
 * @file	ttscon_utils.h
 * @brief	some useful fuctions in ttscon
 * 
 * detail...
 * 
 * @author	lygao
 * @version	1.0
 * @date	2005-11-16
 * 
 * @see		
 * 
 * <b>History:</b><br>
 * <table>
 *  <tr> <th>Version	<th>Date		<th>Author	<th>Notes</tr>
 *  <tr> <td>1.0		<td>2005-11-16	<td>lygao	<td>Create this file</tr>
 * </table>
 * 
 */

#ifndef __TTS_CONSOLE_UTILS_H
#define __TTS_CONSOLE_UTILS_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <iostream>
#include <log4cplus/logger.h>

#ifndef WIN32 
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
typedef int	BOOL;

#else
#include <windows.h>
#define	PATH_MAX	_MAX_PATH
#endif

#ifndef TTSCON_INVALID_HANDLE
#define TTSCON_INVALID_HANDLE	0
#endif

#ifndef INFINITE
#  define INFINITE			0xFFFFFFFF
#endif

#ifndef  TTSCON_INVALID_FUNC_ADDR
#define  TTSCON_INVALID_FUNC_ADDR 0
#endif

#ifndef  TTSCON_INVALID_MUTEX
#define  TTSCON_INVALID_MUTEX 0
#endif

#ifdef  UNICODE //UNICODE

#ifndef __TEXT
#define __TEXT(quote) L##quote
#endif 

#else// UNICODE

#ifndef __TEXT
#define __TEXT(quote) quote
#endif 

#endif// UNICODE

#ifndef TEXT
#define TEXT(quote) __TEXT(quote)
#endif

#if !defined (RTLD_LAZY)
#define RTLD_LAZY 1
#endif /* !RTLD_LAZY */

typedef unsigned long	 TTSCON_Time_Value;

#ifndef  WIN32
typedef pthread_mutex_t* TTSCON_Mutex_t;//Added by lygao, 2005-11-16 17:25:21
#else
typedef void*			 TTSCON_Mutex_t;
#endif

/** 
* @class	class TTSCON_Dll_Handle
* 
* @brief	load or unload library
* 
* detail...
* 
* @author	lygao
* @date	2005-11-16
* 
* @see		
* 
* @par ±¸×¢£º
* 
*/
class TTSCON_Dll_Handle
{
public:
	TTSCON_Dll_Handle(void);

	~TTSCON_Dll_Handle(void);

	int open( const char* dll_name, 
			  void* handle = TTSCON_INVALID_HANDLE, 
			  int open_mode = RTLD_LAZY );

	int close(int unload = 0);

	void* get_handle (int become_owner = 0);

	void* get_func_addr (const char* symbol);

	void set_log(log4cplus::Logger &log);
private:
	// name of the shared library
	char* dll_name_;

	//handle to the actual library loaded by the OS
	void* handle_;

	// Keep track of how many TTSCON_DLL objects have a reference to this
	// dll.
	int	refcount_;
	log4cplus::Logger log;
};

/** 
* @class	class TTSCON_Mutex
* 
* @brief	
* 
* detail...
* 
* @author	lygao
* @date	2005-11-17
* 
* @see		
* 
* @par ±¸×¢£º
* 
*/
class TTSCON_Mutex
{
public:
	TTSCON_Mutex();
	~TTSCON_Mutex();
	int creat (void);
	int remove (void);
	int acquire (TTSCON_Time_Value tv = 0);
	int release (void);

public:	
	TTSCON_Mutex_t lock_;// Mutex type supported by the OS.

};


#endif	// __TTS_CONSOLE_UTILS_H
