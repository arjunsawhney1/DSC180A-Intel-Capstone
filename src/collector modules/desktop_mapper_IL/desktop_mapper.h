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

#ifndef __INCLUDE_PURE_EVENT_DRIVEN_INCREMENTING_INPUT__
#define __INCLUDE_PURE_EVENT_DRIVEN_INCREMENTING_INPUT__

//-----------------------------------------------------------------------------
// Headers inclusions.
//-----------------------------------------------------------------------------
#include <windows.h>
#include "pub_intel_modeler.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/*--------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Defines.
//-----------------------------------------------------------------------------
#define STRING_BUFFERS_SIZE 1024
#define RECT_BUFFERS_SIZE 32
#define MAX_MPLEX_LOGGER_CHECKS 10
#define MAX_WINDOWS 10
#define WAIT_FOR_MULTIPLEX_LOGGER_TIME_IN_MS 1000
#define LOGGER_MAX_LOG_TRIES 10
#define LOG_RETRY_PAUSE_IN_MS 100
#define INPUT_PAUSE_IN_MS 1000 // does not accept floating point

#define INPUT_COUNT 10
#define INPUT_CURR_WINDOW_EXECUTABLE 0
#define INPUT_NEXT_WINDOW_EXECUTABLE 1
#define INPUT_PREV_WINDOW_EXECUTABLE 2
#define INPUT_FOREGROUND_WINDOW_EXECUTABLE 3
#define INPUT_WINDOW_RECT 4
#define INPUT_WINDOW_PLACEMENT 5
#define INPUT_IS_VISIBLE 6
#define INPUT_IS_MINIMIZED 7
#define INPUT_IS_MAXIMIZED 8
#define INPUT_IS_HUNG 9

#define INPUT_NAME_STRING "DESKTOP-MAPPER"
#define MY_INPUT_CLOSE_ERROR_STRINGS \
	"Dynamic Error 1"

#define INPUT_DESCRIPTION_STRINGS \
	"Current Window Executable Name", \
	"Next Window Executable Name", \
	"Previous Window Executable Name", \
	"Foreground Window Executable Name", \
	"Current Window Rectangle", \
	"Current Window Placement", \
	"Current Window is Visible", \
	"Current Window is Minimized", \
	"Current Window is Maximized", \
	"Current Window is Hung"

#define INPUT_TYPES \
	STRING_COUNTER, \
	STRING_COUNTER, \
	STRING_COUNTER, \
	STRING_COUNTER, \
	STRING_COUNTER, \
	STRING_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER, \
	ULL_COUNTER

#define WAIT_EVENTS_COUNT (2)
	#define STOP_EVENT_INDEX (0)
	#define FOREGROUND_WINDOW_CHANGE_INDEX (1)

//-----------------------------------------------------------------------------
// Structs
//-----------------------------------------------------------------------------
typedef struct {
	DWORD ownerpid;
	DWORD childpid;
} windowinfo;

typedef struct _windows_structure {
	HWND currentWindow; //1024*2 bytes;
	TCHAR executable[STRING_BUFFERS_SIZE]; //1024*2 bytes
	TCHAR nextWindow[STRING_BUFFERS_SIZE]; //1024*2 bytes;
	TCHAR prevWindow[STRING_BUFFERS_SIZE]; //1024*2 bytes;
	TCHAR foregroundWindow[STRING_BUFFERS_SIZE]; //1024*2 bytes;
	BOOL isVisible;
	BOOL isMinimized;
	BOOL isZoomed;
	BOOL isHung;
	TCHAR windowRect[RECT_BUFFERS_SIZE]; //32*2 bytes
	TCHAR clientRect[RECT_BUFFERS_SIZE]; //32*2 bytes
	TCHAR windowPlacement[RECT_BUFFERS_SIZE];
	LONG_PTR style;
	LONG_PTR style_ex;
	HMONITOR monitor;
	MONITORINFO monitorInfo;
	INPUT_DWORD_PRIVATE_DATA index;
	SYSTEMTIME time;
} WINDOWS_STRUCTURE, * PWINDOWS_STRUCTURE;

//-----------------------------------------------------------------------------
// Child Windows Callback Function.
//-----------------------------------------------------------------------------

BOOL CALLBACK EnumChildWindowsCallback(HWND hWnd, LPARAM lp) {
	windowinfo* info = (windowinfo*)lp;
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	if (pid != info->ownerpid) info->childpid = pid;
	return TRUE;
}

//-----------------------------------------------------------------------------
// Function prototypes.
//-----------------------------------------------------------------------------
ESRV_API ESRV_STATUS modeler_init_inputs(
	unsigned int *, 
	int *, 
	int *, 
	char *,
	size_t
);
ESRV_API ESRV_STATUS modeler_open_inputs(PINTEL_MODELER_INPUT_TABLE);
ESRV_API ESRV_STATUS modeler_close_inputs(PINTEL_MODELER_INPUT_TABLE);
ESRV_STATUS modeler_read_inputs(PINTEL_MODELER_INPUT_TABLE);
ESRV_STATUS modeler_listen_inputs(PINTEL_MODELER_INPUT_TABLE);
ESRV_STATUS modeler_process_dctl(PINTEL_MODELER_INPUT_TABLE);
ESRV_STATUS modeler_process_lctl(PINTEL_MODELER_INPUT_TABLE);
//-----------------------------------------------------------------------------
ESRV_API unsigned int __stdcall custom_desktop_thread(void*);
ESRV_API unsigned int __stdcall custom_logger_thread(void*);
ESRV_STATUS get_window_info(WINDOWS_STRUCTURE*);

void get_process_image_name(HWND, TCHAR*, DWORD);
void map_desktop();
ESRV_STATUS multiplex_logging(PINTEL_MODELER_INPUT_TABLE, WINDOWS_STRUCTURE*);

/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INCLUDE_PURE_EVENT_DRIVEN_INCREMENTING_INPUT__
