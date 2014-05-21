/**********************************************
 *  File: CamStudioFilterHelper.h
 *  Desc: Utility & helper functions
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
/*
 * Name : AddMouse
 * Desc : Add mouse pointer on the captured bitmap
 * In   : hMemDC - Device context
 *		  lpRect - Retangle to capture
 *		  hSrcDc - Source device context
 *		  hwnd   - Handle of the window, NULL is full screen
 * Out  : None
 */
void AddMouse(HDC hMemDC, LPRECT lpRect, HDC hScrDC, HWND hwnd);

//#include <log4cplus/logger.h>
//#include <log4cplus/fileappender.h>
//#include <log4cplus/layout.h>
//#include <log4cplus/ndc.h>
//#include <log4cplus/helpers/loglog.h>
//#include <log4cplus/helpers/property.h>
//#include <log4cplus/loggingmacros.h>
//
//using namespace log4cplus;
//
//class CamStudioFilterLogger
//{
//public:
//	static CamStudioFilterLogger* GetInstance() {
//		if(!m_pInstance)
//			m_pInstance = new CamStudioFilterLogger();
//		return m_pInstance;
//	}
//	
//	CamStudioFilterLogger::~CamStudioFilterLogger() {
//	}
//
//	static CamStudioFilterLogger* m_pInstance;
//
//	// Logging functions
//	//void CamStudioFilterLogger::LogEntry(std::string sMethodName);
////private:
//	CamStudioFilterLogger::CamStudioFilterLogger() : 
//	   m_filePtr(NULL)
//	{
//		Initialize();
//	}
//	//CamStudioFilterLogger(CamStudioFilterLogger const&);
//	//void operator=(CamStudioFilterLogger const&);
//
//	void CamStudioFilterLogger::Initialize()
//	{
//		log4cplus::initialize ();
//		helpers::LogLog::getLogLog()->setInternalDebugging(true);
//		m_filePtr = new RollingFileAppender(LOG4CPLUS_TEXT("CamStudioFilter.log"), 10*1024, 5, false, true);
//
//		m_filePtr->setName(LOG4CPLUS_TEXT("CSCF"));
//		m_filePtr->setLayout( std::auto_ptr<Layout>(new TTCCLayout()) );
//		m_filePtr->getloc();
//		Logger::getRoot().addAppender(SharedAppenderPtr(m_filePtr.get ()));
//		
//		m_rootLogger = Logger::getRoot();
//		m_filterLogger = Logger::getInstance(LOG4CPLUS_TEXT("CamStudioFilter"));
//		m_pinLogger = Logger::getInstance(LOG4CPLUS_TEXT("CamStudioPin"));
//		m_capWindowLogger = Logger::getInstance(LOG4CPLUS_TEXT("CaptureWindow"));
//		m_flashingWindowLogger = Logger::getInstance(LOG4CPLUS_TEXT("FlashingWindow"));
//	}
//	SharedFileAppenderPtr m_filePtr;
//	Logger m_rootLogger;
//	Logger m_filterLogger;
//	Logger m_pinLogger;
//	Logger m_capWindowLogger;
//	Logger m_flashingWindowLogger;
//};