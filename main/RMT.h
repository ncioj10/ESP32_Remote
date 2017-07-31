/*
 * RMT.h
 *
 *  Created on: Mar 1, 2017
 *      Author: kolban
 */

#ifndef MAIN_RMT_H
#define MAIN_RMT_H


#include "IRremoteInt.h"

#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
extern "C" {
#include "freertos/ringbuf.h"
}

#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"
#include <esp_log.h>
#include <driver/rmt.h>
#include <vector>

/**
 * @brief Drive the %RMT peripheral.
 */
class RMTRemote {



public:

	RMTRemote() = delete;
	RMTRemote(int usPerTick);
	virtual ~RMTRemote();
	void add(bool level, uint32_t duration);
	void clear();
	void rxStart(gpio_num_t rx_pin=GPIO_NUM_19, rmt_channel_t rx_channel=RMT_CHANNEL_2);
	void setIRRemote();
	void rxStop();
	void txStart(int frequency, gpio_num_t tx_pin = GPIO_NUM_16, rmt_channel_t tx_channel=RMT_CHANNEL_0);
	void txStop();
	void write();
	void collectInput();
	void processForIRRemote(rmt_item32_t* item,size_t& rx_size);
	static void printLog(char * log);


private:

	RingbufHandle_t rx_rb = NULL;
	int usPerTick;
	gpio_num_t tx_pin;
	gpio_num_t rx_pin;
	rmt_channel_t tx_channel;
	rmt_channel_t rx_channel;
	bool tx_driver_activated=false;
	bool rx_driver_activated=false;
	std::vector<rmt_item32_t> items;
	int bitCount = 0;
};

#endif /* COMPONENTS_CPP_UTILS_RMT_H_ */
