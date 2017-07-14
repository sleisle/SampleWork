/*
	Sean Leisle
	Alex Lee
	Nicholas Turner

	Linux kernel module for a 2x8 LCD screen
*/

#include "LCD_module.h"

// Initialization of module
static int __init driver_entry(void) {
	int ret;

	// Setting up a timer for multiple displays of text, call mod_timer to use
	setup_timer( &timer, timer_callback, 0);

	// Get Major and Minor numbers for mknod
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "LCD_module: Failed to allocate a major number\n");
		return ret;
	}
	printk(KERN_INFO "LCD_module: major number is %d\n", MAJOR(dev_num));
	printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

	// Allocate cdev, set up file listeners
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	// Add fops to cdev file
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "LCD_module: unable to add cdev to kernel\n");
		return ret;
	}

	// Set semaphore and initialize display
	sema_init(&virtual_device.sem, 1);
	ssleep(1);
	initializeDisplay();

	return 0;
}

// Exit routine for module
static void __exit driver_exit(void) {
	int i;
	int ret;
	int initPins[6] = {RS, RW, E, DATA_OUT, CLOCK_LCD, LATCH_LCD};

	// Clear Display
	writeToLCD(0x01, 0, initPins);

	// Zero gpio pins and release them
	for (i = 0; i < 6; i++) {
		gpio_set_value(initPins[i], 0);
		gpio_unexport(initPins[i]);
	}

	// Remove timer
	ret = del_timer(&timer);
	if (ret) printk(KERN_ALERT "Timer still in use");

	// Remove cdev and unregister Major and Minor numbers
	cdev_del(mcdev);
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "LCD_module: successfully unloaded\n");
}

// Callback function for timer
// Is called when there are characters left over to display
void timer_callback( unsigned long data) {
	int initPins[6] = {RS, RW, E, DATA_OUT, CLOCK_LCD, LATCH_LCD};
	printk(KERN_INFO "timer_callback called (%ld.)\n", jiffies);
	TimerIntrpt++;

	writeToLCD(0x01, 0x00, initPins);
	writeInputToDisplay(virtual_device.data, sizeOfData);
}

// Initializes the LCD Display 
void initializeDisplay(void) {
	int waitBusy = 1;
	int waitLongBusy = 4;

	int initPins[6] = {RS, RW, E, DATA_OUT, CLOCK_LCD, LATCH_LCD};
	int initSequence[8] = {0x30, 0x30, 0x30, 0x38, 0x08, 0x01, 0x06, 0x0C};
	int initWaitTiming[8] = {5000, 120, 120, waitBusy, waitBusy, waitLongBusy, waitBusy, waitBusy};
	int i;

	// Request the gpios and set direction
	for (i = 0; i < 6; i++) {
		gpio_request(initPins[i], "sysfs");
		gpio_direction_output(initPins[i], 0);
		gpio_export(initPins[i], 1);
	}

	// Output init sequence and wait accordingly 
	for (i = 0; i < 8; i++) {
		writeToLCD(initSequence[i], 0, initPins);
		udelay(initWaitTiming[i]);
	}
}

// Generic write to display. Data is what is written, controlLine controls R/W and RS
// pins[6] = {RS, RW, E, DATA_OUT, CLOCK_LCD, LATCH_LCD};
void writeToLCD(int data, int controlLine, int * pins) {
	int writeBits[8];
	int i;

	// Generate array of output bits for gpios
	for (i = 0; i < 7; i++) {
		writeBits[i] = data / (power(2, (7-i)));
		if (data >= (power(2, (7 - i)))) {
			data = data - (power(2, (7-i)));
		}
	}
	writeBits[7] = data % 2;

	// Set RS and RW
	gpio_set_value(pins[0], controlLine / 2);
	gpio_set_value(pins[1], controlLine % 2);

	// DATA_OUT to shift register
	for (i = 0; i < 8; i++) {
		gpio_set_value(pins[3], writeBits[i]);
		udelay(40);
		gpio_set_value(pins[4], 1);
		udelay(40);
		gpio_set_value(pins[4], 0);
		udelay(40);
	}

	// Flip shift register latch and enable lines
	gpio_set_value(pins[5], 1);
	udelay(40);
	gpio_set_value(pins[2], 1);
	udelay(40);
	gpio_set_value(pins[2], 0);
	udelay(40);
	gpio_set_value(pins[5], 0);
	udelay(40);
}

// Returns a^b
int power(int a, int b) {
	int i;
	int sum = a;
	for (i = 0; i < b - 1; i++) {
		sum *= a;
	}
	return sum;
}

// Called when dev file is opened
int device_open(struct inode *inode, struct file* filp) {
	if (down_interruptible(&virtual_device.sem) != 0) {
		printk(KERN_ALERT "LCD_module: could not lock device during open\n");
		return -1;
	}
	printk(KERN_INFO "LCD_module: device opened\n");
	return 0;
}

// Called when dev file is closed
int device_close(struct inode* inode, struct file *filp) {
	up(&virtual_device.sem);
	printk(KERN_INFO "LCD_module: closing device\n");
	return 0;
}

// Called when user wants to put info into device file
ssize_t device_write(struct file* flip, const char* bufSource, size_t bufCount, loff_t* curOffset) {
	int initPins[6] = {RS, RW, E, DATA_OUT, CLOCK_LCD, LATCH_LCD};
	unsigned long ret = 0;
	
	printk(KERN_INFO "LCD_module: writing to device...\n");
	printk(KERN_INFO "%s", bufSource);

	// Check for invalid cases
	if (busyWriting) { // If already writing something, forget new data
		printk(KERN_INFO "Busy writing, throwing away new data");
	} else { // Not busy
		TimerIntrpt = 0; // Reset timer count
		sizeOfData = bufCount;
		if (sizeOfData > 100) { // If more than buffer size, ditch extra
			sizeOfData = 100;
			printk(KERN_ALERT "Input larger than buffer, throwing away after 100 chars");
		}

		ret = copy_from_user(virtual_device.data, bufSource, sizeOfData);

		//Clear display on every write
		writeToLCD(0x01, 0x00, initPins);
		writeInputToDisplay(virtual_device.data, sizeOfData);
	}

	return ret;
}

// Called when user wants to get info from device file
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
	printk(KERN_INFO "new_char: Reading from device...\n");
	return copy_to_user(bufStoreData, virtual_device.data, bufCount);
}

// Writes charValues to display
// TODO: Set timer to deal with 32+ chars
void writeInputToDisplay(const char * charValues, int numberOfChars) {
	int initPins[6] = {RS, RW, E, DATA_OUT, CLOCK_LCD, LATCH_LCD};
	int j;
	int i;
	int currentChar = 32 * TimerIntrpt;

	//Timer Setup for multiple lines, or >32 chars
	if (currentChar + 32 < numberOfChars) {
		mod_timer(&timer, jiffies + msecs_to_jiffies(1000));
		busyWriting = 1;
	} else {
		TimerIntrpt = 0;
		busyWriting = 0;
	}

	//Make sure the cursor is in the right damn place
	writeToLCD(0x80, 0, initPins);

	// Write 16 characters, change cursor, write next 16 chars
	for (i = 0; i < MAX_ROWS; i++) {
		for (j = 0; (j < MAX_CHAR_PER_LINE) && (currentChar < numberOfChars - 1); j++) {
			if (charValues[currentChar] > 10) {
				writeToLCD(charValues[currentChar], 2, initPins);
			}
			currentChar++;
		}
		writeToLCD(0xC0, 0, initPins);
	}
	writeToLCD(0x80, 0, initPins);

}

MODULE_LICENSE("GPL"); // module license
module_init(driver_entry); // delcares which function runs on init
module_exit(driver_exit); // declares which function runs on exit