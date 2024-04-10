import random
import os
import numpy as np
import sys
import math
from functions import convert_to_json, convert_headway_to_nodes, getCriticalRate, get_array, Printlines
par_dir = os.path.dirname(os.path.dirname(__file__))
script_dir = par_dir


############################################### Helper Functions ###########################################
def getRepRates(distance=100, num_nodes=10, headway=25, delta=3):
    repRates = [random.randint(0,delta) for _ in range(num_nodes)]

    # Define the output file name
    output_file = script_dir +  "/repRates-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in repRates:
            file.write(f"{x}\n")

    print(f"Repetition Rates have been saved to {output_file}")


def getPositions(distance=700, num_nodes=10, headway=25, position_model='uniform'):
    positions = []
    if(position_model.startswith('uniform')):
        # Generate random X and Y positions for nodes uniformly distributed between 0 and 2000 meters
        positions = [(random.uniform(0, distance), np.ceil(random.uniform(0, 2))) for _ in range(num_nodes)]

    elif(position_model.startswith('platoon')):
        positions = [((i*headway), 0) for i in range(num_nodes)]

    # Sort the positions by X coordinate in ascending order
    positions.sort(key=lambda x: x[0])
    # Define the output file name
    output_file = script_dir + "/positions-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for i, (x, y) in enumerate(positions):
            file.write(f"{x:.2f} {y:.2f} 0.00\n")

    print(f"Node positions have been saved to {output_file}")


def getVelocities(distance=700, num_nodes=10, mean_velocity=60, std_deviation=10):

    velocities = [random.gauss(mean_velocity, std_deviation) for _ in range(num_nodes)]

    # Convert velocities to meters per second (1 km/h = 1000/3600 m/s)
    velocities_mps = [v * (1000 / 3600) for v in velocities]

    # Define the output file name
    output_file = script_dir +  "/velocities-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in velocities_mps:
            file.write(f"{x:.2f} 0.00 0.00\n")

    print(f"Node Velocities have been saved to {output_file}")


def getStartTime(distance=700, num_nodes=10, mean_st=0, std_deviation=0.01):

    startTimes = [abs(random.gauss(mean_st, std_deviation)) for _ in range(num_nodes)]

    # Define the output file name
    output_file = script_dir + "/startTimes-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in startTimes:
            x = x/1000
            file.write(f"{x:.6f}\n")
        print(f"Client Start Time have been saved to {output_file}")


def getGenRate(num_nodes, mean_packet_gen_rate, type):
    packetGenRate = []
    type = str(type)
    if (type.startswith('poisson')):
        packetGenRate = np.random.poisson(mean_packet_gen_rate, size=num_nodes)
    elif (type.startswith('gaussian')):
        packetGenRate = [int(abs(random.gauss(mean_packet_gen_rate, 10))) for _ in range(num_nodes)]
    else:
        packetGenRate = [int(mean_packet_gen_rate) for _ in range(num_nodes)]
    return packetGenRate


def getPacketGenerationRate(distance=100, num_nodes=10, mean_packet_gen_rate=30, type='constant'):
    packetGenRate = getGenRate(num_nodes,mean_packet_gen_rate, type)
    # Define the output file name
    output_file = script_dir + "/packetGenRates-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in packetGenRate:
            file.write(f"{x}\n")

    print(f"Application Packet Generation Rate have been saved to {output_file}")

def getPrioPacketGenerationRate(distance=100, num_nodes=10, mean_packet_gen_rate=100, type='poisson'):

    packetGenRate = getGenRate(num_nodes, mean_packet_gen_rate, type)

    # Define the output file name
    output_file = script_dir + "/prioPacketGenRates-" + str(num_nodes) + '-' + str(distance) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in packetGenRate:
            file.write(f"{x}\n")

    print(f"Application Priority Packet Generation Rate have been saved to {output_file}")


# Main Script
if __name__ == "__main__":
    if(len(sys.argv) < 3):
        raise Exception("Insufficient arguments!!")
    script_dir = os.path.join(os.path.dirname(sys.argv[1]), 'inputs')
    print(script_dir)
    parameters = sys.argv[2]
    json_data = convert_to_json(parameters)
    position_model = json_data.get('position_model')
    general_type = json_data.get('general_type')
    general_rate = int(json_data.get('general_rate'))
    critical_type = json_data.get('critical_type')
    critical_rate = int(json_data.get('critical_rate'))
    distance_array = get_array(json_data.get('distance_array'))
    num_nodes_array = get_array(json_data.get('num_nodes_array')) # Gives key error make it work later to give None
    headway_array = get_array(json_data.get('headway_array'))
    bd = int(json_data.get('bd'))

    # Code for bd (Only need positions files, rest of the input is provided in the executable itself)
    if bd:
        if str(position_model).endswith('uniform-distance'): # indicating headway is not present
            fixed_num_nodes = int(json_data.get('num_nodes'))
            for distance in distance_array: 
                for idx, num_nodes in enumerate(num_nodes_array):
                    # Printlines(headway=headway_array[idx], distance=distance)
                    # critical_rate = getCriticalRate(headway_array[idx], json_data)
                    getPositions(num_nodes=num_nodes, position_model=position_model, distance=distance)
                    getStartTime(num_nodes=num_nodes, distance=distance)
                    getVelocities(num_nodes=num_nodes, distance=distance)
                    # getPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=general_rate, type=general_type, distance=distance)
                    # getPrioPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=critical_rate, type=critical_type, distance=distance)
                    print("\n\n")
                getPositions(num_nodes=fixed_num_nodes, position_model=position_model, distance=distance)
                getStartTime(num_nodes=fixed_num_nodes, distance=distance)
                getVelocities(num_nodes=fixed_num_nodes, distance=distance)
                print("\n\n")
        
        # Code for platoon (bd)
        elif str(position_model).endswith('platoon-distance'):
            for distance in distance_array:
                num_nodes_array, headway_array = convert_headway_to_nodes(json_data, distance)
                for idx, num_nodes in enumerate(num_nodes_array):
                    Printlines(headway=headway_array[idx], distance=distance)
                    critical_rate = getCriticalRate(headway_array[idx], json_data)
                    getPositions(num_nodes=num_nodes, headway=headway_array[idx], position_model=position_model, distance=distance)
                    getStartTime(num_nodes=num_nodes, distance=distance)
                    getVelocities(num_nodes=num_nodes, distance=distance)
                    getPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=general_rate, type=general_type, distance=distance)
                    getPrioPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=critical_rate, type=critical_type, distance=distance)
                    print("\n\n")


    # Code for platoon (p)
    else:
        if str(position_model).endswith('platoon-distance'):
            for distance in distance_array:
                num_nodes_array, headway_array = convert_headway_to_nodes(json_data, distance)
                for idx, num_nodes in enumerate(num_nodes_array):
                    Printlines(headway=headway_array[idx], distance=distance)
                    critical_rate = getCriticalRate(headway_array[idx], json_data)
                    getPositions(num_nodes=num_nodes, headway=headway_array[idx], position_model=position_model, distance=distance)
                    getStartTime(num_nodes=num_nodes, distance=distance)
                    getVelocities(num_nodes=num_nodes, distance=distance)
                    getPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=general_rate, type=general_type, distance=distance)
                    getPrioPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=critical_rate, type=critical_type, distance=distance)
                    print("\n\n")
        
        elif str(position_model).endswith('platoon-nodes'):
            for nodes in num_nodes_array:
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

    
                
            
        
