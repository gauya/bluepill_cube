
I use bluepill and blackpill a lot, and sometimes cortex-M7. I am using the HAL library and used stm32cubeide and stm32cubeMX as a development environment, but now I work in vscode.
Using the Arduino frame is often convenient, but there are also many limitations, so working with the HAL library through stm32cubemx is helpful in managing basic pin assignments, clocks, etc.
Nevertheless, the HAL library is inconvenient. It still cannot handle multiple grades of products with consistency.
So, I want to work with my library for basic UART communication and basic gpio pin processing, and I also want to process some of the ADC and timers.
I am creating and using a library that handles processes similar to rtos, but in fact, it is not rtos and only looks similar.
This is related to program style.
 

void setup() {
   init_serial(115200);

   init_ticks(eTICK_VOL_100us);
   init_pfn();

   set_tty_func("ps",ps );
   set_tty_func("time",cli_test2);
   set_tty_func("str",strtestf);
   set_tty_func("ls",command_list);

   add_pfn(1000, loop_led, "led blink");
   add_pfn(100,test1,"N1");
   add_pfn(10*1000,test2);
   add_pfn(1200, test_adc,"adc read");
   add_rtpfn(15,rtled);
   add_pfn(0,tty,"key in");
   add_pfn(10000, test6, "elapsed test");
  
   add_proc("viewall", (void (*)(const char *))view_proc_all, 60000, 0);
   add_proc("check", test5, 20000, 0);
  
   scadule();
}

It becomes a program like this.
Each task is executed in scadule(), and tasks that require real-time are scheduled in timer interrupts.

h.cy
