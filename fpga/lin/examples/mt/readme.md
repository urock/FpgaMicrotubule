## What is it?
-----------
This is a program for using and testing the accelerator for calculations of molecular dynamics simulations of microtubes.

This program communicates with connected accelerator via PCI Express and is able to configure accelerator to run for specified number of iterations
and to get the results from it. It is also possible to compare the accelerator results with  results, calculated  with the help of software version of
the algorithm. Time, which is spent for calculations is also provided.

  
 
## How to start?
-----------   
the test should be complied
```
$ cd $ROSTA_SDK_DIR/lin/examples/mt
$ make
```
then test can be run
```
sudo ./rc47_mt_12 -b [board] -v [chip_select] -N [N_d] [-compare/ ] [-rand/ ] [-file output file name]
chip_select: C0 or C1 or C2 or C3
N_d: number of molecules in one filament
-compare to compare CPU and FPGA results
-rand to run CPU test with randomness
-file: if set uses this file to output

```
## Contacts
--------
Yuri Rumyantsev
rumyantsev@rosta.ru
Natalia Abrashitova 
natascha@rosta.ru 
