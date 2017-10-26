#! /usr/bin/env python3

import os
import numpy as np
import mayavi.mlab as mlab
import imageio

from parameters import *
import data_preparation as pr

mlab.options.offscreen = True

if __name__ == '__main__':
    fps = 20
    load_path = "/home/matrosik/Documents/microtubules/FpgaMicrotubule/vizualisation/experiments"
    result_folder_path = load_path
    result_path = os.path.join(result_folder_path, 'movie.mp4')

    mt_coords_raw = pr.get_raw_coords(load_path, 'MT_coords.txt')
    mt_type_raw = pr.get_raw_types(load_path, 'MT_types.txt')

    x, y, z = pr.coords_raw_to_xyz(mt_coords_raw)
    mt_type = pr.types_raw_to_normal(mt_type_raw)
    x, y, z, mt_type = pr.delete_elems(x, y, z, mt_type)

    time_steps = len(x)

    with imageio.get_writer(result_path, mode='I', fps = fps) as writer:

        fig = mlab.figure(size=(400, 800), bgcolor=(1,1,1))
        points = mlab.points3d(x[0],
                               y[0],
                               z[0],
                               mt_type[0],
                                resolution=20, scale_mode = "none",
                                 scale_factor = 5.)
        
        points.glyph.color_mode = "color_by_scalar"
        # Set look-up table and redraw
        colors = np.array([ [0, 100, 0, 255],
                            [0, 250, 0, 255],
                            [125, 75, 0, 255],
                            [200, 0, 0, 255]])
        points.module_manager.scalar_lut_manager.lut.table = colors
        mlab.view(40, distance = 400)

        writer.append_data(mlab.screenshot())

        for i in range(1,time_steps):
            points.mlab_source.set(x = x[i], y = y[i], z = z[i], s = mt_type[i])
            writer.append_data(mlab.screenshot())
            print("step {0} done".format(i))
