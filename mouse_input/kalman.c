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
#include "kalman.h"

//-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
Function: kalman_1d_init
Purpose : initialize Kalman state (1D).
In      : pointer to Kalman state data structure and seed data.
Out     : modified  Kalman state data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS kalman_1d_init(
	PKALMAN_STATE ps,
	double q, 
	double r, 
	double x, 
	double p,
	double k
) {

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(ps != NULL);

	ps->noise_covariance = q;
	ps->measurement_noise_covariance = r;
	ps->predicted_value = x;
	ps->prediction_error_covariance = p;
	ps->kalman_gain = k;

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

}

/*-----------------------------------------------------------------------------
Function: kalman_1d
Purpose : compute Kalman prediction (1D).
In      : pointer to Kalman state data structure and measurement.
Out     : modified  Kalman state data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS kalman_1d(PKALMAN_STATE ps, double measurement) {

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(ps != NULL);

	//-------------------------------------------------------------------------
	// Prediction update.
	//-------------------------------------------------------------------------
	ps->prediction_error_covariance = 
		ps->prediction_error_covariance + 
		ps->noise_covariance
	;

	//-------------------------------------------------------------------------
	// Measurement update.
	//-------------------------------------------------------------------------
	ps->kalman_gain = 
		ps->prediction_error_covariance / 
		(
			ps->prediction_error_covariance + 
			ps->measurement_noise_covariance
		)
	;
	ps->predicted_value = 
		ps->predicted_value + 
		ps->kalman_gain * 
		(
			measurement - 
			ps->predicted_value
		)
	;
	ps->prediction_error_covariance = 
		(1 - ps->kalman_gain) * 
		ps->prediction_error_covariance
	;

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

}

/*-----------------------------------------------------------------------------
Function: kalman_parse_options
Purpose : extract Kalman filter options and populate state structure.
In      : pointer to Kalman state data structure and to options string.
Out     : modified  Kalman state data structure.
Return  : status.
-----------------------------------------------------------------------------*/
ESRV_STATUS kalman_1d_parse_options(PKALMAN_STATE ps, char *po, size_t os) {

	//-------------------------------------------------------------------------
	// Note:
	//    options are in the form or '<param>=<real>[ <param>=<real>]'
	//    valid params are:
	//       noise_covariance, q
	//       measurement_noise_covariance, r
	//       prediction_error_covariance, p
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Generic variables.
	//-------------------------------------------------------------------------
	size_t length = 0;
	unsigned int i = 0;
	unsigned int t = 0;
	size_t memory_size = 0;
	double double_value = 0.0;
	ESRV_STATUS ret = ESRV_FAILURE;

	//-------------------------------------------------------------------------
	// Parsing variables.
	//-------------------------------------------------------------------------
	size_t l = 0;
	size_t l1 = 0;
	size_t l2 = 0;
	char *pc = NULL;
	char *token = NULL;
	char *value = NULL;
	char *buffer = NULL;
	unsigned int assigns_count = 0;
	char *tokens[MOUSE_KALMAN_OPTIONS_COUNT] = {
		MOUSE_KALMAN_OPTIONS
	};

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	assert(ps != NULL);
	assert(po != NULL);
	assert(po[0] != '\0');
	assert(os > 0);

	length = os;

	//-------------------------------------------------------------------------
	// Save a local copy of the options.
	//-------------------------------------------------------------------------
	memory_size = sizeof(char) * length;
	buffer = (char *)malloc(memory_size);
	if(buffer == NULL) {
		goto kalman_parse_options_error;
	}
	INPUT_DIAGNOSTIC_MALLOC(
		buffer,
		memory_size
	);
	(void)memset(
		buffer,
		0,
		memory_size
	);
	(void)memcpy(
		buffer,
		po,
		length
	);
	buffer[(memory_size - 1)] = '\0';
	//-------------------------------------------------------------------------
	memory_size = sizeof(char) * length;
	token = (char *)malloc(memory_size);
	if(token == NULL) {
		goto kalman_parse_options_error;
	}
	INPUT_DIAGNOSTIC_MALLOC(
		token,
		memory_size
	);
	(void)memset(
		token,
		0,
		memory_size
	);
	//-------------------------------------------------------------------------
	memory_size = sizeof(char) * length;
	value = (char *)malloc(memory_size);
	if(value == NULL) {
		goto kalman_parse_options_error;
	}
	INPUT_DIAGNOSTIC_MALLOC(
		value,
		memory_size
	);
	(void)memset(
		value,
		0,
		memory_size
	);

	//-------------------------------------------------------------------------
	// Set default initials.
	//-------------------------------------------------------------------------
	ret = kalman_1d_init(
		ps, 
		INITIAL_NOISE_COVARIANCE,
		INITIAL_MEASUREMENT_NOISE_COVARIANCE,
		INITIAL_PREDICTED_VALUE,
		INITIAL_PREDICTION_ERROR_COVARIANCE, 
		INITIAL_KALMAN_GAIN
	);
	if(ret != ESRV_SUCCESS) {
		goto kalman_parse_options_error;
	}

	//-------------------------------------------------------------------------
	// Find token/value pairs count.
	//-------------------------------------------------------------------------
	pc = buffer;
	pc = service_strip_string(buffer);
	service_trim_string(pc);
	assigns_count = (unsigned int)service_count_assign(pc);

	//-------------------------------------------------------------------------
	// Extract each token/value pairs.
	//-------------------------------------------------------------------------
	for(i = 0; i < assigns_count; i++) {
		pc = service_strip_string(pc);
		//---------------------------------------------------------------------
		ret = (ESRV_STATUS)service_get_pre_assign_token(
			pc,
			token
		);
		if(ret != ESRV_SUCCESS) {
			goto kalman_parse_options_error;
		}
		length = strlen(token);
		pc += length;
		(void)service_trim_string(token);
		(void)service_upper_string(token);
		l1 = strlen(token);
		//---------------------------------------------------------------------
		if(*pc++ != ESRV_SERVICE_ASSIGN) {
			goto kalman_parse_options_error;
		}
		//---------------------------------------------------------------------
		pc = service_strip_string(pc);
		ret = (ESRV_STATUS)service_get_post_assign_token(
			pc,
			value
		);
		if(ret != ESRV_SUCCESS) {
			goto kalman_parse_options_error;
		}
		length = strlen(value);
		pc += length;
		(void)service_trim_string(token);
		double_value = atof(value);

		//---------------------------------------------------------------------
		// Process token/value pair.
		//---------------------------------------------------------------------
		for(t = 0; t < MOUSE_KALMAN_OPTIONS_COUNT; t++) {
			l2 = strlen(tokens[t]);
			if(l1 < l2) {
				l = l2;
			} else {
				l = l1;
			}
			if(
				strncmp(
					token, 
					tokens[t], 
					l
				) == 0
			) {

				//-------------------------------------------------------------
				// Process options.
				//-------------------------------------------------------------
				switch(t) {

					case MOUSE_KALMAN_OPTIONS_Q_ID: // fall through
					case MOUSE_KALMAN_OPTIONS_NOISE_COVARIANCE_ID:
						// add value bound checks if needed.
						ps->noise_covariance = double_value;
						break;

					case MOUSE_KALMAN_OPTIONS_R_ID: // fall through
					case MOUSE_KALMAN_OPTIONS_MEASUREMENT_NOISE_COVARIANCE_ID:
						// add value bound checks if needed.
						ps->measurement_noise_covariance = double_value;
						break;

					case MOUSE_KALMAN_OPTIONS_P: // fall through
					case MOUSE_KALMAN_OPTIONS_PREDICTION_ERROR_COVARIANCE_ID:
						// add value bound checks if needed.
						ps->prediction_error_covariance = double_value;
						break;

					default:
						// unknown option
						goto kalman_parse_options_error;
				} // switch (each known token)
			}
		} // for t (each accepted tokens)

		//---------------------------------------------------------------------
		// Prepare next round.
		//---------------------------------------------------------------------
		(void)memset(
			token,
			0,
			memory_size
		);
		(void)memset(
			value,
			0,
			memory_size
		);
	} // for i (each assignments)

	//-------------------------------------------------------------------------
	// Free resources.
	//-------------------------------------------------------------------------
	if(buffer != NULL) {
		INPUT_DIAGNOSTIC_FREE(buffer);
		free(buffer);
		buffer = NULL;
	}
	//-------------------------------------------------------------------------
	if(token != NULL) {
		INPUT_DIAGNOSTIC_FREE(token);
		free(token);
		token = NULL;
	}
	//-------------------------------------------------------------------------
	if(value != NULL) {
		INPUT_DIAGNOSTIC_FREE(value);
		free(value);
		value = NULL;
	}

	return(ESRV_SUCCESS);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

kalman_parse_options_error:

	//-------------------------------------------------------------------------
	// Exception handling section begin.
	//-------------------------------------------------------------------------
	INPUT_BEGIN_EXCEPTIONS_HANDLING

	//-------------------------------------------------------------------------
	// House keeping.
	//-------------------------------------------------------------------------
	if(buffer != NULL) {
		INPUT_DIAGNOSTIC_FREE(buffer);
		free(buffer);
		buffer = NULL;
	}
	//-------------------------------------------------------------------------
	if(token != NULL) {
		INPUT_DIAGNOSTIC_FREE(token);
		free(token);
		token = NULL;
	}
	//-------------------------------------------------------------------------
	if(value != NULL) {
		INPUT_DIAGNOSTIC_FREE(value);
		free(value);
		value = NULL;
	}

	return(ESRV_FAILURE);

	//-------------------------------------------------------------------------
	// Exception handling section end.
	//-------------------------------------------------------------------------
	INPUT_END_EXCEPTIONS_HANDLING(NULL)

}
