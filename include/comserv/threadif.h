#ifndef XFRAME_THREADIF_H
#define XFRAME_THREADIF_H

#include <sys/syscall.h>
#include <sys/types.h>

//#include "socket.h"

#ifdef WIN32
#  include <BaseTsd.h>
#  include <winbase.h>
#else
#  include <pthread.h>
#endif


//初始化环境变量的线程数据
//这个要在main函数中调用
void threadEnvCreate();
//设置环境变量的线程数据内容
void threadEnvSet(void *p);
//获取环境变量的线程数据内容
void* threadEnvGet();

#include "threadmutex.h"
#include "threadcondition.h"
/**
  @brief A wrapper class to create and spawn a thread.

  It is a base class.
  ThreadIf::thread() is a pure virtual method .

  To use this class, derive from it and override the thread() method.
  To start the thread, call the run() method.  The code in thread() will
  run in a separate thread.

  Call shutdown() from the constructing thread to shut down the
  code.  This will set the bool shutdown_ to true.  The code in
  thread() should react properly to shutdown_ being set, by
  returning.  Call join() to join the code.

Sample:
@code
DerivedThreadIf thread;
thread.run();
// ... do stuff ...
thread.shutdown();
thread.join();
@endcode
*/
class ThreadIf
{
	public:
		ThreadIf();
		ThreadIf(const char* threadName);	//threadName must less 10
		virtual ~ThreadIf();

		// runs the code in thread() .  Returns immediately
		virtual void run();

		// joins to the thread running thread()
		void join();

		// guarantees resources consumed by thread are released when thread terminates
		// after this join can no-longer be used
		void detach();

		// request the thread running thread() to return, by setting  mShutdown
		virtual void shutdown();

		//waits for waitMs, or stops waiting and returns true if shutdown was
		//called
		virtual bool waitForShutdown(int ms) const;

		// returns true if the thread has been asked to shutdown or not running
		bool isShutdown() const;

#ifdef WIN32
		typedef DWORD Id;
		typedef DWORD Pid
#else
			typedef pthread_t Id;
		typedef pid_t Pid;
#endif
		static Id selfId();

#ifdef WIN32
		typedef DWORD TlsKey;
#else
		typedef pthread_key_t TlsKey;
#endif
		typedef void TlsDestructor(void*);

		Pid   getPID() { return mPID;};
		char* getName() { return mThreadName;};
		/** This function follows pthread_key_create() signature */
		static int tlsKeyCreate(TlsKey &key, TlsDestructor *destructor);
		/** This function follows pthread_key_delete() signature */
		static int tlsKeyDelete(TlsKey key);
		/** This function follows pthread_setspecific() signature */
		static int tlsSetValue(TlsKey key, const void *val);
		/** This function follows pthread_getspecific() signature */
		static void *tlsGetValue(TlsKey key);


		/* thread is a virtual method.  Users should derive and define
		   thread() such that it returns when isShutdown() is true.
		   */
		virtual void process() = 0;
		virtual void prethreadstart() {};
		void thread();

		/*** Delay 10 nm during main loop ***********/
		int tosleep();
		int tosleep(long delay);
	protected:
#ifdef WIN32
		HANDLE mThread;
		/// 1088 is the maximum number of TLS slots under Windows.
		/// Refer to http://msdn.microsoft.com/en-us/library/ms686749(VS.85).aspx
		enum {TLS_MAX_KEYS=1088};
	public:
		/// Free data in TLS slots. For internal use only!
		static void tlsDestroyAll();
	protected:
		/// Array of TLS destructors. We have to emulate TLS destructors under Windows.
		static TlsDestructor **mTlsDestructors;
		/// Mutex to protect access to mTlsDestructors
		static Mutex *mTlsDestructorsMutex;
		friend class TlsDestructorInitializer;
#endif
		Id mId;
		Pid mPID;
		char mThreadName[33];

		bool mShutdown;
		mutable Mutex mShutdownMutex;
		mutable Condition mShutdownCondition;

	private:
		// Suppress copying
		ThreadIf(const ThreadIf &);
		const ThreadIf & operator=(const ThreadIf &);
};

#ifdef WIN32
/// Class to initialize TLS destructors array under Windows on program startup.
class TlsDestructorInitializer {
	public:
		TlsDestructorInitializer();
		~TlsDestructorInitializer();
	protected:
		static unsigned int mInstanceCounter;
};
static TlsDestructorInitializer _staticTlsInit;
#endif

#endif

/* ====================================================================
 * The Vovida Software License, Version 1.0
 *
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * ====================================================================
 *
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */
