# EECS 482 Project 2 
# howiemok.lohoi.skimkim.2

# TODOS
	* Error handling
	* Write test cases!
		* comprehensive yield test
		* comprehensive join test 

	* Look at TODOs in the thread library code. Currently, uncommenting these lead to segfaults. We believe this has to do with how we're pushing to the exitQ. I did a memory trace and got the following diagram:

	_________ucontext_t *__________|______i_ptr2_______|_____thread_object_status_____|_______thread execution status_____
			   0x62130	                 0x62f4e0                    DEAD                            ALIVE

			   0x62f7a0                  0x62f4e0                    ALIVE							 ALIVE	

			   0X62fe10					 0x62fb50 					 DEAD							 ALIVE


	Next Breakpoint hit: Running thread was 0x62f7a0 with impl_ptr at 0x62fb50. This makes me think our data is getting corrupted when we're pushing back into the exitQ? 

	Show this to an IA and maybe he or she can give us some ideas on why this could be happening?

this was running with 1 cpu, no sync interrupts, no async interrupts with test-join-thorough.cc with breakpoints at the two places where we execute exitQ pushback in thread.cc (theres one in the dtor and one in the enabler function).  
