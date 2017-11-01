import os
import numpy as np
import parameters as p

def get_raw_coords(load_path, name):
    """
    Read coordinates from load_path/name to np.array
    """
    path = os.path.join(load_path,name)
    with open(path, 'r') as f:
        str_to_floats = lambda line: np.fromstring(line, sep = ' ')
        MT_coords_raw = np.array([str_to_floats(line) for line in f.readlines()])
    return MT_coords_raw

def get_raw_types(load_path, name):
    """
    Read types of monomers from load_path/name to np.array
    """
    path = os.path.join(load_path,name)
    with open(path, 'r') as f:
        str_to_ints = lambda line: np.fromstring(line, dtype = int, sep = ' ')
        MT_type_raw = np.array([str_to_ints(line) for line in f.readlines()])
    return MT_type_raw

def coords_raw_to_xyz(MT_coords_raw,):
    """    
    """
    time_steps = len(MT_coords_raw)
    x = np.zeros((time_steps, p.num_of_prot, p.Nd))
    y = np.zeros((time_steps, p.num_of_prot, p.Nd))
    z = np.zeros((time_steps, p.num_of_prot, p.Nd))
    sin_2pi_13 = lambda N_pf: np.sin(2*np.pi*N_pf/p.num_of_prot);
    cos_2pi_13 = lambda N_pf: np.cos(2*np.pi*N_pf/p.num_of_prot);
    get_slice = lambda N_pf, coord_num: slice(p.nd_start + p.Nd*coord_num + N_pf*p.Nd*3, p.nd_stop + p.Nd*coord_num + N_pf*p.Nd*3)
    for N_pf in range(p.num_of_prot):
        x[:, N_pf, p.nd_start:p.nd_stop] = (p.internal_radius + MT_coords_raw[:, get_slice(N_pf, 0)])*sin_2pi_13(N_pf)
        y[:, N_pf, p.nd_start:p.nd_stop] = (p.internal_radius + MT_coords_raw[:, get_slice(N_pf, 0)])*cos_2pi_13(N_pf)
        z[:, N_pf, p.nd_start:p.nd_stop] = MT_coords_raw[:, get_slice(N_pf, 1)]
    return x,y,z

def types_raw_to_normal(MT_type_raw):
    time_steps = len(MT_type_raw)
    MT_type = np.zeros((time_steps, p.num_of_prot, p.Nd))
    get_slice = lambda N_pf: slice(N_pf*p.Nd,(N_pf+1)*p.Nd)
    for N_pf in range(p.num_of_prot):
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
