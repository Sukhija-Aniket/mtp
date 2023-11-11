import random
import os
import numpy as np
import sys
import math

# cwd = os.getcwd()
par_dir = os.path.dirname((os.path.dirname(__file__)))
app_dir = os.path.join(par_dir, 'inputs')

dictionary = {'general_rate': '500', 'critical_rate': '100', 'position_model': 'uniform', 'general_type': 'poisson'}

# standard format - "alpha:gamma:'convergence value of headway':'velocity of lead node':'tunable param':'start node position':'headway value'"
platoon_standard = {'ps1': '-1.933:0.652:2:25:500:20:2'}

############################################### Helper Functions ###########################################

# improve this function!!!!
def isList(s):
    if(s[0]=='[' and s[-1]==']'):
        return True
    return False
########################################################################

# Convert string that is provided as a cmd argument to dictionary
def stringtoDict(s, dictionary):
    args = s.split(',')
    for arg in args:
        params = arg.split('=')
        if (len(params) == 2):
            dictionary[params[0]] = params[1]
########################################################################

# Get probablity that 2-wheelers(non-communicating nodes) will intercept the platoon
def getProbPlatoonIntercept(standard):
    alpha, gamma, y_star, x_dot0 = [float(x) for x in standard.split(':')[:4]]
    term = alpha + gamma*(y_star/x_dot0)
    return math.exp(term)/(1 + math.exp(term))
########################################################################

############################################### Helper Functions ###########################################


def getPositions(num_nodes=10, type='uniform'):
    positions = []
    if(type.startswith('uniform')):
        # Generate random X and Y positions for nodes uniformly distributed between 0 and 2000 meters
        positions = [(random.uniform(0, 2000), np.ceil(random.uniform(0, 2))) for _ in range(num_nodes)]
        
    elif(type.startswith('platoon')):
        standard = type.split('-')[1]
        critical_rate = round(float(platoon_standard[standard].split(':')[4])*getProbPlatoonIntercept(platoon_standard[standard]), 3)
        dictionary['critical_rate'] = critical_rate
        headway = int(platoon_standard[standard].split(':')[6])
        startNodePos = int(platoon_standard[standard].split(':')[5])
        positions = [(startNodePos+(i*headway), 1) for i in range(num_nodes)]
    
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

    startTimes = [abs(random.gauss(mean_velocity, std_deviation)) for _ in range(num_nodes)]

    # Define the output file name
    output_file = app_dir + "/startTimes-" + str(num_nodes) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in startTimes:
            file.write(f"{x:.2f}\n")
        print(f"Client Start Time have been saved to {output_file}")


def getPacketGenerationRate(num_nodes=10, mean_packet_gen_rate=30, type='poisson'):
    packetGenRate = []
    if (type.startswith('poisson')):
        packetGenRate = np.random.poisson(mean_packet_gen_rate, size=num_nodes)
    else:
        packetGenRate = [mean_packet_gen_rate for _ in range(num_nodes)]

    # Define the output file name
    output_file = app_dir + "/packetGenRates-" + str(num_nodes) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in packetGenRate:
            file.write(f"{x}\n")

    print(f"Application Packet Generation Rate have been saved to {output_file}")

def getPrioPacketGenerationRate(num_nodes=10, mean_packet_gen_rate=100):

    packetGenRate = np.random.poisson(mean_packet_gen_rate, size=num_nodes)

    # Define the output file name
    output_file = app_dir + "/prioPacketGenRates-" + str(num_nodes) + ".txt"

    # Write the positions to the output file
    with open(output_file, "w") as file:
        for x in packetGenRate:
            file.write(f"{x}\n")

    print(f"Application Priority Packet Generation Rate have been saved to {output_file}")


if __name__ == "__main__":
    if(len(sys.argv) < 2):
        raise Exception("Insufficient arguments!!")
    num_nodes = int(sys.argv[1])
    if(len(sys.argv) == 2):
        getPositions(num_nodes=num_nodes)
        getStartTime(num_nodes=num_nodes)
        getVelocities(num_nodes=num_nodes)
        getPacketGenerationRate(num_nodes=num_nodes)
        getPrioPacketGenerationRate(num_nodes=num_nodes)
    else:
        try:
            args = sys.argv[2]
            if not (isList(args)):
                raise Exception("Invalid argument!!")
            args = args[1:-1]
            stringtoDict(args, dictionary)
            getPositions(num_nodes=num_nodes, type=dictionary['position_model'])
            getStartTime(num_nodes=num_nodes)
            getVelocities(num_nodes=num_nodes)
            getPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=int(dictionary['general_rate']), type=dictionary['general_type'])
            getPrioPacketGenerationRate(num_nodes=num_nodes, mean_packet_gen_rate=int(dictionary['critical_rate']))
        except (ValueError, SyntaxError) as err:
            if err:
                raise err
            raise Exception("Something went wrong")

