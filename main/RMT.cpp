/*
 * RMT.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: kolban
 */

#include "RMT.h"

static const char* RMT_TAG = "RMT";

//static char tag[] = "RMT";
/**
 * @brief Create a class instance.
 *
 * @param [in] pin The GPIO pin on which the signal is sent/received.
 * @param [in] channel The RMT channel to work with.
 */

#if RMT_RX_SELF_TEST
#define RMT_RX_ACTIVE_LEVEL  1   /*!< Data bit is active high for self test mode */
#define RMT_TX_CARRIER_EN    0   /*!< Disable carrier for self test mode  */
#else
//Test with infrared LED, we have to enable carrier for transmitter
//When testing via IR led, the receiver waveform is usually active-low.
#define RMT_RX_ACTIVE_LEVEL  0   /*!< If we connect with a IR receiver, the data is active low */
#define RMT_TX_CARRIER_EN    1   /*!< Enable carrier for IR transmitter test with IR led */
#endif

#define RMT_TX_CHANNEL    1     /*!< RMT channel for transmitter */
#define RMT_TX_GPIO_NUM  16     /*!< GPIO number for transmitter signal */
#define RMT_RX_CHANNEL    0     /*!< RMT channel for receiver */
#define RMT_RX_GPIO_NUM  19     /*!< GPIO number for receiver */
#define RMT_CLK_DIV      100    /*!< RMT counter clock divider */
#define RMT_TICK_10_US    (80000000/RMT_CLK_DIV/100000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) */

#define rmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value(us) */

RMTRemote::RMTRemote(int usPerTick) {
	esp_log_level_set(RMT_TAG, ESP_LOG_INFO);
	this->usPerTick = usPerTick;
}

/**
 * @brief Class destructor.
 */
RMTRemote::~RMTRemote() {
	ESP_ERROR_CHECK(::rmt_driver_uninstall(this->tx_channel));
	ESP_ERROR_CHECK(::rmt_driver_uninstall(this->rx_channel));
}

/**
 * @brief Start receiving.
 */
void RMTRemote::rxStart(gpio_num_t rx_pin, rmt_channel_t rx_channel) {
	if (!rx_driver_activated) {
		ESP_LOGD(RMT_TAG, "Called RMT rxStart");

		this->rx_pin = rx_pin;
		this->rx_channel = rx_channel;
		rmt_config_t rmt_rx;
		rmt_rx.channel = rx_channel;
		rmt_rx.gpio_num = rx_pin;
		rmt_rx.clk_div = RMT_CLK_DIV;
		rmt_rx.mem_block_num = 2;
		rmt_rx.rmt_mode = RMT_MODE_RX;
		rmt_rx.rx_config.filter_en = true;
		rmt_rx.rx_config.filter_ticks_thresh = 100;
		rmt_rx.rx_config.idle_threshold = rmt_item32_tIMEOUT_US / 10
				* (RMT_TICK_10_US);
		rmt_config(&rmt_rx);
		ESP_ERROR_CHECK(rmt_driver_install(rmt_rx.channel, 2000, 0));
		ESP_ERROR_CHECK(rmt_rx_start(this->rx_channel, true));

		rx_driver_activated = true;
	}
}

/**
 * @brief Stop receiving.
 */
void RMTRemote::rxStop() {
	ESP_ERROR_CHECK(rmt_rx_stop(this->rx_channel));
}

/**
 * @brief Start transmitting. Frequency is measured in kHz
 */
void RMTRemote::txStart(int frequency, gpio_num_t tx_pin,
		rmt_channel_t tx_channel) {
	if (!tx_driver_activated) {
		ESP_LOGD(RMT_TAG, "Called RMT txStart");
		//rmt_driver_uninstall(this->tx_channel);
		this->tx_pin = tx_pin;
		this->tx_channel = tx_channel;
		rmt_config_t rmt_tx;
		rmt_tx.channel = tx_channel;
		rmt_tx.gpio_num = tx_pin;
		rmt_tx.mem_block_num = 2;
		rmt_tx.clk_div = RMT_CLK_DIV;
		rmt_tx.tx_config.loop_en = false;
		rmt_tx.tx_config.carrier_duty_percent = 50;
		rmt_tx.tx_config.carrier_freq_hz = frequency * 1000;
		rmt_tx.tx_config.carrier_level = (rmt_carrier_level_t) 1;
		rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
		rmt_tx.tx_config.idle_level = (rmt_idle_level_t) 0;
		rmt_tx.tx_config.idle_output_en = true;
		rmt_tx.rmt_mode = (rmt_mode_t) 0;

		ESP_ERROR_CHECK(rmt_config(&rmt_tx));
		ESP_ERROR_CHECK(rmt_driver_install(this->tx_channel, 0, 0));
		ESP_ERROR_CHECK(rmt_tx_start(this->tx_channel, true));
		tx_driver_activated = true;
	}
}

void RMTRemote::collectInput() {

	//get RMT RX ringbuffer
	if (rx_rb == NULL) {
		rmt_get_ringbuf_handler(rx_channel, &rx_rb);
		rmt_rx_start(rx_channel, 1);
	}
	irparams.rawlen = 0;
	//TODO Init with smart data?
	irparams.rawbuf[irparams.rawlen++] = 5000;
	if (rx_rb) {
		size_t rx_size = 0;
		//try to receive data from ringbuffer.
		//RMT driver will push all the data it receives to its ringbuffer.
		//We just need to parse the value and return the spaces of ringbuffer.
		rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rx_rb, &rx_size,
				1000);

		if (item) {
			irparams.rcvstate = STATE_RECEIVING;
			processForIRRemote(item, rx_size);
			//after parsing the data, return spaces to ringbuffer.
			vRingbufferReturnItem(rx_rb, (void*) item);
			irparams.rcvstate = STATE_STOP;
		} else {
			irparams.rcvstate = STATE_NOBUF;
		}

	}
	//vTaskDelete(NULL);

}

void RMTRemote::processForIRRemote(rmt_item32_t* item, size_t &rx_size) {
	for (int i = 0; i < rx_size / 4; i++) {
		uint16_t duration0TicksConv = ((item->duration0 & 0x7fff) * 10
				/ RMT_TICK_10_US) / usPerTick;
		;
		uint16_t duration1TicksConv = ((item->duration1 & 0x7fff) * 10
				/ RMT_TICK_10_US) / usPerTick;

		irparams.rawbuf[irparams.rawlen++] = duration0TicksConv;
		ESP_LOGD(RMT_TAG, "Rawlength in preprocess1: %d", irparams.rawlen);
		irparams.rawbuf[irparams.rawlen++] = duration1TicksConv;
		ESP_LOGD(RMT_TAG, "Rawlength in preprocess2: %d", irparams.rawlen);
		ESP_LOGD(RMT_TAG,
				"RMT RCV --- item-duratio0-raw: %d item-duratio0-conv: %d item-duration1-raw %d item-duration1-conv %d",
				item->duration0, duration0TicksConv, item->duration1,
				duration1TicksConv);
		item++;
	}
}

/**
 * @brief Stop transmitting.
 */
void RMTRemote::txStop() {
	ESP_ERROR_CHECK(::rmt_tx_stop(this->tx_channel));
}

/**
 * @brief Write the items out through the RMT.
 *
 * The level/duration set of bits that were added to the transaction are written
 * out through the RMT device.  After transmission, the list of level/durations
 * is cleared.
 */
void RMTRemote::write() {
	ESP_LOGD(RMT_TAG, "RMTRemote::write executed");
	//add(false, 0);
	for (int i = 0; i < items.size(); i++) {
		ESP_LOGD(RMT_TAG, "Time %d : %d, %d value: %d %d", i,
				items[i].duration0, items[i].duration1, items[i].level0,
				items[i].level1);
	}
	ESP_ERROR_CHECK(
			rmt_write_items(this->tx_channel, &items[0], items.size(), true));
	rmt_wait_tx_done(tx_channel);
	clear();
}

/**
 * @brief Add a level/duration to the transaction to be written.
 *
 * @param [in] level The level of the bit to output.
 * @param [in] duration The duration of the bit to output.
 */
void RMTRemote::add(bool level, uint32_t duration) {
	ESP_LOGD(RMT_TAG, "RMTRemote::add executed");
	if (bitCount % 2 == 0) {
		rmt_item32_t item;
		item.level0 = level;
		item.duration0 = duration / 10 * RMT_TICK_10_US;
		items.push_back(item);
	} else {
		items.at(bitCount / 2).level1 = level;
		items.at(bitCount / 2).duration1 = duration / 10 * RMT_TICK_10_US;
	}
	bitCount++;
}

/**
 * @brief Clear any previously written level/duration pairs that have not been sent.
 */
void RMTRemote::clear() {
	ESP_LOGD(RMT_TAG, "RMTRemote::clear executed");

	items.clear();
	bitCount = 0;
}

