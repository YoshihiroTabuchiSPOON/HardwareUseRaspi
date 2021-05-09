#ifndef _OUTPUTLOG_H_
#define _OUTPUTLOG_H_

#include <pthread.h>

namespace SPCommon
{
	void OutputLog( const char * pcszLogMessage );

	class COutputLog
	{
	public:
		COutputLog();
		virtual ~COutputLog();

		static COutputLog & Inst();

		void OutputLog( const char * pcszLogMessage );

	protected:
		static COutputLog ms_instance;
		pthread_mutex_t m_mutex;
	};
};

#endif
