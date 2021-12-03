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

//-------------------------------------------------------------------------
// Windows info struct.
//-------------------------------------------------------------------------
typedef struct _windows_structure {
	TCHAR *executable[STRING_BUFFERS_SIZE]; //1024 bytes
	TCHAR *className[STRING_BUFFERS_SIZE];
	HWND window;
	HWND nextWindow;
	HWND prevWindow;
	HWND parentWindow;
	HWND shellWindow;
	HWND desktopWindow;
	HWND foregroundWindow;
	BOOL isOcculted;
	BOOL isVisible;
	BOOL isMinimized;
	BOOL isHung;
	BOOL isZoomed;
	BOOL isWindowUnicode;
	RECT windowRect;
	RECT clientRect;
	WINDOWPLACEMENT placement;
	LONG style;
	LONG style_ex;
	HMONITOR monitor;
	LPMONITORINFO monitorInfo;
} WINDOWS_STRUCTURE, * PWINDOWS_STRUCTURE;

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
	static char *descriptions[INPUT_COUNT] = { 
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
	dctl_command = "S";
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
	/*EnterCriticalSection();*/

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

	/*LeaveCriticalSection();*/

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

modeler_process_dctl_exit:
	return(ESRV_FAILURE);
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
				map_desktop(p);
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
Function: map_desktop
Purpose : map the desktop.
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API unsigned int __stdcall map_desktop(PINTEL_MODELER_INPUT_TABLE p) {
	//-------------------------------------------------------------------------
	// Important variables.
	//-------------------------------------------------------------------------
	HWND topWindow = GetTopWindow(NULL); // null if fails

	//iterate through z axis
	while (topWindow != NULL) {
		// declare the structure here and pass the address of this structure to get_window_info
		WINDOWS_STRUCTURE windows_struct = { 0 };
		windows_struct.foregroundWindow = topWindow;
		get_window_info(&windows_struct);

		if (!windows_struct.isOcculted) {
			// do the logging in a logger thread with critical section
			// signal the logger thread using another event -- h_log_window_info
			// critical section to protect access to that data
			// allocate and reallocate the memory manually for the queue
			// logic of logger: log until the log signal queue is empty 
			// signal is auto-reset. 

			if (h_log_window_info != NULL) {
				(void)SetEvent(h_log_window_info);
			}
		}

		// get next window
		topWindow = GetNextWindow(topWindow, GW_HWNDNEXT);
	}

	return(ESRV_SUCCESS);
}

/*-----------------------------------------------------------------------------
Function: get_window_info
Purpose : measure and and store window data.
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API unsigned int __stdcall get_window_info(WINDOWS_STRUCTURE * windows_struct) {

	//-------------------------------------------------------------------------
	// Window Variables
	//-------------------------------------------------------------------------
	LPTSTR* windowTitle;
	RECT windowRect, clientRect, topRect, subtracted;
	WINDOWPLACEMENT wp;
	LPMONITORINFO monitorInfo;

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	windows_struct->isVisible = IsWindowVisible(windows_struct->foregroundWindow);

	//-------------------------------------------------------------------------
	// Gather Window Info
	//-------------------------------------------------------------------------
	if (windows_struct->isVisible) {
		* windows_struct->executable = get_process_image_name(windows_struct->foregroundWindow);

		GetClassName(windows_struct->foregroundWindow, windowTitle, STRING_BUFFERS_SIZE);
		* windows_struct->className = windowTitle;

		windows_struct->parentWindow = GetTopWindow(windows_struct->foregroundWindow);
		windows_struct->shellWindow = GetShellWindow();
		windows_struct->desktopWindow = GetDesktopWindow();
		windows_struct->foregroundWindow = GetForegroundWindow();
		windows_struct->nextWindow = GetNextWindow(windows_struct->foregroundWindow, GW_HWNDNEXT);
		windows_struct->prevWindow = GetNextWindow(windows_struct->foregroundWindow, GW_HWNDPREV);

		GetWindowPlacement(windows_struct->foregroundWindow, &wp);
		windows_struct->placement = wp;

		GetWindowsRect(windows_struct->foregroundWindow, windowRect);
		windows_struct->windowRect = windowRect;

		GetClientRect(windows_struct->foregroundWindow, &clientRect);
		windows_struct->clientRect = clientRect;

		GetWindowsRect(windows_struct->prevWindow, topRect);

		if (SubtractRect(&subtracted, &topRect, &windowRect)) {
			if (subtracted.left + subtracted.right + subtracted.top + subtracted.bottom > 0) {
				windows_struct->isOcculted = TRUE;
			}
			else {
				windows_struct->isOcculted = FALSE;
			}
		}
		
		windows_struct->isHung = IsHungAppWindow(windows_struct->foregroundWindow);
		windows_struct->isMinimized = IsIconic(windows_struct->foregroundWindow);
		windows_struct->isZoomed = IsZoomed(windows_struct->foregroundWindow);
		windows_struct->isZoomed = IsZoomed(windows_struct->foregroundWindow);
		windows_struct->isWindowUnicode = IsWindowUnicode(windows_struct->foregroundWindow);
		windows_struct->style = GetWindowLongPtrA(windows_struct->foregroundWindow, GWL_STYLE);
		windows_struct->style_ex = GetWindowLongPtrA(windows_struct->foregroundWindow, GWL_EXSTYLE);
		
		windows_struct->monitor = MonitorFromWindow(windows_struct->foregroundWindow, MONITOR_DEFAULTTOPRIMARY);
		GetMonitorInfo(windows_struct->monitor, &monitorInfo);
		windows_struct->monitorInfo = monitorInfo;
	}
	else {
		goto get_window_info_exit;
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

get_window_info_exit:
	return(ESRV_FAILURE);
}


/*-----------------------------------------------------------------------------
Function: get_process_image_name
Purpose : get the name of the open process (executable).
In      : HWND window.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
TCHAR get_process_image_name(HWND window) {

	//-------------------------------------------------------------------------
	// Local Variables
	//-------------------------------------------------------------------------
	TCHAR procPath[MAX_PATH];
	TCHAR* token = 0;
	TCHAR* executable = 0;
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
		executable = _tcstok(procPath, s);

		// walking through other tokens 
		while (executable != NULL) {
			// retrieves current token
			token = _tcstok(NULL, s);

			// if current token is not null, we want to assign it to the variable we are outputting
			if (token != NULL) {
				executable = token;
			}
			// in this case, we are done iterating so we break out of the loop
			else {
				break;
			}
		}

		return(executable);
	}
}

/*-----------------------------------------------------------------------------
Function: custom_desktop_thread
Purpose : implement the pure event driven desktop mapper thread.
In      : pointers to the input table (passed as void *).
Out     : modified input variables and time events list data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API unsigned int __stdcall custom_logger_thread(void* px) {
	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	DWORD dwret = 0;
	DWORD debug = 0;
	DWORD check_counts = 0;

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
			multiplex_logging(p);
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
ESRV_API unsigned int __stdcall multiplex_logging(PINTEL_MODELER_INPUT_TABLE p) {

	//---------------------------------------------------------------------
	// Trigger a log.
	//---------------------------------------------------------------------
	LOG_INPUT_VALUES;

	RETURN(ESRV_SUCCESS);
}
