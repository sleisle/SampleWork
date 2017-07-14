/*
	Nicholas Turner
	Sean Leisle
	Alex Lee
	11/18/2016

	Sets timers for analog pin reading
	Used for sensing distance
*/

#include "timerEx.h"

// Initializes the analog pins for reading, then starts a timer to read at intervals
int main()
{
	initAnalog();
	startTimer();
}

// Initializes a timer with frequency FREQUENCY, setting the handler to modify motor values
void startTimer()  {
  	struct sigevent sev;
	long long freq_nanosecs;
	
	struct sigaction sa;

	/* Establish handler for timer signal */

	printf("Establishing handler for signal %d\n", SIG);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIG, &sa, NULL) == -1)
	   errExit("sigaction");

	/* Block timer signal temporarily */

	printf("Blocking signal %d\n", SIG);
	sigemptyset(&mask);
	sigaddset(&mask, SIG);
	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	   errExit("sigprocmask");

	/* Create the timer */

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIG;
	sev.sigev_value.sival_ptr = &timerid;
	if (timer_create(CLOCKID, &sev, &timerid) == -1)
	   errExit("timer_create");

	printf("timer ID is 0x%lx\n", (long) timerid);

	/* Start the timer */

	freq_nanosecs = FREQUENCY;
	its.it_value.tv_sec = freq_nanosecs / 1000000000;
	its.it_value.tv_nsec = freq_nanosecs % 1000000000;
	its.it_interval.tv_sec = freq_nanosecs / 1000000000;
	its.it_interval.tv_nsec = freq_nanosecs % 1000000000;

	if (timer_settime(timerid, 0, &its, NULL) == -1)
	    errExit("timer_settime");

	/* Sleep for a while; meanwhile, the timer may expire
	  multiple times */

	sleep(1);

	/* Unlock the timer signal, so that timer notification
	  can be delivered */

	printf("Unblocking signal %d\n", SIG);
	if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
	   errExit("sigprocmask");

	while (1);

	exit(EXIT_SUCCESS);
}

// Unused, but nice to have for testing purposes
static void print_siginfo(siginfo_t *si)
{
   timer_t *tidp;
   int or;

   tidp = si->si_value.sival_ptr;

   printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
   printf("    *sival_ptr = 0x%lx\n", (long) *tidp);

   or = timer_getoverrun(*tidp);
   if (or == -1)
       errExit("timer_getoverrun");
   else
       printf("    overrun count = %d\n", or);
}

// When timer expires, reads the four analog pins, and sets motors accordingly
static void handler(int sig, siginfo_t *si, void *uc)
{
	int i;
	int flag = 1;
	int distances[4];
	int fds[4];
	int value;
	char val[4];
	char * pins [4] = {"/sys/devices/ocp.3/helper.15/AIN0", "/sys/devices/ocp.3/helper.15/AIN1", 
		"/sys/devices/ocp.3/helper.15/AIN2", "/sys/devices/ocp.3/helper.15/AIN3"};

	// Open all the pins to be read
	for (i = 0; i < 4; i++) {
		fds[i] = open(pins[i], O_RDONLY);
	}
	
	// Read each pin and store value as int into distances
	for (i = 0; i < 4; i++) {
		if (read(fds[i], &val, 4) >= 0) {
			value = atoi(val);
			// printf("%d\n", value); // printf not really correct to use in a handler
			distances[i] = value;
			close(fds[i]);
		} else {
			value = 0;
			flag = 0;
		}
	}

	// printf("\n"); // printf not really correct to use in a handler
	if (flag) {
		setMotors(distances);
	}
}

// Initializes the analog pins for reading
void initAnalog() {

	FILE * setupFile = fopen(SETUP_DIR, "w");
	int i;
	char pinDir[40];
	char pinNum;

	if (setupFile == NULL) {
		printf("%s\n", "Error: Cannot open setup file");
		exit(1);
	}

	// Set up analog pins for reading
	fprintf(setupFile, "cape-bone-iio");
	fflush(setupFile);
}