"""Module for parsing .dump point clouds"""

__autor__ = "Peter Ďurica"
__contact__ = "xduric05@vutbr.cz"
__version__ = "1.0"

import pyrr
import numpy as np
import math


def save_cloud_points(file_path, check=False):
    f = open(file_path, 'r')
    line = f.readline()
    
    points = []
    normals = []
    tangents = []
    bitangents = []
    SHBuffer = []
    
    PointArray = [None, None, None]
    NormalArray = [None, None, None]
    TangentArray = [None, None, None]
    BitangentArray = [None, None, None]
    SHArray = []

    FirstLine = True
    SHlen = 0

    while line:
        line = line.replace(',', ' ')
        line_values = line.split(' ')

        PointArray[0] = float(line_values[0])
        PointArray[1] = float(line_values[1])
        PointArray[2] = float(line_values[2])

        TangentArray[0] = float(line_values[3])
        TangentArray[1] = float(line_values[4])
        TangentArray[2] = float(line_values[5])

        BitangentArray[0] = float(line_values[6])
        BitangentArray[1] = float(line_values[7])
        BitangentArray[2] = float(line_values[8])

        NormalArray[0] = float(line_values[9])
        NormalArray[1] = float(line_values[10])
        NormalArray[2] = float(line_values[11])

        SHArray = [float(val) for val in line_values[12:]]
        SHArray = np.array(SHArray, np.float32)

        if FirstLine:
            SHlen = len(SHArray)
            FirstLine = False
        else:
            if SHlen != len(SHArray):
                raise

        points.append(pyrr.Vector3(PointArray))
        normals.append(pyrr.Vector3(NormalArray))
        tangents.append(pyrr.Vector3(TangentArray))
        bitangents.append(pyrr.Vector3(BitangentArray))
        SHBuffer = np.concatenate((SHBuffer, SHArray), axis=0, dtype=np.float32)
        
        PointArray = [None, None, None]
        NormalArray = [None, None, None]
        TangentArray = [None, None, None]
        BitangentArray = [None, None, None]
        SHArray = []

        if check:
            line = ""
        else:
            line = f.readline()
    f.close()
    return points, normals, tangents, bitangents, SHBuffer, int(math.sqrt(SHlen))

def CheckPoints(file_path):
    try:
        save_cloud_points(file_path, True)
    except:
        return False
    else:
        return True