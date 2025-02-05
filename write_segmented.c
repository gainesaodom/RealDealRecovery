#include <stdint.h>
#include <stdbool.h>
#include "spi23x1024/spi23x1024.c"

#define SPI23X640_MAX_ADDRESS 8192

uint16_t img_write() {

    uint8_t csv_data[8192];
    FILE *file =fopen("Aubie.csv", "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Discard the first line (header)
    char buffer[16];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        perror("Error reading header");
        fclose(file);
        return 1;
    }

    for (int i = 0; i < 8192; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            perror("Error reading data");
            fclose(file);
            return 1;
        }
        
        // Parse the "Address" and "Byte" data
        unsigned int address;
        unsigned int byte;
        if (sscanf(buffer, "%4x,%2x", &address, &byte) != 2) {
            perror("Error parsing data");
            fclose(file);
            return 1;
        }

        // Store the byte data in the array
        csv_data[i] = (uint8_t)byte;
    }

    fclose(file);

    spi_mem_init(5000000);
	int address_idx;
	for int i = 0; i < 16; i++)	{
		
		for (address_idx = i*SPI23X640_MAX_ADDRESS; address_idx < SPI23X640_MAX_ADDRESS + i*SPI23X640_MAX_ADDRESS; address_idx++) {
			//printf("%x\n", address_idx);
			if(i==0) uint8_t next_value = csv_data[address_idx];
			else uint8_t next_value = csv_data[address_idx - i*SPI23X640_MAX_ADDRESS];
			spi_mem_write_byte(address_idx, next_value);

			//printf("Writing value %04" PRIx16 "\n", next_value);

		}	
			
	}
   
	
	for int i = 0; i < 16; i++)	{
		
		for (address_idx = i*SPI23X640_MAX_ADDRESS; address_idx < SPI23X640_MAX_ADDRESS + i*SPI23X640_MAX_ADDRESS; address_idx++) {
			//printf("%x\n", address_idx);
			uint8_t read_byte;
			read_byte = spi_mem_read_byte(address_idx);
			
			if(i==0 & read_byte != csv_data[address_idx])
			{ 
				spi_mem_close(); 
				return -1; 
			}
			else if(read_byte != csv_data[address_idx - i*SPI23X640_MAX_ADDRESS])
			{ 
				spi_mem_close();
				return -1; 
			}

			//printf("Writing value %04" PRIx16 "\n", next_value);

		}	
			
	}
	spi_mem_close();
	return 0;
}

int main()
{
    if (img_write() != 0)  {
        printf("Error: image not written.");
    }
	else printf("Image written successfully. \n");
}