/*
Reading Program for the 23A1024 SRAM Chips.

To be used for the reading of ten SRAMs. Use a 70% square wave PWM with 5s period.

Author: Gaines Odom
Advisor: Ujjwal Guin
Institution: Auburn University
Created: 2/5/2024
Revised: 2/5/2024

*/

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <inttypes.h>
#include <wiringPi.h>

#include "spi23x1024.c"

#define FGEN_PIN 0
char date [100];
char file_name [132];
char main_dir [150];
char dir_name [100];
volatile int s=1;
volatile int chip_seg=1;


int create_and_change_directory(const char *dir_name) {
    struct stat st = {0};
	
    // Check if directory exists
    if (stat(dir_name, &st) == -1) {
        // Directory does not exist, so create it
        if (mkdir(dir_name, 0777) == 0) {
			chdir(dir_name);
            return 0;
        } else {
            return 1; // error making directory
        }
    } else {
        return 0;
    }
}



void chip_on(void)
{
	
	snprintf(dir_name,sizeof(dir_name),"chip_segment_%d",chip_seg);
	if (s % 100==1)
	{
		if(create_and_change_directory(dir_name)!=0) //needs snprintf
				printf("Important error: error with creating new directory. Stop and check for errors.");
	}
	
    if (chip_seg <= 16) {
        printf("Starting Read %d for Segment %d... ", s, chip_seg);

        snprintf(file_name, sizeof(file_name), "chip_segment_%d_%d.csv", chip_seg, s);
        FILE *file = fopen(file_name, "w");  

        fprintf(file, "Address,Word\n");
		
		for (address_idx = chip_seg*SPI23X640_MAX_ADDRESS - SPI23X640_MAX_ADDRESS; address_idx < chip_seg*SPI23X640_MAX_ADDRESS; address_idx++) {
			//printf("%x\n", address_idx);
			uint8_t read_byte;
			read_byte = spi_mem_read_byte(address_idx);
			fprintf(file, "%05" PRIx32 ",%02" PRIx8 "\n", address_idx, read_byte);
		}
	
        fclose(file);
        printf("Done!\n");
    }
    else 
    {
    printf("Completely Done!\n");
}
s++;
        if (s == 101)
        {
            spi_mem_close();
			chip_seg++;
			spi_mem_init(5000000);
			s=1;
			chdir(main_dir);
			
        }
}


int main()
{
    printf("What is today's date? (Use format JUL4 for July 4th): ");
    scanf("%99s", date);
    mkdir(date, 0777);
    chdir(date);
	getcwd(main_dir, sizeof(main_dir));
    spi_mem_init(5000000);

    if (wiringPiSetup() == -1) {
        printf("WiringPi initialization failed.\n");
        return 1;
    }

    pinMode(FGEN_PIN, INPUT);

    // Set up the interrupt handler
    if (wiringPiISR(FGEN_PIN, INT_EDGE_RISING, &chip_on) < 0) {
        printf("Unable to set up ISR.\n");
        return 1;
    }

    pullUpDnControl(FGEN_PIN, PUD_DOWN);

    while(chip_seg <= 16)
    {

    }

    // printf("done\n");
}