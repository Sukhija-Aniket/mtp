import random
import os
import numpy as np
import sys
import math
from functions import convert_to_json, convert_headway_to_nodes, getCriticalRate, get_array, Printlines
par_dir = os.path.dirname((os.path.dirname(__file__)))
app_dir = os.path.join(par_dir, 'inputs')


############################################### Helper Functions ###########################################
def getPositions(distance=100, num_nodes=10, headway=25, position_model='uniform'):
    positions = []
    if(position_model.startswith('uniform')):
        # Generate random X and Y positions for nodes uniformly distributed between 0 and 2000 meters
        positions = [(random.uniform(0, distance), np.ceil(random.uniform(0, 2))) for _ in range(num_nodes)]

    elif(position_model.startswith('platoon')):
        positions = [((i*headway), 0) for i in range(num_nodes)]

    # Sort the positions by X coordinate in ascending order
    positions.sort(key=lambda x: x[0])
    # Define the output file name
    output_file = app_dir + "/positions-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for i, (x, y) in enumerate(positions):
            file.write(f"{x:.2f} {y:.2f} 0.00\n")

    print(f"Node positions have been saved to {output_file}")


def getVelocities(distance=100, num_nodes=10, mean_velocity=60, std_deviation=10):

    velocities = [random.gauss(mean_velocity, std_deviation) for _ in range(num_nodes)]

    # Convert velocities to meters per second (1 km/h = 1000/3600 m/s)
    velocities_mps = [v * (1000 / 3600) for v in velocities]

    # Define the output file name
    output_file = app_dir +  "/velocities-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in velocities_mps:
            file.write(f"{x:.2f} 0.00 0.00\n")

    print(f"Node Velocities have been saved to {output_file}")


def getStartTime(distance=100, num_nodes=10, mean_st=0, std_deviation=0.3):

    startTimes = [abs(random.gauss(mean_st, std_deviation)) for _ in range(num_nodes)]

    # Define the output file name
    output_file = app_dir + "/startTimes-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in startTimes:
            file.write(f"{x:.2f}\n")
        print(f"Client Start Time have been saved to {output_file}")


def getGenRate(num_nodes, mean_packet_gen_rate, type):
    packetGenRate = []
    type = str(type)
    if (type.startswith('poisson')):
        packetGenRate = np.random.poisson(mean_packet_gen_rate, size=num_nodes)
    elif (type.startswith('gaussian')):
        packetGenRate = [abs(random.gauss(mean_packet_gen_rate, 10)) for _ in range(num_nodes)]
    else:
        packetGenRate = [mean_packet_gen_rate for _ in range(num_nodes)]
    return packetGenRate


def getPacketGenerationRate(distance=100, num_nodes=10, mean_packet_gen_rate=30, type='constant'):
    packetGenRate = getGenRate(num_nodes,mean_packet_gen_rate, type)
    # Define the output file name
    output_file = app_dir + "/packetGenRates-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in packetGenRate:
            file.write(f"{x}\n")

    print(f"Application Packet Generation Rate have been saved to {output_file}")

def getPrioPacketGenerationRate(distance=100, num_nodes=10, mean_packet_gen_rate=100, type='poisson'):

    packetGenRate = getGenRate(num_nodes, mean_packet_gen_rate, type)

    # Define the output file name
    output_file = app_dir + "/prioPacketGenRates-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in packetGenRate:
            file.write(f"{x}\n")

    print(f"Application Priority Packet Generation Rate have been saved to {output_file}")


# Main Script
if __name__ == "__main__":
    if(len(sys.argv) < 2):
        raise Exception("Insufficient arguments!!")
    parameters = sys.argv[1]
    json_data = convert_to_json(parameters)
    position_model = json_data['position_model']
    general_type = json_data['general_type']
    general_rate = json_data['general_rate']
    critical_type = json_data['critical_type']
    distance_array = get_array(json_data['distance_array'])
    nodes_array = get_array(json_data['num_nodes_array'])
    headway_array = get_array(json_data['headway_array'])
    
    
    if str(position_model).endswith('distance'):
        for distance in distance_array:
            nodes, headways = convert_headway_to_nodes(json_data, distance)
            for idx, num_nodes in enumerate(nodes):
                Printlines(headways[idx], distance)
                critical_rate = getCriticalRate(headways[idx], json_data)
                getPositions(num_nodes=num_nodes, headway=headways[idx], position_model=position_model, distance=distance)
                getStartTime(num_nodes=num_nodes, distance=distance)
                getVelocities(num_nodes=num_nodes, distance=distance)
                getPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=general_rate, type=general_type, distance=distance)
                getPrioPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=critical_rate, type=critical_type, distance=distance)
                print("\n\n")
    
    elif str(position_model).endswith('nodes'):
        for nodes in nodes_array:
            for headway in headway_array:
                Printlines(headway=headway, nodes=nodes)
                critical_rate = getCriticalRate(headway, json_data)
                distance=headway*(nodes-1)
                getPositions(num_nodes=nodes, headway=headway, position_model=position_model, distance=distance)
                getStartTime(num_nodes=nodes, distance=distance)
                getVelocities(num_nodes=nodes, distance=distance)
                getPacketGenerationRate(num_nodes=nodes, mean_packet_gen_rate=general_rate, type=general_type, distance=distance)
                getPrioPacketGenerationRate(num_nodes=nodes, mean_packet_gen_rate=critical_rate, type=critical_type, distance=distance)
                print("\n\n")
    
                
            
        
