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

	// Was commented out earlier, uncommented to create a thread
	DWORD thread_id = 0;
	HANDLE h_thread = NULL;
	h_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		custom_event_listner_thread,
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


	HWND foregroundWindow_handle = GetForegroundWindow();
	

	DWORD processID = GetWindowThreadProcessId(
		HWND foregroundWindow_handle,
	);

	BOOL isImmersive = IsImmersiveProcess(
		HANDLE foregroundWindow_handle
	);

	HANDLE openProc OpenProcess(
		DWORD lpdwProcessID,
		BOOL  isImmersive,
		DWORD processID
	);

	LONG GetPackageFullName(
		HANDLE hProcess,
		UINT32* packageFullNameLength,
		PWSTR  packageFullName
	);



//-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
Function: custom_event_listner_thread
Purpose : implement the pure event listener thread.
In      : pointers to the input table (passed as void *).
Out     : modified input variables and time events list data.
Return  : status.
-----------------------------------------------------------------------------*/
//ESRV_API unsigned int __stdcall custom_event_listner_thread(void *px) {
//
//	//-------------------------------------------------------------------------
//	// Generic variables.
//	//-------------------------------------------------------------------------
//	DWORD dwret = 0;
//
//	//-------------------------------------------------------------------------
//	// Watchdog variables.
//	//-------------------------------------------------------------------------
//	WATCHDOG_VARIABLES
//
//	//-------------------------------------------------------------------------
//	// Access helper variables.
//	//-------------------------------------------------------------------------
//	PINTEL_MODELER_INPUT_TABLE p = NULL;
//
//	//-------------------------------------------------------------------------
//
//	//-------------------------------------------------------------------------
//	// Exception handling section begin.
//	//-------------------------------------------------------------------------
//	INPUT_BEGIN_EXCEPTIONS_HANDLING
//
//	assert(px != NULL);
//	p = (PINTEL_MODELER_INPUT_TABLE)px;
//
//	//-------------------------------------------------------------------------
//	// Name this thread (for debug mode only).
//	//-------------------------------------------------------------------------
//	INPUT_DIAGNOSTIC_NAME_THIS_THREAD(
//		p,
//		"custom_event_listner_thread"
//	);
//
//	//-------------------------------------------------------------------------
//	// Register this thread with watchdog.
//	//-------------------------------------------------------------------------
//	INPUT_REGISTER_EVENT_LOCKED_THREAD_WITH_WATCHDOG(
//		p,
//		"custom_event_listner_thread",
//		h_thread,
//		thread_id,
//		STOP_SIGNAL,
//		custom_event_listner_thread_exit
//	);
//
//	while(STOP_REQUEST == 0) {
//		//---------------------------------------------------------------------
//			// Pause to simulate event triggering.
//			// Note:
//			//    Rather than using a sleep, which would lock the event listener 
//			//    thread, we recommend using the method shown below. In general
//			//    developers of event-driven input libraries should add into the
//			//    end condition the event / semaphore via the STOP_SIGNAL macro 
//			//    (also - but not instead - use the STOP_REQUEST macro).
//			//---------------------------------------------------------------------
//		dwret = WaitForSingleObject(
//			STOP_SIGNAL,
//			INPUT_PAUSE_IN_MS //# 100ms
//		);
//		switch (dwret) {
//		case WAIT_OBJECT_0:
//			goto custom_event_listner_thread_exit; // time to leave!
//			break;
//		case WAIT_TIMEOUT:
//			c.cbSize = sizeof(c);
//			GetCursorInfo(&c);
//
//			for (int i = 0; i < sizeof(handles)/sizeof(handles[0]); i = i + 1) {
//				if (c.hCursor == handles[i]) {
//					icon = i;
//					break; 
//				}
//			}
//
//			break; // all good, time to make a measurement
//		default:
//			goto custom_event_listner_thread_exit; // error condition
//		} // switch
//
//		//---------------------------------------------------------------------
//		// Trigger logging.
//		//---------------------------------------------------------------------
//		LOG_INPUT_VALUES;
//
//		//---------------------------------------------------------------------
//		// Set input values.
//		//---------------------------------------------------------------------
//
//		SET_INPUT_ULL_VALUE(
//			INPUT_CURSOR_ICON,
//			icon
//		);
//	} // while
//
//	//-------------------------------------------------------------------------
//	// Un-register this thread with watchdog.
//	//-------------------------------------------------------------------------
//	INPUT_UNREGISTER_THREAD_WITH_WATCHDOG(
//		p,
//		h_thread,
//		thread_id
//	);
//
//custom_event_listner_thread_exit:
//	return(ESRV_FAILURE);
//
//	return(ESRV_SUCCESS);
//
//	//-------------------------------------------------------------------------
//	// Exception handling section end.
//	//-------------------------------------------------------------------------
//	INPUT_END_EXCEPTIONS_HANDLING(p)
//
//}
