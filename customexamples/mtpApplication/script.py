import random
import os
import numpy as np


def getPositions(num_nodes=10):

    # Generate random X and Y positions for nodes uniformly distributed between 0 and 2000 meters
    positions = [(random.uniform(0, 2000), np.ceil(random.uniform(0, 2))) for _ in range(num_nodes)]

    # Sort the positions by X coordinate in ascending order
    positions.sort(key=lambda x: x[0])

    # Define the output file name
    output_file = "/home/aniket/D_drive/Study/MTP/ns-allinone-3.36.1/ns-3.36.1/scratch/mtp/customexamples/mtpApplication/positions.txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for i, (x, y) in enumerate(positions):
            file.write(f"{x:.2f} {y:.2f} 0.00\n")

    print(f"Node positions have been saved to {output_file}")


def getVelocities(mean_velocity=60, std_deviation=10, num_nodes=10):

    velocities = [random.gauss(mean_velocity, std_deviation) for _ in range(num_nodes)]

    # Convert velocities to meters per second (1 km/h = 1000/3600 m/s)
    velocities_mps = [v * (1000 / 3600) for v in velocities]

    # Define the output file name
    output_file = "/home/aniket/D_drive/Study/MTP/ns-allinone-3.36.1/ns-3.36.1/scratch/mtp/customexamples/mtpApplication/velocities.txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in velocities_mps:
            file.write(f"{x:.2f} 0.00 0.00\n")

    print(f"Node Velocities have been saved to {output_file}")


# getPositions()
getVelocities()
