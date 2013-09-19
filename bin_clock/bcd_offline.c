#include <stdio.h>
#include <stdlib.h>

static void bcd_correct(unsigned char *a_bcd) {
	unsigned char x = 0;
	for (; x<2; x++) {
		if ((*a_bcd & (0x0f << (4*x))) > (0x09 << (4*x))) {
				*a_bcd += (0x06 << (4*x));
		}
	}
}


void bin2bcd(unsigned char *a_bin) {
	unsigned char bin = *a_bin;
	unsigned char i = 0x00;

	*a_bin = (((bin/10) % 10) << 4) | (bin % 10);
}


int main(int argc, char const *argv[])
{
	unsigned char num = atoi(argv[1]);
	unsigned char uncorr = 0;
	unsigned char corr = 0;

	bin2bcd(&num);
	uncorr = num + 1;
	corr = uncorr;
	bcd_correct(&corr);

	printf("BCD: [%x], inc: [%x], cor: [%x]\n", num, uncorr, corr);




	return 0;
}
