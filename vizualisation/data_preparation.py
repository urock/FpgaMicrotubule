import os
import numpy as np
from parameters import *

def get_raw_coords(load_path, name):
    path = os.path.join(load_path,name)
    with open(path, 'r') as f:
        str_to_floats = lambda line: np.fromstring(line[1:], sep = ' ')
        MT_coords_raw = np.array([str_to_floats(line) for line in f.readlines()])
    return MT_coords_raw

def get_raw_types(load_path, name):
    path = os.path.join(load_path,name)
    with open(path, 'r') as f:
        str_to_ints = lambda line: np.fromstring(line, dtype = int, sep = ' ')
        MT_type_raw = np.array([str_to_ints(line) for line in f.readlines()])
    return MT_type_raw

def coords_raw_to_xyz(MT_coords_raw,):
    time_steps = len(MT_coords_raw)
    x = np.zeros((time_steps, num_of_prot, Nd))
    y = np.zeros((time_steps, num_of_prot, Nd))
    z = np.zeros((time_steps, num_of_prot, Nd))
    sin_2pi_13 = lambda N_pf: np.sin(2*np.pi/13*(N_pf-1));
    cos_2pi_13 = lambda N_pf: np.cos(2*np.pi/13*(N_pf-1));
    get_slice = lambda N_pf, coord_num: slice(nd_start + Nd*coord_num + N_pf*Nd*3, nd_stop + Nd*coord_num + N_pf*Nd*3)
    for N_pf in range(num_of_prot):
        x[:, N_pf, nd_start:nd_stop] = (internal_radius + MT_coords_raw[:, get_slice(N_pf, 0)])*sin_2pi_13(N_pf)
        y[:, N_pf, nd_start:nd_stop] = (internal_radius + MT_coords_raw[:, get_slice(N_pf, 0)])*cos_2pi_13(N_pf)
        z[:, N_pf, nd_start:nd_stop] = MT_coords_raw[:, get_slice(N_pf, 1)]
    return x,y,z

def types_raw_to_normal(MT_type_raw):
    time_steps = len(MT_type_raw)
    MT_type = np.zeros((time_steps, num_of_prot, Nd))
    get_slice = lambda N_pf: slice(N_pf*Nd,(N_pf+1)*Nd)
    for N_pf in range(num_of_prot):
        MT_type[:, N_pf] = MT_type_raw[:, get_slice(N_pf)]
    MT_type *= 2
    MT_type[:,:,::2] += 1
    return MT_type

def delete_elems(x, y, z, MT_type):
    mask = np.array([elem < -5. for elem in z])
    x[mask] = 0
    y[mask] = 0
    z[mask] = 15
    MT_type[mask] = 0
    return x, y, z, MT_type
