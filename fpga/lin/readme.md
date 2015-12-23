## What is it?
-----------
This is SDK which is designed to work with RB8-V7 harware platform See http://rosta.ru/rb_8v7 for details. 

The SDK contains the following parts:
	-driver	   																							
			location ./driver
		
	-libraries
			location ./libraries 
 
	-utilities	
			location ./utilities 
			see ./utilities/readme for details   	
	-test for working with accelerator for calculations of molecular dynamics simulations of microtubes 
			location ./examples/mt
			see ./examples/mt/readme for details 			

  
 
## How to start?
-----------   
1. the system variable should be set:
	* 
	```
	export ROSTA_SDK_DIR=[path_to_sdk]
	```
2. the driver should be installed 
	*  
	```
	$ cd $ROSTA_SDK_DIR/lin/driver
	$ make
	$ insmod $ROSTA_SDK_DIR/lin/driver/rosta_drv.ko
	```
3. the libraries should be compiled
	* 
	```
	$ cd $ROSTA_SDK_DIR/lin/ApiLib
	$ make  
	```
4. the utilitites should be complied
	* 
	```
	$ cd $ROSTA_SDK_DIR/lin/utilities
	$ make  
	```
5. the test should be complied
	* 
	```
	$ cd $ROSTA_SDK_DIR/lin/examples/mt
	$ make
	```

6. the user should check, that the boards are correctly recognized by the system 
	* 
	```
	$ cd $ROSTA_SDK_DIR/lin/utilities
	$ sudo ./rc47_find_devices
	```        
7. the user should configure the board via PCI Express with the specified bitstream
	* 
	```
	$ cd $ROSTA_SDK_DIR/lin/utilities
	$ sudo rc47_config -b [board index] -v [chip select] -f [bit file name]
	chip select: any combination of C0, C1, C2, C3 or all
	```  
8. the program for using and testung accelerator can now be run. See ./examples/mt/readme for details

## Contacts
--------
Yuri Rumyantsev
rumyantsev@rosta.ru
Natalia Abrashitova 
natascha@rosta.ru 
	
