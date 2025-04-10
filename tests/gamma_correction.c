#include <stdlib.h>
#include <stdio.h>
#include <math.h>



int main (void)
{
	// 100 ^2.8 = 398
	printf ("unsigned char _GAMMA_CORRECTION_LEVEL\t[] = {\r\n");
	int line    = 0;
	int value   = 0;
	for (line=0; line<100; line+=10) {
		for (value=0; value<10; ++value) {
			int lvl	= line + value + 1;
			float gc = powf(((float)lvl / 100), 2.4) * 254 + 1;
			printf ("\t/*%3d*/ 0x%02X,", lvl, (int)gc);
		}
		printf ("\r\n");
	}
	printf ("};\r\n");
	return(0);
}
