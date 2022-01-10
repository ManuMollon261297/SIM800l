/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "peripherals.h"
#include "board.h"

#include "fsl_uart_freertos.h"
#include "fsl_uart.h"

// C program for implementation of ftoa()
#include <math.h>
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* UART instance and clock */
#define UART_RX_TX_IRQn UART3_RX_TX_IRQn
/* Task priorities. */
#define uart_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*  */
static void uart_task(void *pvParameters);
/*  */
void check_status(int error);
/*  */
bool getNetworkStatus(void);
/*  */
void enableGPRS(void);
/*  */
void test_sim_connection(void);
/*  */
void send_sms(void);
/*  */
void send_at_cmd(char *sent_str);
/*  */
bool sendGetRequest(char *get_request_string);
/*  */
void receive_cmd(char *sent_str);
/* */
int create_string(char *final_string, char *url_string, float pressure_value, float temp_value, float pm_value, float v_value, int node_value);
/* */
void ftoa(float n, char* res, int afterpoint);
/* */
int intToStr(int x, char str[], int d);
/* */
void reverse(char* str, int len);
/* */
bool parse_response(char *message_buffer, char *response);
/* */
bool shut_sim_connection(void);
/* */
void reset_sim(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t recv_buffer[1];
uint8_t message_buffer[100];

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */


char *apn = "internet.movil";
char *apnusername = "internet";
char *apnpassword = "internet";
char *useragent = "TUENTI_WEB";
char *ok_reply = "OK";


int main(void){
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    NVIC_SetPriority(UART_RX_TX_IRQn, 5);
    if (xTaskCreate(uart_task, "Uart_task", 512, NULL, uart_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Task responsible for loopback.
 */
static void uart_task(void *pvParameters){

	/* Setup */
	char *url_string="GET https://api.thingspeak.com/update\?api_key=0CM35ZLCLQ2JMTLG";
	float pressure_value= 50.5;
	float temp_value = 24.7;
	float pm_value = 0.678;
	float v_value = 3.657;
	int node_value = 1;

	char final_string[150] = {0};

	bool status_ok = false;

	create_string(final_string, url_string, pressure_value, temp_value, pm_value, v_value, node_value);

	do{
		status_ok = sendGetRequest(final_string);
	}while(!status_ok);

    vTaskSuspend(NULL);
}


void receive_cmd(char *message_buffer){
	size_t n = 0;
	uint8_t index = 0;
	uart_rtos_handle_t *handler = &UART3_rtos_handle;
	handler->rxTransfer.data     = recv_buffer;
	handler->rxTransfer.dataSize = (uint32_t)1;

	do{
	    UART_TransferReceiveNonBlocking(handler->base, handler->t_state, &handler->rxTransfer, &n);
		message_buffer[index]=recv_buffer[0];
		index++;
	}while(n !=0);

	message_buffer[index]=0;

	PRINTF("String: %s\n", message_buffer);

}

bool getNetworkStatus(void){

	bool status_ok;
	char *connection_response = ",1";
	/* Get Network Status */
	char *cmd_get_network_status = "AT+CREG?\n";
	send_at_cmd(cmd_get_network_status); // Expects OK reply if not send CIPSHUT
	vTaskDelay(pdMS_TO_TICKS( 1000.0 ));
	receive_cmd(message_buffer); // !sendParseReply(F("AT+CREG?"), F("+CREG: "), &status, ',', 1)
	status_ok = parse_response(message_buffer, connection_response);

	return status_ok;
}

bool sendGetRequest(char *get_request_string){

	bool status_ok;

	/* Test connectivity */
	do{
		status_ok = getNetworkStatus();
		vTaskDelay(pdMS_TO_TICKS( 10000.0 ));
	}while(!status_ok);

	char *ok_response = "OK";
	char *ip_response = ".";
	char *connect_response = "CONNECT OK";
	char *send_response = "SEND OK";
	char *close_response = "CLOSED";

	/* Set Single IP connection */
	char *cmd_set_mux = "AT+CIPMUX=0\n";
	send_at_cmd(cmd_set_mux); // Expects OK reply if not send CIPSHUT
	vTaskDelay(pdMS_TO_TICKS( 1000.0 ));
	receive_cmd(message_buffer);
	status_ok = parse_response(message_buffer, ok_response);
	if(!status_ok){
		reset_sim();
		return false;
	}


	/* Set APN */
	char *cmd_set_apn = "AT+CSTT=\"internet.movil\",\"internet\",\"internet\"\n";
	send_at_cmd(cmd_set_apn); // Expects OK reply  if not start again
	vTaskDelay(pdMS_TO_TICKS( 1000.0 ));
	receive_cmd(message_buffer);
	status_ok = parse_response(message_buffer, ok_response);
	if(!status_ok){
		reset_sim();
		return false;
	}

	/* Bring Up Wireless Connection with GPRS */
	char *cmd_set_gprs_conn = "AT+CIICR\n";
	send_at_cmd(cmd_set_gprs_conn); // Expects OK reply if not start again
	vTaskDelay(pdMS_TO_TICKS( 10000.0 ));
	receive_cmd(message_buffer);
	status_ok = parse_response(message_buffer, ok_response);
	if(!status_ok){
		reset_sim();
		return false;
	}

	/* Get local IP Address */
	char *cmd_get_ip = "AT+CIFSR\n";
	send_at_cmd(cmd_get_ip); // Expects IP return
	vTaskDelay(pdMS_TO_TICKS( 1000.0 ));
	receive_cmd(message_buffer);
	status_ok = parse_response(message_buffer, ip_response);
	if(!status_ok){
		reset_sim();
		return false;
	}

	/* Set Prompt of ‘>’ When Module Sends Data */
	char *cmd_set_prompt = "AT+CIPSPRT=0\n";
	send_at_cmd(cmd_set_prompt); // Expects OK reply
	vTaskDelay(pdMS_TO_TICKS( 1000.0 ));
	receive_cmd(message_buffer);
	status_ok = parse_response(message_buffer, ok_response);
	if(!status_ok){
		reset_sim();
		return false;
	}

	/* Start Up TCP Connection */
	char *cmd_start_tcp_conn = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\n";
	send_at_cmd(cmd_start_tcp_conn); // Expects OK reply and later a CONNECT OK
	vTaskDelay(pdMS_TO_TICKS( 8000.0 ));
	receive_cmd(message_buffer);
	status_ok = parse_response(message_buffer, ok_response);
	status_ok = parse_response(message_buffer, connect_response);
	if(!status_ok){
		reset_sim();
		return false;
	}

	/* Send Data Through TCP Connection */
	char *cmd_send_data = "AT+CIPSEND\n";
	send_at_cmd(cmd_send_data); // Doesnt expect response, wait for long time
	vTaskDelay(pdMS_TO_TICKS( 10000.0 ));
	receive_cmd(message_buffer);


	/* Send Data Through TCP Connection */
	char *cmd_enter = "\n";
	send_at_cmd(get_request_string);
	send_at_cmd(cmd_enter); // Doesn't expect response
	vTaskDelay(pdMS_TO_TICKS( 5000.0 ));
	receive_cmd(message_buffer);

	/* Send end of command */
	char cmd_eoc = 26;
	send_at_cmd(&cmd_eoc);
	send_at_cmd(cmd_enter); // Expects SEND OK a number and CLOSED
	vTaskDelay(pdMS_TO_TICKS( 5000.0 ));
	receive_cmd(message_buffer);
	status_ok = parse_response(message_buffer, send_response) && parse_response(message_buffer, close_response);
	if(!status_ok){
		// reset sim
		return false;
	}

	/* Close Connection */
	status_ok = shut_sim_connection();
	if(!status_ok){
		reset_sim();
		return false;
	}

	return true;

}

void reset_sim(void){
	/* Close Connection */
	bool status_ok = shut_sim_connection();
	if(!status_ok){
		// RST SIM
	}
}

bool shut_sim_connection(void){
	char *shut_response = "SHUT OK";
	char *cmd_disc_sockets = "AT+CIPSHUT\n";
	send_at_cmd(cmd_disc_sockets); // Expects SHUT OK reply
	vTaskDelay(pdMS_TO_TICKS( 5000.0 ));
	receive_cmd(message_buffer);
	return parse_response(message_buffer, shut_response);
}

bool parse_response(char *message_buffer, char *response){
	if(strstr(message_buffer, response) != NULL) {
	    return true;
	}
	else{
		return false;
	}
}

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}


// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

int create_string(char *final_string, char *url_string, float pressure_value, float temp_value, float pm_value, float v_value, float h_value, int node_value){

	char str_pressure_value[10];
	char str_temp_value[10];
	char str_pm_value[10];
	char str_v_value[10];
	char str_h_value[10];
	char str_node_value[10];

	ftoa(pressure_value, str_pressure_value, 2);
	ftoa(temp_value, str_temp_value, 2);
	ftoa(pm_value, str_pm_value, 2);
	ftoa(v_value, str_v_value, 2);
	ftoa(h_value, str_h_value, 2);
	sprintf(str_node_value, "%d", node_value);

    strcat(final_string, url_string);
    strcat(final_string, "&field1=");
    strcat(final_string, str_pm_value);
    strcat(final_string, "&field2=");
    strcat(final_string, str_temp_value);
    strcat(final_string, "&field3=");
    strcat(final_string, str_h_value);
    strcat(final_string, "&field4=");
    strcat(final_string, str_pressure_value);
    strcat(final_string, "&field5=");
    strcat(final_string, str_v_value);
    strcat(final_string, "&field6=");
    strcat(final_string, str_node_value);

    return strlen(final_string);

}


void send_at_cmd(char *sent_str){

	/* Send AT command */
    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *)sent_str, strlen(sent_str));

}

void send_sms(void){

    int error;
    int status;
    size_t n = 0;

    char *send_ring_overrun     = "\r\nRing buffer overrun!\r\n";
    char *send_hardware_overrun = "\r\nHardware buffer overrun!\r\n";
    char *cmd_sms_to_text_mode 	= "AT+CMGF=1\r";
    char *cmd_send_sms			= "AT+CMGS=\"";
    char *cmd_separator_one		= "\"\r";
    char *cmd_separator_two		= "\r";
    char *phone_number 			= "+5491124506362";
    char *text 					= "Hello World!";
    char end					= 26;

    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *)cmd_sms_to_text_mode, strlen(cmd_sms_to_text_mode));
    error = UART_RTOS_Receive(&UART3_rtos_handle, recv_buffer, sizeof(recv_buffer), &n);
    check_status(error);
    vTaskDelay(pdMS_TO_TICKS( 3000.0 ));

    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *)cmd_send_sms, strlen(cmd_send_sms));
    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *)phone_number, strlen(phone_number));
    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *)cmd_separator_one, strlen(cmd_separator_one));
    error = UART_RTOS_Receive(&UART3_rtos_handle, recv_buffer, sizeof(recv_buffer), &n);
    check_status(error);
    vTaskDelay(pdMS_TO_TICKS( 3000.0 ));

    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *)text, strlen(text));
    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *)cmd_separator_two, strlen(cmd_separator_two));
    error = UART_RTOS_Receive(&UART3_rtos_handle, recv_buffer, sizeof(recv_buffer), &n);
    check_status(error);
    vTaskDelay(pdMS_TO_TICKS( 3000.0 ));
    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *) &end, 1);
    error = UART_RTOS_Receive(&UART3_rtos_handle, recv_buffer, sizeof(recv_buffer), &n);
    check_status(error);

}

void test_sim_connection(void){

	int status;
	int n = 0;
	char *cmd_pin_ready      = "AT+CPIN?\r\n";

    UART_RTOS_Send(&UART3_rtos_handle, (uint8_t *)cmd_pin_ready, strlen(cmd_pin_ready));
    status = UART_RTOS_Receive(&UART3_rtos_handle, recv_buffer, strlen(cmd_pin_ready)+16, &n);
    check_status(status);
}

void enableGPRS(void){

	/* Disconnect all sockets */
	char *cmd_disc_sockets = "AT+CIPSHUT\r\n";
	send_at_cmd(cmd_disc_sockets); // Expects SHUT OK
	PRINTF("Sent: %s\r\n", cmd_disc_sockets);

    /* Set bearer profile connection type GPRS */
	char *cmd_set_gprs = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
	send_at_cmd(cmd_set_gprs); // Expects OK reply
	PRINTF("Sent: %s\r\n", cmd_set_gprs);

    /* Set APN */
	char *cmd_set_apn = "AT+SAPBR=3,1,\"APN\",\"internet.movil\"\r\n";
	send_at_cmd(cmd_set_apn); // Expects OK reply
	PRINTF("Sent: %s\r\n", cmd_set_apn);

    /* Set APN username */
	char *cmd_set_apn_username = "AT+SAPBR=3,1,\"USER\",\"internet\"\r\n";
	send_at_cmd(cmd_set_apn_username); // Expects OK reply
	PRINTF("Sent: %s\r\n", cmd_set_apn_username);

    /* Set APN password */
	char *cmd_set_apn_password = "AT+SAPBR=3,1,\"PWD\",\"internet\"\r\n";
	send_at_cmd(cmd_set_apn_password); // Expects OK reply
	PRINTF("Sent: %s\r\n", cmd_set_apn_password);

    /* Open GPRS context */
    char *cmd_open_gprs = "AT+SAPBR=1,1\r\n";
    send_at_cmd(cmd_open_gprs); // Expects OK reply
    PRINTF("Sent: %s\r\n", cmd_open_gprs);

    /* Query GPRS context */
    char *cmd_query_gprs = "AT+SAPBR=2,1\r\n";
    send_at_cmd(cmd_query_gprs); // Expects OK reply
    PRINTF("Sent: %s\r\n", cmd_query_gprs);

    /* After data transfer, the GPRS connection should be terminated with [AT+SAPBR=0,1] */
}

void disableGPRS(void){

    /* Close GPRS context */
    char *cmd_close_gprs = "AT+SAPBR=0,1\r\n";
    send_at_cmd(cmd_close_gprs); // Expects OK reply

}

void check_status(int error){
    if (error == kStatus_UART_RxHardwareOverrun)
    {
        /* Notify about hardware buffer overrun */
    	PRINTF("Hardware buffer overrun.\r\n");

    }
    if (error == kStatus_UART_RxRingBufferOverrun)
    {
        /* Notify about ring buffer overrun */
    	PRINTF("Ring buffer overrun.\r\n");
    }
}
