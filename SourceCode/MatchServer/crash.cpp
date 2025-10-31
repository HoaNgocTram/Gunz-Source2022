#include "stdafx.h"
#include "crash.h"

#if defined(_WINDOWS)
#include "StackWalker/StackWalker.h"

class EQEmuStackWalker : public StackWalker
{
public:
	EQEmuStackWalker() : StackWalker() { }
	EQEmuStackWalker(DWORD dwProcessId, HANDLE hProcess) : StackWalker(dwProcessId, hProcess) { }
	virtual void OnOutput(LPCSTR szText) {
		char buffer[4096];
		for(int i = 0; i < 4096; ++i) {
			if(szText[i] == 0) {
				buffer[i] = '\0';
				break;
			}

			if(szText[i] == '\n' || szText[i] == '\r') {
				buffer[i] = ' ';
			} else {
				buffer[i] = szText[i];
			}
		}
		mlog(buffer);
		mlog("\n");
		StackWalker::OnOutput(szText);
	}
};

LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS *ExceptionInfo)
{
	switch(ExceptionInfo->ExceptionRecord->ExceptionCode)
	{
		case EXCEPTION_ACCESS_VIOLATION:
			mlog("EXCEPTION_ACCESS_VIOLATION\n");
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			mlog("EXCEPTION_ARRAY_BOUNDS_EXCEEDED\n");
			break;
		case EXCEPTION_BREAKPOINT:
			mlog("EXCEPTION_BREAKPOINT\n");
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			mlog("EXCEPTION_DATATYPE_MISALIGNMENT\n");
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			mlog("EXCEPTION_FLT_DENORMAL_OPERAND\n");
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			mlog("EXCEPTION_FLT_DIVIDE_BY_ZERO\n");
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			mlog("EXCEPTION_FLT_INEXACT_RESULT\n");
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			mlog("EXCEPTION_FLT_INVALID_OPERATION\n");
			break;
		case EXCEPTION_FLT_OVERFLOW:
			mlog("EXCEPTION_FLT_OVERFLOW\n");
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			mlog("EXCEPTION_FLT_STACK_CHECK\n");
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			mlog("EXCEPTION_FLT_UNDERFLOW\n");
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			mlog("EXCEPTION_ILLEGAL_INSTRUCTION\n");
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			mlog("EXCEPTION_IN_PAGE_ERROR\n");
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			mlog("EXCEPTION_INT_DIVIDE_BY_ZERO\n");
			break;
		case EXCEPTION_INT_OVERFLOW:
			mlog("EXCEPTION_INT_OVERFLOW\n");
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			mlog("EXCEPTION_INVALID_DISPOSITION\n");
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			mlog("EXCEPTION_NONCONTINUABLE_EXCEPTION\n");
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			mlog("EXCEPTION_PRIV_INSTRUCTION\n");
			break;
		case EXCEPTION_SINGLE_STEP:
			mlog("EXCEPTION_SINGLE_STEP\n");
			break;
		case EXCEPTION_STACK_OVERFLOW:
			mlog("EXCEPTION_STACK_OVERFLOW\n");
			break;
		default:
			mlog("Unknown Exception\n");
			break;
	}

	if(EXCEPTION_STACK_OVERFLOW != ExceptionInfo->ExceptionRecord->ExceptionCode)
	{
		EQEmuStackWalker sw; sw.ShowCallstack(GetCurrentThread(), ExceptionInfo->ContextRecord);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void set_exception_handler() {
	SetUnhandledExceptionFilter(windows_exception_handler);
}
#else
// crash is off or an unhandled platform
void set_exception_handler() {
}
#endif
