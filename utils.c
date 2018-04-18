#include <stdio.h>
#include <stdlib.h>

/* Dumps raw memory in hex byte and printable split format */
void dump(const unsigned char *data_buffer, const unsigned int length) {
  unsigned char byte;
  unsigned int i, j;
  for (i=0; i < length; i++) {
    byte = data_buffer[i];
    printf("%02x ", data_buffer[i]); // display byte in hex
    if(((i%16)==15) || (i==length-1)) {
      for(j=0; j < 15-(i%16); j++)
        printf("    ");
    printf("|  ");
    for(j=(i-(i%16)); j <= i; j++) { // display printable bytes 
      byte = data_buffer[j];
      if((byte > 31)&&(byte < 127)) 
        printf("%c", byte);
      else
        printf("."); // this is inprintable char
    }
    printf("\n"); // End of file
    } // End if
  } // End for
}
