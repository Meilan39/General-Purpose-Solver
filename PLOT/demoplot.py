import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# ==============================================================================
# --- USER CONFIGURATION ---
# Modify the values in this section to customize your plot.
# ==============================================================================

# 1. DEFINE THE 3D FUNCTION
def f(x, y):
    return np.sin(x) * np.cos(y) + 0.15 * (y - x - 1)**2



# 2. DEFINE CUSTOM POINTS
# Add any number of (x, y) points to be plotted on the surface.
# The z-value will be calculated automatically using the function f(x, y).
# Format: [(x1, y1), (x2, y2), ...]
custom_points = [
]
# (2.628392, 3.836596),

# 3. DEFINE THE 2D LINE
# The line will be drawn on the (x, y) plane and projected onto the 3D surface.
# Here, we define a simple line y = mx + c.
line_slope = 1 # -0.24 # 0.04 'm' in y = mx + c
line_intercept = 1 # 4.48 # 3.73 'c' in y = mx + c

# 4. SET GRAPH DIMENSIONS (AXIS LIMITS)
# Define the viewable range for each axis.
x_limits = (0, 5)
y_limits = (1, 5)
z_limits = (-1.5, 1.5)

# 5. SET VIEWING PERSPECTIVE
# 'elev' is the elevation angle (up/down rotation).
# 'azim' is the azimuth angle (left/right rotation).
elevation_angle = 40  # Angle in degrees
azimuth_angle = -135   # Angle in degrees

# 6. SET OUTPUT FILENAME
# The plot will be saved to this file in SVG format.
output_filename = "sincos10.svg"

# 7. PLOT RESOLUTION
# Higher numbers create a smoother surface but use more memory.
surface_resolution = 200
line_resolution = 100

# ==============================================================================
# --- PLOTTING SCRIPT ---
# You generally don't need to modify the code below this line.
# ==============================================================================

def generate_plot():
    """
    Main function to generate and save the 3D plot.
    """
    print("Generating plot...")

    # --- Setup Figure and 3D Axes ---
    fig = plt.figure(figsize=(12, 9))
    ax = fig.add_subplot(111, projection='3d')

    # --- Generate Data for the 3D Surface ---
    x_surf = np.linspace(x_limits[0], x_limits[1], surface_resolution)
    y_surf = np.linspace(y_limits[0], y_limits[1], surface_resolution)
    X, Y = np.meshgrid(x_surf, y_surf)
    Z = f(X, Y)

    # --- Plot the 3D Surface ---
    # Using a colormap for better visualization of height.
    # `rstride` and `cstride` can be adjusted for different wireframe looks.
    ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.7, edgecolor='none', antialiased=True, rasterized=False)

    # --- Plot the Custom Points ---
    if custom_points:
        # Convert list of points to numpy arrays for easier processing
        points_x = np.array([p[0] for p in custom_points])
        points_y = np.array([p[1] for p in custom_points])
        points_z = f(points_x, points_y)

        # Use scatter for distinct markers
        ax.scatter(points_x, points_y, points_z, color='red', s=50, depthshade=True, label='(3.6, 3.6)')

    # --- Generate and Plot the 2D Line's Contour ---
    # Create x-values for the line spanning the graph's width
    line_x = np.linspace(x_limits[0], x_limits[1], line_resolution)
    # Calculate corresponding y-values based on the line equation
    line_y = line_slope * line_x + line_intercept
    # Calculate the z-values to trace the line on the 3D surface
    line_z = f(line_x, line_y)

    # Plot the line
    ax.plot(line_x, line_y, line_z, color='black', lw=3, label=f'y={line_slope}x+{line_intercept}')


    # --- Customize the Plot Appearance ---
    ax.set_xlabel('X-axis', fontweight='bold')
    ax.set_ylabel('Y-axis', fontweight='bold')
    ax.set_zlabel('Z-axis (f(x,y))', fontweight='bold')
    # ax.set_title('Customizable 3D Function Plot', fontsize=16)

    # Set the defined axis limits
    ax.set_xlim(x_limits)
    ax.set_ylim(y_limits)
    ax.set_zlim(z_limits)

    # Set the viewing angle
    ax.view_init(elev=elevation_angle, azim=azimuth_angle)

    # Add a legend to identify points and lines
    ax.legend()

    # --- Save the Figure to an SVG file ---
    try:
        plt.savefig(output_filename, format='svg', bbox_inches='tight')
        print(f"Successfully saved plot to '{output_filename}'")
    except Exception as e:
        print(f"Error saving file: {e}")

    # Optionally display the plot interactively
    plt.show()


if __name__ == "__main__":
    # This block runs when the script is executed directly.
    generate_plot()
