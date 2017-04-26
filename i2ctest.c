#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "i2c-dev.h"

int main(int argc, char *argv[])
{
	int i2cbus, i2caddress, regaddress, row, date;
	int fd, res;
	char filename[20];
	//char buf[10];
	char value[20];

	/* handle (optional) flags first */
	if(argc < 3) {
		fprintf(stderr, "Usage:  %s <i2c-bus> <i2c-address> <register address row>\n", argv[0]);
		exit(1);
	}
    /* get i2c bus number, device address number (in decimal number) ---
    --- and row offset number(start from 0) */
	i2cbus   = atoi(argv[1]);
	i2caddress  = atoi(argv[2]);
	row = atoi(argv[3]);

	/* open i2c bus */
	sprintf(filename, "/dev/i2c-%d", i2cbus);
	fd = open(filename, O_RDWR);
	if (fd<0) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open i2c bus "
				"/dev/i2c-%d: %s\n", i2cbus, strerror(ENOENT));
		} else {
			fprintf(stderr, "Error: Could not open file "
				"`%s': %s\n", filename, strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
		exit(1);
	}

	/* set device address to i2c bus */
	if (ioctl(fd, I2C_SLAVE, i2caddress) < 0) {
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			i2caddress, strerror(errno));
		return -errno;
	}

	/* start to write 1 column for the row specified with word write */
	for (int i=0;i<8;i++)
	    {
	    	regaddress = (row<<4) | (2*i);
	    	//date = 0xffff;
	    	date = ((regaddress<<8)+0x0100) | regaddress;
	    	//printf("regaddress:0x%04x\n", date);
	    	res = i2c_smbus_write_word_data(fd, regaddress, date);
		    if (res < 0) {
			    fprintf(stderr, "Warning - write failed, filename=%s, register address=%d\n",
				filename, regaddress);
		    }
	        usleep(2000);
	    }

	/* start to write 1 column for the row specified with byte write */
	/*
	for (int i=0;i<16;i++)
    {
    	regaddress = (row<<4) | i;
    	printf("regaddress:0x%02x\n", regaddress);
    	res = i2c_smbus_write_byte_data(fd, regaddress, regaddress);
	    if (res < 0) {
		    fprintf(stderr, "Warning - write failed, filename=%s, register address=%d\n",
			filename, regaddress);
	    }
        usleep(2000);
    }*/

	/* start to read 1 colume for the row specified */

	for (int j=0; j<16; j++)
	{
		regaddress = (row<<4) | j;
		value[j] = i2c_smbus_read_byte_data(fd, regaddress);
	    if (res < 0) {
		    fprintf(stderr, "Error: Read failed, res=%d\n", res);
		    fprintf(stderr, "%s\n", strerror(errno));
		    exit(2);
	    }
	    printf("0x%02x ", regaddress);
	}
	printf("\n");

	/* samples with direct using write() and read() application to write i2c device */
    /*
	for(int i=0;i<16;i++)
	    {
	        regaddress = (row<<4) | i;
	        buf[0] = regaddress;
            buf[1] = regaddress;
	        if(write(fd, buf, 2) != 2)
            {
    	        fprintf(stderr, "Warning - write failed, filename=%s, regaddress=%d\n", filename, regaddress);
            }
            usleep(2000);
            if (i< 15){
                res = read(fd, &value[i+1], 1);
                if(res != 1)
                {
    	            fprintf(stderr, "Error: Read failed, res=%d\n", res);
                    exit(2);
                }
	        }
        }
	value[0] = (row<<4);*/

	/* start to output the value readed for 1 colume */
	for(int k=0; k<16; k++)
	    printf("0x%02x ", value[k]);

	printf("\n");
    /* close i2c bus */

	close(fd);
	exit(0);
}
