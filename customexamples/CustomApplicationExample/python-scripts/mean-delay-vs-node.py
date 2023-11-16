
import random
import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from functions import convert_headway_to_nodes, convert_to_json, plot_figure, func_tcr

'''
    --------------------------------------------README--------------------------------------------

    Python Script to calculate the mean delay of enqueue and dequeue in the MAC layer.

    # Param
        - Takes a file name (should be in "{relative path to ns3 folder}/ns-allinone-3.36.1/ns-3.36.1/scratch/mtp/customexamples/mtpApplication/outputs" directory) or a list of file names
        - Flags
            - '--plot' to plot the mean delays calculated by processing all the log files and their corresponding node density
    # Return
        - Returns the mean delay calculated

    The results (traces of packet for enqueue and dequeue with timestamps) will be written to a file in outputs folder

'''

app_dir = os.path.dirname(os.path.dirname(__file__))

input_path = os.path.join(app_dir, "outputs")
plot_path = os.path.join(app_dir, "plots")
input_file_template = "wave-project-n"

queueMap = {
    'BE': 0,
    'BK': 1,
    'VI': 2,
    'VO': 3,
    }
context_map = {}
inverse_map = ['BE', 'BK', 'VI', 'VO']

for key, pair in queueMap.items():
    context_map[key + "dequeue"] = f"WifiNetDevice/Mac/Txop/{key}Queue/Dequeue"
    context_map[key + "enqueue"] = f"WifiNetDevice/Mac/Txop/{key}Queue/Enqueue"

def get_mean_std_mac_delay(fileName, nodes=None, headway=None):

    mean_delays = np.zeros(4)
    std_delays = np.zeros(4)
    rbl_delays = np.zeros(4)
    counters = np.zeros(4)
    # mean_delay = 0
    uid_enqueue = {}

    input_file = os.path.join(input_path, fileName)
    if not os.path.isfile(input_file):
        raise FileNotFoundError(f"{fileName} doesn't exist in the {input_path} directory!!")

    output_file_path = os.path.join(input_path, "mean-delay-calculation.log")

    if not (nodes==None):
        output_file_path = os.path.join(input_path, f"enqueue_dequeue_trace_n{nodes}")

    file_descriptor = os.open(output_file_path, os.O_WRONLY | os.O_CREAT, 0o644)

    if file_descriptor == -1:
        raise FileNotFoundError(f"{output_file_path} doesn't exist")

    # TODO fix it later
    t_cr = {
        25.0: 0.004777,
        30.0: 0.005130,
        35.0: 0.004951,
        40.0: 0.004368,
        45.0: 0.004375,
        50.0: 0.004246,
        55.0: 0.003597,
        60.0: 0.003182,
        65.0: 0.002529,
        70.0: 0.001498,
        75.0: 0.000671
    }
    with open(input_file, "r") as file:
        for line in file:
            attr = line.split(' ')
            uid = int(attr[0])
            context = attr[1]
            time = float(attr[2])
            for key, value in queueMap.items():
                if (context.endswith(context_map[key + "dequeue"]) and uid_enqueue.__contains__(uid)):
                    os.write(file_descriptor, bytes(f"Dequeue time for uid {uid} is {time}ns \n", 'utf-8'))
                    mean_delays[value] = mean_delays[value] + (time - uid_enqueue[uid])
                    std_delays[value] = std_delays[value] + (time - uid_enqueue[uid])**2
                    rbl_delays[value] = rbl_delays[value] + 1 if t_cr[headway] > ((time-uid_enqueue[uid])/1000000000) else rbl_delays[value]
                    counters[value] = counters[value] + 1
                elif (context.endswith(context_map[key + "enqueue"])):
                    os.write(file_descriptor, bytes(f"Enqueue time for uid {uid} is {time}ns \n", 'utf-8'))
                    uid_enqueue[uid] = time

    for x in range(4):
        if counters[x] == 0:
            continue
        mean_delays[x] = mean_delays[x]/counters[x]
        std_delays[x] = std_delays[x] - (counters[x] * mean_delays[x]**2)
        std_delays[x] = np.sqrt(std_delays[x]/counters[x])
        rbl_delays[x] = rbl_delays[x]/counters[x]
        os.write(file_descriptor, bytes(f"Mean Delay for {x}: {mean_delays[x]}ns \n", 'utf-8'))
        os.write(file_descriptor, bytes(f"std Delay for {x}: {std_delays[x]}ns \n", 'utf-8'))
        os.write(file_descriptor, bytes(f"reliability for {x}: {rbl_delays[x]}ns \n", 'utf-8'))

    os.close(file_descriptor)
    return mean_delays, std_delays, rbl_delays

def main():
    if(len(sys.argv) < 2):
        raise TypeError("Insufficient arguments. At least one additional argument is required.")

    parameters = sys.argv[1]
    json_data = convert_to_json(parameters)
    nodes, headways, printlines = convert_headway_to_nodes(json_data)
    position_model = str(json_data['position_model'])

    mean_delays = [[], [], [], []]
    std_delays = [[], [], [], []]
    rbl_delays = [[], [], [], []]

    for idx, num_nodes in enumerate(nodes):
        input_file = input_file_template + str(num_nodes) + ".log"
        temparr_mean, temparr_std, temparr_rbl = get_mean_std_mac_delay(input_file, nodes=num_nodes, headway=headways[idx])
        for x in range(4):
            mean_delays[x].append(round(temparr_mean[x]/1000000, 5))
            std_delays[x].append(round(temparr_std[x]/1000000, 5))
            rbl_delays[x].append(temparr_rbl[x])
    xlabel, plt_data = 'Number of Nodes', nodes
    if position_model.startswith('platoon'):
        xlabel = 'Headway'
        plt_data = headways


    data_map = {}
    for x in range(4):
        if sum(mean_delays[x]) == 0:
            continue
        data_map[f'mean_{inverse_map[x]}'] = mean_delays[x]
        data_map[f'std_{inverse_map[x]}'] = std_delays[x]
        data_map[f'rbl_{inverse_map[x]}'] = rbl_delays[x]
    row = ['mean', 'std', 'rbl']
    col = len(data_map)/len(row) + 1
    plot_figure(data_map, row, col, plt_data, xlabel, plot_path)


if __name__ == "__main__":
    try:
        main()
    except (TypeError, FileNotFoundError) as e:
        print(f"Error: {e}")
