#include "IRremote.h"
#include "RMT.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"
#include "sdkconfig.h"
#include "IRRecvDebugger.cpp"

//IRsend irsend;
IRrecv irrecv(19);
decode_results  results;
static const char* MAIN_TAG = "MAIN";

extern "C" {
   void app_main();

}

/*
void sendIRTask(){
	vTaskDelay(10);
	esp_log_level_set(MAIN_TAG, ESP_LOG_INFO);

	ESP_LOGI(MAIN_TAG, "Iteration");

	for(;;){

	irsend.sendNEC(0xFDCA35, 32);
	ESP_LOGI(MAIN_TAG, "Sent NEC");
    irsend.sendSignal();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
	}

}
*/


void receiveIRTask(){
	irrecv.enableIRIn();
	for(;;){
	decode_results  results;
	  if (irrecv.decode(&results)) {  // Grab an IR code
	    dumpInfo(&results);           // Output the results
	    dumpRaw(&results);            // Output the results in RAW format
	    dumpCode(&results);           // Output the results as source code
	    printf("/n ");           // Blank line between entries
	    irrecv.resume();              // Prepare for the next value
	    vTaskDelay(100 / portTICK_PERIOD_MS);
	    printf("Test ");           // Blank line between entries
	  }
	}
}


void app_main(void)
{

	receiveIRTask();
	//xTaskCreate(&sendIrTask, "sendIRTask", 2048, NULL, 10, NULL);
	//sendIRTask();
}



