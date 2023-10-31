import random
import os
import numpy as np
import sys

# cwd = os.getcwd()
par_dir = os.path.dirname((os.path.dirname(__file__)))
app_dir = os.path.join(par_dir, 'inputs')
print(app_dir)

def getPositions(num_nodes=10):

    # Generate random X and Y positions for nodes uniformly distributed between 0 and 2000 meters
    positions = [(random.uniform(0, 2000), np.ceil(random.uniform(0, 2))) for _ in range(num_nodes)]

    # Sort the positions by X coordinate in ascending order
    positions.sort(key=lambda x: x[0])

    # Define the output file name
    output_file = app_dir + "/positions-" + str(num_nodes) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for i, (x, y) in enumerate(positions):
            file.write(f"{x:.2f} {y:.2f} 0.00\n")

    print(f"Node positions have been saved to {output_file}")


def getVelocities(num_nodes=10, mean_velocity=60, std_deviation=10):

    velocities = [random.gauss(mean_velocity, std_deviation) for _ in range(num_nodes)]

    # Convert velocities to meters per second (1 km/h = 1000/3600 m/s)
    velocities_mps = [v * (1000 / 3600) for v in velocities]

    # Define the output file name
    output_file = app_dir +  "/velocities-" + str(num_nodes) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in velocities_mps:
            file.write(f"{x:.2f} 0.00 0.00\n")

    print(f"Node Velocities have been saved to {output_file}")



def getStartTime(num_nodes=10, mean_velocity=0, std_deviation=0.3):

    startTimes = [abs(1+random.gauss(mean_velocity, std_deviation)) for _ in range(num_nodes)]

    # Define the output file name
    output_file = app_dir + "/startTimes-" + str(num_nodes) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in startTimes:
            file.write(f"{x:.2f}\n")
        print(f"Client Start Time have been saved to {output_file}")


def getPacketGenerationRate(num_nodes=10, mean_packet_gen_rate=500):

    packetGenRate = np.random.poisson(mean_packet_gen_rate, size=num_nodes)

    # Define the output file name
    output_file = app_dir + "/packetGenRate-" + str(num_nodes) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in packetGenRate:
            file.write(f"{x}\n")

    print(f"Application Packet Generation Rate have been saved to {output_file}")


if __name__ == "__main__":
    if(len(sys.argv) < 2):
        raise Exception("Insufficient arguments!!")
    num_nodes = int(sys.argv[1])
    getPositions(num_nodes=num_nodes)
    getStartTime(num_nodes=num_nodes)
    getVelocities(num_nodes=num_nodes)
    getPacketGenerationRate(num_nodes=num_nodes)
