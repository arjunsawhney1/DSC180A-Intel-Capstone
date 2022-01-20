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

//-----------------------------------------------------------------------------
// Global variables.
//-----------------------------------------------------------------------------
int counter = 0;
// string pattern to split by
const wchar_t s[3] = L"\\";

//-------------------------------------------------------------------------
// Custom event-listener variables.
//-------------------------------------------------------------------------
DWORD window_collector_thread_id = 0;
DWORD desktop_collector_thread_id = 0;
HANDLE h_window_collector_thread = NULL;
HANDLE h_desktop_collector_thread = NULL;

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
// Data structures.
//-------------------------------------------------------------------------
typedef struct _windows_structure {
	unsigned n_windows;
	HWND windows[100];
	LONG top_corners[100];
} WINDOWS_STRUCTURE, * PWINDOWS_STRUCTURE;

typedef struct _samples_structure {
	unsigned entry_count;
	PWINDOWS_STRUCTURE windows_entry;
} SAMPLES_STRUCTURE, * PSAMPLES_STRUCTURE;

WINDOWS_STRUCTURE windows_struct = { 0 };
SAMPLES_STRUCTURE samples_struct = { 0 };

// temp variables
HWND h_window_2 = NULL;
HWND h_window_3 = NULL;
LPRECT window_rect = { 0 };

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
	// Register RDCTLs.
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
	dctl_command = DCTL_QUERY_COMMAND;
	REGISTER_INPUT_LIBRARY_RDCTL(
		DCTL_QUERY_DCTL_NAME,
		DCTL_ALLOW_PARTIAL_TOKEN_MATCH,
		DCTL_QUERY_TOKEN,
		dctl_command,
		dctl_command_argument,
		dctl_answer
	);

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
	h_desktop_collector_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		map_desktop,
		(void*)p,
		0,
		(unsigned int*)&desktop_collector_thread_id
	);
	if (h_desktop_collector_thread == NULL) {
		goto modeler_process_dctl_exit;
	}
	//-------------------------------------------------------------------------
	h_window_collector_thread = (HANDLE)_beginthreadex(
		NULL,
		0,
		get_window_info,
		(void*)p,
		0,
		(unsigned int*)&window_collector_thread_id
	);
	if (h_window_collector_thread == NULL) {
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
unsigned int __stdcall map_desktop(void* pv) {

}

/*-----------------------------------------------------------------------------
Function: get_window_info
Purpose : measure and and store window data.
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
unsigned int __stdcall get_window_info(HWND window, void* pv) {

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
Function: get_process_image_name
Purpose : get the name of the open process (executable).
In      : none.
Out     : updated input data.
Return  : status.
-----------------------------------------------------------------------------*/
unsigned int __stdcall get_process_image_name(HWND window) {
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
	}
}
