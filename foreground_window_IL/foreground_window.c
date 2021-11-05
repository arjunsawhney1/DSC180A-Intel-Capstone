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
#include "foreground_window.h"

//-----------------------------------------------------------------------------
// Custom event-listener thread data.
//-----------------------------------------------------------------------------
HANDLE h_thread = NULL;
HANDLE h_stop = NULL;
DWORD thread_id = 0;

//-----------------------------------------------------------------------------
// Global variables.
//-----------------------------------------------------------------------------
CRITICAL_SECTION cs = { NULL };
HANDLE h_click_detected = NULL;
HHOOK h_mouse_hook = NULL;

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
	} // for i (each input)

	//-------------------------------------------------------------------------
	// Setup threads and synch data.
	//-------------------------------------------------------------------------
	h_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		custom_foreground_thread,
		(void*)p,
		0,
		(unsigned int*)&thread_id
	);
	if (h_thread == NULL) {
		goto modeler_open_inputs_error;
	}

	//-------------------------------------------------------------------------
	// Create our own exit event.
	//-------------------------------------------------------------------------
	h_stop = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	);
	if (h_stop == NULL) {
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
	// Stop our event thread.
	//-------------------------------------------------------------------------
	if (h_stop != NULL) {
		bret = SetEvent(h_stop);
		if (bret == FALSE) {
			goto modeler_close_inputs_error;
		}
	}

	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if (h_stop != NULL) {
		bret = CloseHandle(h_stop);
		if (bret == FALSE) {
			goto modeler_close_inputs_error;
		}
		h_stop = NULL;
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
	HWND refWindow = GetForegroundWindow(); // initial foreground window
	assert(refWindow != NULL);
	HWND lastWindow = GetForegroundWindow(); // used to check against current 
	assert(lastWindow != NULL);
	DWORD ref_pid = GetWindowThreadProcessId(refWindow, &thread_id); // initial process ID
	assert(ref_pid != NULL);
	DWORD last_pid = 0; // used to check against current 
	TCHAR procPath[MAX_PATH];
	TCHAR* executable = _T("N/A");
	HANDLE openProc = NULL;
	BOOL isImmersive = FALSE;
	BOOL isHung = FALSE;

	//-------------------------------------------------------------------------
	// Hook thread variables.
	//-------------------------------------------------------------------------
	HANDLE h_msg_loop_thread = NULL;
	DWORD msg_loop_thread_id = 0;

	//-------------------------------------------------------------------------
	// Wait variables.
	//-------------------------------------------------------------------------
	HANDLE wait_events[WAIT_EVENTS_COUNT] = { NULL, NULL };

	//-------------------------------------------------------------------------
	// Watchdog variables.
	//-------------------------------------------------------------------------
	WATCHDOG_VARIABLES

	//-------------------------------------------------------------------------
	// Access helper variables.
	//-------------------------------------------------------------------------
	PINTEL_MODELER_INPUT_TABLE p = NULL;

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(px != NULL);
	p = (PINTEL_MODELER_INPUT_TABLE)px;

	//-------------------------------------------------------------------------
	// Name this thread (for debug mode only).
	//-------------------------------------------------------------------------
	INPUT_DIAGNOSTIC_NAME_THIS_THREAD(
		p,
		"custom_foreground_thread"
	);

	//-------------------------------------------------------------------------
	// Register this thread with watchdog.
	//-------------------------------------------------------------------------
	INPUT_REGISTER_EVENT_LOCKED_THREAD_WITH_WATCHDOG(
		p,
		"custom_foreground_thread",
		h_thread,
		thread_id,
		STOP_SIGNAL,
		custom_foreground_thread_exit
	);

	//-------------------------------------------------------------------------
	// Setup threads and synch data.
	//-------------------------------------------------------------------------
	InitializeCriticalSection(&cs);
	//-------------------------------------------------------------------------
	h_click_detected = CreateEvent(
		NULL,
		FALSE,
		FALSE,
		NULL
	);
	if (h_click_detected == NULL) {
		goto custom_foreground_thread_exit;
	}
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
		goto custom_foreground_thread_exit;
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

	//-------------------------------------------------------------------------
	// Setup wait variables.
	//-------------------------------------------------------------------------
	wait_events[STOP_EVENT_INDEX] = STOP_SIGNAL;
	wait_events[CLICK_EVENT_INDEX] = h_click_detected;
	assert(wait_events[STOP_EVENT_INDEX] != NULL);
	assert(wait_events[CLICK_EVENT_INDEX] != NULL);

	while(STOP_REQUEST == 0) {
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
			TRUE, // #not sure how this affects us
			INPUT_PAUSE_IN_MS // 1000ms
		);
		switch (dwret) {
		case WAIT_OBJECT_0:
			goto custom_foreground_thread_exit; // time to leave!
			break;
		case WAIT_TIMEOUT:
			// get new window
			lastWindow = GetForegroundWindow();
			assert(thisWindow != NULL);

			// get the latest pid (lastWindow's PID)
			last_pid = GetWindowThreadProcessId(lastWindow, &ref_pid);
			assert(last_pid != NULL);

			//check for a change in foreground window
			if (ref_pid != last_pid) {
				// update refWindow and ref_pid
				refWindow = lastWindow;
				ref_pid = last_pid;

				// passing in the thread ID to grant access as well as create a process handle
				openProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ref_pid);
				// in the case where openProc is not null
				if (openProc) {
					// obtaining the path to the process
					if (GetProcessImageFileNameA(openProc, (LPSTR)procPath, MAX_PATH) > 0) {
						executable = wcsrchr(procPath, '/');
					}
					else {
						executable = _T("N/A");
					}
					isImmersive = IsImmersiveProcess(openProc);
					isHung = IsHungAppWindow(refWindow);
				}
				// if null, get the error (used for debugging)
				else {
					debug = GetLastError();
				}
			}
			break;
		default:
			goto custom_foreground_thread_exit; // error condition
		} // switch
		
		//---------------------------------------------------------------------
		// Set input values.
		//---------------------------------------------------------------------
		SET_INPUT_UNICODE_STRING_ADDRESS(
			INPUT_EXECUTABLE,
			executable
		);

		SET_INPUT_ULL_VALUE(
			INPUT_IS_IMMERSIVE,
			isImmersive
		);

		SET_INPUT_ULL_VALUE(
			INPUT_IS_HUNG,
			isHung
		);

		//---------------------------------------------------------------------
		// Trigger logging.
		//---------------------------------------------------------------------
		LOG_INPUT_VALUES;
	}

	//-------------------------------------------------------------------------
	// Un-register this thread with watchdog.
	//-------------------------------------------------------------------------
	INPUT_UNREGISTER_THREAD_WITH_WATCHDOG(
		p,
		h_thread,
		thread_id
	);

	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if (h_click_detected != NULL) {
		CloseHandle(h_click_detected);
		h_click_detected = NULL;
	}
	DeleteCriticalSection(&cs);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

custom_foreground_thread_exit:
	return(ESRV_FAILURE);
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
	// Generic variables.
	//-------------------------------------------------------------------------
	BOOL bret = FALSE;

	//-------------------------------------------------------------------------
	// Message handling variables.
	//-------------------------------------------------------------------------
	MOUSEHOOKSTRUCT* p_mouse_struct = (MOUSEHOOKSTRUCT*)lparam;

	//-------------------------------------------------------------------------

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
	// Capture the data.
	//-------------------------------------------------------------------------
	if (
		(p_mouse_struct != NULL) &&
		(
			(wparam == WM_LBUTTONDOWN) ||
			(wparam == WM_RBUTTONDOWN)
			)
		) {
		bret = TryEnterCriticalSection(&cs);
		if (bret == FALSE) {
			goto process_mouse_messages_exit;
		}
		LeaveCriticalSection(&cs);
		SetEvent(h_click_detected);
	}

process_mouse_messages_exit:
	return(
		CallNextHookEx(
			h_mouse_hook,
			code,
			wparam,
			lparam
		)
		);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)
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

		h_mouse_hook = SetWindowsHookEx(
			WH_MOUSE_LL,
			process_mouse_messages,
			h_instance,
			0
		);

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

	UnhookWindowsHookEx(h_mouse_hook);

	return(0);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)
}
