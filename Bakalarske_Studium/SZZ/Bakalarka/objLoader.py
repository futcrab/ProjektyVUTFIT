"""Module for loading obj files"""

__autor__ = "Peter Ďurica"
__contact__ = "xduric05@vutbr.cz"
__version__ = "1.0"

import numpy as np


class ObjLoader:
    buffer = []

    @staticmethod
    def search_data(data_values, coordinates, scale):
        for d in data_values:
            coordinates.append(float(d) * scale)
    
    @staticmethod
    def create_sorted_vertex_buffer(indices_data, vertices):
        for ind in indices_data:
            start = ind * 3
            end = start + 3
            ObjLoader.buffer.extend(vertices[start:end])

    @staticmethod
    def load_model(file, scale=1.0):
        coords = []
        indices = []
        vals = [None, None, None, None]

        with open(file, "r") as f:
            line = f.readline()
            while line:
                values = line.split()
                try:
                    if values[0] == 'v':
                        ObjLoader.search_data(values[1:], coords, scale)
                    elif values[0] == 'f':
                        if len(values[1:]) == 4:
                            for i,v in enumerate(values[1:]):
                                val = v.split('/')
                                vals[i] = int(val[0])-1
                                if i == 3:
                                    indices.append(vals[0])
                                    indices.append(vals[2])
                                indices.append(vals[i])
                        elif len(values[1:]) == 3:
                            for v in values[1:]:
                                val = v.split('/')
                                indices.append(int(val[0])-1)
                except:
                    pass
                line = f.readline()
        
        ObjLoader.create_sorted_vertex_buffer(indices, coords)

        buffer = ObjLoader.buffer.copy()
        ObjLoader.buffer.clear()

        return np.array(indices, dtype=np.uint32), np.array(buffer, dtype=np.float32)