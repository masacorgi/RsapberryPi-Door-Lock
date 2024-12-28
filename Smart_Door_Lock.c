include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "./fpga_dot_font.h"
#define MAX_BUFF 32
#define MAX_BUTTON 9

int push_dev;
int dip_dev;
int fnd_dev;
int text_lcd_dev;
int step_motor_dev;
int led_dev;
int dot_dev;

unsigned char quit = 0;

void open_devices(void);

int main(void)
{
        open_devices(); //open all devices
        printf("devices opened\n");

        printf("\nWelcome to Home!Pull dip switch - text lcd\n");
        write(text_lcd_dev, "Welcome to Home!Pull dip swtich ", MAX_BUFF);//text lcd outputs greeting

        unsigned char dip_sw_buff = 0;
	unsigned char fnd_data[4];
	int fnd_str_size;
	fnd_str_size=4;
	memset(fnd_data,0,sizeof(fnd_data));

        while(1){ //repeat until dip switch[1] pushed
                sleep(1);// read repeated on 1 second
                read(dip_dev, &dip_sw_buff, 100);
                if(dip_sw_buff==0x01){   // if dip switch - 1st button pulled - next process start
                        // while escape
                        break;
                }

        }

	while(1){ 
// while loop for intifite -     1. password wrong- repeat, 2. password correct- door open, repeat.
        printf("\npassword process start\n");
	printf("\ndip_switch pulled\nplease press password - text lcd\n"); 
	write(text_lcd_dev,"please press         password           ",MAX_BUFF);//print to fpga_text_lcd

	unsigned char push_sw_buff[MAX_BUTTON];
        int buff_size = sizeof(push_sw_buff);
        int password[4] = {1,2,3,4};
        int password_input[4]={0,0,0,0};
        int count=0;

        int str_size;
        while(1){
                read(push_dev, &push_sw_buff, buff_size); //read push switch device on 1 sec cycle

                for(int i=0; i<9; i++){ // check button  0~8
                        if(push_sw_buff[i] == 1){ // if button[ i ] pushed - store and write on fpga_dot
                                password_input[count] = i+1;
				fnd_data[count]=i+1+'0';
                                str_size = sizeof(fpga_number[password_input[count]]);
                                write(dot_dev,fpga_number[password_input[count]],str_size);
                                count++;
                                sleep(1);
                        }
                        if(count == 4) break;
                }
                if(count == 4) break; // 4 num filled - break
        }
        printf("pressed input : %d %d %d %d\n",password_input[0],password_input[1],
		password_input[2],password_input[3]); // 4 num print to console
	write(fnd_dev,&fnd_data,4); // 4 num print to fpga_fnd

	// if wrong password - flash lights 3 times on LED
        int data = 255;
        int data2 = 0;
        for(int i=0; i<4; i++){
                if(password[i] != password_input[i]){
                        printf("wrong password - text lcd\n");
                        write(text_lcd_dev,"wrong password                        ",MAX_BUFF);
                        write(led_dev,&data,1);  // led on
                        sleep(1);
                        write(led_dev,&data2,1); // led off
                        sleep(1);
                        write(led_dev,&data,1);  // led on
                        sleep(1);
                        write(led_dev,&data2,1); // led off
                        sleep(1);
                        write(led_dev,&data,1);  // led on
                        sleep(1);
                        write(led_dev,&data2,1); // led off
                        sleep(1);
			write(text_lcd_dev,"                                            ",MAX_BUFF);
                        break;
                }
        }

	//if password incorrect - goto while loop's start point(password inserting process)
	if(password[0] != password_input[0]){
		memset(fnd_data,0,sizeof(fnd_data));
        	write(fnd_dev,&fnd_data,4); //fnd reset to 0000
        	str_size = sizeof(fpga_number[0]);
        	write(dot_dev,fpga_number[0],str_size); //dot reset to 0
		continue; // while loop restart -> goto password input process
	}
	if(password[1] != password_input[1]){
                memset(fnd_data,0,sizeof(fnd_data));
                write(fnd_dev,&fnd_data,4); //fnd reset to 0000
                str_size = sizeof(fpga_number[0]);
                write(dot_dev,fpga_number[0],str_size); //dot reset to 0
                continue; // while loop restart -> goto password input process
	}
	if(password[2] != password_input[2]){
                memset(fnd_data,0,sizeof(fnd_data));
                write(fnd_dev,&fnd_data,4); //fnd reset to 0000
                str_size = sizeof(fpga_number[0]);
                write(dot_dev,fpga_number[0],str_size); //dot reset to 0
                continue; // while loop restart -> goto password input process
	}
	if(password[3] != password_input[3]){
                memset(fnd_data,0,sizeof(fnd_data));
                write(fnd_dev,&fnd_data,4); //fnd reset to 0000
                str_size = sizeof(fpga_number[0]);
                write(dot_dev,fpga_number[0],str_size); //dot reset to 0
                continue; // while loop restart -> goto password input process
	}

	int motor_action=1;
        int motor_direction=1;
        int motor_speed=250;
	unsigned char motor_state[3];
        motor_state[0]=(unsigned char)motor_action;
        motor_state[1]=(unsigned char)motor_direction;
        motor_state[2]=(unsigned char)motor_speed;
	for(int i=0;i<4;i++){ //step_motor - door openning process
		if(i<2){//door openning
			motor_state[0] = 1+'0'; // set motor on
			motor_state[1] = 1+'0'; // set turn to right
			write(step_motor_dev,motor_state,3); // motor run
			sleep(2); //run for 1*2 seconds
		}
		else if(i == 3){//stop
			motor_state[0] = 0 + '0';// set motor off
			write(step_motor_dev,motor_state,3); // motor off
			sleep(3); // stop for 2 seconds
		}
		else{//door closing
			motor_state[0] = 1+'0';// set motor on
			motor_state[1] = 0+'0';// set turn to left
			write(step_motor_dev,motor_state,3);// motor run
			sleep(4); //reverse run for 1*2 seconds

			motor_state[0] = 0+'0';// set motor off
			write(step_motor_dev,motor_state,3); // motor off
		}
		
	}
	
	memset(fnd_data,0,sizeof(fnd_data));
	write(fnd_dev,&fnd_data,4); //fnd reset to 0000
	str_size = sizeof(fpga_number[0]);
        write(dot_dev,fpga_number[0],str_size); //dot reset to 0
}//while(1) end
}

void open_devices(void) {
        dip_dev = open("/dev/fpga_dip_switch", O_RDWR);
        if(dip_dev<0) {
                printf("Dip Switch Device open error\n");
                exit(1);
        }
        text_lcd_dev=open("/dev/fpga_text_lcd", O_RDWR);// text_lcd_open
        if(text_lcd_dev<0) {
                printf("Text LCD Device open error\n");
                exit(1);
        }

        push_dev = open("/dev/fpga_push_switch", O_RDWR);
        if(push_dev<0) {
                printf("Push Switch Device open error\n");
                exit(1);
        }

        fnd_dev = open("/dev/fpga_fnd", O_RDWR);
        if(fnd_dev<0) {
                printf("FND device open error\n");
                exit(1);
        }

        step_motor_dev = open("/dev/fpga_step_motor", O_RDWR);
        if(step_motor_dev<0) {
                printf("Step Motor Device open error\n");
                exit(1);
        }

        led_dev = open("/dev/fpga_led", O_RDWR);
        if(dip_dev<0) {
                printf("LED Device open error\n");
                exit(1);
        }

        dot_dev = open("/dev/fpga_dot", O_RDWR);
        if(dip_dev<0) {
                printf("Dot Device open error\n");
                exit(1);
	}
}
