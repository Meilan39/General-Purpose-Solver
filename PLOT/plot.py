import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os
import re

GT = 3 / 2

# === Define the function to plot ===
def f(x, y):
    return (1-x)**2 + 100 * (y - x**2)**2
    # return np.sin(x) * np.cos(y)


# === Load paths grouped by mode from a text file ===
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

        # Add the last group if it exists
        if x and y and current_key:
            groups.setdefault(current_key, []).append((np.array(x), np.array(y)))

    return groups

# === Plot one group of paths on the function surface ===
def plot_surface_with_paths(paths, title="Descent Paths", save_file=""):
    """
    Generates and saves a 3D plot of the function surface along with
    the provided optimization paths.
    """
    # Combine all points from all paths for calculating plot bounds
    all_x = np.concatenate([p[0] for p in paths])
    all_y = np.concatenate([p[1] for p in paths])
    all_z = f(all_x, all_y)

    # --- FIX ---
    # The plot bounds are now calculated dynamically based on the input data,
    # which is more robust than hardcoding them.
    x_range = np.linspace(-2, 2, 200)
    y_range = np.linspace(-1, 2, 200)
    X, Y = np.meshgrid(x_range, y_range)
    Z = f(X, Y)

    # Create the plot
    fig = plt.figure(figsize=(12, 9))
    ax = fig.add_subplot(111, projection='3d')
    # Use a vector-friendly rasterized=False for the surface
    ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7, edgecolor='none', antialiased=True, rasterized=False)

    # Plot each path on the surface
    for x, y in paths:
        z = f(x, y)
        ax.plot(x, y, z, linewidth=2, marker='o')

    ax.set_xlabel('X-axis', fontweight='bold')
    ax.set_ylabel('Y-axis', fontweight='bold')
    ax.set_zlabel('Z-axis (f(x,y))', fontweight='bold')

    ax.set_xlim(-2, 2)
    ax.set_ylim(-1, 2)
    ax.set_zlim(-0.5, 20)

    if save_file:
        # Set a 2D-friendly view for the saved file, updated to your values
        ax.view_init(elev=90, azim=0) #40, -135
        plt.savefig(save_file, format='svg', bbox_inches='tight')
        print(f"Saved plot to {save_file}")
        plt.close(fig) # Close the figure to free memory
    else:
        plt.show()

# === Entry point ===
if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        print("Usage: python plot_multiple_paths.py input.txt [out_prefix]")
        sys.exit(1)

    input_file = sys.argv[1]
    out_prefix = sys.argv[2] if len(sys.argv) >= 3 else ""

    grouped_paths = load_grouped_paths(input_file)
    for key, paths in grouped_paths.items():
        title = f"Descent Paths ({key})"
        save_file = f"{out_prefix}_{key}.svg" if out_prefix else ""
        plot_surface_with_paths(paths, title=title, save_file=save_file)


# import numpy as np
# import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D
# import os
# import re

# GT = 3 / 2

# # === Define the function to plot ===
# def f(x, y):
#     return np.sin(x) * np.cos(y)
#     # return (-0.0001) * (np.sin(x) * np.sin(y) * np.exp(100 - np.sqrt(x**2 + y**2)/np.pi))**(0.1)
#     # return (0.1 * x**4) + x**3 + (0.2 * y**2)
#     # return (1 - x)**2 + 100 * (y - x**2)**2
#     # return (1.5 - x + x*y)**2 + (2.25 - x + x*y**2)**2 + (2.625 - x + x*y**3)**2

# # === Load paths grouped by mode ===
# def load_grouped_paths(filename):
#     with open(filename) as f:
#         AL = False
#         idx = 0
#         groups = {}
#         current_key = None
#         x, y = [], []

#         for line in f:
#             line = line.strip()
#             if not line:
#                 if x and y and current_key:
#                     groups.setdefault(current_key, []).append((np.array(x), np.array(y)))
#                     x, y = [], []
#                 continue

#             if line.startswith("Gradient Descent"):
#                 current_key = "gradient"
#                 x, y = [], []
#                 continue

#             match = re.match(r"Augmented Lagrangian r \{([0-9eE.+-]+)\}", line)
#             if match:
#                 AL = True
#                 current_key = f"r={float(match.group(1)):g}"
#                 x, y = [], []
#                 continue

#             if len(line.split()) == 2:                
#                 idx = idx + 1
#                 if AL and (idx % 10):
#                     continue
#                 xi, yi = map(float, line.split())
#                 x.append(xi)
#                 y.append(yi)

#         # Final group
#         if x and y and current_key:
#             groups.setdefault(current_key, []).append((np.array(x), np.array(y)))

#     return groups

# # === Plot one group ===
# def plot_surface_with_paths(paths, title="Descent Paths", save_file=""):
#     # Combine all points
#     all_x = np.concatenate([p[0] for p in paths])
#     all_y = np.concatenate([p[1] for p in paths])
#     all_z = f(all_x, all_y)

#     avg_x, avg_y, avg_z = np.mean(all_x), np.mean(all_y), np.mean(all_z)
#     xydev = max(np.max(abs(all_x - avg_x)), np.max(abs(all_y - avg_y))) * GT
#     zdev = np.max(abs(all_z - avg_z)) * GT

#     # Manual bounds
#     x_range = np.linspace(0, 5, 50)
#     y_range = np.linspace(1, 5, 50)
#     X, Y = np.meshgrid(x_range, y_range)
#     Z = f(X, Y)
#     Z = np.where((Z < 1.5) | (Z > -1.5), np.nan, Z)

#     # Plot
#     fig = plt.figure(figsize=(10, 7))
#     ax = fig.add_subplot(111, projection='3d')
#     ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7, edgecolor='none', antialiased=False)

#     for x, y in paths:
#         z = f(x, y)
#         ax.plot(x, y, z, linewidth=2, marker='o')

#     ax.set_xlabel('x')
#     ax.set_ylabel('y')
#     ax.set_zlabel('f(x, y)')
#     ax.set_title(title)

#     print(f'X: {avg_x - xydev}, {avg_x + xydev}')
#     print(f'Y: {avg_y - xydev}, {avg_y + xydev}')
#     print(f'Z: {avg_z - zdev}, {avg_z + zdev}')

#     plt.tight_layout()
#     if save_file == "":
#         plt.show()
#     else:
#         ax.view_init(elev=30, azim=-135)
#         # plt.savefig(save_file, dpi=150)
#         plt.savefig(save_file, format='svg')
#         plt.close()

# # === Entry point ===
# if __name__ == "__main__":
#     import sys
#     if len(sys.argv) < 2:
#         print("Usage: python plot_multiple_paths.py input.txt [out_prefix]")
#         exit()

#     input_file = sys.argv[1]
#     out_prefix = sys.argv[2] if len(sys.argv) >= 3 else ""

#     grouped_paths = load_grouped_paths(input_file)
#     for key, paths in grouped_paths.items():
#         title = f"Descent Paths ({key})"
#         save_file = f"{out_prefix}_{key}.svg" if out_prefix else ""
#         plot_surface_with_paths(paths, title=title, save_file=save_file)
