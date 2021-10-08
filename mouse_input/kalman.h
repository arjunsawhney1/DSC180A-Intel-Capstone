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

#ifndef __INCLUDE_KALMAN__
#define __INCLUDE_KALMAN__

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
// Kalman related defines.
//-----------------------------------------------------------------------------
#define MOUSE_KALMAN_OPTIONS_SEPARATOR_CHAR ','
#define MOUSE_KALMAN_OPTIONS_SEPARATOR_STRING ","
#define MOUSE_KALMAN_OPTIONS_ASSIGN_CHAR '='
#define MOUSE_KALMAN_OPTIONS_ASIGN_STRING "="
#define MOUSE_KALMAN_OPTIONS_COUNT 6
#define MOUSE_KALMAN_OPTIONS \
	"NOISE_COVARIANCE", \
	"Q", \
	"MEASUREMENT_NOISE_COVARIANCE", \
	"R", \
	"PREDICTION_ERROR_COVARIANCE", \
	"P"
//-------------------------------------------------------------------------
#define MOUSE_NOISE_RANGE_MAX (40)
#define MOUSE_NOISE_RANGE_MIN \
	(-MOUSE_NOISE_RANGE_MAX)
#define INITIAL_NOISE_COVARIANCE \
	(0.125) 
#define INITIAL_MEASUREMENT_NOISE_COVARIANCE \
	(0.30)
#define INITIAL_PREDICTED_VALUE \
	(0.0)
#define INITIAL_PREDICTION_ERROR_COVARIANCE \
	(10.0)
#define INITIAL_KALMAN_GAIN \
	(4.0)

//-----------------------------------------------------------------------------
// Enums and structures.
//-----------------------------------------------------------------------------
typedef enum _kalman_options_ids {

	MOUSE_KALMAN_OPTIONS_NOISE_COVARIANCE_ID,
	MOUSE_KALMAN_OPTIONS_Q_ID,
	MOUSE_KALMAN_OPTIONS_MEASUREMENT_NOISE_COVARIANCE_ID,
	MOUSE_KALMAN_OPTIONS_R_ID,
	MOUSE_KALMAN_OPTIONS_PREDICTION_ERROR_COVARIANCE_ID,
	MOUSE_KALMAN_OPTIONS_P

} KALMAN_OPTIONS_ID, *PKALMAN_OPTIONS_ID;

typedef struct _kalman_state {

	double noise_covariance; //q
	double measurement_noise_covariance; //r
	double predicted_value; //x
	double prediction_error_covariance; // p
	double kalman_gain; //k

} KALMAN_STATE, *PKALMAN_STATE;

//-----------------------------------------------------------------------------
// Function prototypes.
//-----------------------------------------------------------------------------
ESRV_STATUS kalman_1d_init(
	PKALMAN_STATE, 
	double, 
	double, 
	double, 
	double,
	double
);
ESRV_STATUS kalman_1d(PKALMAN_STATE, double);
ESRV_STATUS kalman_1d_parse_options(PKALMAN_STATE, char *, size_t);
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INCLUDE_KALMAN__
