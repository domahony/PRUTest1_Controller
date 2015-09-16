/*
 * main.c
 */

#include <stdio.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <math.h>
#include "GY80.h"

#define PRU_NUM0 0
#define PRU_NUM1 1

#define INT2 {   \
{ \
	PRU0_PRU1_INTERRUPT, \
	PRU1_PRU0_INTERRUPT, \
	PRU0_ARM_INTERRUPT, \
	PRU1_ARM_INTERRUPT, \
	ARM_PRU0_INTERRUPT, \
	ARM_PRU1_INTERRUPT, \
	24, \
	(char)-1  },  \
{ \
		{PRU0_PRU1_INTERRUPT,CHANNEL1}, \
		{PRU1_PRU0_INTERRUPT, CHANNEL0}, \
		{PRU0_ARM_INTERRUPT,CHANNEL2}, \
		{PRU1_ARM_INTERRUPT, CHANNEL3},\
		{ARM_PRU0_INTERRUPT, CHANNEL0}, \
		{ARM_PRU1_INTERRUPT, CHANNEL1}, \
		{24, 5}, \
		{-1,-1}},  \
 { \
			{CHANNEL0,PRU0}, \
			{CHANNEL1, PRU1}, \
			{CHANNEL2, PRU_EVTOUT0}, \
			{CHANNEL3, PRU_EVTOUT1}, \
			{5, 5}, \
			{-1,-1} },  \
 (PRU0_HOSTEN_MASK | PRU1_HOSTEN_MASK | PRU_EVTOUT0_HOSTEN_MASK | PRU_EVTOUT1_HOSTEN_MASK | PRU_EVTOUT3_HOSTEN_MASK) /*Enable PRU0, PRU1, PRU_EVTOUT0 */ \
}

int main(void) {
	
	//tpruss_intc_initdata intc = PRUSS_INTC_INITDATA;
	tpruss_intc_initdata intc = INT2;

	prussdrv_init();
	prussdrv_open(3);
	prussdrv_pru_reset(PRU_NUM0);

	prussdrv_pruintc_init(&intc);
	prussdrv_exec_program(PRU_NUM0, "./pru0.bin");

	printf("Waiting for Event 1\n");

	void* pruDataMem;
	prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pruDataMem);

	GY80 *gy80 = (GY80*)(pruDataMem + 0x888);

	while (1) {
		prussdrv_pru_wait_event(3);
		prussdrv_pru_clear_event(3, 24);

		float theta = atan2(gy80->accelerometer.y/1000., gy80->accelerometer.z/1000.);
		float phi = acos(gy80->accelerometer.z/1000.);
		/*
		float phi = acos(gy80->accelerometer.z/1000. /
				sqrt(((gy80->accelerometer.x/1000.) * (gy80->accelerometer.x/1000.)) +
				((gy80->accelerometer.y/1000.) * (gy80->accelerometer.y/1000)) +
				((gy80->accelerometer.z/1000.) * gy80->accelerometer.z/1000.)));
		*/

		theta = theta * 180./M_PI;

		if (gy80->accelerometer.x < 0 && gy80->accelerometer.y < 0) {
			theta -= 180.0;
		}

		if (gy80->accelerometer.x > 0 && gy80->accelerometer.y < 0) {
			theta += 180.0;
		}

		phi = phi * 180./M_PI;

		printf("Temp: % 4.1f ", gy80->temperature);
		printf("Accelerometer X: %+11.6f Y: %+11.6f Z: %+11.6f\n",
				gy80->accelerometer.x, gy80->accelerometer.y, gy80->accelerometer.z);
		printf(" Theta: %+4.6f Phi: %+4.6f\n", theta, phi);
		printf("Gyroscope X: %+11.6f Y: %+11.6f Z: %+11.6f\n",
				gy80->gyroscope.x, gy80->gyroscope.y, gy80->gyroscope.z);
	}

	return 0;
}
