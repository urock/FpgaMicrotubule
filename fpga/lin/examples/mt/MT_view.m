function MT_view(Load_path, N_exp, N_dir, N_exp_prior, old_format)

d_write = (2e+4);
dt = 2e-10;
doNotShowSeed = 0;
% add_suff = 'd_';
add_suff = '';


global delta_gamma;
global add_kinesins;
global delta_length;
show_sites = 0;
Max_kin_number = 300;
global index_file;
global Nd;
nd_start = 1;
nd_stop = Nd;

bottom_dimer = zeros(1,13);
for npf = 1 : 1 : 13    
    bottom_dimer(npf) = 2.0*2 + 2.0*6/13*npf + 2*2*(9-2) + 0.2; %9
end

global mov;

global writerObj;

framesps = 4;
if (N_exp_prior <= 0)
    
    
    Names_0 =  load([Load_path,'Names.txt']);  
    Names = [];
    for first = 1 : 1 : length(Names_0)
        current_n = Names_0(first);
        flag_coin = 0;
        for second = 1 : 1 : length(Names_0)
            check_n = Names_0(second);
            if (current_n == check_n && first ~= second)
                flag_coin = 1;
            end
        end
        if flag_coin == 0       
            Names = [Names, Names_0(first)];
        end
    end
    
    dlmwrite([Load_path,'\Names_sel.txt'],Names,'delimiter','\n','precision', 7);
    
   
%    MT_coords_raw =   load([Load_path,add_suff,'MT_coords_FPGA',num2str(index_file),'.txt']);%load([Load_path,add_suff,num2str(Names(N_exp)),'______MT_coords.txt']); %Type_mon =   load([Load_path,N_exp,'Type_mon.txt']); 
    MT_coords_raw =   load([Load_path,add_suff,'MT_coords_CPU.txt']);
%    Type_mon =   load([Load_path,add_suff,num2str(Names(N_exp)),'______Type_mon.txt']);
    Type_mon =   load([Load_path,add_suff,'type_MT_coords_CPU.txt']);

    %mov = avifile([N_dir, '_', num2str(N_exp),'_', num2str(Names(N_exp)), '__MT.avi'], 'compression','Cinepak','fps', framesps);
	%writerObj = VideoWriter([N_dir, '_', num2str(N_exp),'_', num2str(Names(N_exp)), '__MT.avi']);
    writerObj = VideoWriter([Load_path,add_suff,'MT_coords_FPGA',num2str(index_file),'.avi']);
     if add_kinesins == 1  
        Kinesins_points =   load([Load_path,num2str(Names(N_exp)),'______Kinesins.txt']);
    end

elseif(~old_format)
    MT_coords_raw =   load([Load_path,add_suff,num2str(N_exp_prior),'______MT_coords.txt']); %Type_mon =   load([Load_path,N_exp,'Type_mon.txt']); 
    Type_mon =   load([Load_path,add_suff,num2str(N_exp_prior),'______Type_mon.txt']);    
    if add_kinesins == 1
        Kinesins_points =   load([Load_path,num2str(N_exp_prior),'______Kinesins.txt']);            
    end
    %mov = avifile([N_dir, '_', num2str(N_exp_prior),'__MT.avi'], 'compression','Cinepak','fps', framesps);
	%writerObj = VideoWriter([N_dir, '_', num2str(N_exp_prior),'__MT.avi']);
    writerObj = VideoWriter([Load_path,add_suff,'MT_coords_FPGA',num2str(index_file),'.avi']);
elseif(old_format)
    MT_coords_raw =   load([Load_path,'00',num2str(N_exp_prior),'_MT_coords.txt']); %Type_mon =   load([Load_path,N_exp,'Type_mon.txt']); 
    Type_mon =   load([Load_path,'00',num2str(N_exp_prior),'_Type_mon.txt']);    
    %mov = avifile([N_dir, '_', num2str(N_exp_prior),'_MT.avi'], 'compression','Cinepak','fps', framesps);
	writerObj = VideoWriter([Load_path,add_suff,'MT_coords_FPGA',num2str(index_file),'.avi']);%VideoWriter([N_dir, '_', num2str(N_exp_prior),'_MT.avi']);
end

writerObj.FrameRate = framesps;
open(writerObj);

sz_M = size(MT_coords_raw);
MT_coords = zeros(13, Nd*3);
internal_radius = 8.12;

% % lateral view, short MT

% fig_handle = figure('position',[10 30 400   900]); 
fig_handle = figure('position',[10 30 400   900], 'visible','off'); 
%axis([-60 60 -60 60 40+70 240+70]); az = 180; el = 15;
axis([-60 60 -60 60 70 240+70]); az = 180; el = 15;

%axis([-60 60 -60 60 0 200]); az = 180; el = 15;

% figure('position',[10 30 400   900]); axis([-60 60 -60 60 60-250 290-250]); az = 180; el = 5; 


% ---------->  
% figure('position',[10 30 400   900]); 
% axis([-80 80 -80 80 20+110 290+110]); az = 180; el = 5; %long MT
% axis([-80 80 -80 80 20 290]); az = 180; el = 5;%from seed
% axis([-80 80 -80 80 20+65 290+65]); az = 180; el = 5;
% lateral view, long MT
% figure('position',[10 20 300   1000]); axis([-100 100 -100 100 -140+140 60+140+500]);az = 0; el = 15; view(az, el);

% lateral view, short MT, nd = 70
% figure('position',[10 30 400   900]); axis([-60 60 -60 60 -140+200 60+200]);az = 0; el = 15; view(az, el);

view(az, el);
hold on;

p = 15; % precision
t = 0:1/(p-1):1;
[th,ph] = meshgrid( t*pi,t*2*pi );

Col_dark_green(:,:,1) = ones(p)*0;
Col_dark_green(:,:,2) = ones(p)*0.6;
Col_dark_green(:,:,3) = ones(p)*0;

Col_lite_green(:,:,1) = ones(p)*0.1;
Col_lite_green(:,:,2) = ones(p)*1;
Col_lite_green(:,:,3) = ones(p)*0.1;

Col_red(:,:,1) = ones(p)*1.0;
Col_red(:,:,2) = ones(p)*0.62;
Col_red(:,:,3) = ones(p)*0.4;

                        
Col_dark_red(:,:,1) = ones(p)*1.0*1.2;
Col_dark_red(:,:,2) = ones(p)*0.62*0.7;
Col_dark_red(:,:,3) = ones(p)*0.4*0.7; 
Col_lite_red(:,:,1) = ones(p)*1.4;
Col_lite_red(:,:,2) = ones(p)*0.62*1.4;
Col_lite_red(:,:,3) = ones(p)*0.4*1.2;


Col_redred(:,:,1) = ones(p)*1.0;
Col_redred(:,:,2) = ones(p)*0.42;
Col_redred(:,:,3) = ones(p)*0.2;

Col_lite_blue(:,:,1) = ones(p)*1.0;
Col_lite_blue(:,:,2) = ones(p)*0.4;
Col_lite_blue(:,:,3) = ones(p)*0.3;

Col_blue(:,:,1) = ones(p)*0;
Col_blue(:,:,2) = ones(p)*0.1;
Col_blue(:,:,3) = ones(p)*0.7;

first = @(v) v(1);

sin_cos_th = 2*sin(th).*cos(ph) ;
sin_sin_th = 2*sin(th).*sin(ph);
cos_th = 2*cos(th);

if show_sites == 1
    sin_cos_th_Int = 0.5*sin(th).*cos(ph) ;
    sin_sin_th_Int = 0.5*sin(th).*sin(ph);
    cos_th_Int = 0.5*cos(th);
end
mon_rad = 2;

Kin_psi = pi/2;
Kin_fi = -pi/9;

psi_angle_R = 1.801785;
fi_angle_R = 1.3291395;

psi_angle_left = -1.339725;
fi_angle_left = 1.0856;


% set(gcf,'renderer','zbuffer')
opengl('hardware');
for iter =  1 : 1 : sz_M(1)

    disp(iter)
    cla
    for N_pf = 1 : 1 : 13    
        sin_2pi_13 = sin(2*pi/13*(N_pf-1) + delta_gamma);
        cos_2pi_13 = cos(2*pi/13*(N_pf-1) + delta_gamma);
        for nd = nd_start : 1 : nd_stop
            if(MT_coords_raw(iter, nd + Nd + (N_pf-1)*Nd*3) ~= -1000.0) 
                MT_coords(N_pf, nd) =    (internal_radius + MT_coords_raw(iter, nd + (N_pf-1)*Nd*3))*sin_2pi_13;        
                MT_coords(N_pf, nd+Nd) = (internal_radius + MT_coords_raw(iter, nd + (N_pf-1)*Nd*3))*cos_2pi_13;
                MT_coords(N_pf, nd+Nd*2) = MT_coords_raw(iter, nd + Nd + (N_pf-1)*Nd*3) + delta_length;
            elseif(MT_coords_raw(iter, nd + Nd + (N_pf-1)*Nd*3) == -1000.0)
                MT_coords(N_pf, nd) =    0;
                MT_coords(N_pf, nd+Nd) = 0;
                MT_coords(N_pf, nd+Nd*2) = 0;
            end
        end
    end
    

    for N_pf = 1 : 1 : 13
                
        if(N_pf == 1) gamma = (2*pi/13*(N_pf-23)); 
        elseif(N_pf == 2) gamma = (2*pi/13*(N_pf-25)); 
        elseif(N_pf == 3) gamma = (2*pi/13*(N_pf-1)); 
        elseif(N_pf == 4) gamma = (2*pi/13*(N_pf-3)); 
        elseif(N_pf == 5) gamma = (2*pi/13*(N_pf-5)); 
        elseif(N_pf == 6) gamma = (2*pi/13*(N_pf-7)); 
        elseif(N_pf == 7) gamma = (2*pi/13*(N_pf-9)); 
        elseif(N_pf == 8) gamma = (2*pi/13*(N_pf-11));
        elseif(N_pf == 9) gamma = (2*pi/13*(N_pf-13)); 
        elseif(N_pf == 10) gamma = (2*pi/13*(N_pf-15));
        elseif(N_pf == 11) gamma = (2*pi/13*(N_pf-17));
        elseif(N_pf == 12) gamma = (2*pi/13*(N_pf-19));
        elseif(N_pf == 13) gamma = (2*pi/13*(N_pf-21));
        end
        
        R_gamma_z_right = [cos(gamma) -sin(gamma) 0; sin(gamma)   cos(gamma) 0; 0 0 1];
        R_gamma_z_left = [cos(gamma) -sin(gamma) 0; sin(gamma)    cos(gamma) 0; 0 0 1]; 
        
        for nd = nd_start : 1 : nd_stop
            cos_t = cos(MT_coords_raw(iter, nd + Nd*2 + (N_pf-1)*Nd*3));
            sin_t = sin(MT_coords_raw(iter, nd+Nd*2 + (N_pf-1)*Nd*3));  
          
            R_theta_y = [cos_t 0 sin_t; 0 1 0; -sin_t 0 cos_t];  
            A_str = [mon_rad*sin(psi_angle_R)*cos(fi_angle_R);  mon_rad*sin(psi_angle_R)*sin(fi_angle_R);   mon_rad*cos(psi_angle_R)];
            A_str_left = [mon_rad*sin(psi_angle_left)*cos(fi_angle_left);  mon_rad*sin(psi_angle_left)*sin(fi_angle_left);   mon_rad*cos(psi_angle_left)];
           
            if(MT_coords(N_pf, nd) ~= 0.0 || MT_coords(N_pf, nd+Nd) ~= 0) 
                 
                 x = sin_cos_th + MT_coords(N_pf, nd);
                 y = sin_sin_th + MT_coords(N_pf, nd+Nd);
                 z = cos_th + MT_coords(N_pf, nd+Nd*2);
                if show_sites == 1      
                 MT_int_points_r = [MT_coords(N_pf, nd); MT_coords(N_pf, nd+Nd); MT_coords(N_pf, nd+Nd*2)]+ ...
                     R_gamma_z_right*R_theta_y*A_str;
                 MT_int_points_L = [MT_coords(N_pf, nd); MT_coords(N_pf, nd+Nd); MT_coords(N_pf, nd+Nd*2)]+ ...
                    R_gamma_z_left*R_theta_y*A_str_left;
                
                x_int_L = sin_cos_th_Int + MT_int_points_L(1);
                y_int_L = sin_sin_th_Int + MT_int_points_L(2);
                z_int_L = cos_th_Int + MT_int_points_L(3);
                
                x_int_right = sin_cos_th_Int + MT_int_points_r(1);
                y_int_right = sin_sin_th_Int + MT_int_points_r(2);
                z_int_right = cos_th_Int + MT_int_points_r(3);
                end
             
                if ( MT_coords(N_pf, nd+Nd*2) < bottom_dimer(N_pf) + 4 )  
                    Col = Col_lite_blue;                                        
%                 elseif(Type_mon(iter, nd + (N_pf-1)*Nd) == 0  || Type_mon(iter, nd + (N_pf-1)*Nd) == -1)                    
%                 Col =  Col_dark_green;
                elseif(Type_mon(iter, nd + (N_pf-1)*Nd) == 0  || Type_mon(iter, nd + (N_pf-1)*Nd) == -1 || (MT_coords(N_pf, nd+Nd*2) < bottom_dimer(N_pf) + 4))                    
% Type_mon(iter, nd + (N_pf-1)*Nd) == 1  || 
                    if first(factor(nd)) - 2
                        %odd       
                        Col = Col_dark_green;           
                    else
                        %even
                        Col = Col_lite_green;                           
                    end            
                elseif(Type_mon(iter, nd + (N_pf-1)*Nd) == 1)% && ~(doNotShowSeed && MT_coords(N_pf, nd+Nd*2) < bottom_dimer(N_pf) + 4))      
%                     Col = Col_red;

                    if first(factor(nd)) - 2
                        %odd       
                        Col = Col_dark_red;           
                    else
                        %even
                        Col = Col_lite_red;                           
                    end
                    
                end
                
                if(0)%(Type_mon(iter, nd + (N_pf-1)*Nd) == -1 && Type_mon(iter, nd + (N_pf-1)*Nd +1) ~= -1)                      
                    Col = Col_blue;
                end
                surf(x,y,z, Col);
                if show_sites == 1
                    surf(x_int_L, y_int_L, z_int_L, Col_blue);
                    surf(x_int_right, y_int_right, z_int_right, Col_lite_green);
                end
              
                gamma = gamma;
            end
        end
    end
    
    for N_pf= 1 : 1 : 13
    for nd = 1 : 1 : nd_stop       
    end
    end
    
    Kinesins_column = [2*sin(Kin_psi)*cos(Kin_fi);  2*sin(Kin_psi)*sin(Kin_fi);   2*cos(Kin_psi)];     
    if add_kinesins == 1  
    for nsite_kin = 1 : 1 : Max_kin_number          
        if(Kinesins_points(iter, nsite_kin + 2*Max_kin_number)  ~=  -1)
            %                     if(Kinesins_points(iter, nsite_kin + 1*Max_kin_number) == nd-1 || Kinesins_points(iter, nsite_kin) == nd-1 )
            N_pf = Kinesins_points(iter, nsite_kin + 2*Max_kin_number) + 1;    
            
             if(N_pf == 1) gamma = (2*pi/13*(N_pf-23)); 
            elseif(N_pf == 2) gamma = (2*pi/13*(N_pf-25)); 
            elseif(N_pf == 3) gamma = (2*pi/13*(N_pf-1)); 
            elseif(N_pf == 4) gamma = (2*pi/13*(N_pf-3)); 
            elseif(N_pf == 5) gamma = (2*pi/13*(N_pf-5)); 
            elseif(N_pf == 6) gamma = (2*pi/13*(N_pf-7)); 
            elseif(N_pf == 7) gamma = (2*pi/13*(N_pf-9)); 
            elseif(N_pf == 8) gamma = (2*pi/13*(N_pf-11));
            elseif(N_pf == 9) gamma = (2*pi/13*(N_pf-13)); 
            elseif(N_pf == 10) gamma = (2*pi/13*(N_pf-15));
            elseif(N_pf == 11) gamma = (2*pi/13*(N_pf-17));
            elseif(N_pf == 12) gamma = (2*pi/13*(N_pf-19));
            elseif(N_pf == 13) gamma = (2*pi/13*(N_pf-21));
             
             end
             R_gamma_z_right = [cos(gamma) -sin(gamma) 0; sin(gamma)   cos(gamma) 0; 0 0 1];
            
            if(Kinesins_points(iter, nsite_kin + 1*Max_kin_number) ~=  -1 )% Site B
                  nd_1 = Kinesins_points(iter, nsite_kin + 1*Max_kin_number) + 1;         
                  Kinesins_column = [2*sin(Kin_psi)*cos(Kin_fi);  2*sin(Kin_psi)*sin(Kin_fi);   2*cos(Kin_psi)];                    
                  Kin_koords = [MT_coords(N_pf, nd_1); MT_coords(N_pf, nd_1+Nd); MT_coords(N_pf, nd_1+Nd*2)]+R_gamma_z_right * R_theta_y*Kinesins_column; 
                  x_Kin = sin_cos_th + Kin_koords(1);
                  y_Kin = sin_sin_th + Kin_koords(2);
                  z_Kin = cos_th + Kin_koords(3);
                  surf(x_Kin, y_Kin, z_Kin, Col_blue);
                  hold on;
                  X1 = [Kin_koords(1) Kin_koords(2) Kin_koords(3)];
            end
            
            if (  Kinesins_points(iter, nsite_kin)    ~=  -1 )% Site A
                nd_2 = Kinesins_points(iter, nsite_kin) + 1;          
                Kin_koords = [MT_coords(N_pf, nd_2); MT_coords(N_pf, nd_2+Nd); MT_coords(N_pf, nd_2+Nd*2)]+R_gamma_z_right * R_theta_y*Kinesins_column;         
                x_Kin = sin_cos_th + Kin_koords(1);
                y_Kin = sin_sin_th + Kin_koords(2);
                z_Kin = cos_th + Kin_koords(3);
                surf(x_Kin, y_Kin, z_Kin, Col_blue);
                hold on;
                X2 = [Kin_koords(1) Kin_koords(2) Kin_koords(3)];
            end         
             if(Kinesins_points(iter, nsite_kin + 1*Max_kin_number) ~=  -1  && Kinesins_points(iter, nsite_kin)    ~=  -1 )                        
                        r = 1.5;
                        n = 20;
                        cyl_color = 'b';
                        closed = 1;
                        lines = 0;
                        Cylinder_along_line(X1,X2,r,n,cyl_color,closed,lines);                 
             end
                          
        end        
    end
    end

   
    view(az - 50 , el);
    shading interp;
    lighting gouraud;
    camlight infinite;    
    view(az, el);
    
%     view(az-220, el);

% 
% text(-20, 150, 100,...
% 	['Time = ',num2str(iter * d_write * dt , '% .2f'), '  s '],...
% 	'HorizontalAlignment','left',... 
% 	'BackgroundColor',[1 1 1],'FontSize',12);


% axis([-33 33 -30 30 80 180]); view([-120 35])

    %imwrite(fig_handle, [num2str(iter), '.png']);

    %F = getframe(gca);
    %pause(0.2); %drawnow 
    
    F = getframe(fig_handle);
    %mov = addframe(mov,F);
	writeVideo(writerObj,F);


end
%mov = close(mov);
close(writerObj);
% axis off;
% close all;

