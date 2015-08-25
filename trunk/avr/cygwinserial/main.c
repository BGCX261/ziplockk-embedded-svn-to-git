#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/select.h>
#include "gps.h"

int main(int argc, char *argv[])
{
	int fd;
	struct termios options;

	fd=open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);

	if(fd==-1)
	{
		return 1;
	}

	fcntl(fd,F_SETFL,0);

	tcgetattr(fd,&options);

	cfsetispeed(&options,B57600);
	cfsetospeed(&options,B57600);

	options.c_cflag |= (CLOCAL | CREAD); /* enable */
	options.c_cflag &= ~PARENB; /* 8N1 */
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	tcsetattr(fd, TCSANOW, &options);

	const int bufferSize=100;
	char buffer[bufferSize];

	while(1)
	{
		ssize_t maxSize=bufferSize;
		ssize_t readSize=read(fd,(void *)buffer,maxSize);
		
		for(int loop=0;loop<readSize;loop++)
		{
			processGPSCharacter(buffer[loop]);
		}
	}
}

