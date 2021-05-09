#include <stdio.h>
#include <time.h>

#include "OutputLog.h"

using namespace SPCommon;

COutputLog COutputLog::ms_instance;

void SPCommon::OutputLog( const char * pcszLogMessage )
{
	COutputLog::Inst().OutputLog( pcszLogMessage );
}

COutputLog & COutputLog::Inst()
{
	return COutputLog::ms_instance;
}

///////////////////////////////////////////////////////////////////////////////

COutputLog::COutputLog()
{
	pthread_mutex_init( &this->m_mutex, NULL );
}

COutputLog::~COutputLog()
{
	pthread_mutex_destroy( &this->m_mutex );
}

void COutputLog::OutputLog( const char * pcszLogMessage )
{
	time_t timeNow = time(NULL);
	struct tm tmNow;
	localtime_r( &timeNow, &tmNow );

	char szLogFileName[64];
	snprintf( szLogFileName, 64, "%04d%02d%02d.log"
			, tmNow.tm_year+1900, tmNow.tm_mon+1, tmNow.tm_mday );

	pthread_mutex_lock( &this->m_mutex );

	printf( "%04d/%02d/%02d %02d:%02d:%02d\t%s\n"
			, tmNow.tm_year+1900, tmNow.tm_mon+1, tmNow.tm_mday
			, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec
			, pcszLogMessage );fflush( stdout );

	FILE * fp = fopen( szLogFileName, "a" );
	if( fp )
	{
		fprintf( fp, "%04d/%02d/%02d %02d:%02d:%02d\t%s\n"
				, tmNow.tm_year+1900, tmNow.tm_mon+1, tmNow.tm_mday
				, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec
				, pcszLogMessage );
		fclose( fp );
	}

	pthread_mutex_unlock( &this->m_mutex );
}
