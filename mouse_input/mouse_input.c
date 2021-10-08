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
#include "incrementing_input.h"
#include "service.h"
#include "kalman.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global counter.
//-----------------------------------------------------------------------------
unsigned long long int mouse_x = 0;
unsigned long long int mouse_y = 0;
unsigned long long int noisy_mouse_x = 0;
unsigned long long int noisy_mouse_y = 0;
KALMAN_STATE kalman_state_x = { 0 };
KALMAN_STATE kalman_state_y = { 0 };

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
	size_t so
) {

	ESRV_STATUS ret = ESRV_FAILURE;

	assert(p != NULL);
	assert(pfd != NULL);
	assert(pfe != NULL);

	if (
		(po != NULL) &&
		(*po != '\0')
	) {

		ret = kalman_1d_parse_options(
			&kalman_state_x,
			po,
			so
		);
		if (ret != ESRV_SUCCESS) {
			goto modeler_init_inputs_error;
		}

		kalman_state_y.kalman_gain = kalman_state_x.kalman_gain;

		kalman_state_y.measurement_noise_covariance =
			kalman_state_x.measurement_noise_covariance;

		kalman_state_y.noise_covariance =
			kalman_state_x.noise_covariance;

		kalman_state_y.predicted_value =
			kalman_state_x.predicted_value;

		kalman_state_y.prediction_error_covariance =
			kalman_state_x.prediction_error_covariance;

		ret = kalman_1d_init(
			&kalman_state_x,
			INITIAL_NOISE_COVARIANCE,
			INITIAL_MEASUREMENT_NOISE_COVARIANCE,
			INITIAL_PREDICTED_VALUE,
			INITIAL_PREDICTION_ERROR_COVARIANCE,
			INITIAL_KALMAN_GAIN
		);
		if (ret != ESRV_SUCCESS) {
			goto modeler_init_inputs_error;
		}

		ret = kalman_1d_init(
			&kalman_state_y,
			INITIAL_NOISE_COVARIANCE,
			INITIAL_MEASUREMENT_NOISE_COVARIANCE,
			INITIAL_PREDICTED_VALUE,
			INITIAL_PREDICTION_ERROR_COVARIANCE,
			INITIAL_KALMAN_GAIN
		);
		if (ret != ESRV_SUCCESS) {
			goto modeler_init_inputs_error;
		}
	}

	srand((unsigned)time(NULL));
	
	SET_INPUTS_COUNT(INPUT_COUNT);

	return(ESRV_SUCCESS);
modeler_init_inputs_error:
	return(ESRV_FAILURE);
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
			ULL_COUNTER
		);
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(p)
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
extern ESRV_STATUS modeler_read_inputs(PINTEL_MODELER_INPUT_TABLE p) {

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

		assert(p != NULL);

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	BOOL bret = FALSE;
	POINT point = { 0 };
	int noise_x = 0;
	int noise_y = 0;
	int integer_value = 0;
	ESRV_STATUS ret = ESRV_FAILURE;

	//-------------------------------------------------------------------------
	// Get cursor position.
	//-------------------------------------------------------------------------

	bret = GetCursorPos(&point);
	if (bret == FALSE) {
		goto modeler_read_inputs_error;
	}
	mouse_x = (unsigned long long int)point.x;
	mouse_y = (unsigned long long int)point.y;

	//-------------------------------------------------------------------------
	// Set input values.
	//-------------------------------------------------------------------------
	noise_x = GET_SOME_NOISE;
	noise_y = GET_SOME_NOISE;

	integer_value = (int)mouse_x + noise_x;

	if (integer_value > 0) {
		noisy_mouse_x = integer_value;
	}
	else {
		noisy_mouse_x = mouse_x;
	}
	integer_value = (int)mouse_y + noise_y;
	if (integer_value > 0) {
		noisy_mouse_y = integer_value;
	}
	else {
		noisy_mouse_y = mouse_y;
	}

	ret = kalman_1d(
		&kalman_state_x,
		(double)noisy_mouse_x
	);
	if (ret != ESRV_SUCCESS) {
		goto modeler_read_inputs_error;
	}

	ret = kalman_1d(
		&kalman_state_y,
		(double)noisy_mouse_y
	);
	if (ret != ESRV_SUCCESS) {
		goto modeler_read_inputs_error;
	}

	SET_INPUT_ULL_VALUE(
		MOUSE_X_INPUT_INDEX,
		mouse_x
	);

	SET_INPUT_ULL_VALUE(
		MOUSE_Y_INPUT_INDEX,
		mouse_y
	);

	SET_INPUT_ULL_VALUE(
		MOUSE_NOISY_X_INPUT_INDEX,
		noisy_mouse_x
	);

	SET_INPUT_ULL_VALUE(
		MOUSE_NOISY_Y_INPUT_INDEX,
		noisy_mouse_y
	);

	SET_INPUT_ULL_VALUE(
		MOUSE_X_KALMAN_INPUT_INDEX,
		(unsigned long long int)kalman_state_x.predicted_value
	);

	SET_INPUT_ULL_VALUE(
		MOUSE_Y_KALMAN_INPUT_INDEX,
		(unsigned long long int)kalman_state_y.predicted_value
	);

	return(ESRV_SUCCESS);
modeler_read_inputs_error:
	return(ESRV_FAILURE);

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
extern ESRV_STATUS modeler_listen_inputs(PINTEL_MODELER_INPUT_TABLE p) {

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