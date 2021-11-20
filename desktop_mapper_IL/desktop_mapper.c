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

//-----------------------------------------------------------------------------
// Global variables.
//-----------------------------------------------------------------------------
// string pattern to split by
const wchar_t s[3] = L"\\";

//-------------------------------------------------------------------------
// Custom event-listener variables.
//-------------------------------------------------------------------------
DWORD collector_thread_id = 0;
HANDLE h_collector_thread = NULL;
HANDLE h_foreground_window_changed = NULL;

//-----------------------------------------------------------------------------
// Custom DCTL Command
//-----------------------------------------------------------------------------
DCTL_COMMAND dctl_command;
DCTL_COMMAND_DATA dctl_command_argument[
	DCTL_ARGUMENTS_SIZE
] = { 0 };
RDCTL_ANSWER_DATA dctl_answer[
	DCTL_ARGUMENTS_SIZE
] = { 0 };

//-----------------------------------------------------------------------------
// Child Windows Struct & Callback Function
//-----------------------------------------------------------------------------
typedef struct {
	DWORD ownerpid;
	DWORD childpid;
} windowinfo;

BOOL CALLBACK EnumChildWindowsCallback(HWND hWnd, LPARAM lp) {
	windowinfo* info = (windowinfo*)lp;
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	if (pid != info->ownerpid) info->childpid = pid;
	return TRUE;
}

//-------------------------------------------------------------------------
// Windows info struct.
//-------------------------------------------------------------------------
typedef struct _windows_structure {
	TCHAR executable;
	TCHAR className;
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
	// Register RDCTLs.
	//-------------------------------------------------------------------------
	dctl_command = DCTL_QUERY_COMMAND;
	REGISTER_INPUT_LIBRARY_RDCTL(
		DCTL_QUERY_DCTL_NAME,
		DCTL_ALLOW_PARTIAL_TOKEN_MATCH,
		DCTL_QUERY_TOKEN,
		dctl_command,
		dctl_command_argument,
		dctl_answer
	);
	//-------------------------------------------------------------------------
	dctl_command = DCTL_RESET_COMMAND;
	REGISTER_INPUT_LIBRARY_RDCTL(
		DCTL_RESET_DCTL_NAME,
		DCTL_ALLOW_PARTIAL_TOKEN_MATCH,
		DCTL_RESET_TOKEN,
		dctl_command,
		dctl_command_argument,
		dctl_answer
	);

	//-------------------------------------------------------------------------
	// Setup threads and sync data.
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

	//-------------------------------------------------------------------------
	// Setup threads and sync data.
	//-------------------------------------------------------------------------
	h_collector_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		custom_desktop_thread,
		(void*) p,
		0,
		(unsigned int*)&collector_thread_id
	);
	if (h_collector_thread == NULL) {
		goto modeler_process_dctl_exit;
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
	return(ESRV_FAILURE);
}

/*-----------------------------------------------------------------------------
Function: map_desktop
Purpose : map the desktop.
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
unsigned int __stdcall map_desktop(PINTEL_MODELER_INPUT_TABLE p) {
	//-------------------------------------------------------------------------
	// Important variables.
	//-------------------------------------------------------------------------
	HWND topWindow = GetTopWindow(NULL); // null if fails

	//iterate through z axis
	while (topWindow != NULL) {
		// get window info and add to windowArray
		WINDOWS_STRUCTURE window = get_window_info(topWindow); // pointer to table

		if (!window.isOcculted) {
			// log values only if not completely occulted
			SET_INPUT_UNICODE_STRING_ADDRESS(
				INPUT_EXECUTABLE,
				window.executable
			);

			SET_INPUT_UNICODE_STRING_ADDRESS(
				INPUT_WINDOW_TITLE,
				window.className
			);

			SET_INPUT_UNICODE_STRING_ADDRESS(
				INPUT_NEXT_WINDOW,
				window.nextWindow
			);

			SET_INPUT_UNICODE_STRING_ADDRESS(
				INPUT_PREV_WINDOW,
				window.prevWindow
			);

			SET_INPUT_UNICODE_STRING_ADDRESS(
				INPUT_PARENT_WINDOW,
				window.parentWindow
			);

			SET_INPUT_UNICODE_STRING_ADDRESS(
				INPUT_SHELL_WINDOW,
				window.shellWindow
			);

			SET_INPUT_UNICODE_STRING_ADDRESS(
				INPUT_DESKTOP_WINDOW,
				window.desktopWindow
			);

			SET_INPUT_UNICODE_STRING_ADDRESS(
				INPUT_FOREGROUND_WINDOW,
				window.foregroundWindow
			);

			SET_INPUT_ULL_VALUE(
				INPUT_IS_HUNG,
				window.isHung
			);

			SET_INPUT_ULL_VALUE(
				INPUT_IS_ZOOMED,
				window.isZoomed
			);

			SET_INPUT_ULL_VALUE(
				INPUT_IS_VISIBLE,
				window.isVisible
			);

			SET_INPUT_ULL_VALUE(
				INPUT_IS_MINIMIZED,
				window.isMinimized
			);

			SET_INPUT_ULL_VALUE(
				INPUT_IS_WINDOW_UNICODE,
				window.isWindowUnicode
			);

			SET_INPUT_ULL_VALUE(
				INPUT_WINDOW_RECT,
				window.windowRect
			);

			SET_INPUT_ULL_VALUE(
				INPUT_CLIENT_WINDOW_RECT,
				window.clientRect
			);

			SET_INPUT_ULL_VALUE(
				INPUT_WINDOW_STYLE,
				window.style
			);

			SET_INPUT_ULL_VALUE(
				INPUT_WINDOW_STYLE_EX,
				window.style_ex
			);

			SET_INPUT_ULL_VALUE(
				INPUT_WINDOW_PLACEMENT,
				window.wp
			);

			SET_INPUT_ULL_VALUE(
				INPUT_WINDOW_MONITOR,
				window.windowMonitor
			);

			SET_INPUT_ULL_VALUE(
				INPUT_MONITOR_INFO,
				window.monitorInfo
			);

			//---------------------------------------------------------------------
			// Trigger a log.
			//---------------------------------------------------------------------
			LOG_INPUT_VALUES;
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
WINDOWS_STRUCTURE get_window_info(HWND window) {

	//-------------------------------------------------------------------------
	// Window Variables
	//-------------------------------------------------------------------------
	WINDOWS_STRUCTURE windows_struct = { 0 };
	windows_struct.isVisible = IsWindowVisible(window);
	windows_struct.isMinimized = IsIconic(window);

	LPTSTR* windowTitle;
	RECT windowRect, clientRect, topRect, subtracted;
	WINDOWPLACEMENT wp;
	LPMONITORINFO monitorInfo;

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	//-------------------------------------------------------------------------
	// Gather Window Info
	//-------------------------------------------------------------------------
	if (windows_struct.isVisible) {
		windows_struct.window = window;
		windows_struct.executable = get_process_image_name(window);

		GetClassName(window, windowTitle, STRING_BUFFERS_SIZE);
		windows_struct.className = windowTitle;

		windows_struct.parentWindow = GetTopWindow(window);
		windows_struct.shellWindow = GetShellWindow();
		windows_struct.desktopWindow = GetDesktopWindow();
		windows_struct.foregroundWindow = GetForegroundWindow();
		windows_struct.nextWindow = GetNextWindow(window, GW_HWNDNEXT);
		windows_struct.prevWindow = GetNextWindow(window, GW_HWNDPREV);

		GetWindowPlacement(window, &wp);
		windows_struct.placement = wp;

		GetWindowsRect(window, windowRect);
		windows_struct.windowRect = windowRect;

		GetClientRect(window, &clientRect);
		windows_struct.clientRect = clientRect;

		GetWindowsRect(windows_struct.prevWindow, topRect);

		if (SubtractRect(&subtracted, &topRect, &windowRect)) {
			if (subtracted.left + subtracted.right + subtracted.top + subtracted.bottom > 0) {
				windows_struct.isOcculted = TRUE;
			}
			else {
				windows_struct.isOcculted = FALSE;
			}
		}
		
		windows_struct.style = GetWindowLongPtrA(window, GWL_STYLE);
		windows_struct.style_ex = GetWindowLongPtrA(window, GWL_EXSTYLE);
		windows_struct.isHung = IsHungAppWindow(window);
		windows_struct.isZoomed = IsZoomed(window);
		windows_struct.isZoomed = IsZoomed(window);
		windows_struct.isWindowUnicode = IsWindowUnicode(window);
		windows_struct.monitor = MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY);
		
		GetMonitorInfo(windows_struct.monitor, &monitorInfo);
		windows_struct.monitorInfo = monitorInfo;
	}

	return(windows_struct);
}
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
