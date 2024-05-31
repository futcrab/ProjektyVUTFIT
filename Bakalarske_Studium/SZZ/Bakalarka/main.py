"""Main source file of the project"""

__autor__ = "Peter Ďurica"
__contact__ = "xduric05@vutbr.cz"
__version__ = "1.0"

import imgui
from imgui.integrations.glfw import GlfwRenderer
import glfw
from OpenGL.GL import *
from OpenGL.GL.shaders import compileProgram, compileShader
import numpy as np
import pyrr
import random
import os
import sys
import math
from objLoader import ObjLoader
from read_xml_scene import read_xml_scene, CheckXml
from read_point_cloud import save_cloud_points, CheckPoints
from FileDialog import FileDialog

#For mouse movements
from camera import Camera

cam = Camera()
Width, Height = 1600, 900
lastX, lastY = Width / 2, Height / 2
first_mouse = True

UsageString = '''
Usage:

python3 main.py [Nastavenie typu heatmapy]

heatmapa: 
-v alebo -vert alebo -vertex : Heatmapa sa pocita na vertexoch objektov (menej presne ale lepsi vykon)
-f alebo -frag alebo -fragment : Heatmapa sa pocita na fragmentoch objektov na obrazovke (viac presna ale slabsi vykon)
-h alebo -help : print usage
chyba v spustani programu : print usage
Parametre niesu case sensitive
'''

vertex_src = ""
fragment_src = ""
heatmapType = ""

if len(sys.argv) == 2:
    match sys.argv[1].upper():
        case "-V" | "-VERTEX" | "-VERT":
            with open('shaders/vertex.glsl', 'r') as f:
                vertex_src = f.read()
            with open('shaders/fragment.glsl', 'r') as f:
                fragment_src = f.read()
            heatmapType = "perVertex"
        
        case "-F" | "-FRAGMENT" | "-FRAG":
            with open('shaders/PFvertex.glsl', 'r') as f:
                vertex_src = f.read()
            with open('shaders/PFfragment.glsl', 'r') as f:
                fragment_src = f.read()
            heatmapType = "perFragment"

        case "-H" | "-HELP" | _:
            print(UsageString)
            exit()

else:
    print(UsageString)
    exit()

def FilterPoints(p_positions: list, object_buffer: np.array, distance, model_matrix):
    indexes = []
    o_positions = np.copy(object_buffer.reshape(-1, 3))

    o_positions = np.c_[o_positions, np.ones(len(o_positions))]
    o_positions = np.dot(o_positions, model_matrix)

    o_positions = o_positions.swapaxes(0,1)

    padding = distance

    sizeofObj = {
        'minX': min(o_positions[0]) - padding,
        'maxX': max(o_positions[0]) + padding,
        'minY': min(o_positions[1]) - padding,
        'maxY': max(o_positions[1]) + padding,
        'minZ': min(o_positions[2]) - padding,
        'maxZ': max(o_positions[2]) + padding,
    }

    for i, position in enumerate(p_positions):
        inX = position[0] >= sizeofObj["minX"] and position[0] <= sizeofObj["maxX"]
        inY = position[1] >= sizeofObj["minY"] and position[1] <= sizeofObj["maxY"]
        inZ = position[2] >= sizeofObj["minZ"] and position[2] <= sizeofObj["maxZ"]

        if inX and inY and inZ:
            indexes.append(i)
    
    return np.array(indexes, dtype=np.int32)

def generate_half_sphere(rows, cols):
    global HSphereAO, HSphereBO
    # Calculate the vertex positions for the half-sphere
    vertices = []
    for i in range(rows + 1):
        theta = i / (rows) * math.pi / 2.0
        for j in range(cols):
            phi = j / (cols) * math.pi * 2.0
            x = math.cos(phi) * math.sin(theta)
            y = math.cos(theta)
            z = math.sin(phi) * math.sin(theta)
            vertices.append([x, y, z])

    buffer = []
    for i in range(rows):
        for j in range(cols):
            # Calculate indices for the four vertices of the quad
            v0 = i * cols + j
            v1 = v0 + 1
            v2 = (i + 1) * cols + j
            v3 = v2 + 1

            # Check if segfault
            if v1 % cols == 0:
                v1 -= cols
                v3 -= cols
            
            # Add two triangles to form the quad
            buffer.extend(vertices[v0])
            buffer.extend(vertices[v1])
            buffer.extend(vertices[v2])
            buffer.extend(vertices[v2])
            buffer.extend(vertices[v1])
            buffer.extend(vertices[v3])

    # Create a numpy array for the vertices
    buffer = np.array(buffer, dtype=np.float32)

    glDeleteBuffers(1, [HSphereBO])

    # Create a vertex buffer object (VBO)
    HSphereBO = glGenBuffers(1)
    glBindVertexArray(HSphereAO)
    glBindBuffer(GL_ARRAY_BUFFER, HSphereBO)
    glBufferData(GL_ARRAY_BUFFER, buffer.nbytes, buffer, GL_STATIC_DRAW)

    # Set up the vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, False, 0, None)
    glEnableVertexAttribArray(0)

    # Return the number of vertices in the mesh
    return int(len(buffer)/3)

def window_resize(window, width, height):
    glViewport(0, 0, width, height)
    if height != 0.0:
        projection = pyrr.matrix44.create_perspective_projection(60, width/height, 0.01, 200)
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection)

movementSpeed = 2.5
# For Movement and looking with mouse
def processInput(window):
    global camera_pos, camera_front, camera_up, delta_time, first_mouse
    cameraSpeed = movementSpeed * delta_time
    if glfw.get_key(window, glfw.KEY_W) == glfw.PRESS:
        camera_pos += cameraSpeed * camera_front

    if glfw.get_key(window, glfw.KEY_S) == glfw.PRESS:
        camera_pos -= cameraSpeed * camera_front

    if glfw.get_key(window, glfw.KEY_A) == glfw.PRESS:
        camera_pos -= np.cross(camera_front, camera_up) * cameraSpeed

    if glfw.get_key(window, glfw.KEY_D) == glfw.PRESS:
        camera_pos += np.cross(camera_front, camera_up) * cameraSpeed

    if glfw.get_mouse_button(window, glfw.MOUSE_BUTTON_RIGHT) == glfw.PRESS:
        camera_front, camera_up = cam.get_values()
        first_mouse = False


def mouse_look_callback(window, xpos, ypos):
    global lastX, lastY, first_mouse

    if first_mouse:
        lastX = xpos
        lastY = ypos

    xoffset = xpos - lastX
    yoffset = lastY - ypos

    lastX = xpos
    lastY = ypos

    cam.process_mouse_movement(xoffset, yoffset)


def mouse_button_callback(window, button, action, mods):
    global first_mouse
    if button == glfw.MOUSE_BUTTON_RIGHT and action == glfw.RELEASE:
        first_mouse = True

def GetXYZfromValue(string):
    xyz = string.split(',')
    
    if len(xyz) == 1:
        xyz[0] = float(xyz[0])
        return [xyz[0], xyz[0], xyz[0]]
    
    xyz = [float(i) for i in xyz]

    return xyz

def GetMatrixFromString(string):
    matrix = string.split(' ')
    matrix = [float(i) for i in matrix]
    if len(matrix) == 9:
        matrix = np.array(matrix).reshape(3,3).transpose()
        matrix = pyrr.matrix44.create_from_matrix33(matrix)
    else:
        matrix = np.array(matrix).reshape(4,4).transpose()
    return matrix


def CreateModelMatrix(i):
    matrix = pyrr.matrix44.create_identity()
    
    for transformation in i:
        xyzT = [0.0, 0.0, 0.0]
        xyzR = [0.0, 0.0, 0.0]
        angle = 0.0
        xyzS = [1.0, 1.0, 1.0]
        
        if transformation["type"] == "scale" or transformation["type"] == "radius":
            if "value" in transformation:
                xyzS = GetXYZfromValue(transformation["value"])
            if "x" in transformation:
                xyzS[0] = float(transformation["x"])
            if "y" in transformation:
                xyzS[1] = float(transformation["y"])
            if "z" in transformation:
                xyzS[2] = float(transformation["z"])
            scaleX = pyrr.matrix33.create_direction_scale([1.0, 0.0, 0.0], xyzS[0])
            scaleY = pyrr.matrix33.create_direction_scale([0.0, 1.0, 0.0], xyzS[1])
            scaleZ = pyrr.matrix33.create_direction_scale([0.0, 0.0, 1.0], xyzS[2])
            scaleM = pyrr.matrix33.multiply(scaleX, scaleY)
            scaleM = pyrr.matrix33.multiply(scaleM, scaleZ)
            matrix = pyrr.matrix44.multiply(matrix, pyrr.matrix44.create_from_matrix33(scaleM))
        
        if transformation["type"] == "rotate":
            if "angle" in transformation:
                angle = math.radians(float(transformation["angle"]))
            if "value" in transformation:
                xyzR = GetXYZfromValue(transformation["value"])
            if "x" in transformation:
                xyzR[0] = float(transformation["x"])
            if "y" in transformation:
                xyzR[1] = float(transformation["y"])
            if "z" in transformation:
                xyzR[2] = float(transformation["z"])
            matrix = pyrr.matrix44.multiply(matrix, pyrr.matrix44.create_from_axis_rotation(np.array(xyzR), angle))

        if transformation["type"] == "matrix":
            matrix = pyrr.matrix44.multiply(matrix, GetMatrixFromString(transformation["value"]))
        
        if transformation["type"] == "translate":
            if "value" in transformation:
                xyzT = GetXYZfromValue(transformation["value"])
            if "x" in transformation:
                xyzT[0] = float(transformation["x"])
            if "y" in transformation:
                xyzT[1] = float(transformation["y"])
            if "z" in transformation:
                xyzT[2] = float(transformation["z"])
            matrix = pyrr.matrix44.multiply(matrix, pyrr.matrix44.create_from_translation(np.array(xyzT)))
    
    return matrix

def ReadXMLObjects(i):
    path = ""
    addDir = False
    matrix = pyrr.matrix44.create_identity()
    if i["type"] == "obj":
        path = i["string"]["value"]
        addDir = True
    if i["type"] == "sphere":
        path = "obj/sphere.obj"
    if i["type"] == "rectangle":
        path = "obj/rectangle.obj"
    if "transform" in i:
        matrix = CreateModelMatrix(i["transform"]["transform"])

    return path, matrix, addDir

def free_object_data(mode):
    global VAO, VBO, InstanceSphereBO, PointTangentBuffer, PointBitangentBuffer, PointNormalBuffer, SHBufferPos,BufferObjectPointsPos, BufferPointsPos, BufferPointsNor, nOfObjects, ShowObj, ShowPoints, SceneLoaded, CloudLoaded
    if mode == 1 or mode == 3:
        if nOfObjects != 0:
            glDeleteBuffers(nOfObjects, VBO)
            glDeleteVertexArrays(nOfObjects, VAO)
            VAO = []
            VBO = []
        nOfObjects = 0
        SceneLoaded = False
    if mode == 2 or mode == 3:
        if InstanceSphereBO != -1:
            glDeleteBuffers(1, [InstanceSphereBO])
            InstanceSphereBO = -1
            glDeleteBuffers(1, [PointTangentBuffer])
            PointTangentBuffer = -1
            glDeleteBuffers(1, [PointBitangentBuffer])
            PointBitangentBuffer = -1
            glDeleteBuffers(1, [PointNormalBuffer])
            PointNormalBuffer = -1
            glDeleteBuffers(1, [SHBufferPos])
            SHBufferPos = -1
        if BufferObjectPointsPos != -1 and BufferPointsPos != -1 and BufferPointsNor != -1:
            glDeleteBuffers(1, [BufferObjectPointsPos])
            glDeleteBuffers(1, [BufferPointsPos])
            glDeleteBuffers(1, [BufferPointsNor])
            BufferObjectPointsPos = -1
            BufferPointsPos = -1
            BufferPointsNor = -1
        CloudLoaded = False

def ReloadScene(mode, distance):
    global PointTangentBuffer, PointBitangentBuffer, PointNormalBuffer, nOfPoints_loc, SHLineLength_loc, scene_xml, pointCloud_file, VAO, VBO, InstanceSphereBO, BufferObjectPointsPos, BufferPointsPos, BufferPointsNor, nOfObjects, LenOfVert, NobjectPoints, Colors, pos, nOfPoints, SphereAO, HSphereAO, SceneLoaded, CloudLoaded, m_bandsMax

    free_object_data(3)

    p_positions = 0
    # Loading Point Cloud
    if mode == 2 or mode == 3:
        p_positions, p_normals, p_tangents, p_bitangents, SHBuffer, m_bandsMax = save_cloud_points(pointCloud_file)
        nOfPoints = len(p_positions)

        #Loading nOfPoints to shader and linelength and VerticeCount of halfSphere
        glUniform1i(nOfPoints_loc, nOfPoints)
        glUniform1i(SHLineLength_loc, m_bandsMax**2)


        pos_points = []
        
        hp_points = np.array(p_positions, np.float32).flatten()

        print("Loading data for heatmap to shaders")
        
        #Adding points position and normal to shader for heatmap calculations 
        #Create buffer object with points and fill with data
        BufferPointsPos = glGenBuffers(1)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferPointsPos)
        glBufferData(GL_SHADER_STORAGE_BUFFER, hp_points.nbytes, hp_points, GL_STATIC_DRAW)

        #Bind buffer to shader storage binding point
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BufferPointsPos)

        #Adding SH values to shaders for SH calculations
        SHBufferPos = glGenBuffers(1)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SHBufferPos)
        glBufferData(GL_SHADER_STORAGE_BUFFER, SHBuffer.nbytes, SHBuffer, GL_STATIC_DRAW)
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, SHBufferPos)

        for i in range(nOfPoints):
            pos_matrix = pyrr.matrix44.create_from_translation(p_positions[i])
            for i in pos_matrix:
                for j in i:
                    pos_points.append(j)
        pos_points = np.array(pos_points, np.float32)

        # Loading Instance positions
        InstanceSphereBO = glGenBuffers(1)
        glBindVertexArray(SphereAO)
        glBindBuffer(GL_ARRAY_BUFFER, InstanceSphereBO)

        glBufferData(GL_ARRAY_BUFFER, pos_points.nbytes , pos_points, GL_STATIC_DRAW)

        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, pos_points.itemsize*16, ctypes.c_void_p(0))
        glVertexAttribDivisor(1, 1)
        
        glEnableVertexAttribArray(2)
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, pos_points.itemsize*16, ctypes.c_void_p(pos_points.itemsize*4))
        glVertexAttribDivisor(2, 1)

        glEnableVertexAttribArray(3)
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, pos_points.itemsize*16, ctypes.c_void_p(2 * pos_points.itemsize*4))
        glVertexAttribDivisor(3, 1)

        glEnableVertexAttribArray(4)
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, pos_points.itemsize*16, ctypes.c_void_p(3 * pos_points.itemsize*4))
        glVertexAttribDivisor(4, 1)

        glBindVertexArray(HSphereAO)
        glBindBuffer(GL_ARRAY_BUFFER, InstanceSphereBO)

        glBufferData(GL_ARRAY_BUFFER, pos_points.nbytes , pos_points, GL_STATIC_DRAW)

        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, pos_points.itemsize*16, ctypes.c_void_p(0))
        glVertexAttribDivisor(1, 1)
        
        glEnableVertexAttribArray(2)
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, pos_points.itemsize*16, ctypes.c_void_p(pos_points.itemsize*4))
        glVertexAttribDivisor(2, 1)

        glEnableVertexAttribArray(3)
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, pos_points.itemsize*16, ctypes.c_void_p(2 * pos_points.itemsize*4))
        glVertexAttribDivisor(3, 1)

        glEnableVertexAttribArray(4)
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, pos_points.itemsize*16, ctypes.c_void_p(3 * pos_points.itemsize*4))
        glVertexAttribDivisor(4, 1)

        # Loading tangents and bitangents to shader
        PointTangentBuffer = glGenBuffers(1)
        PointBitangentBuffer = glGenBuffers(1)
        PointNormalBuffer = glGenBuffers(1)

        p_tangents = np.array(p_tangents, np.float32).flatten()
        p_bitangents = np.array(p_bitangents, np.float32).flatten()
        p_normals = np.array(p_normals, np.float32)

        glBindBuffer(GL_ARRAY_BUFFER, PointTangentBuffer)
        glBufferData(GL_ARRAY_BUFFER, p_tangents.nbytes, p_tangents, GL_STATIC_DRAW)

        glEnableVertexAttribArray(5)
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, p_tangents.itemsize*3, ctypes.c_void_p(0))
        glVertexAttribDivisor(5, 1)

        glBindBuffer(GL_ARRAY_BUFFER, PointBitangentBuffer)
        glBufferData(GL_ARRAY_BUFFER, p_bitangents.nbytes, p_bitangents, GL_STATIC_DRAW)

        glEnableVertexAttribArray(6)
        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, p_bitangents.itemsize*3, ctypes.c_void_p(0))
        glVertexAttribDivisor(6, 1)

        glBindBuffer(GL_ARRAY_BUFFER, PointNormalBuffer)
        glBufferData(GL_ARRAY_BUFFER, p_normals.nbytes, p_normals, GL_STATIC_DRAW)

        glEnableVertexAttribArray(7)
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, p_normals.itemsize*3, ctypes.c_void_p(0))
        glVertexAttribDivisor(7, 1)

        CloudLoaded = True

    if mode == 1 or mode == 3:
        Objects = read_xml_scene(scene_xml)
        Paths = []
        LenOfVert = []
        buffer = []
        addDirBool = []
        objectPoints = np.empty(1, dtype=int)
        NobjectPoints = []
        Matrices = []
        dirname = os.path.dirname(scene_xml)

        for i in Objects:
            if i["type"] == "obj" or i["type"] == "sphere" or i["type"] == "rectangle":
                path, pos_mat, addDir = ReadXMLObjects(i)
                
                Paths.append(path)
                Matrices.append(pos_mat)
                addDirBool.append(addDir)
        # Loading Objects
        cislo = 1
        nOfObjects = len(Paths)
        for n , i in enumerate(Paths):
            if addDirBool[n]:
                object_indices, object_buffer = ObjLoader.load_model(dirname + "/" + i)
            else:
                object_indices, object_buffer = ObjLoader.load_model(i)
            LenOfVert.append(len(object_indices))
            buffer.append(object_buffer)
            if mode == 3:
                Points_for_obj = FilterPoints(p_positions, object_buffer, distance, Matrices[n])
                NobjectPoints.append(len(Points_for_obj))
                objectPoints = np.concatenate((objectPoints, np.array(Points_for_obj)), axis=0, dtype=np.int32)
            print("Objects", round(cislo/nOfObjects*100, 1), "%", "loaded")
            cislo += 1
        
        # Generate VAO, VBO
        VAO = glGenVertexArrays(nOfObjects)
        VBO = glGenBuffers(nOfObjects)
        Colors = []
        pos = []

        # VAO and VBO
        for i in range(nOfObjects):
            glBindVertexArray(VAO[i])
            glBindBuffer(GL_ARRAY_BUFFER, VBO[i])
            glBufferData(GL_ARRAY_BUFFER, buffer[i].nbytes, buffer[i], GL_STATIC_DRAW)

            # vertices
            glEnableVertexAttribArray(0)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, None)

            # colors
            # Generating random color from white to blue
            color = random.random()
            Colors.append(np.array([1.0-color, 1.0-color, 1.0]))

            #Getting position matrices
            pos.append(Matrices[i])
        
        if NobjectPoints != []:
            print("Loading point indexes for objects to shaders")
            BufferObjectPointsPos = glGenBuffers(1)
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, BufferObjectPointsPos)
            glBufferData(GL_SHADER_STORAGE_BUFFER, objectPoints.nbytes, objectPoints, GL_STATIC_DRAW)

            #Bind buffer to shader storage binding point
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, BufferObjectPointsPos)
        
        print("Scene Reloaded")
        SceneLoaded = True


#Creating glfw window
if not glfw.init():
    raise Exception("Error in glfw initiation")

window = glfw.create_window(Width, Height, "", None, None)

if not window:
    glfw.terminate()
    raise Exception("Error in window creation")


#Setting window pos
glfw.set_window_pos(window, 150, 150)

glfw.make_context_current(window)

glfw.swap_interval(0)

#Compiling shaders
shader = compileProgram(compileShader(
    vertex_src, GL_VERTEX_SHADER), compileShader(fragment_src, GL_FRAGMENT_SHADER))

glUseProgram(shader)

# Initialize ImGUI
imgui.create_context()
renderer = GlfwRenderer(window)

glfw.set_window_size_callback(window, window_resize)
glfw.set_cursor_pos_callback(window, mouse_look_callback)
glfw.set_mouse_button_callback(window, mouse_button_callback)

#Loading shaders into GL and setting background color
glClearColor(0.5, 0.5, 0.5, 1)

glEnable(GL_DEPTH_TEST)
# transparency
glEnable(GL_BLEND)
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

# Loading sphere object
sphere_indices, sphere_buffer = ObjLoader.load_model("obj/point.obj", scale=0.01)
sphere_indicesLength = len(sphere_indices)

SphereAO = glGenVertexArrays(1)
SphereBO = glGenBuffers(1)

glBindVertexArray(SphereAO)

glBindBuffer(GL_ARRAY_BUFFER, SphereBO)
glBufferData(GL_ARRAY_BUFFER, sphere_buffer.nbytes, sphere_buffer, GL_STATIC_DRAW)

# vertices
glBindBuffer(GL_ARRAY_BUFFER, SphereBO)
glEnableVertexAttribArray(0)
glVertexAttribPointer(0, 3, GL_FLOAT, False, 0, None)

# Loading halfsphere object

HSphereAO = glGenVertexArrays(1)
HSphereBO = glGenBuffers(1)

Hsphere_indicesLength = generate_half_sphere(20,20)

#Creating perspective projection matrix
projection = pyrr.matrix44.create_perspective_projection(60, Width/Height, 0.01, 200)

#Getting location of variables in shader
model_loc = glGetUniformLocation(shader, "model")
proj_loc = glGetUniformLocation(shader, "projection")
view_loc = glGetUniformLocation(shader, "view")
color_loc = glGetUniformLocation(shader, "objectColor")
switcher_loc = glGetUniformLocation(shader, "switcher")
offset_loc = glGetUniformLocation(shader, "offset")
NPointsInObject_loc = glGetUniformLocation(shader, "NPointsInObject")
size_loc = glGetUniformLocation(shader, "size")
pointColor_loc = glGetUniformLocation(shader, "pointColor")
m_bands_loc = glGetUniformLocation(shader, "m_bands")
SHLineLength_loc = glGetUniformLocation(shader, "SHLineLength")
nOfPoints_loc = glGetUniformLocation(shader, "nOfPoints")
nofVerticesPerSH_loc = glGetUniformLocation(shader, "nofVerticesPerSH")
ncols_loc = glGetUniformLocation(shader, "ncols")
nrows_loc = glGetUniformLocation(shader, "nrows")
SHColor_loc = glGetUniformLocation(shader, "SHColor")
scaleFactor_loc = glGetUniformLocation(shader, "scaleFactor")
f_switcher_loc = glGetUniformLocation(shader, "f_switcher")
HMintensity_loc = glGetUniformLocation(shader, "HMintensity")
HMdistance_loc = glGetUniformLocation(shader, "HMdistance")

#Loading projection matrix to shader
glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection)

#Loading nOfVertices of halfsphere into shader
glUniform1i(nofVerticesPerSH_loc, Hsphere_indicesLength)

# setup camera
camera_pos = pyrr.Vector3([0.0, 1.0, 3.0])
camera_front = pyrr.Vector3([0.0, 0.0, -1.0])
camera_up = pyrr.Vector3([0.0, 1.0, 0.0])

#VAO and VBO global variables
VAO = []
VBO = []
InstanceSphereBO = -1
PointTangentBuffer = -1
PointBitangentBuffer = -1
PointNormalBuffer = -1
BufferObjectPointsPos = -1
SHBufferPos = -1
BufferPointsPos = -1
BufferPointsNor = -1
nOfObjects = 0
nOfPoints = 0
LenOfVert = []
NobjectPoints = []
Colors = []
pos = []

# Control variables
delta_time = 0.0
last_frame = 0.0
vsync = False

sizeofPoints = 1.0
changedSize = False
glUniform1f(size_loc, sizeofPoints)

m_bands = 1
m_bandsChanged = False
m_bandsMax = 0
ncols = 20
nrows = 20
SHColor = 0
glUniform1i(m_bands_loc, m_bands)

scaleFactor = 1.0
HMintensity = 100.0

HMDistance = 1.0
LoadingPointsDistance = 1.0
LoadedPointDistance = 1.0

scene_xml = ""
pointCloud_file = ""

ShowObj = False
ShowPoints = False
ShowSH = False

LoadObj = True
LoadSpheres = True
LoadRectangles = False

validXml = False
validPC = False

SceneLoaded = False
CloudLoaded = False

ShowHeatmap = False

ShowPointsSettings = False
ExpandedPointsSettings = False

ShowSceneSettings = False
ExpandedSceneSettings = False

PointColor = [1.0, 1.0, 1.0]
changedColor = False
glUniform3fv(pointColor_loc, 1, PointColor)

WireFrameBool = False
RenderType = GL_TRIANGLES

#Start of render loop
while not glfw.window_should_close(window):
    glfw.poll_events()

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    #Processing Input from key presses and mouse movements
    processInput(window)
    
    # New view matrix based on new Input
    view = pyrr.matrix44.create_look_at(camera_pos, camera_pos + camera_front, camera_up)
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view)

    #------------------------IMGUI-----------------------------------
    # New imgui frame
    renderer.process_inputs()
    imgui.new_frame()
    #ImGUI window
    imgui.begin("Settings")
    imgui.text("Scene xml file:")
    imgui.same_line()
    _, scene_xml = imgui.input_text("", scene_xml, 2048)
    
    if imgui.button("Select XML"):
        scene_xml = FileDialog("xml", "Scene XML")
        if CheckXml(scene_xml):
            print("Scene", scene_xml, "saved")
            validXml = True
        else:
            print("Invalid scene file")
            validXml = False
    imgui.same_line()
    if imgui.button("Save scene"):
        if CheckXml(scene_xml):
            print("Scene", scene_xml, "saved")
            validXml = True
        else:
            print("Invalid scene file")
            validXml = False
    
    imgui.new_line()
    
    imgui.text("Point Cloud file:")
    imgui.same_line()
    _, pointCloud_file = imgui.input_text(" ", pointCloud_file, 2048)
    
    if imgui.button("Select Cloud"):
        pointCloud_file = FileDialog("dump", "Point Cloud")
        if CheckPoints(pointCloud_file):
            print("Point Cloud", pointCloud_file, "saved")
            validPC = True
        else:
            print("Invalid Point Cloud file")
            validPC = False
    imgui.same_line()
    if imgui.button("Save Point Cloud"):
        if CheckPoints(pointCloud_file):
            print("Point Cloud", pointCloud_file, "saved")
            validPC = True
        else:
            print("Invalid Point Cloud file")
            validPC = False
    imgui.new_line()
    imgui.push_item_width(imgui.get_window_width() * 0.1)
    
    imgui.text("Padding distance for filtering points: ")
    imgui.same_line()
    _, LoadingPointsDistance = imgui.drag_float("                        ", LoadingPointsDistance, 0.001, 0.01, 1000.0, "%.3f")
    imgui.text("Must reload scene to take effect")

    imgui.new_line()

    if imgui.button("Reload scene"):
        SceneLoaded = False
        CloudLoaded = False
        if validPC and validXml:
            print("Scene Realoading with:", scene_xml, "as scene and", pointCloud_file, "as point cloud")
            ReloadScene(3, LoadingPointsDistance)
        elif validPC and not(validXml):
            print("Scene Realoading with:", pointCloud_file, "as point cloud")
            ReloadScene(2, LoadingPointsDistance)
        elif not(validPC) and validXml:
            print("Scene Realoading with:", scene_xml, "as scene")
            ReloadScene(1, LoadingPointsDistance)
        else:
            print("Scene nor point cloud is loaded")
    imgui.same_line()
    if imgui.button("Unload scene"):
        if validXml:
            print("Unloading scene")
            free_object_data(1)
            validXml = False
            SceneLoaded = False
        else:
            print("No scene loaded")
    imgui.same_line()
    if imgui.button("Unload cloud"):
        if validPC:
            print("Unloading cloud")
            free_object_data(2)
            validPC = False
            CloudLoaded = False
        else:
            print("No point cloud loaded")    

    if ShowHeatmap and not(SceneLoaded):
        print("Scene not loaded, unavailable to show heatmap")
        ShowHeatmap = False

    if ShowHeatmap and not(CloudLoaded):
        print("Cloud not loaded, unavailable to show heatmap")
        ShowHeatmap = False
    
    if ShowObj and not(SceneLoaded):
        print("Scene not loaded, unavailable to show")
        ShowObj = False

    if ShowPoints and not(CloudLoaded):
        print("Point Cloud not loaded, unavailable to show")
        ShowPoints = False

    imgui.new_line()
    imgui.text("Movement Speed: ")
    imgui.same_line()
    _, movementSpeed = imgui.drag_float("  ", movementSpeed, 0.1, 0.0, 1000000.0, "%.3f")

    imgui.new_line()
    imgui.text("Scene scale: ")
    imgui.same_line()
    scaleFactorChange, scaleFactor = imgui.drag_float("   ", scaleFactor, 0.1, 0.0, 1000000.0, "%.3f")

    if scaleFactorChange:
        glUniform1f(scaleFactor_loc, scaleFactor)

    imgui.new_line()
    ChangedVsync, vsync = imgui.checkbox("Vsync (Lock framerate to screen refresh)", vsync)
    ChangedWireFrame, WireFrameBool = imgui.checkbox("Wireframe rendering", WireFrameBool)

    if ChangedVsync and vsync:
        glfw.swap_interval(1)
    elif ChangedVsync:
        glfw.swap_interval(0)

    if ChangedWireFrame and WireFrameBool:
        RenderType = GL_LINES
    elif ChangedWireFrame:
        RenderType = GL_TRIANGLES

    if SceneLoaded:
        ShowSceneSettings = None
    else:
        ShowSceneSettings = False

    imgui.new_line()
    ExpandedSceneSettings, ShowSceneSettings = imgui.collapsing_header("Objects Settings", ShowSceneSettings)

    if ExpandedSceneSettings:
        _, ShowObj = imgui.checkbox("Show Scene", ShowObj)
        if CloudLoaded:
            _, ShowHeatmap = imgui.checkbox("Show Heatmap", ShowHeatmap)
            imgui.text("Heatmap Intensity: ")
            imgui.same_line()
            changedHMI, HMintensity = imgui.drag_float("    ", HMintensity, 1.0, 1.0, 1000000.0, "%.2f")
            if changedHMI:
                glUniform1f(HMintensity_loc, HMintensity)
            
            imgui.text("Heatmap counting distance: ")
            imgui.same_line()
            changedHMD, HMDistance = imgui.drag_float("     ", HMDistance, 0.001, 0.001, 1000000.0, "%.3f")
            if changedHMD:
                glUniform1f(HMdistance_loc, HMDistance)


    if CloudLoaded:
        ShowPointsSettings = None
    else:
        ShowPointsSettings = False

    imgui.new_line()
    ExpandedPointsSettings, ShowPointsSettings = imgui.collapsing_header("Points Settings", ShowPointsSettings)

    if ExpandedPointsSettings:
        _, ShowPoints = imgui.checkbox("Show Points", ShowPoints)
        
        imgui.push_item_width(imgui.get_window_width() * 0.1)
        
        if ShowPoints:
            imgui.new_line()
            imgui.text("Size of Points: ")
            imgui.same_line()
            changedSize, sizeofPoints = imgui.drag_float("      ", sizeofPoints, 0.1, 0.0, 1000000.0, "%.3f")
            if changedSize:
                glUniform1f(size_loc, sizeofPoints)
            imgui.push_item_width(imgui.get_window_width() * 0.25)
            imgui.text("Color of Points: ")
            imgui.same_line()
            changedColor, PointColor = imgui.color_edit3("       ", PointColor[0], PointColor[1], PointColor[2])
            if changedColor:
                glUniform3fv(pointColor_loc, 1, PointColor)
            
            imgui.new_line()
            _, ShowSH = imgui.checkbox("Show Hemispherical harmonics", ShowSH)
            if ShowSH:
                imgui.push_item_width(imgui.get_window_width() * 0.33)
                imgui.text("m_bands:")
                imgui.same_line()
                m_bandsChanged, m_bands = imgui.slider_int("        ", m_bands, 1, m_bandsMax)
                if m_bandsChanged:
                    glUniform1i(m_bands_loc, m_bands)
                
                imgui.push_item_width(imgui.get_window_width() * 0.15)
                imgui.text("HSHColor (0 = Normal direction, 1 = HSH value heatmap):")
                SHColor_change, SHColor = imgui.slider_int("         ", SHColor, 0, 1)
                if SHColor_change:
                    glUniform1i(SHColor_loc, SHColor)
                imgui.push_item_width(imgui.get_window_width() * 0.4)
                imgui.new_line()
                imgui.text("nCols:")
                imgui.same_line()
                cols_changed, ncols = imgui.slider_int("          ", ncols, 5, 50)

                imgui.text("nRows:")
                imgui.same_line()
                rows_changed, nrows = imgui.slider_int("           ", nrows, 5, 50)
                
                if cols_changed or rows_changed:
                    glUniform1i(ncols_loc, ncols)
                    glUniform1i(nrows_loc, nrows)
                    Hsphere_indicesLength = generate_half_sphere(nrows, ncols)
                imgui.pop_item_width()
    imgui.end()
    #-----------------------END-----------------------------------------
    
    offset = 0

    #Rendering objects
    if ShowObj:
        if ShowHeatmap:
            glUniform1i(switcher_loc, 0)
            glUniform1i(f_switcher_loc, 0)
            for i in np.arange(nOfObjects):
                glBindVertexArray(VAO[i])
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, pos[i])
                glUniform1i(offset_loc, offset)
                glUniform1i(NPointsInObject_loc, NobjectPoints[i])
                glDrawArrays(RenderType, 0, LenOfVert[i])
                offset += NobjectPoints[i]
            glUniform1i(f_switcher_loc, 1)
        else:
            glUniform1i(switcher_loc, 2)
            for i in np.arange(nOfObjects):
                glBindVertexArray(VAO[i])
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, pos[i])
                glUniform3fv(color_loc, 1, Colors[i])
                glDrawArrays(RenderType, 0, LenOfVert[i])
    
    #Rendering points
    if ShowPoints:
        if ShowSH:
            glUniform1i(switcher_loc, 3)
            glBindVertexArray(HSphereAO)
            glDrawArraysInstanced(RenderType, 0, Hsphere_indicesLength, nOfPoints)
        else:
            glUniform1i(switcher_loc, 1)
            glBindVertexArray(SphereAO)
            glDrawArraysInstanced(RenderType, 0, sphere_indicesLength, nOfPoints)

    glBindVertexArray(0)

    #Render ImGUI
    imgui.render()
    renderer.render(imgui.get_draw_data())

    #Counting FPS
    current_frame = glfw.get_time()
    delta_time = current_frame - last_frame
    last_frame = current_frame
    fps = round(1/delta_time, 1)

    Title = "Heatmap type: " + heatmapType + " |" + " FPS: " + str(fps) + " ms: " + str(round(delta_time, 4))
    glfw.set_window_title(window, Title)

    glfw.swap_buffers(window)

renderer.shutdown()
glfw.terminate()