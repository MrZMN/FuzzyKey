// C library headers
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#define DUDECT_IMPLEMENTATION
#include "dudect.h"

#define test1TX		//TX of (31,29,1) RS syndrome extractor
#define test1RX		//RX of (31,29,1) RS syndrome extractor
#define test2TX		//TX of (31,27,2) RS syndrome extractor
#define test2RX		//RX of (31,27,2) RS syndrome extractor
#define test3TX		//TX of (63,49,7) RS syndrome extractor
#define test3RX 	//RX of (63,49,7) RS syndrome extractor
#define test4TX		//TX of (255,50,2) Pinsketch 	
#define test4RX		//RX of (255,50,2) Pinsketch 	
#define test5TX		//TX of (255,20,2) Pinsketch 
#define test5RX 	//RX of (255,20,2) Pinsketch 
#define test6TX		//TX of (255,10,2) Pinsketch 
#define test6RX 	//RX of (255,10,2) Pinsketch 


#if defined(test1TX) || defined(test1RX) || defined(test2TX) || defined(test2RX)
	#define inputlen 31
	#define HammingMetric
#elif defined(test3TX) || defined(test3RX)
	#define inputlen 63
	#define HammingMetric
#elif defined(test4TX) || defined(test4RX)
	#define inputlen 50
	#define SetMetric
#elif defined(test5TX) || defined(test5RX)
	#define inputlen 20
	#define SetMetric
#elif defined(test6TX) || defined(test6RX)
	#define inputlen 10
	#define SetMetric
#endif

int serial_port;


//check if one element exists in an array
int isnewelement(uint8_t arr[], uint8_t arrsize, uint8_t value){
    int flag = 1;
    for(int i = 0; i < arrsize; i++){
      if(arr[i] == value){
        flag = 0;
      }
    }
    return flag;
}


//Get the CPU cycles in one execution
uint64_t do_one_computation(uint8_t *data){
	//random input to the algorithm
  	uint8_t in[inputlen] = {0};
	
	//Hamming-metric methods
	#if defined HammingMetric

	memcpy(in, data, inputlen);
	for(int i = 0; i < inputlen; i++){
		#if defined(test1TX) || defined(test1RX) || defined(test2TX) || defined(test2RX)
			in[i] &= 0x1F;  
		#elif defined(test3TX) || defined(test3RX) 
			in[i] &= 0x3F;
		#endif
	}

	#endif
	
	//Set-metric methods
	#if defined SetMetric

	if(data[0]==0 & data[1]==0){
		for(int i = 0; i < inputlen; i++){
			in[i] = i+1;
		}
	}else{
		uint8_t randval = 0;
		for(int i = 0; i < inputlen; i++){
			randval = rand()%255;
			while(!isnewelement(in, inputlen, randval) | randval == 0 | randval == 254){
				randval = rand()%255; 
			}
			in[i] = randval;
		}
	}

	#endif

	
  	/////////////////////Constant-time testing via serial port///////////////////// 
  	// Write to serial port to start a session
  	write(serial_port, "e", 1);	
  
  	write(serial_port, in, inputlen);	//the third parameter is the length of transmitted message
  	usleep(700000);		//give a delay for the execution

  	// Allocate memory for read buffer, set size according to your needs
  	char read_buf [8];

  	// Normally you wouldn't do this memset() call, but since we will just receive
  	// ASCII data for this example, we'll set everything to 0 so we can
  	// call printf() easily.
  	memset(&read_buf, '\0', sizeof(read_buf));

  	// Read bytes from serial port. The behaviour of read() (e.g. does it block?,
  	// how long does it block for?) depends on the configuration
  	// settings above, specifically VMIN and VTIME
  	int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));		//return of read() is num of bytes. The bytes are written into read_buf

  	// num_bytes is the number of bytes read, which may be 0 if no bytes were received, and can also be -1 to signal an error.
  	if (num_bytes < 0) {
    	printf("Error reading: %s", strerror(errno));
    	return 1;
  	}else{
    	//printf("Read %i bytes.\n", num_bytes);
  	}

  	// concatenate the 64-bit cycle duration
  	uint64_t duration = 0;
  	for(int i = 0; i < 8; i++){
    	duration = duration ^ (uint8_t)read_buf[i];
    	if(i != 7){
      		duration = duration << 8;
    	}
  	}

	//printf("Cycles used on this round: %llu \n\n", duration);

  	return duration;
}

//Randomness generation
void prepare_inputs(dudect_config_t *c, uint8_t *input_data, uint8_t *classes) {
  	randombytes(input_data, c->number_measurements * c->chunk_size);
  	for (size_t i = 0; i < c->number_measurements; i++) {
    	classes[i] = randombit();   //core
    	if (classes[i] == 0) {
      		memset(input_data + (size_t)i * c->chunk_size, 0x00, c->chunk_size);
    	} else {
      // leave random
    	}
	}
}


int main(){
 
	// Open the serial port. Change device path as needed
  	serial_port = open("/dev/ttyACM1", O_RDWR);

  	// Create new termios struc, we call it 'tty' for convention
	struct termios tty;

	// Read in existing settings, and handle any error
	if(tcgetattr(serial_port, &tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		return 1;
	}
	
	//Configurations
  	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
	// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

	tty.c_cc[VTIME] = 10;    // these two combinations will result in different settings on blocking time/condition
	tty.c_cc[VMIN] = 0;

	// Set in/out baud rate to be 9600
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);
	
	// Save tty settings, also checking for error
  	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return 1;
  	}else{
		printf("Serial port works \n");
	}
	
	sleep(2);
	tcflush(serial_port, TCIOFLUSH);	//flush the serial port
	
	//dudect configuration
	dudect_config_t config = {
	 	.chunk_size = inputlen,  //number of random input (bytes)
	 	.number_measurements = 10000,    //number of iterations run per time
	};
	dudect_ctx_t ctx;

	dudect_init(&ctx, &config);

	dudect_state_t state = DUDECT_NO_LEAKAGE_EVIDENCE_YET;
	while (state == DUDECT_NO_LEAKAGE_EVIDENCE_YET) {
		state = dudect_main(&ctx);    //run infenitely or stops (not constant time)
	}
	dudect_free(&ctx);

	close(serial_port);
	return 0; // success
}
	
//compile this file: cc -O2 serialtest.c -lm