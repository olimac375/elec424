#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <time.h>

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

#define UNIT 8
#define DEFAULT_DELAY 64
#define SHORT 64
#define LONG 512

void delay(int milli_seconds);

int main(int argc, char **argv)
{
    // GPIO Chip
	char *chipname = "gpiochip0";
    struct gpiod_chip *chip;

    // LED Output

	unsigned int led_line_num = 27;	// GPIO Pin #24
	int led_val;
	
	struct gpiod_line *led_line;
	int i, led_ret;

    // Button Input
    
	unsigned int btn_line_num = 23;	// GPIO Pin #23
	unsigned int btn_val;
	
	struct gpiod_line *btn_line;
	int btn_ret;

    // Delay variablies

    int num_units = DEFAULT_DELAY;
    int time_pressed = 0;

    // Chip init 

	chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		perror("Open chip failed\n");
		goto end;
	}

     // LED Output init

    led_line = gpiod_chip_get_line(chip, led_line_num);
	if (!led_line) {
		perror("Get line failed\n");
		goto close_chip;
	}

	led_ret = gpiod_line_request_output(led_line, CONSUMER, 0);
	if (led_ret < 0) {
		perror("Request line as output failed\n");
		goto release_line;
	}

    // Button Input init

	btn_line = gpiod_chip_get_line(chip, btn_line_num);
	if (!btn_line) {
		perror("Get line failed\n");
		goto close_chip;
	}

	btn_ret = gpiod_line_request_input(btn_line, CONSUMER);
	if (btn_ret < 0) {
		perror("Request line as input failed\n");
		goto release_line;
	}





    led_val = 0;
    while(true) {
        led_ret = gpiod_line_set_value(led_line, led_val);
		if (led_ret < 0) {
			perror("Set line output failed\n");
			goto release_line;
		}
        //printf("Output %u on line #%u\n", led_val, led_line_num);

        for (i = 0; i < num_units; i++) {
            delay(UNIT);
            if (gpiod_line_get_value(btn_line)) {
                time_pressed += UNIT;
            } else {
                if (time_pressed >= LONG) {
                    printf("LONG\n");
                    num_units = DEFAULT_DELAY;
                } else if (time_pressed >= SHORT){
                    printf("SHORT\n");
                    num_units = num_units >> 1;
                    if (!num_units)
                        num_units = 1;
                }
                time_pressed = 0;
            }
                
        }

        led_val = !led_val;
	}




release_line:
	gpiod_line_release(led_line);
    gpiod_line_release(btn_line);
close_chip:
	gpiod_chip_close(chip);
end:
	return 0;
}


inline void delay(int milli_seconds) {

    clock_t start_time = clock();

    while(clock() < start_time + 1000 * milli_seconds);
        

}
