
global mov;
clc
old_format = 0;
global add_kinesins;
add_kinesins = 0; 
global Nd;
global catiter; catiter = 0;
Nd = 36; % SIZE OF MICROTUBES
global delta_gamma;
global delta_length;
delta_gamma = 132*pi/180    +(230-80-35)*pi/180; 
delta_length = 100;
global index_file;
N=18; %NUMBER OF FILES WE WANT TO PROCEED

close all;
% for j=19:20,
% NAME OF FILE = "MT_FPGA#####", ####=index_file
% index_file=['_C1_' num2str(j)]; %=j;
index_file=''; 

N_dir = ''; Load_path = [pwd, N_dir,'/'];                                     
N_exp = 1;  N_exp_prior = 0; tic; MT_view(Load_path, N_exp, N_dir, N_exp_prior, old_format); disp([N_dir,'_',num2str(N_exp)]); toc
% end



