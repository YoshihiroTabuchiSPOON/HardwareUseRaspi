#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <errno.h>

int main()
{
	int fd = open( "/dev/i2c-1", O_RDWR );
	if( fd < 0 )
	{
		printf( "Cannot open i2c\n" );
		return 1;
	}

	printf( "FD=%d\n", fd );

	uint8_t ui8ReceiveData[24];memset( ui8ReceiveData, 0, 24 );
	uint8_t ui8RegisterAddress = 0x88;

	struct i2c_msg messages[] = {
			{0x76, 0, 1, &ui8RegisterAddress},
			{0x76, I2C_M_RD, 24, ui8ReceiveData }
	};
	struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };

	if( ioctl( fd, I2C_RDWR, &ioctl_data ) != 2 )
	{
		printf( "Cannot read ioctl %d\n", errno );
		close( fd );
		return 2;
	}

	printf( "ReadData:" );
	for( int i=0; i<24; i++ )
	{
		printf( " %02X", ui8ReceiveData[i] );
	}
	printf( "\n" );

	close( fd );

	return 0;
}
