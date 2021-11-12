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

/*Follow up with Jamel
	- How to do private data to connect isHung/isImmersive with exe string data
	- What to do with debug? 
*/

//-----------------------------------------------------------------------------
// Headers inclusions.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <windows.h>
#include <process.h> // for _beginthreadex
#include <psapi.h>
#include "foreground_window.h"

//-----------------------------------------------------------------------------
// Global variables.
//-----------------------------------------------------------------------------
HHOOK h_mouse_hook = NULL;
HANDLE h_click_detected = NULL;
// string pattern to split by
const wchar_t s[3] = L"\\";

//-------------------------------------------------------------------------
// Custom event-listener variables.
//-------------------------------------------------------------------------
DWORD collector_thread_id = 0;
HANDLE h_collector_thread = NULL;

//-------------------------------------------------------------------------
// Hook thread variables.
//-------------------------------------------------------------------------
DWORD msg_loop_thread_id = 0;
HANDLE h_msg_loop_thread = NULL;

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
	// Setup threads and sync data.
	//-------------------------------------------------------------------------
	h_click_detected = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	);
	if (h_click_detected == NULL) {
		goto modeler_open_inputs_error;
	}
	//-------------------------------------------------------------------------
	h_collector_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		custom_foreground_thread,
		(void *)p,
		0,
		(unsigned int*)&collector_thread_id
	);
	if (h_collector_thread == NULL) {
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
	// Remove mouse hook -- if not removed yet.
	//-------------------------------------------------------------------------
	if (h_mouse_hook != NULL) {
		bret = UnhookWindowsHookEx(h_mouse_hook);
		if (bret == FALSE) {
			// Should push an error (read chapter 8)
			goto modeler_close_inputs_error;
		}
		h_mouse_hook = NULL;
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

modeler_close_inputs_error:
	return(ESRV_FAILURE);
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

	//-------------------------------------------------------------------------
	h_msg_loop_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		mouse_messages_loop,
		NULL,
		0,
		(unsigned int*)&msg_loop_thread_id
	);
	if (h_msg_loop_thread == NULL) {
		goto custom_event_listner_thread_exit;
	}
	//-------------------------------------------------------------------------
	// Run the message loop!
	//-------------------------------------------------------------------------
	if (h_msg_loop_thread != NULL) {
		WaitForSingleObject(
			h_msg_loop_thread,
			INFINITE
		);
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

custom_event_listner_thread_exit:
	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if (h_click_detected != NULL) {
		CloseHandle(h_click_detected);
		h_click_detected = NULL;
	}
	return(ESRV_FAILURE);
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
Function: modeler_process_lctl
Purpose : process LCTL commands on LCTL interrupt notification.
In      : pointer to PINTEL_MODELER_INPUT_TABLE data structure.
Out     : modified PINTEL_MODELER_INPUT_TABLE data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS modeler_process_lctl(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------

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
Function: process_mouse_messages
Purpose : mouse event handler.
In      : mouse hook message data.
Out     : updated click position data.
Return  : status.
-----------------------------------------------------------------------------*/
LRESULT CALLBACK process_mouse_messages(
	int code,
	WPARAM wparam,
	LPARAM lparam
) {

	//-------------------------------------------------------------------------
	// Message handling variables.
	//-------------------------------------------------------------------------
	MOUSEHOOKSTRUCT* p_mouse_struct = (MOUSEHOOKSTRUCT*)lparam;

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		//-------------------------------------------------------------------------
		// Do as MSDN says!
		//-------------------------------------------------------------------------
		if (code < 0) {
			goto process_mouse_messages_exit;
		}

	//-------------------------------------------------------------------------
	// Get click.
	//-------------------------------------------------------------------------
	if (
		(p_mouse_struct != NULL) &&
		(
			(wparam == WM_LBUTTONDOWN) ||
			(wparam == WM_RBUTTONDOWN)
			)
		) {
		if (h_click_detected != NULL) {
			(void)SetEvent(h_click_detected);
		}
	}

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

process_mouse_messages_exit:
	//-------------------------------------------------------------------------
	// Call next hook in line.
	//-------------------------------------------------------------------------
	return(
		CallNextHookEx(
			h_mouse_hook,
			code,
			wparam,
			lparam
		)
		);
}

/*-----------------------------------------------------------------------------
Function: mouse_messages_loop
Purpose : mouse message hook function.
In      : pointer - as void - to a message structure.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
unsigned int __stdcall mouse_messages_loop(void* pv) {

	//-------------------------------------------------------------------------
	// Message handling variables.
	//-------------------------------------------------------------------------
	MSG message = { 0 };
	HINSTANCE h_instance = GetModuleHandle(NULL);

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	//-------------------------------------------------------------------------
	// Setup mouse hook.
	//-------------------------------------------------------------------------
	h_mouse_hook = SetWindowsHookEx(
		WH_MOUSE_LL,
		process_mouse_messages,
		h_instance,
		0
	);
	if (h_mouse_hook == NULL) {
		goto mouse_messages_loop_error;
	}
	//-------------------------------------------------------------------------
	while (
		GetMessage(
			&message,
			NULL,
			0,
			0
		)
		) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	//-------------------------------------------------------------------------
	// Remove mouse hook.
	//-------------------------------------------------------------------------
	if (h_mouse_hook != NULL) {
		(void)UnhookWindowsHookEx(h_mouse_hook);
		h_mouse_hook = NULL;
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

mouse_messages_loop_error:
	return(ESRV_FAILURE);
}

/*-----------------------------------------------------------------------------
Function: custom_foreground_thread
Purpose : implement the pure event foreground detector thread.
In      : pointers to the input table (passed as void *).
Out     : modified input variables and time events list data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API unsigned int __stdcall custom_foreground_thread(void *px) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	DWORD dwret = 0;
	DWORD debug = 0;

	//-------------------------------------------------------------------------
	// Input variables.
	//-------------------------------------------------------------------------
	HWND lastWindow = GetForegroundWindow(); // used to check current window 
	assert(lastWindow != NULL);
	DWORD ref_pid = 0;	// initial pid
	DWORD last_pid = 0; // used to check against current pid
	TCHAR procPath[MAX_PATH];
	TCHAR* token = 0;
	TCHAR* executable = 0;
	HANDLE openProc = NULL;
	BOOL isImmersive = FALSE;
	BOOL isHung = FALSE;

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
		goto custom_foreground_thread_exit;
	}
	p = (PINTEL_MODELER_INPUT_TABLE)px;

	//-------------------------------------------------------------------------
	// Setup wait variables.
	//-------------------------------------------------------------------------
	wait_events[STOP_EVENT_INDEX] = STOP_SIGNAL;
	wait_events[CLICK_EVENT_INDEX] = h_click_detected;
	assert(wait_events[STOP_EVENT_INDEX] != NULL);
	assert(wait_events[CLICK_EVENT_INDEX] != NULL);

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
				goto custom_foreground_thread_exit; // time to leave!
				break;
			case CLICK_EVENT_INDEX:
				// fall through to wait_timeout
			case WAIT_TIMEOUT:
				lastWindow = GetForegroundWindow();
				// if not null
				if (lastWindow) { 
					// get window pid
					(void)GetWindowThreadProcessId(lastWindow, &last_pid);
					assert(last_pid != NULL);
				
					// Check if different from previous pid
					if (ref_pid != last_pid) {
						// obtains parent id and just copies that to child 
						windowinfo info = { 0 };
						(void)GetWindowThreadProcessId(lastWindow, &info.ownerpid);
						info.childpid = info.ownerpid;
					
						// Find true exe
						(void)EnumChildWindows(lastWindow, EnumChildWindowsCallback, (LPARAM)&info);
					
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
							// Check if open process is immersive (full screen)
							isImmersive = IsImmersiveProcess(openProc);
							// Check if window is hung
							isHung = IsHungAppWindow(lastWindow);
						}
						// used for debugging
						else {
							debug = GetLastError();
						}

						// update the ref_pid
						ref_pid = last_pid;

						//---------------------------------------------------------------------
						// Generate and store metrics.
						//---------------------------------------------------------------------
						SET_INPUT_UNICODE_STRING_ADDRESS(
							INPUT_EXECUTABLE,
							executable
						);
						SET_INPUT_AS_LOGGED(INPUT_EXECUTABLE);

						SET_INPUT_ULL_VALUE(
							INPUT_IS_IMMERSIVE,
							isImmersive
						);
						SET_INPUT_AS_LOGGED(INPUT_IS_IMMERSIVE);

						SET_INPUT_ULL_VALUE(
							INPUT_IS_HUNG,
							isHung
						);
						SET_INPUT_AS_LOGGED(INPUT_IS_HUNG);
					
						//---------------------------------------------------------------------
						// Trigger logging.
						//---------------------------------------------------------------------
						LOG_INPUT_VALUES;

						SET_INPUT_AS_NOT_LOGGED(INPUT_EXECUTABLE);
						SET_INPUT_AS_NOT_LOGGED(INPUT_IS_IMMERSIVE);
						SET_INPUT_AS_NOT_LOGGED(INPUT_IS_HUNG);
					}
				}
				break;
			default:
				goto custom_foreground_thread_exit; // error condition
		} // switch
	} // while

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

custom_foreground_thread_exit:
	return(ESRV_FAILURE);
}
