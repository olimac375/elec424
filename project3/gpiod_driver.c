#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/kernel.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
/* YOU WILL NEED OTHER HEADER FILES */

/* YOU WILL HAVE TO DECLARE SOME VARIABLES HERE */

struct gpio_desc *led_gpiod;
struct gpio_desc *btn_gpiod;
int irq_number;
 

/* ADD THE INTERRUPT SERVICE ROUTINE HERE */

/** variable contains pin number o interrupt controller to which GPIO 17 is mapped to */

/**
* @brief Interrupt service routine is called, when interrupt is triggered
*/
static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
	printk("gpio_irq: Interrupt was triggered and ISR was called!\n");
  	return (irq_handler_t) IRQ_HANDLED;
}


// probe function
static int led_probe(struct platform_device *pdev)
{
	int time = 0;

	printk(KERN_INFO "LED PROBE\n");

	led_gpiod = devm_gpiod_get(&pdev->dev, "led-gpios", GPIOD_OUT_LOW);
	btn_gpiod = devm_gpiod_get(&pdev->dev, "btn-gpios", GPIOD_IN);

	irq_number = gpiod_to_irq(led_gpiod);


	if(request_irq(irq_number, (irq_handler_t)gpio_irq_handler, IRQF_TRIGGER_RISING, "oli-gpio", NULL) != 0) {
		printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
   		// gpiod_free(led_gpiod);
   		// gpiod_free(btn_gpiod);
   		return -1;

	}


	//Turn LED on for 0.5s for a total of 5 seconds.
	while(time < 5){
		gpiod_set_value(led_gpiod, 1); //turn LED on
		msleep(500);
		gpiod_set_value(led_gpiod, 0); //turn LED off
		msleep(500);

		time += 1;
	}
	return 0;
}

// remove function
static int led_remove(struct platform_device *pdev)
{
	/* INSERT: Free the irq and print a message */
	free_irq((irq_handler_t)gpio_irq_handler, NULL);
	printk(KERN_INFO "Goodbye!\n");
	return 0;
}

static struct of_device_id matchy_match[] = {
    {.compatible = "gpio-oli"},
    {/* leave alone - keep this here (end node) */},
};

// platform driver object
static struct platform_driver adam_driver = {
	.probe	 = led_probe,
	.remove	 = led_remove,
	.driver	 = {
	       .name  = "The Rock: this name doesn't even matter",
	       .owner = THIS_MODULE,
	       .of_match_table = matchy_match,
	},
};

module_platform_driver(adam_driver);

MODULE_DESCRIPTION("424\'s finest");
MODULE_AUTHOR("GOAT");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:adam_driver");
