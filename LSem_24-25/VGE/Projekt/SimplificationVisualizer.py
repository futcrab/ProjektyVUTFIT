import open3d as o3d
import polyscope as ps
import polyscope.imgui as psim

import numpy as np
from scipy.spatial import cKDTree

import subprocess
import os
import glob

# Convert Open3D mesh to numpy arrays for Polyscope
def to_numpy(mesh : o3d.geometry.TriangleMesh):
    vertices = np.asarray(mesh.vertices)
    faces = np.asarray(mesh.triangles)
    return vertices, faces

def normalize_mesh(mesh : o3d.geometry.TriangleMesh):
    # Center the mesh at the origin
    mesh.compute_vertex_normals()
    mesh.translate(-mesh.get_center())
    
    # Scale the mesh to fit within a unit sphere
    max_extent = np.max(mesh.get_max_bound() - mesh.get_min_bound())
    scale = 1.0 / max_extent
    mesh.scale(scale, center=mesh.get_center())

    # Shift mesh so the lowest vertex sits on Y=0
    vertices = np.asarray(mesh.vertices)
    min_y = np.min(vertices[:, 1])
    mesh.translate((0.0, -min_y, 0.0))  # Move bottom to Y=0

    return mesh

def compute_face_area_colors(vertices: np.ndarray, triangles: np.ndarray):
    # Compute face areas sizes
    v0 = vertices[triangles[:, 0]]
    v1 = vertices[triangles[:, 1]]
    v2 = vertices[triangles[:, 2]]
    face_areas = 0.5 * np.linalg.norm(np.cross(v1 - v0, v2 - v0), axis=1)

    # Normalize to [0, 1]
    area_min, area_max = face_areas.min(), face_areas.max()
    norm_areas = (face_areas - area_min) / (area_max - area_min + 1e-8)
    norm_areas = norm_areas ** 0.5

    # Aggregate to vertices
    vertex_values = np.zeros(len(vertices))
    counts = np.zeros(len(vertices))

    for i, tri in enumerate(triangles):
        for v in tri:
            vertex_values[v] += norm_areas[i]
            counts[v] += 1

    vertex_values /= (counts + 1e-8)

    # Map to red-white color gradient
    colors = np.ones((len(vertices), 3))
    colors[:, 1] = vertex_values     # Green channel
    colors[:, 2] = vertex_values     # Blue channel

    return colors

# Function to simplify the mesh using Quadric Decimation or Vertex Clustering
# Clustering is selected by passing a voxel size > 0.0
def simplify_mesh(mesh : o3d.geometry.TriangleMesh, target_faces : int = 5, voxel_size : float = -1.0):
    global total_distance_quadric, total_distance_clustering

    # Select simplification method
    if voxel_size < 0.0:
        simplified_mesh = mesh.simplify_quadric_decimation(target_faces)
    else:
        simplified_mesh = mesh.simplify_vertex_clustering(voxel_size=voxel_size)

    # Get vertices and faces into numpy arrays
    orig_v, orig_f = to_numpy(mesh)
    simp_v, simp_f = to_numpy(simplified_mesh)

    # Create a KDTree for fast nearest neighbor search
    tree = cKDTree(simp_v)
    dists, _ = tree.query(orig_v)

    # Calculate the total distance from each vertex to its nearest neighbor in the simplified mesh
    if voxel_size < 0.0:
        # Quadric Decimation
        total_distance_quadric = np.sum(dists)
    else:
        # Vertex Clustering
        total_distance_clustering = np.sum(dists)

    # Create a color array to visualize changed vertices    
    threshold = 1e-4
    changed = dists > threshold

    colors = np.zeros_like(orig_v)  # default: black
    colors[~changed] = [0.7, 0.7, 0.7]  # unchanged: gray
    colors[changed] = [1.0, 0.0, 0.0]   # changed: red

    orig_v = orig_v - np.array([1.0, 0, 0])  # Shift original mesh left
    simp_v = simp_v + np.array([1.0, 0, 0])  # Shift simplified mesh right

    return orig_v, orig_f, simp_v, simp_f, colors


def my_callback():
    global new_faces, faces_step, max_faces, skull, monkey, bunny, original_mesh, orig_mesh, new_voxel, clust_f, max_voxel, vertices_spec, eigenvecs, flips, spec_mesh_ID, orig_v
    
    changed_obj = False
    
    # Checkboxes for selecting the object to visualize
    checkbox_skull, skull = psim.Checkbox("skull", skull) 
    psim.SameLine()
    checkbox_monkey, monkey = psim.Checkbox("monkey", monkey)
    psim.SameLine()
    checkbox_bunny, bunny = psim.Checkbox("bunny", bunny)
    psim.SameLine()
    psim.Text("Faces in original object: " + str(max_faces))

    if checkbox_skull:
        monkey = False
        bunny = False

        original_mesh = o3d.io.read_triangle_mesh("obj/skull.obj")
        original_mesh = normalize_mesh(original_mesh)
        changed_obj = True
        max_faces = len(original_mesh.triangles)
        spec_mesh_ID = 1
    if checkbox_monkey:
        skull = False
        bunny = False

        original_mesh = o3d.io.read_triangle_mesh("obj/monkey.obj")
        original_mesh = normalize_mesh(original_mesh)
        changed_obj = True
        max_faces = len(original_mesh.triangles)
        spec_mesh_ID = 1
    if checkbox_bunny:
        skull = False
        monkey = False

        original_mesh = o3d.io.read_triangle_mesh("obj/bunny.obj")
        original_mesh = normalize_mesh(original_mesh)
        changed_obj = True
        max_faces = len(original_mesh.triangles)
    
    psim.NewLine()
    psim.Text(f"Summed distance from each vertex to its neighbour on quadric: {total_distance_quadric:.2f}")
    _, faces_step = psim.InputInt("Select step size", faces_step, step=1.0)
    # Select the number of faces quadric decimation should target
    changed_faces, new_faces = psim.InputInt("New number of faces", new_faces, step=faces_step)
    if changed_faces:
        new_faces = np.clip(new_faces, 5, max_faces)
        # Create new simplified mesh with the new number of faces
        _, _, simp_v, simp_f, colors = simplify_mesh(original_mesh, target_faces=new_faces)
        # Calculate face area colors for the simplified mesh
        colors_simp = compute_face_area_colors(simp_v, simp_f)
        
        orig_mesh.remove_quantity("Changed Vertices to quadric")
        orig_mesh.add_color_quantity("Changed Vertices to quadric", colors, enabled=True)

        ps.remove_surface_mesh("Simplified Mesh quadric")
        quad = ps.register_surface_mesh(
        "Simplified Mesh quadric",
        simp_v,
        simp_f,
        color=(0.2, 0.6, 0.8),
        edge_width=1.0,
        )
        quad.add_color_quantity("Face Area Colors", colors_simp)
    
    psim.NewLine()
    psim.Text(f"Summed distance from each vertex to its neighbour on clust.: {total_distance_clustering:.2f}")
    # Select the maximum voxel size for clustering
    _, max_voxel = psim.SliderFloat("Max voxel size", max_voxel, v_min=0.002, v_max=1.0)
    changed_voxel, new_voxel = psim.SliderFloat("Voxel size on clustering", new_voxel, v_min=0.001, v_max=max_voxel)
    if changed_voxel:
        # Create new simplified mesh with the new voxel size
        _, _, clust_v, clust_f, colors_clust = simplify_mesh(original_mesh, voxel_size=new_voxel)
        colors_simp = compute_face_area_colors(clust_v, clust_f)
        # Shift clustered mesh for visualization
        clust_v = clust_v + np.array([2.0, 0, 0])

        orig_mesh.remove_quantity("Changed Vertices to clustering")
        orig_mesh.add_color_quantity("Changed Vertices to clustering", colors_clust, enabled=True)

        ps.remove_surface_mesh("Simplified Mesh clustering")
        clust = ps.register_surface_mesh(
            "Simplified Mesh clustering",
            clust_v,
            clust_f,
            color = (0.8, 0.75, 0.2),
            edge_width=1.0,
        )

        clust.add_color_quantity("Face Area Colors", colors_simp)

    # If object was changed, update the original mesh and its simplifications
    if changed_obj:
        new_faces = len(original_mesh.triangles) // 2
        changed_obj = False

        new_faces = np.clip(new_faces, 5, max_faces)
        orig_v, orig_f, simp_v, simp_f, colors = simplify_mesh(original_mesh, target_faces=new_faces)
        _, _, clust_v, clust_f, colors_clust = simplify_mesh(original_mesh, voxel_size=new_voxel)

        colors_simp = compute_face_area_colors(simp_v, simp_f)
        colors_clust_simp = compute_face_area_colors(clust_v, clust_f)

        # Shift clustered mesh for visualization
        clust_v = clust_v + np.array([2.0, 0, 0])
        ps.remove_all_structures()
        orig_mesh = ps.register_surface_mesh(
            "Orig Mesh",
            orig_v,
            orig_f,
            color=(0.2, 0.6, 0.8),
            edge_width=1.0,
        )
        orig_mesh.add_color_quantity("Changed Vertices to quadric", colors, enabled=True)
        orig_mesh.add_color_quantity("Changed Vertices to clustering", colors_clust, enabled=False)

        quad = ps.register_surface_mesh(
            "Simplified Mesh quadric",
            simp_v,
            simp_f,
            color=(0.2, 0.6, 0.8),
            edge_width=1.0,
        )

        clust = ps.register_surface_mesh(
            "Simplified Mesh clustering",
            clust_v,
            clust_f,
            color = (0.8, 0.75, 0.2),
            edge_width=1.0,
        )

        quad.add_color_quantity("Face Area Colors", colors_simp, enabled=False)
        clust.add_color_quantity("Face Area Colors", colors_clust_simp, enabled=False)

    # Check if spectral simplification was installed
    # Only properly works on bunny mesh
    if bunny and os.path.exists("spectral-mesh-simplification/bin/spectral-collapsing.exe"):
        psim.NewLine()
        # Spectral simplification parameters
        _, vertices_spec = psim.InputInt("Vertices in mesh", vertices_spec, step=1.0)
        vertices_spec = np.clip(vertices_spec, 6, 34817)
        _, eigenvecs = psim.InputInt("Eigenvectors", eigenvecs, step=1.0)
        eigenvecs = np.clip(eigenvecs, 1, 100)
        _, flips = psim.Checkbox("Include Flips (Could be up to 10x harder to compute)", flips)
        
        # Run subprocess to call the spectral simplification executable and then load the generated mesh
        if psim.Button("Add Spectral Simplification Mesh"):
            stream_value = "stream+flips" if flips else "stream"
            result = subprocess.run(["./spectral-mesh-simplification/bin/spectral-collapsing.exe", "obj/bunny.obj", f"obj/simpl{spec_mesh_ID}.obj", str(vertices_spec), stream_value, "lowpass", f"eigenvectors={eigenvecs}", "optimal_pos=true"])
            if result.returncode == 0:
                print("Spectral simplification completed successfully.")
                spectral_mesh = o3d.io.read_triangle_mesh(f"obj/simpl{spec_mesh_ID}.obj")
                spectral_mesh = normalize_mesh(spectral_mesh)
                spec_v, spec_f = to_numpy(spectral_mesh)

                spec_v = spec_v - np.array([1.0, 0, 0]) # Shift simplified mesh left to match original mesh
                
                # Calculate the total distance from each vertex to its nearest neighbor in the simplified mesh and print it
                tree = cKDTree(spec_v)
                dists, _ = tree.query(orig_v)
                print("Total distance from each vertex to its neighbour in the new spectral mesh: ", np.sum(dists))

                # Calculate face area colors for the spectral mesh
                colors_spectral = compute_face_area_colors(spec_v, spec_f)
                # Shift spectral mesh for visualization by its ID
                # Multiple spectral meshes can be added
                # The ID is used to shift the mesh in the x direction
                # 4.0 = shift to the right, 2.0 = distance between meshes, this will be at least third mesh
                spec_v = spec_v + np.array([4.0 + spec_mesh_ID*2.0, 0, 0])

                spec = ps.register_surface_mesh(
                    f"Simplified Mesh spectral {spec_mesh_ID}",
                    spec_v,
                    spec_f,
                    color = (0.2, 0.8, 0.4),
                    edge_width=1.0,
                )
                spec.add_color_quantity("Face Area Colors", colors_spectral, enabled=True)
                spec_mesh_ID += 1
            else:
                print("Spectral simplification failed. Please try again.")

# Load first mesh and normalize it
original_mesh = o3d.io.read_triangle_mesh("obj/skull.obj")
original_mesh = normalize_mesh(original_mesh)

# Initialize variables for distances
# These are used to calculate the distance from each vertex to its nearest neighbour in the simplified mesh
total_distance_quadric = 0.0
total_distance_clustering = 0.0

# Simplify the mesh using Quadric Decimation and Vertex Clustering
orig_v, orig_f, simp_v, simp_f, colors = simplify_mesh(original_mesh, target_faces=len(original_mesh.triangles) // 2)
_, _, clust_v, clust_f, colors_clust = simplify_mesh(original_mesh, voxel_size=0.05)

# Calculate face area colors for the simplified meshes
colors_simp = compute_face_area_colors(simp_v, simp_f)
colors_clust_simp = compute_face_area_colors(clust_v, clust_f)

# Shift clustered mesh for visualization
clust_v = clust_v + np.array([2.0, 0, 0])
            
# Initialize variables for Polyscope imgui
new_faces = len(simp_f)
new_voxel = 0.05
max_voxel = 1.0
faces_step = 1
max_faces = len(orig_f)
skull = True
monkey = False
bunny = False
vertices_spec = 10000
eigenvecs = 5
flips = False
spec_mesh_ID = 1

# Set first person moving camera
ps.set_navigation_style("first_person")

# Initialize polyscope
ps.init()

# Register the original mesh with Polyscope
orig_mesh = ps.register_surface_mesh(
    "Orig Mesh",
    orig_v,
    orig_f,
    color=(0.2, 0.6, 0.8),
    edge_width=1.0,
)

# Add colors to indicate changed vertices
orig_mesh.add_color_quantity("Changed Vertices to quadric", colors, enabled=True)
orig_mesh.add_color_quantity("Changed Vertices to clustering", colors_clust, enabled=False)

# Register the simplified meshes with Polyscope
quad = ps.register_surface_mesh(
    "Simplified Mesh quadric",
    simp_v,
    simp_f,
    color=(0.2, 0.6, 0.8),
    edge_width=1.0,
)

quad.add_color_quantity("Face Area Colors", colors_simp, enabled=False)

clust = ps.register_surface_mesh(
    "Simplified Mesh clustering",
    clust_v,
    clust_f,
    color = (0.8, 0.75, 0.2),
    edge_width=1.0,
)

clust.add_color_quantity("Face Area Colors", colors_clust_simp, enabled=False)

# Set callback for the GUI
ps.set_user_callback(my_callback)

# Start polyscope visualization
ps.show()

# If any files were generated using the spectral simplification, clean them up
AnyGenerated = False
# Clean up the generated files
for file in glob.glob("obj/simpl*.obj"):
    AnyGenerated = True
    os.remove(file)

if AnyGenerated:
    print("Cleaned up generated files.")