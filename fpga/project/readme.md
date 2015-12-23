## What is it?
-----------

There is ready-to-use *.bit file for the accelerator, which is able to perform calculations for microtube with the length N_d=12
In case of need reconfiguration accelerator for other value of N_d the changes should be done in the code and the new *.bit file should be generated.
In this case the following software should be installed to be able to work with the projects:
	- Vivado HLS 2014.4
	- Vivado	 2014.4
	
	
This is files for project which implements the accelerator for calculations of molecular dynamics simulations of microtubes.

It can be divided into three functional parts:
- Vivado HLS project, which can produce the IP core for performing the calculations 
location: rc_mt_2000_12\hls
- Vivado HLS project, which can produce the IP core for generating pseudo random numbers 
location: rc_mt_2000_12\rand
- Vivado project , which uses the above IP cores and also has DMA controller,DDR3 controller and some other functional blocks inside. 
This project produces the configure *.bit file  
  
 
## How to change N_d?
-----------   
- go to rc_mt_2000_12\hls
- make changes in  file  rc_mt_2000_12\hls\src\mt_defines.h
set the N_d. *It should be multile of 3.* 
- make changes in the file rc_mt_2000_12\hls\run.bat. Specify vivado_bin, which is the path to Vivado HLS 2014.4 folder
- create the project and package the ip by running rc_mt_2000_12\hls\run.bat
-go to rc_mt_2000_12\rand
- make changes in the file rc_mt_2000_12\rand\run.bat. Specify vivado_bin, which is the path to Vivado HLS 2014.4 folder
- create the project and package the ip by running rc_mt_2000_12\rand\run.bat   
- go to rc_mt_2000_12\project
- make changes in the file rc_mt_2000_12\project\create_project.bat. Specify the path to Vivado 2014.4 folder
- create the project by running rc_mt_2000_12\project\create_project.bat
- start gui Vivado 2014.4 and open the created project located in  rc_mt_2000_12\project directory
- create bitstream         

## Contacts
--------
Yuri Rumyantsev
rumyantsev@rosta.ru
Natalia Abrashitova 
natascha@rosta.ru 
	
