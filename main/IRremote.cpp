//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Copyright 2009 Ken Shirriff
// For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
//
// Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
// Modified  by Mitra Ardron <mitra@mitra.biz>
// Added Sanyo and Mitsubishi controllers
// Modified Sony to spot the repeat codes that some Sony's send
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
// JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
// LG added by Darryl Smith (based on the JVC protocol)
// Whynter A/C ARC-110WD added by Francesco Meschia
//******************************************************************************

// Defining IR_GLOBAL here allows us to declare the instantiation of global variables
#define IR_GLOBAL
#	include "IRremote.h"
#	include "IRremoteInt.h"
#	include "RMT.h"
#undef IR_GLOBAL

#ifdef ESP32
static const char* IRREMOTE_TAG = "IRRemote.cpp";
#endif

//+=============================================================================
// The match functions were (apparently) originally MACROs to improve code speed
//   (although this would have bloated the code) hence the names being CAPS
// A later release implemented debug output and so they needed to be converted
//   to functions.
// I tried to implement a dual-compile mode (DEBUG/non-DEBUG) but for some
//   reason, no matter what I did I could not get them to function as macros again.
// I have found a *lot* of bugs in the Arduino compiler over the last few weeks,
//   and I am currently assuming that one of these bugs is my problem.
// I may revisit this code at a later date and look at the assembler produced
//   in a hope of finding out what is going on, but for now they will remain as
//   functions even in non-DEBUG mode
//
int  MATCH (int measured,  int desired)
{
 	DBG_PRINT(F("Testing: "));
 	DBG_PRINT(TICKS_LOW(desired), DEC);
 	DBG_PRINT(F(" <= "));
 	DBG_PRINT(measured, DEC);
 	DBG_PRINT(F(" <= "));
 	DBG_PRINT(TICKS_HIGH(desired), DEC);

  bool passed = ((measured >= TICKS_LOW(desired)) && (measured <= TICKS_HIGH(desired)));
  if (passed)
    DBG_PRINTLN(F("?; passed"));
  else
    DBG_PRINTLN(F("?; FAILED")); 
 	return passed;
}

//+========================================================
// Due to sensor lag, when received, Marks tend to be 100us too long
//
int  MATCH_MARK (int measured_ticks,  int desired_us)
{
	ESP_LOGD(IRREMOTE_TAG,"Testing mark (actual vs desired): ");
	ESP_LOGD(IRREMOTE_TAG,"%d",measured_ticks * USECPERTICK);
	ESP_LOGD(IRREMOTE_TAG,"us vs ");
	ESP_LOGD(IRREMOTE_TAG,"%d",desired_us);
	ESP_LOGD(IRREMOTE_TAG,"us");
	ESP_LOGD(IRREMOTE_TAG,": ");
	ESP_LOGD(IRREMOTE_TAG,"%d",TICKS_LOW(desired_us + MARK_EXCESS) * USECPERTICK);
	ESP_LOGD(IRREMOTE_TAG," <= ");
	ESP_LOGD(IRREMOTE_TAG,"%d",measured_ticks * USECPERTICK);
	ESP_LOGD(IRREMOTE_TAG," <= ");
	ESP_LOGD(IRREMOTE_TAG,"%d",TICKS_HIGH(desired_us + MARK_EXCESS) * USECPERTICK);

  bool passed = ((measured_ticks >= TICKS_LOW (desired_us + MARK_EXCESS))
                && (measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS)));
  if(passed){
	  ESP_LOGD(IRREMOTE_TAG,"?; passed");
  } else{
	  ESP_LOGD(IRREMOTE_TAG,"?; FAILED");
  }
  return passed;
}

//+========================================================
// Due to sensor lag, when received, Spaces tend to be 100us too short
//
int  MATCH_SPACE (int measured_ticks,  int desired_us)
{
	DBG_PRINT(F("Testing space (actual vs desired): "));
	DBG_PRINT(measured_ticks * USECPERTICK, DEC);
	DBG_PRINT(F("us vs "));
	DBG_PRINT(desired_us, DEC);
	DBG_PRINT("us"); 
	DBG_PRINT(": ");
	ESP_LOGD(IRREMOTE_TAG,"%d",(TICKS_LOW(desired_us - MARK_EXCESS) * USECPERTICK));
	DBG_PRINT(F(" <= "));
	DBG_PRINT(measured_ticks * USECPERTICK, DEC);
	DBG_PRINT(F(" <= "));
	DBG_PRINT(TICKS_HIGH(desired_us - MARK_EXCESS) * USECPERTICK, DEC);

  bool passed = ((measured_ticks >= TICKS_LOW (desired_us - MARK_EXCESS))
                && (measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS)));
  if (passed)
    DBG_PRINTLN(F("?; passed"));
  else
    DBG_PRINTLN(F("?; FAILED")); 
 	return passed;
}



