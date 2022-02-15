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
#include <windows.h>
#include "pub_intel_modeler.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/*--------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Global variable.
//-----------------------------------------------------------------------------
DCTL_COMMAND dctl_command;
DCTL_COMMAND_DATA dctl_command_argument[
	ESRV_COMMAND_OPTION_LENGTH
] = { 0 };

//-----------------------------------------------------------------------------
// DCTL Defines.
//-----------------------------------------------------------------------------

#define DCTL_ARGUMENTS_SIZE \
	(20)

#define DCTL_NAME "WINDOW-CHANGED"
#define DCTL_CUSTOM_TOKEN "FOREGROUND-WINDOW-CHANGED"

#define DCTL_ARGUMENTS_SIZE \
	(20)

#define BUFFER_SIZE \
	(64)
