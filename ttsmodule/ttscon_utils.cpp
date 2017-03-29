/** 
 * @file	ttscon_utils.cpp
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

#include "ttscon_utils.h"
#include <log4cplus/loggingmacros.h>

/** 
* @brief 	TTSCON_Dll_Handle::TTSCON_Dll_Handle
*           Constructor
*  
*  
* @author	lygao
* @date	2005-11-16
* @see		
*/
TTSCON_Dll_Handle::TTSCON_Dll_Handle(void) 
: dll_name_ (0),
  handle_ (TTSCON_INVALID_HANDLE),
  refcount_ (0)
{
	this->log = log4cplus::Logger::getInstance("iFlyTTS.DLL");
}

/** 
* @brief 	TTSCON_Dll_Handle::~TTSCON_Dll_Handle
*           Deconstructor
*  
*  
* @author	lygao
* @date	2005-11-16
* @see		
*/
TTSCON_Dll_Handle::~TTSCON_Dll_Handle(void)
{
}

/** 
 * @brief 	TTSCON_Dll_Handle::open
 *  
 * This method opens and dynamically links <dll_name>.  The default
 * mode is <RTLD_LAZY>, which loads identifier symbols but not the
 * symbols for functions, which are loaded dynamically on-demand.
 * Other supported modes include: <RTLD_NOW>, which performs all
 * necessary relocations when <dll_name> is first loaded and
 * <RTLD_GLOBAL>, which makes symbols available for relocation
 * processing of any other DLLs.Note that <open_mode> not used in
 * Win32
 *  
 * @author	lygao
 * @date	2005-11-16
 * @return	 Returns -1 on failure and 0 on success.
 * @param	const char* dll_name	- [in] Name of the Library
 * @param	void* handle	- [in] 
 * @see		
 */
int 
TTSCON_Dll_Handle::open(const char* dll_name, 
						void* handle ,
						int open_mode)
{
#ifdef _DEBUG
	LOG4CPLUS_DEBUG(log, "TTSCON_DLL_Handle::open");
#endif

	if (this->dll_name_)
	{
		// Once dll_name_ has been set, it can't be changed..
		if (strcmp (this->dll_name_, dll_name) != 0)
		{
			LOG4CPLUS_ERROR(log, "TTSCON_DLL_Handle::open: error tried to reopen "<< this->dll_name_
				<<" with name " << dll_name );
			return -1;
		}
	}
	else
		this->dll_name_ = strdup(dll_name);

	// If it hasn't been loaded yet, go ahead and do that now.
	if (this->handle_ == TTSCON_INVALID_HANDLE)
	{
		if (handle)
			this->handle_ = handle;
		else
		{
#ifdef _DEBUG
			LOG4CPLUS_DEBUG(log,"TTSCON_DLL_Handle::open: calling dlopen on \"" << dll_name << "\"");
#endif

			// The TTSCON_HANDLE object is obtained.
#ifdef WIN32
			this->handle_ = LoadLibrary( dll_name );
#else
			this->handle_ = dlopen ( dll_name, open_mode);		

#endif

			if (this->handle_ == TTSCON_INVALID_HANDLE)
			{				
#ifdef WIN32
			unsigned err = GetLastError(); 
#else				
			const char * err =  dlerror() ;
#endif
				LOG4CPLUS_ERROR(log, "TTSCON_DLL_Handle::open():load library failed with \"" << std::hex << err << "\"");
				return -1;
			}
		}
	}

	++this->refcount_;
	return 0;
}


/** 
* @brief 	TTSCON_Dll_Handle::close
*  
* Call to close the DLL object.  If unload = 0, it only decrements
* the refcount, but if unload = 1, then it will actually unload 
* the library when the refcount == 0; 
*  
* @author	lygao
* @date	2005-11-16
* @return	int	- Return 0 in success, otherwise return error code.
* @param	int unload	- [in] 
* @see		
*/
int 
TTSCON_Dll_Handle::close(int unload)
{
#ifdef _DEBUG	
	LOG4CPLUS_DEBUG(log, "TTSCON_DLL_Handle::close..." );
#endif
	int retval = 0;

	// Since we don't actually unload the dll as soon as the refcount
	// reaches zero, we need to make sure we don't decrement it below
	// zero.
	if (this->refcount_ > 0)
		--this->refcount_;
	else
		this->refcount_ = 0;

	if (this->refcount_ == 0 &&
		this->handle_ != TTSCON_INVALID_HANDLE &&
		unload == 1)
	{
#ifdef _DEBUG	
		LOG4CPLUS_DEBUG(log, "Waiting! TTSCON_DLL_Handle::closing "<< this->dll_name_
		<< ",its handle is:" << this->handle_ );
#endif

#ifdef  WIN32
		retval = FreeLibrary( (HMODULE) handle_ );
		if ( retval != 0 )
		{
			// If FreeLibrary successful,the return value is nonzero.
			// But here we use zero to express successful.
			retval = 0;
		}
#else
		retval = dlclose ( handle_ );
#endif	

		this->handle_ = TTSCON_INVALID_HANDLE;
	}

	if (retval != 0)
	{
		LOG4CPLUS_ERROR(log, "TTSCON_DLL_Handle::close error");
	}
#ifdef _DEBUG
	else
	{
		LOG4CPLUS_DEBUG(log, "TTSCON_DLL_Handle::close successful!");
	}
#endif

	return retval;
}


/** 
* @brief 	TTSCON_Dll_Handle::get_handle
*  
* Return the handle to the caller.  If <become_owner> is non-0 then
* caller assumes ownership of the handle so we decrement the retcount. 
*  
* @author	lygao
* @date	2005-11-16
* @return	the handle to the caller
* @param	int become_owner	- [in] 
* @see		
*/
void* 
TTSCON_Dll_Handle::get_handle (int become_owner)
{
	void* handle = TTSCON_INVALID_HANDLE;

	if (this->refcount_ == 0 && become_owner != 0)
	{
#ifdef  _DEBUG
		LOG4CPLUS_DEBUG(log, "TTSCON_DLL_Handle::get_handle:cannot become owner, refcount == 0.");
#endif
		return TTSCON_INVALID_HANDLE;
	}

	handle = this->handle_;

	if (become_owner != 0)
	{
		if (--this->refcount_ == 0)
			this->handle_ = TTSCON_INVALID_HANDLE;
	}

#ifdef  _DEBUG
	LOG4CPLUS_DEBUG(log, "TTSCON_DLL_Handle::get_handle:post call: handle "
	<< (this->handle_ == TTSCON_INVALID_HANDLE ? ("invalid") : ("valid"))
	<< (", refcount ") << this->refcount_);
#endif
	return handle;
}


/** 
* @brief 	TTSCON_Dll_Handle::get_func_addr
*  
* Return the the address of an exported function in the DLL. 
*  
* @author	lygao
* @date	2005-11-16
* @return	void* the the address of an exported function in the DLL.
* @param	char* symbol	- [in]  Name of the function 
* @see		
*/
void* 
TTSCON_Dll_Handle::get_func_addr ( const char* symbol)
{
	if ( symbol == NULL )
	{	
		LOG4CPLUS_ERROR(log, "Invalid function name");
		return TTSCON_INVALID_FUNC_ADDR;
	}

	if (this->handle_ == TTSCON_INVALID_HANDLE)
	{
#ifdef _DEBUG
		LOG4CPLUS_DEBUG(log, "TTSCON_DLL_Handle::get_func_addr: ungetting " << symbol);
#endif

		return TTSCON_INVALID_FUNC_ADDR;
	}

	void* func_addr = TTSCON_INVALID_FUNC_ADDR;

#ifdef  WIN32
	func_addr = GetProcAddress( (HMODULE)handle_, symbol );
#else
	func_addr = dlsym( handle_, symbol);
#endif	

	return func_addr;
}

void TTSCON_Dll_Handle::set_log(log4cplus::Logger &log)
{
	this->log = log;
}
/** 
* @brief 	TTSCON_Mutex::TTSCON_Mutex
*  
*  
*  
* @author	lygao
* @date	2005-11-17
* @see		
*/
TTSCON_Mutex::TTSCON_Mutex()
:lock_(TTSCON_INVALID_MUTEX)
{
}


/** 
* @brief 	TTSCON_Mutex::~TTSCON_Mutex
*  
*  
*  
* @author	lygao
* @date	2005-11-17
* @see		
*/
TTSCON_Mutex::~TTSCON_Mutex()
{
#ifndef WIN32
	delete lock_;
#endif
}


/** 
* @brief 	TTSCON_Mutex::creat
*  
*  Explicitly creat the mutex.   
*  
* @author	lygao
* @date	2005-11-17
* @return	int	- Return 0 in success, otherwise return error code.
* @see		
*/
int TTSCON_Mutex::creat (void)
{
#ifdef _DEBUG
	std::cout << TEXT("TTSCON_Mutex::creat") << std::endl;
#endif

	// If it hasn't been loaded yet, go ahead and do that now.
	if (this->lock_ == TTSCON_INVALID_MUTEX)
	{
		// The TTSCON_HANDLE object is obtained.
#ifdef WIN32
		this->lock_ = CreateEvent( NULL, FALSE, FALSE, NULL);
#else
		lock_ = new pthread_mutex_t;
		pthread_mutex_init( this->lock_, NULL);
		pthread_mutex_lock( this->lock_);
#endif

		if (this->lock_ == TTSCON_INVALID_MUTEX)
		{
			std::cout << TEXT("TTSCON_Mutex::Invalid mutex,creat() failed!") << std::endl;
			return -1;
		}
	}
	return 0;
}


/** 
* @brief 	TTSCON_Mutex::remove  
*  
* Explicitly destroy the mutex. Note that only one thread should
* call this method since it doesn't protect against race conditions.
*
*  
* @author	lygao
* @date	2005-11-17
* @return	int	- Return 0 in success, otherwise return error code.
* @see		
*/
int TTSCON_Mutex::remove (void)
{
	if ( this->lock_ != TTSCON_INVALID_MUTEX )
	{
#ifdef WIN32
		CloseHandle( this->lock_);
#else
		pthread_mutex_destroy( this->lock_ );
#endif
		return 0;

	}
	std::cout << TEXT( "TTSCON_Mutex::remove() failed!") <<std::endl;
	return -1;
}

/** 
* @brief 	TTSCON_Mutex::acquire
*
* Block the thread until the mutex is acquired or  times out.,  
* If <tv> is zero, the function tests the object's state and 
* returns immediately. If <tv> is INFINITE, the function's
* time-out interval never elapses. Note that <tv> is assumed
* to used in Win32.Default <tv> is zero.
*  
* @author	lygao
* @date	2005-11-17
* @return	int	- Return 0 in success, otherwise return -1.
* @param	TTSCON_Time_Value tv	- [in] Time-out interval, in
*			milliseconds. 
* @see		
*/
int TTSCON_Mutex::acquire (TTSCON_Time_Value tv)
{
	if ( this->lock_ == TTSCON_INVALID_MUTEX )
	{
		std::cout << TEXT(" TTSCON_Mutex::acquire() failed!(Invalid mutex)") <<std::endl;
		return -1;
	}

#ifdef WIN32
	 if( WaitForSingleObject( this->lock_,tv ) == WAIT_TIMEOUT )
	 {
		 std::cout << TEXT(" TTSCON_Mutex::acquire() wait time out!") <<std::endl;
		 return -1;
	 }
#else
	pthread_mutex_lock( this->lock_);
	pthread_mutex_unlock( this->lock_);
#endif

	return 0;

}

/** 
* @brief 	TTSCON_Mutex::release
*  
*  Release lock and unblock a thread
*  
* @author	lygao
* @date	2005-11-17
* @return	int	- Return 0 in success, otherwise return error code.
* @see		
*/
int TTSCON_Mutex::release (void)
{
	if ( this->lock_ == TTSCON_INVALID_MUTEX )
	{
		std::cout << TEXT(" TTSCON_Mutex::release() failed!") <<std::endl;
		return -1;
	}

#ifdef WIN32
	SetEvent( this->lock_);
#else
	pthread_mutex_unlock( this->lock_);
#endif

	return 0;
}



