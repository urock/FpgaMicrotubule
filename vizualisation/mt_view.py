#! /usr/bin/env python3

import os
import numpy as np
import mayavi.mlab as mlab
import imageio

import parameters as p
import data_preparation as pr

mlab.options.offscreen = True

if __name__ == '__main__':
    mt_coords_raw = pr.get_raw_coords(p.def_load_path, 'MT_coords.txt')
    mt_type_raw = pr.get_raw_types(p.def_load_path, 'MT_types.txt')

    x, y, z = pr.coords_raw_to_xyz(mt_coords_raw)
    mt_type = pr.types_raw_to_normal(mt_type_raw)
    x, y, z, mt_type = pr.delete_elems(x, y, z, mt_type)

    time_steps = len(x)

    result_path = os.path.join(p.def_result_path, 'movie.mp4')
    with imageio.get_writer(result_path, mode='I', fps = p.fps) as writer:

        fig = mlab.figure(size=(400, 800), bgcolor=(1,1,1))
        points = mlab.points3d(x[0], y[0], z[0], mt_type[0],
                                resolution=20,
                                scale_mode = "none",
                                scale_factor = 5.)        
        points.glyph.color_mode = "color_by_scalar"
        points.module_manager.scalar_lut_manager.lut.table = p.colors
        mlab.view(40, distance = 400)

        writer.append_data(mlab.screenshot())

        for i in range(1,time_steps):
            points.mlab_source.set(x = x[i], y = y[i], z = z[i], s = mt_type[i])
            writer.append_data(mlab.screenshot())
            print("step {0} done".format(i))
