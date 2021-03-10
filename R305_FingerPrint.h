/*
 * R305 FingerPrint.h
 *
 *  Created on: Mar 8, 2021
 *      Author: Sajad
 */

#ifndef INC_R305_FINGERPRINT_H_
#define INC_R305_FINGERPRINT_H_


#ifdef __cplusplus
 extern "C" {
#endif

void	 finger_init();							// Initialize the finger print sensor. Use inside main function
uint16_t finger_read();							// reading fingerprint
uint16_t finger_generate_char_file(uint16_t);	// generate char file from buffer. send buff id 0x01 or 0x02
uint16_t finger_generate_template();			// comabine buffers to generate template
uint16_t finger_get_storage_location();			// get pageId to store the template in flash storage
char*	 finger_get_response_string(uint8_t);	// get the string representation of response codes
uint16_t finger_delete_all();					// delete all the templates in flash store
uint16_t finger_store(uint16_t);				// stores fingerprint template at given location. send location from finger_get_storage_location();
uint16_t finger_delete(uint16_t);				// delets the template at the location. send location to delete
uint16_t finger_search();						// search flash sotreage for matching templates

void enroll_finger(); //sequence for enrolling a finger

uint16_t upImg();
uint16_t upChar(uint16_t);

#ifdef __cplusplus
}
#endif

#endif /* INC_R305_FINGERPRINT_H_ */
