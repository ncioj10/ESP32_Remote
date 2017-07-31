#include <stdio.h>
#include "IRremote.h"

//+=============================================================================
// Display IR code
//
void  ircode (decode_results *results)
{
  // Panasonic has an Address
  if (results->decode_type == PANASONIC) {
    printf("%04x", results->address);
    printf(":");
  }

  // Print Code
  printf("%lx",results->value);
}

//+=============================================================================
// Display encoding type
//
void  encoding (decode_results *results)
{
  switch (results->decode_type) {
    default:
    case UNKNOWN:      printf("UNKNOWN");       break ;
    case NEC:          printf("NEC");           break ;
    case SONY:         printf("SONY");          break ;
    case RC5:          printf("RC5");           break ;
    case RC6:          printf("RC6");           break ;
    case DISH:         printf("DISH");          break ;
    case SHARP:        printf("SHARP");         break ;
    case JVC:          printf("JVC");           break ;
    case SANYO:        printf("SANYO");         break ;
    case MITSUBISHI:   printf("MITSUBISHI");    break ;
    case SAMSUNG:      printf("SAMSUNG");       break ;
    case LG:           printf("LG");            break ;
    case WHYNTER:      printf("WHYNTER");       break ;
    case AIWA_RC_T501: printf("AIWA_RC_T501");  break ;
    case PANASONIC:    printf("PANASONIC");     break ;
    case DENON:        printf("Denon");         break ;
  }
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpInfo (decode_results *results)
{
  // Check if the buffer overflowed
  if (results->overflow) {
    printf("/n IR code too long. Edit IRremoteInt.h and increase RAWLEN");
    return;
  }

  // Show Encoding standard
  printf("Encoding  : ");
  encoding(results);
  printf("/n ");

  // Show Code & length
  printf("Code      : ");
  ircode(results);
  printf(" (");
  printf("%d",results->bits);
  printf("/n  bits)");
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpRaw (decode_results *results)
{
  // Print Raw data
  printf("Timing[");
  printf("%d",results->rawlen-1);
  printf("/n ]: ");

  for (int i = 1;  i < results->rawlen;  i++) {
    unsigned long  x = results->rawbuf[i] * USECPERTICK;
    if (!(i & 1)) {  // even
      printf("-");
      if (x < 1000)  printf(" ") ;
      if (x < 100)   printf(" ") ;
      printf("%lu",x);
    } else {  // odd
      printf("     ");
      if (x < 1000)  printf(" ") ;
      if (x < 100)   printf(" ") ;
      printf("%lu", x);
      if (i < results->rawlen-1) printf(", "); //',' not needed for last one
    }
    if (!(i % 8))  printf("/n ");
  }
  printf("/n ");                    // Newline
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpCode (decode_results *results)
{
  // Start declaration
  printf("unsigned int  ");          // variable type
  printf("rawData[");                // array name
  printf("%d",results->rawlen - 1);  // array size
  printf("] = {");                   // Start declaration

  // Dump data
  for (int i = 1;  i < results->rawlen;  i++) {
    printf("%d", results->rawbuf[i] * USECPERTICK);
    if ( i < results->rawlen-1 ) printf(","); // ',' not needed on last one
    if (!(i & 1))  printf(" ");
  }

  // End declaration
  printf("};");  //

  // Comment
  printf("  // ");
  encoding(results);
  printf(" ");
  ircode(results);

  // Newline
  printf("/n ");

  // Now dump "known" codes
  if (results->decode_type != UNKNOWN) {

    // Some protocols have an address
    if (results->decode_type == PANASONIC) {
      printf("unsigned int  addr = 0x");
      printf("%04x",results->address);
      printf("/n;");
    }

    // All protocols have data
    printf("unsigned int  data = 0x");
    printf("%lx",results->value);
    printf("/n ;");
  }
}
