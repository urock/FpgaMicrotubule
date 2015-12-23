## What is it?
-----------
This is project which implements the accelerator for calculations of molecular dynamics simulations of microtubes.
The project contains two parts:
- FPGA 		project (see fpga/readme for details)
- Software 	test 	(see lin/readme for details)
	
The project was tested 
- on [RB8-V7](http://rosta.ru/rb_8v7) harware platform (accelerator).
- on supermicro server  Intel(R) Xeon(R) CPU E5-2660 0 @ 2.20GHz , OS "Ubuntu 12.04.2 LTS (host-computer).

	
Brief description of this hardwate platform is the following:  
- 2 PCI Express boards [RC47](http://rosta.ru/rc_47), each contains 4 FPGA Virtex 7 2000Ò. PCI Express x4 Gen 3. 
- 4 (2 for each board)   EM4-DDR3 modules (up to 1 Gb DDR3 memory per FPGA)		

There is ready-to-use *.bit file for the accelerator, which is able to perform calculations for microtube with the length N_d=12
In case of need reconfiguration accelerator for other value of N_d the changes should be done in the code and the new *.bit file should be generated.
In this case the following software should be installed to be able to work with the projects:
- Vivado HLS 2014.4
- Vivado	 2014.4
	
 
## How to start?
-----------   
- the project for should be created  			(see project/readme for details)
- the SDK should be installed on the target PC 	(see lin/readme for details)
- the FPGA(s) should be configured				(see lin/readme for details)
- the test  should be run 						(see lin/readme for details)
        

## Contacts
--------
Yuri Rumyantsev
rumyantsev@rosta.ru
Natalia Abrashitova 
natascha@rosta.ru 
	
