stm32cubemx - gen code for stm32cubeide
vscode - 

testing

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


