/**
*** INTEL CONFIDENTIAL
*** 
*** Copyright (March 2011) (March 2011) Intel Corporation All Rights Reserved. 
*** The source code contained or described herein and all documents related to the
*** source code ("Material") are owned by Intel Corporation or its suppliers or 
*** licensors. Title to the Material remains with Intel Corporation or its 
*** suppliers and licensors. The Material contains trade secrets and proprietary 
*** and confidential information of Intel or its suppliers and licensors. 
*** The Material is protected by worldwide copyright and trade secret laws 
*** and treaty provisions. No part of the Material may be used, copied, 
*** reproduced, modified, published, uploaded, posted, transmitted, distributed,
*** or disclosed in any way without Intel's prior express written permission.
***
*** No license under any patent, copyright, trade secret or other intellectual
*** property right is granted to or conferred upon you by disclosure or delivery
*** of the Materials, either expressly, by implication, inducement, estoppel or
*** otherwise. Any license under such intellectual property rights must be 
*** express and approved by Intel in writing.
**/

//-----------------------------------------------------------------------------
// Headers inclusions.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <windows.h>
#include <process.h> // for _beginthreadex
#include <psapi.h>
#include "desktop_mapper.h"
#include "dctl_variables.h"

//-------------------------------------------------------------------------
// Custom event-listener variables.
//-------------------------------------------------------------------------
DWORD collector_thread_id = 0;
HANDLE h_collector_thread = NULL;
DWORD logger_thread_id = 0;
HANDLE h_logger_thread = NULL;
HANDLE h_foreground_window_changed = NULL;
HANDLE h_log_window_info = NULL;

// first fix the logging with shadow period.
// start collecting
// create new IL and try to fix 

//-------------------------------------------------------------------------
// Windows info struct.
//-------------------------------------------------------------------------
WINDOWS_STRUCTURE desktop[MAX_WINDOWS];
// replace to dynamic memory allocation 
//WINDOWS_STRUCTURE* sample[MAX_SAMPLES];
// can have many max samples
// look into pooling memory
// malloc and free

//-----------------------------------------------------------------------------
// Global variables.
//-----------------------------------------------------------------------------
CRITICAL_SECTION cs = { NULL };
// string pattern to split by
const wchar_t s[3] = L"\\";

/*-----------------------------------------------------------------------------
Function: modeler_init_inputs
Purpose : return the inputs count.
In      : pointer to an unsigned integer.
Out     : modified unsigned integer.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API ESRV_STATUS modeler_init_inputs(
	unsigned int *p, 
	int *pfd,
	int *pfe,
	char *po,
	size_t so
) {
	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(p != NULL);
	assert(pfd != NULL);
	assert(pfe != NULL);

	SIGNAL_PURE_EVENT_DRIVEN_MODE;
	SIGNAL_MULTIPLEXED_LOG_MODE;
	SET_INPUTS_COUNT(INPUT_COUNT);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)
}

/*-----------------------------------------------------------------------------
Function: modeler_open_inputs
Purpose : open inputs.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API ESRV_STATUS modeler_open_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	unsigned int i = 0;

	//-------------------------------------------------------------------------
	// Input descriptions.
	//-------------------------------------------------------------------------
	static char* descriptions[INPUT_COUNT] = {
		INPUT_DESCRIPTION_STRINGS
	};
	static INTEL_MODELER_INPUT_TYPES types[INPUT_COUNT] = {
		INPUT_TYPES
	};

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING
	assert(p != NULL);

	//-------------------------------------------------------------------------
	// Set input information.
	//-------------------------------------------------------------------------
	SET_INPUTS_NAME(INPUT_NAME_STRING);
	
	for (i = 0; i < INPUT_COUNT; i++) {
		SET_INPUT_DESCRIPTION(
			i,
			descriptions[i]
		);
		SET_INPUT_TYPE(
			i,
			types[i]
		);
		SET_INPUT_AS_NOT_LOGGED(i);
	}

	//-------------------------------------------------------------------------
	// Register IDCTLs.
	//-------------------------------------------------------------------------
	dctl_command = 'S';
	REGISTER_INPUT_LIBRARY_DCTL(
		DCTL_NAME,
		0,
		DCTL_CUSTOM_TOKEN,
		dctl_command,
		dctl_command_argument
	);

	//-------------------------------------------------------------------------
	// Setup threads and sync data.
	//-------------------------------------------------------------------------
	InitializeCriticalSection(&cs);
	h_log_window_info = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	);
	if (h_log_window_info == NULL) {
		goto modeler_open_inputs_error;
	}
	//-------------------------------------------------------------------------
	h_foreground_window_changed = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	);
	if (h_foreground_window_changed == NULL) {
		goto modeler_open_inputs_error;
	}
	//-------------------------------------------------------------------------
	h_collector_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		custom_desktop_thread,
		(void*)p,
		0,
		(unsigned int*)&collector_thread_id
	);
	if (h_collector_thread == NULL) {
		goto modeler_open_inputs_error;
	}
	//-------------------------------------------------------------------------

	h_logger_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		custom_logger_thread,
		(void*)p,
		0,
		(unsigned int*)&logger_thread_id
	);
	if (h_logger_thread == NULL) {
		goto modeler_open_inputs_error;
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

modeler_open_inputs_error:
	return(ESRV_FAILURE);
}

/*-----------------------------------------------------------------------------
Function: modeler_close_inputs
Purpose : close inputs.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API ESRV_STATUS modeler_close_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	BOOL bret = FALSE;

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING
	
	assert(p != NULL);

	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if (h_foreground_window_changed != NULL) {
		CloseHandle(h_foreground_window_changed);
		h_foreground_window_changed = NULL;
	}

	if (h_log_window_info != NULL) {
		CloseHandle(h_log_window_info);
		h_log_window_info = NULL;
	}

	// free memory

	// check if not already deleted with global variable flag
	// if pointer is not null
	if (&cs != NULL) {
		// set pointer to NULL
		DeleteCriticalSection(&cs);
	}
	
	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

/*-----------------------------------------------------------------------------
Function: modeler_read_inputs
Purpose : collect all inputs.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_read_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(p != NULL);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

/*-----------------------------------------------------------------------------
Function: modeler_listen_inputs
Purpose : listen for all inputs.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_listen_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(p != NULL);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

/*-----------------------------------------------------------------------------
Function: modeler_process_dctl
Purpose : process DCTL commands on DCTL interrupt notification.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_process_dctl(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(p != NULL);

	if (h_foreground_window_changed != NULL) {
		(void)SetEvent(h_foreground_window_changed);
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)
}

/*-----------------------------------------------------------------------------
Function: modeler_process_lctl
Purpose : process LCTL commands on LCTL interrupt notification.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_process_lctl(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(p != NULL);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}

/*-----------------------------------------------------------------------------
Function: map_desktop
Purpose : map the desktop.
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
void map_desktop() {
	//-------------------------------------------------------------------------
	// Important variables.
	//-------------------------------------------------------------------------
	WINDOWS_STRUCTURE windows_struct = { 0 };
	HWND topWindow = GetForegroundWindow();
	HWND shellWindow = GetShellWindow();
	HWND desktopWindow = GetDesktopWindow();
	int counter = 0;

	EnterCriticalSection(&cs);

	while ((topWindow != NULL) && (topWindow != desktopWindow) && (topWindow != shellWindow) && (counter < MAX_WINDOWS)) {
		windows_struct.currentWindow = topWindow;
		get_window_info(&windows_struct);
		windows_struct.index = counter;
		GetSystemTime(&windows_struct.time);
		memcpy_s(&desktop[counter], sizeof(WINDOWS_STRUCTURE), &windows_struct, sizeof(windows_struct));
		topWindow = GetNextWindow(topWindow, GW_HWNDNEXT);
		counter++;
	}

	LeaveCriticalSection(&cs);

	// Have a collection of windows and then log after loop 
	if (h_log_window_info != NULL) {
		(void)SetEvent(h_log_window_info);
	}
}


/*-----------------------------------------------------------------------------
Function: custom_desktop_thread
Purpose : implement the pure event driven desktop mapper thread.
In      : pointers to the input table (passed as void *).
Out     : modified input variables and time events list data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API unsigned int __stdcall custom_desktop_thread(void* px) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	DWORD dwret = 0;
	DWORD debug = 0;

	//-------------------------------------------------------------------------
	// Access helper variables.
	//-------------------------------------------------------------------------
	PINTEL_MODELER_INPUT_TABLE p = NULL;

	//-------------------------------------------------------------------------
	// Wait variables.
	//-------------------------------------------------------------------------
	HANDLE wait_events[WAIT_EVENTS_COUNT] = { NULL, NULL };

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	//-------------------------------------------------------------------------
	// Get PILT pointer.
	//-------------------------------------------------------------------------
	assert(px != NULL);
	if (px == NULL) {
		goto custom_desktop_thread_exit;
	}
	p = (PINTEL_MODELER_INPUT_TABLE)px;

	//-------------------------------------------------------------------------
	// Setup wait variables.
	//-------------------------------------------------------------------------
	wait_events[STOP_EVENT_INDEX] = STOP_SIGNAL;
	wait_events[FOREGROUND_WINDOW_CHANGE_INDEX] = h_foreground_window_changed;
	assert(wait_events[STOP_EVENT_INDEX] != NULL);
	assert(wait_events[FOREGROUND_WINDOW_CHANGE_INDEX] != NULL);

	while (STOP_REQUEST == 0) {
		//---------------------------------------------------------------------
		// Pause to simulate event triggering.
		// Note:
		//    Rather than using a sleep, which would lock the event listener 
		//    thread, we recommend using the method shown below. In general
		//    developers of event-driven input libraries should add into the
		//    end condition the event / semaphore via the STOP_SIGNAL macro 
		//    (also - but not instead - use the STOP_REQUEST macro).
		//---------------------------------------------------------------------
		dwret = WaitForMultipleObjects(
			WAIT_EVENTS_COUNT,
			wait_events, // array of handles (events)
			FALSE, // #checks if we should wait for ALL events or not b4 executing
			INPUT_PAUSE_IN_MS // 1000ms
		);
		switch (dwret) {
			case STOP_EVENT_INDEX:
				goto custom_desktop_thread_exit; // time to leave!
				break;
			case FOREGROUND_WINDOW_CHANGE_INDEX:
				// fall through to wait_timeout
			case WAIT_TIMEOUT:
				// allocate memory to store the map
				// map_desktop will take the address of allocated address

				map_desktop();
				
				// check max samples
				// lock
				// put desktop into sample array by assigning address
				// increment count
				// unlock

				break;
			default:
				goto custom_desktop_thread_exit; // error condition
		} // switch
	} // while

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

custom_desktop_thread_exit:
	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if (h_foreground_window_changed != NULL) {
		CloseHandle(h_foreground_window_changed);
		h_foreground_window_changed = NULL;
	}
	return(ESRV_FAILURE);
}

/*-----------------------------------------------------------------------------
Function: get_window_info
Purpose : measure and and store window data.
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS get_window_info(WINDOWS_STRUCTURE* windows_struct) {

	//-------------------------------------------------------------------------
	// Window Variables
	//-------------------------------------------------------------------------
	RECT windowRect;
	WINDOWPLACEMENT windowPlacement = { 0 };

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	//-------------------------------------------------------------------------
	// Gather Window Info
	//-------------------------------------------------------------------------
	memset(windows_struct->executable, 0, sizeof(windows_struct->executable));
	get_process_image_name(windows_struct->currentWindow, windows_struct->executable, _countof(windows_struct->executable));
	
	HWND nextWindow = GetNextWindow(windows_struct->currentWindow, GW_HWNDNEXT);
	memset(windows_struct->nextWindow, 0, sizeof(windows_struct->nextWindow));
	get_process_image_name(nextWindow, windows_struct->nextWindow, _countof(windows_struct->nextWindow));
	
	HWND prevWindow = GetNextWindow(windows_struct->currentWindow, GW_HWNDPREV);
	memset(windows_struct->prevWindow, 0, sizeof(windows_struct->prevWindow));
	get_process_image_name(prevWindow, windows_struct->prevWindow, _countof(windows_struct->prevWindow));

	HWND foregroundWindow = GetForegroundWindow();
	memset(windows_struct->foregroundWindow, 0, sizeof(windows_struct->foregroundWindow));
	get_process_image_name(foregroundWindow, windows_struct->foregroundWindow, _countof(windows_struct->foregroundWindow));

	memset(&windowRect, 0, sizeof(windowRect));
	GetWindowRect(windows_struct->currentWindow, &windowRect);
	memset(windows_struct->windowRect, 0, sizeof(windows_struct->windowRect));
	_sntprintf_s(windows_struct->windowRect, _countof(windows_struct->windowRect), _TRUNCATE, _T("%ld-%ld-%ld-%ld"), windowRect.left, windowRect.right, windowRect.top, windowRect.bottom);

	memset(&windowPlacement, 0, sizeof(windowPlacement));
	GetWindowPlacement(windows_struct->currentWindow, &windowPlacement);
	memset(windows_struct->windowPlacement, 0, sizeof(windows_struct->windowPlacement));
	_sntprintf_s(windows_struct->windowPlacement, _countof(windows_struct->windowPlacement), _TRUNCATE, _T("%ld-%ld-%ld-%ld"), windowPlacement.rcNormalPosition.left, windowPlacement.rcNormalPosition.right, windowPlacement.rcNormalPosition.top, windowPlacement.rcNormalPosition.bottom);

	windows_struct->isVisible = IsWindowVisible(windows_struct->currentWindow);
	windows_struct->isMinimized = IsIconic(windows_struct->currentWindow);
	windows_struct->isZoomed = IsZoomed(windows_struct->currentWindow);
	windows_struct->isHung = IsHungAppWindow(windows_struct->currentWindow);

	/*GetClientRect(windows_struct->currentWindow, &clientRect);
	_sntprintf_s(windows_struct->clientRect, _countof(windows_struct->clientRect), _TRUNCATE, _T("%ld-%ld-%ld-%ld"), clientRect.left, clientRect.right, clientRect.top, clientRect.bottom);

	windows_struct->style = GetWindowLongPtrA(windows_struct->currentWindow, GWL_STYLE);
	windows_struct->style_ex = GetWindowLongPtrA(windows_struct->currentWindow, GWL_EXSTYLE);
	windows_struct->monitor = MonitorFromWindow(windows_struct->currentWindow, MONITOR_DEFAULTTOPRIMARY);

	GetMonitorInfo(windows_struct->monitor, &windows_struct->monitorInfo);*/

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

	return(ESRV_SUCCESS);
}


/*-----------------------------------------------------------------------------
Function: get_process_image_name
Purpose : get the name of the open process (executable).
In      : HWND window.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
void get_process_image_name(HWND window, TCHAR* buffer, DWORD size) {

	//-------------------------------------------------------------------------
	// Local Variables
	//-------------------------------------------------------------------------
	TCHAR procPath[MAX_PATH];
	memset(procPath, 0, sizeof(procPath));
	TCHAR* token = 0;
	TCHAR* lastToken = 0;
	HANDLE openProc = NULL;

	// obtains parent id and just copies that to child 
	windowinfo info = { 0 };
	(void)GetWindowThreadProcessId(window, &info.ownerpid);
	info.childpid = info.ownerpid;

	// Find true exe
	(void)EnumChildWindows(window, EnumChildWindowsCallback, (LPARAM)&info);

	// Get executable process handle
	openProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, info.childpid);

	// in the case where openProc is not null
	if (openProc) {
		// Get path to executable
		(void)GetProcessImageFileName(openProc, procPath, MAX_PATH);

		// start with the first token
		_tcstok(procPath, s);

		// walking through other tokens 
		while (procPath != NULL) {
			// retrieves current token
			token = _tcstok(NULL, s);

			// if current token is not null, we want to assign it to the variable we are outputting
			if (token != NULL) {
				lastToken = token;
			}
			// in this case, we are done iterating so we break out of the loop
			else {
				_sntprintf_s(buffer, size, _TRUNCATE, _T("%s"), lastToken);
				break;
			}
		}
	}
}

/*-----------------------------------------------------------------------------
Function: custom_desktop_thread
Purpose : implement the pure event driven desktop mapper thread.
In      : pointers to the input table (passed as void *).
Out     : modified input variables and time events list data.
Return  : status.
-----------------------------------------------------------------------------*/
unsigned int __stdcall custom_logger_thread(void* px) {
	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	DWORD dwret = 0;
	DWORD debug = 0;
	DWORD check_counts = 0;
	ESRV_STATUS stat = ESRV_FAILURE;

	//-------------------------------------------------------------------------
	// Access helper variables.
	//-------------------------------------------------------------------------
	PINTEL_MODELER_INPUT_TABLE p = NULL;

	//-------------------------------------------------------------------------
	// Wait variables.
	//-------------------------------------------------------------------------
	HANDLE wait_events[WAIT_EVENTS_COUNT] = { NULL, NULL };

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		//-------------------------------------------------------------------------
		// Get PILT pointer.
		//-------------------------------------------------------------------------
		assert(px != NULL);
	if (px == NULL) {
		goto custom_logger_thread_exit;
	}
	p = (PINTEL_MODELER_INPUT_TABLE)px;

	/*-------------------------------------------------------------------------
	 Check if logger is MPlex capable.
	-------------------------------------------------------------------------*/
logger_thread_check_logger_mplex_support:
	if (IS_MULTIPLEX_LOG_SUPPORTED == 0) {
		if (check_counts++ < MAX_MPLEX_LOGGER_CHECKS) {
			dwret = WaitForSingleObject(
				STOP_SIGNAL,
				WAIT_FOR_MULTIPLEX_LOGGER_TIME_IN_MS
			);
			switch (dwret) {
			case WAIT_OBJECT_0: // leave!
				goto custom_logger_thread_exit;
				break;
			case WAIT_TIMEOUT:
				goto logger_thread_check_logger_mplex_support;
				break; // wait again
			}
			if (STOP_REQUEST == 1) {
				goto custom_logger_thread_exit;
			}
		}
	}

	//-------------------------------------------------------------------------
	// Check if MPlex logger is running.
	//-------------------------------------------------------------------------
	check_counts = 0;
logger_thread_check_logger:
	if (IS_MULTIPLEX_LOG_RUNNING == 0) {
		if (check_counts++ < MAX_MPLEX_LOGGER_CHECKS) {
			dwret = WaitForSingleObject(
				STOP_SIGNAL,
				WAIT_FOR_MULTIPLEX_LOGGER_TIME_IN_MS
			);
			switch (dwret) {
			case WAIT_OBJECT_0: // leave!
				goto custom_logger_thread_exit;
				break;
			case WAIT_TIMEOUT:
				goto logger_thread_check_logger;
				break; // wait again
			}
			if (STOP_REQUEST == 1) {
				goto custom_logger_thread_exit;
			}
		}
	}

	//-------------------------------------------------------------------------
	// Setup wait variables.
	//-------------------------------------------------------------------------
	wait_events[STOP_EVENT_INDEX] = STOP_SIGNAL;
	wait_events[FOREGROUND_WINDOW_CHANGE_INDEX] = h_log_window_info;
	assert(wait_events[STOP_EVENT_INDEX] != NULL);
	assert(wait_events[FOREGROUND_WINDOW_CHANGE_INDEX] != NULL);

	while (STOP_REQUEST == 0) {
		//---------------------------------------------------------------------
		// Pause to simulate event triggering.
		// Note:
		//    Rather than using a sleep, which would lock the event listener 
		//    thread, we recommend using the method shown below. In general
		//    developers of event-driven input libraries should add into the
		//    end condition the event / semaphore via the STOP_SIGNAL macro 
		//    (also - but not instead - use the STOP_REQUEST macro).
		//---------------------------------------------------------------------
		dwret = WaitForMultipleObjects(
			WAIT_EVENTS_COUNT,
			wait_events, // array of handles (events)
			FALSE, // #checks if we should wait for ALL events or not b4 executing
			INPUT_PAUSE_IN_MS // 1000ms
		);
		switch (dwret) {
		case STOP_EVENT_INDEX:
			goto custom_logger_thread_exit; // time to leave!
			break;
		case FOREGROUND_WINDOW_CHANGE_INDEX:
			// fall through to wait_timeout
		case WAIT_TIMEOUT:
			// in do while loop, while not empty, or stop signal
			// copy to local, set pointer to NULL
			// EnterCriticalSection just for extraction of address of sample

			EnterCriticalSection(&cs);
			
			stat = multiplex_logging(p, &desktop[0]);
			
			LeaveCriticalSection(&cs);

			// leave 
			// now free memory

			if (stat == ESRV_FAILURE) {
				goto custom_logger_thread_exit;
			}
			break;
		default:
			goto custom_logger_thread_exit; // error condition
		} // switch
	} // while

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

custom_logger_thread_exit:
	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if (h_log_window_info != NULL) {
		CloseHandle(h_log_window_info);
		h_log_window_info = NULL;
	}

	return(ESRV_FAILURE);
}

/*-----------------------------------------------------------------------------
Function: multiplex_logging
Purpose : perform multiplex_logging
In      : PINTEL_MODELER_INPUT_TABLE p
Out     : ESRV_STATUS.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS multiplex_logging(PINTEL_MODELER_INPUT_TABLE p, WINDOWS_STRUCTURE* d) {
	DWORD dwret = 0;
	DWORD check_counts = 0;
	DWORD retries_count = 0;
	
	MULTIPLEX_LOG_SATUS lret = MULTIPLEX_LOG_OK;
	ERROR_STATUS eret = ERROR_FAILURE;

	assert(d != NULL);
	assert(p != NULL);

	//-------------------------------------------------------------
	// Log and Un-mask inputs.
	//-------------------------------------------------------------

	for (int i = 0; i < MAX_WINDOWS; i++) {
		SET_INPUT_UNICODE_STRING_ADDRESS(INPUT_CURR_WINDOW_EXECUTABLE,d[i].executable);
		SET_INPUT_PRIVATE_DWORD(INPUT_CURR_WINDOW_EXECUTABLE, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_CURR_WINDOW_EXECUTABLE, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_CURR_WINDOW_EXECUTABLE);

		SET_INPUT_UNICODE_STRING_ADDRESS(INPUT_PREV_WINDOW_EXECUTABLE, d[i].prevWindow);
		SET_INPUT_PRIVATE_DWORD(INPUT_PREV_WINDOW_EXECUTABLE, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_PREV_WINDOW_EXECUTABLE, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_PREV_WINDOW_EXECUTABLE);

		SET_INPUT_UNICODE_STRING_ADDRESS(INPUT_NEXT_WINDOW_EXECUTABLE, d[i].nextWindow);
		SET_INPUT_PRIVATE_DWORD(INPUT_NEXT_WINDOW_EXECUTABLE, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_NEXT_WINDOW_EXECUTABLE, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_NEXT_WINDOW_EXECUTABLE);

		SET_INPUT_UNICODE_STRING_ADDRESS(INPUT_FOREGROUND_WINDOW_EXECUTABLE, d[i].foregroundWindow);
		SET_INPUT_PRIVATE_DWORD(INPUT_FOREGROUND_WINDOW_EXECUTABLE, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_FOREGROUND_WINDOW_EXECUTABLE, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_FOREGROUND_WINDOW_EXECUTABLE);

		SET_INPUT_UNICODE_STRING_ADDRESS(INPUT_WINDOW_RECT, d[i].windowRect);
		SET_INPUT_PRIVATE_DWORD(INPUT_WINDOW_RECT, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_WINDOW_RECT, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_WINDOW_RECT);

		SET_INPUT_UNICODE_STRING_ADDRESS(INPUT_WINDOW_PLACEMENT, d[i].windowPlacement);
		SET_INPUT_PRIVATE_DWORD(INPUT_WINDOW_PLACEMENT, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_WINDOW_PLACEMENT, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_WINDOW_PLACEMENT);

		SET_INPUT_ULL_VALUE(INPUT_IS_VISIBLE, d[i].isVisible);
		SET_INPUT_PRIVATE_DWORD(INPUT_IS_VISIBLE, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_IS_VISIBLE, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_IS_VISIBLE);

		SET_INPUT_ULL_VALUE(INPUT_IS_MINIMIZED, d[i].isMinimized);
		SET_INPUT_PRIVATE_DWORD(INPUT_IS_MINIMIZED, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_IS_MINIMIZED, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_IS_MINIMIZED);

		SET_INPUT_ULL_VALUE(INPUT_IS_MAXIMIZED, d[i].isZoomed);
		SET_INPUT_PRIVATE_DWORD(INPUT_IS_MAXIMIZED, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_IS_MAXIMIZED, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_IS_MAXIMIZED);

		SET_INPUT_ULL_VALUE(INPUT_IS_HUNG, d[i].isHung);
		SET_INPUT_PRIVATE_DWORD(INPUT_IS_HUNG, d[i].index);
		SET_INPUT_UPDATE_TIME(INPUT_IS_HUNG, INPUT_TIME_SYSTEMTIME, &d[i].time);
		SET_INPUT_AS_LOGGED(INPUT_IS_HUNG);
	}

	/*
	SET_INPUT_UNICODE_STRING_ADDRESS(
			INPUT_CLIENT_RECT,
			d[i].clientRect
		);
	SET_INPUT_AS_LOGGED(INPUT_CLIENT_RECT);
		
	SET_INPUT_UNICODE_STRING_ADDRESS(
		INPUT_WINDOW_TITLE,
		windows_struct.className
	);
	SET_INPUT_AS_LOGGED(INPUT_WINDOW_TITLE);

	SET_INPUT_ULL_VALUE(
		INPUT_WINDOW_STYLE,
		windows_struct.style
	);
	SET_INPUT_AS_LOGGED(INPUT_WINDOW_STYLE);

	SET_INPUT_ULL_VALUE(
		INPUT_WINDOW_STYLE_EX,
		windows_struct.style_ex
	);
	SET_INPUT_AS_LOGGED(INPUT_WINDOW_STYLE_EX);

	SET_INPUT_ULL_VALUE(
		INPUT_WINDOW_MONITOR,
		windows_struct.monitor
	);
	SET_INPUT_AS_LOGGED(INPUT_WINDOW_MONITOR);

	SET_INPUT_ULL_VALUE(
		INPUT_MONITOR_INFO,
		windows_struct.monitorInfo
	);
	SET_INPUT_AS_LOGGED(INPUT_MONITOR_INFO);

	SET_INPUT_UPDATE_TIME(
		INPUT_IS_HUNG,
		INPUT_TIME_SYSTEMTIME
		SYSTEMTIME st = { 0 };
	);
	*/

retry_log_window:
	SET_DATA_READY;
	lret = LOG_INPUT_VALUES;
	switch (lret) {
	case MULTIPLEX_LOG_OK:
		break;
	case MULTIPLEX_LOG_BUSY:
	case MULTIPLEX_LOG_FAILED:
	case MULTIPLEX_LOG_BUFFER_FULL:
		if (retries_count < LOGGER_MAX_LOG_TRIES) {
			retries_count++;
			Sleep(LOG_RETRY_PAUSE_IN_MS);
			goto retry_log_window;
		}

		break;
	case MULTIPLEX_LOG_OUT_OF_MEMORY:
		goto custom_logger_thread_exit;
	default:
		;
	} // switch

	return(ESRV_SUCCESS);

custom_logger_thread_exit:
	return ESRV_FAILURE;
}
