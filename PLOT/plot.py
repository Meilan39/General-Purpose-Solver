import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os
import re

GT = 3 / 2

# === Define the function to plot ===
def f(x, y):
    return np.sin(x) + np.cos(y)
    # return (-0.0001) * (np.sin(x) * np.sin(y) * np.exp(100 - np.sqrt(x**2 + y**2)/np.pi))**(0.1)
    # return (0.1 * x**4) + x**3 + (0.2 * y**2)
    # return (1 - x)**2 + 100 * (y - x**2)**2
    # return (1.5 - x + x*y)**2 + (2.25 - x + x*y**2)**2 + (2.625 - x + x*y**3)**2

# === Load paths grouped by mode ===
def load_grouped_paths(filename):
    with open(filename) as f:
        AL = False
        idx = 0
        groups = {}
        current_key = None
        x, y = [], []

        for line in f:
            line = line.strip()
            if not line:
                if x and y and current_key:
                    groups.setdefault(current_key, []).append((np.array(x), np.array(y)))
                    x, y = [], []
                continue

            if line.startswith("Gradient Descent"):
                current_key = "gradient"
                x, y = [], []
                continue

            match = re.match(r"Augmented Lagrangian r \{([0-9eE.+-]+)\}", line)
            if match:
                AL = True
                current_key = f"r={float(match.group(1)):g}"
                x, y = [], []
                continue

            if len(line.split()) == 2:                
                idx = idx + 1
                if AL and (idx % 10):
                    continue
                xi, yi = map(float, line.split())
                x.append(xi)
                y.append(yi)

        # Final group
        if x and y and current_key:
            groups.setdefault(current_key, []).append((np.array(x), np.array(y)))

    return groups

# === Plot one group ===
def plot_surface_with_paths(paths, title="Descent Paths", save_file=""):
    # Combine all points
    all_x = np.concatenate([p[0] for p in paths])
    all_y = np.concatenate([p[1] for p in paths])
    all_z = f(all_x, all_y)

    avg_x, avg_y, avg_z = np.mean(all_x), np.mean(all_y), np.mean(all_z)
    xydev = max(np.max(abs(all_x - avg_x)), np.max(abs(all_y - avg_y))) * GT
    zdev = np.max(abs(all_z - avg_z)) * GT

    # Manual bounds
    x_range = np.linspace(-10, 10, 50)
    y_range = np.linspace(-10, 10, 50)
    X, Y = np.meshgrid(x_range, y_range)
    Z = f(X, Y)
    Z = np.where((Z < -3) | (Z > 3), np.nan, Z)

    # Plot
    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection='3d')
    ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7, edgecolor='none', antialiased=False)

    for x, y in paths:
        z = f(x, y)
        ax.plot(x, y, z, linewidth=2, marker='o')

    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('f(x, y)')
    ax.set_title(title)

    print(f'X: {avg_x - xydev}, {avg_x + xydev}')
    print(f'Y: {avg_y - xydev}, {avg_y + xydev}')
    print(f'Z: {avg_z - zdev}, {avg_z + zdev}')

    plt.tight_layout()
    if save_file == "":
        plt.show()
    else:
        ax.view_init(elev=90, azim=-90)
        plt.savefig(save_file, dpi=150)
        plt.close()

# === Entry point ===
if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        print("Usage: python plot_multiple_paths.py input.txt [out_prefix]")
        exit()

    input_file = sys.argv[1]
    out_prefix = sys.argv[2] if len(sys.argv) >= 3 else ""

    grouped_paths = load_grouped_paths(input_file)
    for key, paths in grouped_paths.items():
        title = f"Descent Paths ({key})"
        save_file = f"{out_prefix}_{key}.png" if out_prefix else ""
        plot_surface_with_paths(paths, title=title, save_file=save_file)



# import numpy as np
# import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D

# GT = 3 / 2

# # === Define the function to plot ===
# def f(x, y):
#     return np.sin(x) + np.cos(y)
#     # return (-0.0001) * (np.sin(x) * np.sin(y) * np.exp(100 - np.sqrt(x**2 + y**2)/np.pi))**(0.1)
#     # return (0.1 * x**4) + x**3 + (0.2 * y**2)
#     # return (1 - x)**2 + 100 * (y - x**2)**2
#     # return (1.5 - x + x*y)**2 + (2.25 - x + x*y**2)**2 + (2.625 - x + x*y**3)**2

# # === Load multiple (x, y) paths separated by blank lines ===
# def load_paths(filename):
#     paths = []
#     with open(filename) as f:
#         x, y = [], []
#         for line in f:
#             if line.strip() == "":
#                 if x and y:
#                     paths.append((np.array(x), np.array(y)))
#                     x, y = [], []
#             else:
#                 parts = line.strip().split()
#                 if len(parts) == 2:
#                     xi, yi = map(float, parts)
#                     x.append(xi)
#                     y.append(yi)
#         if x and y:
#             paths.append((np.array(x), np.array(y)))
#     return paths

# # === Main plotting function ===
# def plot_surface_with_paths(path_file, save_file=""):
#     paths = load_paths(path_file)

#     # Combine all path points for bounds
#     all_x = np.concatenate([p[0] for p in paths])
#     all_y = np.concatenate([p[1] for p in paths])
#     all_z = f(all_x, all_y)

#     # Calculate center and deviation for mesh
#     avg_x, avg_y, avg_z = np.mean(all_x), np.mean(all_y), np.mean(all_z)
#     xydev = max(np.max(abs(all_x - avg_x)), np.max(abs(all_y - avg_y))) * GT
#     zdev = np.max(abs(all_z - avg_z)) * GT

#     # Automatic
#     # x_range = np.linspace(avg_x - xydev, avg_x + xydev, 50)
#     # y_range = np.linspace(avg_y - xydev, avg_y + xydev, 50)
#     # X, Y = np.meshgrid(x_range, y_range)
#     # Z = f(X, Y)
#     # Z = np.where((Z < avg_z - zdev) | (Z > avg_z + zdev), np.nan, Z)
#     # Manual
#     x_range = np.linspace(-10, 10, 50)
#     y_range = np.linspace(-10, 10, 50)
#     X, Y = np.meshgrid(x_range, y_range)
#     Z = f(X, Y)
#     Z = np.where((Z < -3) | (Z > 3), np.nan, Z) 

#     # Plotting
#     fig = plt.figure(figsize=(10, 7))
#     ax = fig.add_subplot(111, projection='3d')
#     ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7, edgecolor='none', antialiased=False)

#     # Plot each path
#     for i, (x, y) in enumerate(paths):
#         z = f(x, y)
#         ax.plot(x, y, z, linewidth=2, marker='o')

#     # Axis labels and legend
#     ax.set_xlabel('x')
#     ax.set_ylabel('y')
#     ax.set_zlabel('f(x, y)')
#     ax.set_title('Multiple Descent Paths')
#     ax.legend()

#     print(f'X: {avg_x - xydev}, {avg_x + xydev}')
#     print(f'Y: {avg_y - xydev}, {avg_y + xydev}')
#     print(f'Z: {avg_z - zdev}, {avg_z + zdev}')

#     plt.tight_layout()
#     if save_file == "":
#         plt.show()
#     else:
#         ax.view_init(elev=90, azim=-90)
#         plt.savefig(save_file, dpi=150)

# # === Entry point ===
# if __name__ == "__main__":
#     import sys
#     if len(sys.argv) == 2:
#         plot_surface_with_paths(sys.argv[1])
#     elif len(sys.argv) == 3:
#         plot_surface_with_paths(sys.argv[1], sys.argv[2])
#     else:
#         print("Usage: python plot_multiple_paths.py input.txt [output.png]")


# # generated by ChatGPT 5/21

# import numpy as np
# import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D

# GT = 3 / 2

# # === Define the function you want to plot ===
# def f(x, y):
#     return np.sin(x) + np.cos(y)
#     # return (0.1 * x**4) + x**3 + (0.2 * y**2)
#     # return (1 - x)**2 + 100 * (y - x**2)**2
#     # return (1.5 - x + x*y)**2 + (2.25 - x + x*y**2)**2 + (2.625 - x + x*y**3)**2

# # === Load (x, y) points from file ===
# def load_path(filename):
#     data = np.loadtxt(filename)
#     x = data[:, 0]
#     y = data[:, 1]
#     z = f(x, y)
#     return x, y, z

# # === Main plotting function ===
# def plot_surface_with_path(path_file, save_file=""):
#     # Load path
#     path_x, path_y, path_z = load_path(path_file)

#     # center of graph
#     avg_x = np.mean(path_x)
#     avg_y = np.mean(path_y)
#     avg_z = np.mean(path_z)
#     xydev = np.max(abs(path_x - avg_x))
#     xydev = max(xydev, np.max(path_y - avg_y))
#     xydev = xydev * GT
#     zdev  = np.max(abs(path_z - avg_z))
#     zdev  = zdev * GT

#     # Create surface mesh
#     x_range = np.linspace(avg_x - xydev, avg_x + xydev, 50)
#     y_range = np.linspace(avg_y - xydev, avg_y + xydev, 50)
#     X, Y = np.meshgrid(x_range, y_range)
#     Z = f(X, Y)
#     Z = np.where((Z < avg_z - zdev) | (Z > avg_z + zdev), np.nan, Z) 
#     # get surface mesh
#     # x_range = np.linspace(-1.46678415, 6.216204749999999, 50)
#     # y_range = np.linspace(-3.7098825, 3.9731064, 50)
#     # X, Y = np.meshgrid(x_range, y_range)
#     # Z = f(X, Y)
#     # Z = np.where((Z < -28.838043307735568) | (Z > 34.79935866154711), np.nan, Z) 

#     # Plotting
#     fig = plt.figure(figsize=(10, 7))
#     ax = fig.add_subplot(111, projection='3d')

#     # Surface plot
#     ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7, edgecolor='none', antialiased=False)

#     # Path plot
#     ax.plot(path_x, path_y, path_z, color='red', linewidth=2, marker='o', label='Descent Path')

#     # print graph range
#     print(f'X: {avg_x - xydev}, {avg_x + xydev}')  
#     print(f'Y: {avg_y - xydev}, {avg_y + xydev}')
#     print(f'Z: {avg_z -  zdev}, {avg_z +  zdev}')

#     ax.set_xlabel('x')
#     ax.set_ylabel('y')
#     ax.set_zlabel('f(x, y)')
#     ax.set_title('Gradient Descent Path')
#     ax.legend()
#     plt.tight_layout()

#     if save_file=="":
#         plt.show()
#     else:
#         ax.view_init(elev=90, azim=-90)
#         plt.savefig(sys.argv[2], dpi=150)

# # === Example usage ===
# # Save this as `plot_path.py`, then run:
# # python plot_path.py path.txt
# if __name__ == "__main__":
#     import sys
#     if len(sys.argv) == 2:
#         plot_surface_with_path(sys.argv[1])
#     elif len(sys.argv) == 3:
#         plot_surface_with_path(sys.argv[1], sys.argv[2])
#     else:
#         print("... debug_file.txt save_file.png")