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
#include "foreground_window.h"

//-----------------------------------------------------------------------------
// Custom event-listener thread data.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global variables.
//-----------------------------------------------------------------------------


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
	SET_INPUTS_COUNT(INPUTS_COUNT);

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
	static char *descriptions[INPUTS_COUNT] = { 
		INPUT_DESCRIPTION_STRINGS 
	};
	static INTEL_MODELER_INPUT_TYPES types[INPUTS_COUNT] = {
		INPUT_TYPES
	};

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(p != NULL);

	// added vars to get rid of errors
	DWORD thread_id = 0;
	HANDLE h_thread = NULL;

	//Start our thread **************************************************************
	h_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		custom_foreground_thread,
		(void *)p,
		0,
		(unsigned int *)& thread_id 
	);
	if(h_thread == NULL) {
		goto modeler_open_inputs_error;
	}


	//-------------------------------------------------------------------------
	// Set input information.
	//-------------------------------------------------------------------------
	SET_INPUTS_NAME(INPUT_NAME_STRING);
	for( i = 0; i < INPUTS_COUNT; i++) {
		SET_INPUT_DESCRIPTION(
			i,
			descriptions[i]
		);
		SET_INPUT_TYPE(
			i,
			types[i]
		);
	} // for i (each input)

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

//******************************************************************
//CODE ADDED *******************************************************
//******************************************************************


//-----------------------------------------------------------------------------
// Function: getterForegroundWindow()
// Purpose : use GetForegroundWindow() to get the foreground window handle to be inputted into getThreadProcessId().
// In      : no input
// Out     : HWND foreground window handle.
// Return  : The return value is a handle to the foreground window. The foreground window can be NULL in certain circumstances, such as when a window is losing activation.
//-----------------------------------------------------------------------------
HWND getterForegroundWindow()
{
	HWND hwnd = GetForegroundWindow();
	return hwnd;
}

//^ This is redundant so will have to remove it.

// create  fucntion to get the process id of the foreground window
//-----------------------------------------------------------------------------
// Function: get_foreground_pid
// Purpose : return the process id of the foreground window.
// In      : HWND A handle to the window.
// Out     : DWORD process id.
// Return  : process id
//-----------------------------------------------------------------------------
DWORD get_foreground_pid(HWND hwnd) {
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	return pid;
}

// create a function to get the thread id of the foreground window
//-----------------------------------------------------------------------------
// Function: get_foreground_tid
// Purpose : return the thread id of the foreground window.
// In      : HWND A handle to the window.
// Out     : DWORD thread id.
// Return  : thread id
//-----------------------------------------------------------------------------
DWORD get_foreground_tid(HWND hwnd) {
	DWORD tid = 0;
	GetWindowThreadProcessId(hwnd, &tid);
	return tid;
}


//******************************************************************
//CODE ADDED *******************************************************
//******************************************************************



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
//-----------------------------------------------------------------------------

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

	//-------------------------------------------------------------------------
	// Watchdog variables.
	//-------------------------------------------------------------------------
	WATCHDOG_VARIABLES

	//-------------------------------------------------------------------------
	// Access helper variables.
	//-------------------------------------------------------------------------
	PINTEL_MODELER_INPUT_TABLE p = NULL;

	//-------------------------------------------------------------------------

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
		dwret = WaitForSingleObject(
			STOP_SIGNAL,
			INPUT_PAUSE_IN_MS //# 100ms
		);
		switch (dwret) {
		case WAIT_OBJECT_0:
			goto custom_foreground_thread_exit; // time to leave!
			break;
		case WAIT_TIMEOUT:

			HWND foregroundWindow_handle = GetForegroundWindow();

			// Returns window Process ID
			DWORD processID = GetWindowThreadProcessId(
				foregroundWindow_handle,
				thread_id
			);
			// Checks for windows app
			BOOL isImmersive = IsImmersiveProcess(
				foregroundWindow_handle
			);
			// Opens a process?
			HANDLE openProc OpenProcess(
				thread_id,
				isImmersive,
				processID
			);

			LONG GetPackageFullName(
				HANDLE openProc,
				UINT32 packageFullNameLength,
				PWSTR  packageFullName
			);

			break; // all good, time to make a measurement
		default:
			goto custom_foreground_thread_exit; // error condition
		} // switch

		//---------------------------------------------------------------------
		// Trigger logging.
		//---------------------------------------------------------------------
		LOG_INPUT_VALUES;

		//---------------------------------------------------------------------
		// Set input values.
		//---------------------------------------------------------------------

		SET_INPUT_ULL_VALUE(
			INPUT_CURSOR_ICON,
			icon
		);
	} // while

	//-------------------------------------------------------------------------
	// Un-register this thread with watchdog.
	//-------------------------------------------------------------------------
	INPUT_UNREGISTER_THREAD_WITH_WATCHDOG(
		p,
		h_thread,
		thread_id
	);

custom_foreground_thread_exit:
	return(ESRV_FAILURE);

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

}
















//-----------------------------------------------------------------------------
// Headers inclusions.
//-----------------------------------------------------------------------------
#include <assert.h>
#include <process.h> // for _beginthreadex
#include <windows.h>
#include <tchar.h>
#include <Psapi.h> // GetProcessImageFileName

#ifndef __PL_WINDOWS_CORE__
#pragma comment (lib, "Psapi.lib")
#endif // !__PL_WINDOWS_CORE__

#include "hook_input.h"

//-----------------------------------------------------------------------------
// Global counter variable.
//-----------------------------------------------------------------------------
BOOL f_stop = FALSE;
HHOOK h_mouse_hook = NULL;
POINT click_position = { 0 };
CRITICAL_SECTION cs = { NULL };
HANDLE h_click_detected = NULL;

//-----------------------------------------------------------------------------
// Inputs global variables.
//-----------------------------------------------------------------------------
unsigned long long int x = 0;
unsigned long long int y = 0;
unsigned long long int id = 0;
unsigned long long int root_id = 0;
char class_name[STRING_BUFFERS_SIZE] = { '\0' };
unsigned long long int style = 0;
unsigned long long int style_ex = 0;
char window_name[STRING_BUFFERS_SIZE] = { '\0' };
char image_name[MAX_PATH] = { '\0' };

//-----------------------------------------------------------------------------
// Custom event-listener thread data.
//-----------------------------------------------------------------------------
HANDLE h_thread = NULL;
DWORD thread_id = 0;
HANDLE h_stop = NULL;

/*-----------------------------------------------------------------------------
Function: modeler_init_inputs
Purpose : return the inputs count.
In      : pointer to an unsigned integer.
Out     : modified unsigned integer.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API ESRV_STATUS modeler_init_inputs(
	unsigned int* p,
	int* pfd,
	int* pfe,
	char* po,
	size_t os
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
	static char* descriptions[INPUT_COUNT] = {
		INPUT_DESCRIPTION_STRINGS
	};
	static INTEL_MODELER_INPUT_TYPES types[INPUT_COUNT] = {
		INPUT_TYPES
	};

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);

	//-------------------------------------------------------------------------
	// Set input information.
	//-------------------------------------------------------------------------
	SET_INPUTS_NAME(INPUT_NAME_STRING);
	//-------------------------------------------------------------------------
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
	// Set string inputs information.
	//-------------------------------------------------------------------------
	SET_INPUT_STRING_ADDRESS(
		INPUT_CLICKED_UI_OBJECT_CLASS_NAME_INDEX,
		class_name
	);
	SET_INPUT_STRING_ADDRESS(
		INPUT_CLICKED_UI_OBJECT_NAME_INDEX,
		window_name
	);
	SET_INPUT_STRING_ADDRESS(
		INPUT_CLICKED_UI_OBJECT_OWNING_PROCESS_IMAGE_INDEX,
		image_name
	);

	//-------------------------------------------------------------------------
	// Start the pure event-driven thread.
	//-------------------------------------------------------------------------
	h_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		custom_event_listner_thread,
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

//-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
Function: custom_event_listner_thread
Purpose : implement the pure event listener thread.
In      : pointers to the input table (passed as void *).
Out     : modified input variables and time events list data.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_API unsigned int __stdcall custom_event_listner_thread(void* px) {

	//-------------------------------------------------------------------------
	// Hook thread variables.
	//-------------------------------------------------------------------------
	HANDLE h_msg_loop_thread = NULL;
	DWORD msg_loop_thread_id = 0;
	HANDLE h_collector_thread = NULL;
	DWORD collector_thread_id = 0;

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(px != NULL);

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
		goto custom_event_listner_thread_exit;
	}
	// so if theres a mouse click detected, then get the foreground window handle
	else if (h_click_detected != NULL) {
		HWND foregroundWindow_handle = GetForegroundWindow();
	}
	//-------------------------------------------------------------------------
	h_collector_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		get_object_info,
		px,
		0,
		(unsigned int*)&collector_thread_id
	);
	if (h_collector_thread == NULL) {
		goto custom_event_listner_thread_exit;
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

custom_event_listner_thread_exit:

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
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

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
		click_position.x = p_mouse_struct->pt.x;
		click_position.y = p_mouse_struct->pt.y;
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
Function: get_object_info
Purpose : measure and and store input data.
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
unsigned int __stdcall get_object_info(void* pv) {

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	size_t stret = 0;
	BOOL bret = FALSE;

	//-------------------------------------------------------------------------
	// Metrics extraction variables.
	//-------------------------------------------------------------------------
	POINT pt = { 0 };
	HWND h_root = NULL;
	HWND h_clicked = NULL;
	WINDOWINFO wi = { 0 };
	//-------------------------------------------------------------------------
	static HWND last_h = NULL;
	static POINT last_p = { 0 };
	static BOOL last_bret = FALSE;
	static WINDOWINFO last_wi = { 0 };
	static TCHAR tchar_last_window_name[STRING_BUFFERS_SIZE] = { _T('\0') };
	static TCHAR tchar_last_class_name[STRING_BUFFERS_SIZE] = { _T('\0') };
	//-------------------------------------------------------------------------
	HWND handle = NULL;
	DWORD pid = 0;
	DWORD tid = 0;
	HANDLE process = NULL;
	DWORD dwret = 0;
	//-------------------------------------------------------------------------
	TCHAR tchar_image_name[MAX_PATH] = { _T('\0') };
	TCHAR tchar_class_name[STRING_BUFFERS_SIZE] = { _T('\0') };
	TCHAR tchar_window_name[STRING_BUFFERS_SIZE] = { _T('\0') };

	//-------------------------------------------------------------------------
	// Ease access variables.
	//-------------------------------------------------------------------------
	PINTEL_MODELER_INPUT_TABLE p = NULL;

	//-------------------------------------------------------------------------
	// Wait variables.
	//-------------------------------------------------------------------------
	HANDLE wait_events[WAIT_EVENTS_COUNT] = { NULL, NULL };

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		//-------------------------------------------------------------------------
		// Get PILT pointer.
		//-------------------------------------------------------------------------
		assert(pv != NULL);
	p = (PINTEL_MODELER_INPUT_TABLE)pv;
	assert(p != NULL);

	//-------------------------------------------------------------------------
	// Setup wait variables.
	//-------------------------------------------------------------------------
	wait_events[STOP_EVENT_INDEX] = STOP_SIGNAL;
	wait_events[CLICK_EVENT_INDEX] = h_click_detected;
	assert(wait_events[STOP_EVENT_INDEX] != NULL);
	assert(wait_events[CLICK_EVENT_INDEX] != NULL);

	//-------------------------------------------------------------------------
	// Waiting for the end of run.
	//-------------------------------------------------------------------------
	while (f_stop == FALSE) {

		//---------------------------------------------------------------------
		// Waiting for mouse event thread's signal.
		//---------------------------------------------------------------------
		dwret = WaitForMultipleObjects(
			WAIT_EVENTS_COUNT,
			wait_events,
			FALSE,
			INFINITE
		);
		switch (dwret) {
		case WAIT_OBJECT_0 + STOP_EVENT_INDEX:
			goto get_object_info_exit; // time to leave!
			break;
		case WAIT_OBJECT_0 + CLICK_EVENT_INDEX:
			break; // all good
		default:
			goto get_object_info_exit; // error condition
		} // switch

		//---------------------------------------------------------------------
		// Get click data - as fast as possible.
		//---------------------------------------------------------------------
		EnterCriticalSection(&cs);
		x = pt.x = click_position.x;
		y = pt.y = click_position.y;
		LeaveCriticalSection(&cs);
		//---------------------------------------------------------------------
		SET_INPUT_ULL_VALUE(
			INPUT_CLICK_X_POSITION_INDEX,
			x
		);
		//---------------------------------------------------------------------
		SET_INPUT_ULL_VALUE(
			INPUT_CLICK_Y_POSITION_INDEX,
			y
		);

		//---------------------------------------------------------------------
		// We have a bit more time to extract the inputs data now. Start with
		// the clicked window's handle.
		//---------------------------------------------------------------------
		h_clicked = WindowFromPoint(pt);
		//---------------------------------------------------------------------
		id = (unsigned long long int)h_clicked;
		SET_INPUT_ULL_VALUE(
			INPUT_CLICKED_UI_OBJECT_ID_INDEX,
			id
		);

		//---------------------------------------------------------------------
		// Then the name and class name. Warning: name may contain PII.
		//---------------------------------------------------------------------
		(void)InternalGetWindowText(
			h_clicked,
			tchar_window_name,
			sizeof(tchar_window_name) / sizeof(TCHAR)
		);
		(void)GetClassName(
			h_clicked,
			tchar_class_name,
			sizeof(tchar_class_name) / sizeof(TCHAR)
		);
		//---------------------------------------------------------------------
		(void)wcstombs_s(
			&stret,
			window_name,
			sizeof(window_name),
			tchar_window_name,
			_TRUNCATE
		);
		SET_INPUT_STRING_ADDRESS(
			INPUT_CLICKED_UI_OBJECT_NAME_INDEX,
			window_name
		);
		(void)wcstombs_s(
			&stret,
			class_name,
			sizeof(class_name),
			tchar_class_name,
			_TRUNCATE
		);
		SET_INPUT_STRING_ADDRESS(
			INPUT_CLICKED_UI_OBJECT_CLASS_NAME_INDEX,
			class_name
		);

		//---------------------------------------------------------------------
		// Get the root window too.
		//---------------------------------------------------------------------
		h_root = GetAncestor(
			h_clicked,
			GA_ROOT
		);
		//---------------------------------------------------------------------
		root_id = (unsigned long long int)h_clicked;
		SET_INPUT_ULL_VALUE(
			INPUT_CLICKED_UI_OBJECT_ROOT_ID_INDEX,
			root_id
		);

		//---------------------------------------------------------------------
		// Get clicked window's styles.
		//---------------------------------------------------------------------
		(void)GetWindowInfo(
			h_clicked,
			&wi
		);
		//---------------------------------------------------------------------
		style = (unsigned long long int)wi.dwStyle;
		SET_INPUT_ULL_VALUE(
			INPUT_CLICKED_UI_OBJECT_STYLE_INDEX,
			style
		);
		style_ex = (unsigned long long int)wi.dwExStyle;
		SET_INPUT_ULL_VALUE(
			INPUT_CLICKED_UI_OBJECT_EXTENDED_STYLE_INDEX,
			style_ex
		);

		//---------------------------------------------------------------------
		// Get the owning process image too.
		// Note:
		//    You may want to add support for immersive or universal
		// applications to this code.
		//---------------------------------------------------------------------
		tid = GetWindowThreadProcessId(
			h_root,
			&pid
		);
		process = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE,
			pid
		);
		if (process != NULL) {
			dwret = GetProcessImageFileName(
				process,
				tchar_image_name,
				sizeof(tchar_image_name)
			);
			bret = CloseHandle(process);
			process = NULL;
		}
		else {
			(void)_tcsncpy(
				tchar_image_name,
				_T("Unable To Open Process"),
				sizeof(tchar_image_name) / sizeof(TCHAR)
			);
		}
		//---------------------------------------------------------------------
		(void)wcstombs_s(
			&stret,
			image_name,
			sizeof(image_name),
			tchar_image_name,
			_TRUNCATE
		);
		SET_INPUT_STRING_ADDRESS(
			INPUT_CLICKED_UI_OBJECT_OWNING_PROCESS_IMAGE_INDEX,
			image_name
		);

		//---------------------------------------------------------------------
		// Trigger a log.
		//---------------------------------------------------------------------
		LOG_INPUT_VALUES;

		//---------------------------------------------------------------------
		// Save last data - so if filtering is needed in the future, the data
		// is already available.
		//---------------------------------------------------------------------
		last_h = h_clicked;
		(void)_tcsncpy(
			tchar_last_window_name,
			tchar_window_name,
			sizeof(tchar_window_name) / sizeof(TCHAR)
		);
		(void)_tcsncpy(
			tchar_last_class_name,
			tchar_class_name,
			sizeof(tchar_class_name) / sizeof(TCHAR)
		);
		(void)memcpy(
			&last_wi,
			&wi,
			sizeof(wi)
		);
	}

get_object_info_exit:

	return(0);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)

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
