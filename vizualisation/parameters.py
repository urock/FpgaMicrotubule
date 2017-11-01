import numpy as np

internal_radius = 8.12

Nd = 24
num_of_prot = 13

nd_start = 1
nd_stop = Nd


fps = 20
def_load_path = "/home/matrosik/Documents/microtubules/FpgaMicrotubule/results"
def_result_path = def_load_path

# Set look-up table and redraw
colors = np.array([ [0, 100, 0, 255],
                    [0, 250, 0, 255],
                    [125, 75, 0, 255],
                    [200, 0, 0, 255]])
