/*
 * R305 FingerPrint.c
 *
 *  Created on: Mar 8, 2021
 *      Author: Sajad
 */
#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include "R305_FingerPrint.h"
#include "usart.h"

//UART_HandleTypeDef huart1;

//////////////////////////////////////////////
void finger_transmit_header(uint16_t length){

	uint8_t Packet[10]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01};
	Packet[7]=(length & 0xFF) >> 8;
	Packet[8]=length & 0xFF;

	HAL_UART_Transmit(&huart1, Packet, 9, 10);
}

uint16_t finger_receive(){

	uint16_t len,ret[1];
	uint8_t Packet[10];
	uint8_t rett[1];

	HAL_UART_Receive(&huart1, Packet, 9, 10);

	len = Packet[7] >> 8 | Packet[8];
	len-=2;

	HAL_UART_Receive(&huart1, rett, 2, 2);
	if(len==0x0002){
		HAL_UART_Receive(&huart1, rett, 2, 2);
			ret[0] = rett[0] >> 8;
		}else if(len==0x0003){
			if(ret==0x00){
				HAL_UART_Receive(&huart1, rett, 2, 2);
				ret[0] = rett[0] << 8 | rett[0];
			}
		}

	HAL_UART_Receive(&huart1, Packet, 2, 10);

	return ret[0];
}


uint16_t finger_search_receive(){

	uint8_t ret[2];
	uint8_t Packet[10];
	uint8_t temp[1];
	uint16_t a;

	HAL_UART_Receive(&huart1, Packet, 9, 10);


	HAL_UART_Receive(&huart1, temp, 1, 10);

	//pageid
	if(temp[0]==0x01){
		uint8_t low[1];
		uint8_t high[1];

		HAL_UART_Receive(&huart1, low, 1, 10);
		HAL_UART_Receive(&huart1, high, 1, 10);

		ret[0] = (low[0] << 8) + high[0];
	}
	else if(temp[0]==0x09){
		ret[0] = 0;
		HAL_UART_Receive(&huart1, Packet, 2, 10);

	}
	else{
		HAL_UART_Receive(&huart1, ret, 2, 10);
		ret[1] = ret[1] << 8;
		a = ret[1] + ret[0];

	}

	HAL_UART_Receive(&huart1, Packet, 4, 10);

	return a;
	//return temp[0];
}


char* finger_get_response_string(uint8_t code){
	switch(code){
		case 0x00: return "complete";
		case 0x01: return "error receiving data";
		case 0x02: return "no finger";
		case 0x03: return "fail to enroll";
		case 0x06: return "disorderly fingerprint image";
		case 0x07: return "small fingerprint image";
		case 0x08: return "finger doesn’t match";
		case 0x09: return "fail to find the matching";
		case 0x0A: return "fail to combine";
		case 0x0B: return "invalid PageID";
		case 0x0C: return "invalid template";
		case 0x0D: return "error uploading template";
		case 0x0E: return "can’t data packages.";
		case 0x0F: return "error uploading image";
		case 0x10: return "fail to delete template";
		case 0x11: return "fail to clear library";
		case 0x13: return "wrong password!";
		case 0x15: return "invalid primary image";
		case 0x18: return "error writing flash";
		case 0x19: return "undefined error";
		case 0x1A: return "invalid register";
		case 0x1B: return "incorrect configuration";
		case 0x1C: return "wrong notepad page";
		case 0x1D: return "failed communication port";
	}
	return "Unknown error";
}


uint16_t finger_read(){
	uint16_t code = 2;

	uint8_t tr[5];


	while(code!=0x0000){
		finger_transmit_header(0x0003);

		tr[0] = 0x01;
		HAL_UART_Transmit(&huart1, tr, 1, 10);

		tr[0] = 0x00;
		HAL_UART_Transmit(&huart1, tr, 1, 10);
		tr[0] = 0x05;
		HAL_UART_Transmit(&huart1, tr, 1, 10);


		tr[1] = tr[1] << 8;
		code = tr[1] + tr[0];

		HAL_Delay(500);
	}

	return code;
}


uint16_t upImg(){
	uint8_t packet;
	finger_transmit_header(0x0003);
	packet = 0x0A;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x00;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x0E;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);

	return finger_receive();
}


uint16_t upChar(uint16_t buff_id){

	uint8_t packet;
	finger_transmit_header(0x0004);

	packet = 0x08;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);

	uint8_t highbuff = buff_id >> 8 ;
	uint8_t lowbuff = buff_id & 0xFF ;
	HAL_UART_Transmit(&huart1, &highbuff, 1, 10);
	HAL_UART_Transmit(&huart1, &lowbuff, 1, 10);



	buff_id += 7;
	highbuff = buff_id >> 8;
	lowbuff = buff_id & 0xFF ;
	HAL_UART_Transmit(&huart1, &highbuff, 1, 10);
	HAL_UART_Transmit(&huart1, &lowbuff, 1, 10);

	return finger_receive();
}


uint16_t finger_generate_char_file(uint16_t buff_id){

	uint8_t packet;
	finger_transmit_header(0x0004);

	packet = 0x02;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);

	uint8_t highbuff = buff_id >> 8 ;
	uint8_t lowbuff = buff_id & 0xFF ;
	HAL_UART_Transmit(&huart1, &highbuff, 1, 10);
	HAL_UART_Transmit(&huart1, &lowbuff, 1, 10);



	buff_id += 7;
	highbuff = buff_id >> 8;
	lowbuff = buff_id & 0xFF ;
	HAL_UART_Transmit(&huart1, &highbuff, 1, 10);
	HAL_UART_Transmit(&huart1, &lowbuff, 1, 10);

	return finger_receive();
}


uint16_t finger_generate_template(){

	uint8_t packet;
	finger_transmit_header(0x0003);

	packet = 0x05;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x00;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x09;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);


	return finger_receive();
}


uint16_t finger_get_storage_location(){

	uint8_t packet;
	finger_transmit_header(0x0003);

	packet = 0x1d;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x00;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x21;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);


	return finger_receive();
}


uint16_t finger_delete_all(){

	uint8_t packet;
	finger_transmit_header(0x0003);

	packet = 0x0d;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x00;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x11;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);


	return finger_receive();
}


uint16_t finger_store(uint16_t location){

	uint8_t packet;
	finger_transmit_header(0x0006);

	packet = 0x06;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x01;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);


	uint8_t highloc= location >> 8 ;
	uint8_t lowloc = location & 0xFF ;
	HAL_UART_Transmit(&huart1, &highloc, 1, 10);
	HAL_UART_Transmit(&huart1, &lowloc, 1, 10);



	location += 14;
	highloc = location >> 8;
	lowloc = location & 0xFF ;
	HAL_UART_Transmit(&huart1, &highloc, 1, 10);
	HAL_UART_Transmit(&huart1, &lowloc, 1, 10);


	return finger_receive();
}


uint16_t finger_delete(uint16_t location){

	uint8_t packet;
	finger_transmit_header(0x0007);

	packet = 0x0C;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);

	uint8_t highloc= location >> 8 ;
	uint8_t lowloc = location & 0xFF ;
	HAL_UART_Transmit(&huart1, &highloc, 1, 10);
	HAL_UART_Transmit(&huart1, &lowloc, 1, 10);

	packet = 0x00;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x01;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);


	location += 21;
	highloc = location >> 8;
	lowloc = location & 0xFF ;
	HAL_UART_Transmit(&huart1, &highloc, 1, 10);
	HAL_UART_Transmit(&huart1, &lowloc, 1, 10);


	return finger_receive();
}


uint16_t finger_search(){

	uint8_t packet;
	finger_transmit_header(0x0008);

	packet = 0x04;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);

	packet = 0x01;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);

	packet = 0x00;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x01;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);

	packet = 0x00;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0xFF;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);

	packet = 0x01;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);
	packet = 0x0E;
	HAL_UART_Transmit(&huart1, &packet, 1, 10);


	return finger_search_receive();
}


