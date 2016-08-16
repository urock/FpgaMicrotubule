@echo off
rem C:\Xilinx\Vivado\2014.2\bin\vivado.bat -mode batch -source create_project.tcl -notrace
rem C:\Xilinx\Vivado\2014.4\bin\vivado.bat -mode tcl -source create_project.tcl -notrace


set vivado_bin=C:\Xilinx\Vivado_HLS\2014.4\bin\

set PATH=%vivado_bin%;%PATH%;%vivado_bin%..\msys\bin

set AUTOESL_HOME=%~dp0..
set VIVADO_HLS_HOME=%~dp0..

echo ===============================
echo == Vivado HLS Command Prompt 
echo == Available commands:
echo == vivado_hls,apcc,gcc,g++,make
echo ===============================


set RDI_OS_ARCH=32
if [%PROCESSOR_ARCHITECTURE%] == [x86] (
  if defined PROCESSOR_ARCHITEW6432 (
    set RDI_OS_ARCH=64
  )
) else (
  if defined PROCESSOR_ARCHITECTURE (
    set RDI_OS_ARCH=64
  )
)

vivado_hls -f script.tcl

pause