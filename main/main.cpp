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

extern "C" {
   void app_main();
}

void test(){
	RMTRemote rmt = RMTRemote(3);
	rmt.clear();
	rmt.txStart(38);

	IRsend irsend;

    for(;;){
    irsend.sendNEC(0xFDCA35,24);
    vTaskDelay(2000 / 10);
    }
}


void app_main(void)
{

test();

}



