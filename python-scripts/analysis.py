
import random
import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from functions import convert_headway_to_nodes, convert_to_json, plot_figure, plot_figure_solo, get_array, get_float_array, get_tcr
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
script_dir = app_dir


queueMap = {
    'BE': 0,
    'BK': 1,
    'VI': 2,
    'VO': 3,
    }
context_map = {}
inverse_map = ['BE', 'BK', 'VI', 'VO']

folder_names = ["variable_nodes", "variable_data_rate", "variable_pkt_size", "variable_lamda0", "variable_lamda1"]

for key, pair in queueMap.items():
    context_map[key + "dequeue"] = f"WifiNetDevice/Mac/Txop/{key}Queue/Dequeue"
    context_map[key + "enqueue"] = f"WifiNetDevice/Mac/Txop/{key}Queue/Enqueue"

def get_mean_std_mac_delay(input_path, fileName):

    mean_delays = np.zeros(4)
    std_delays = np.zeros(4)
    rbl_delays = np.zeros(4)
    counters = np.zeros(4)
    # mean_delay = 0
    uid_enqueue = {}

    input_file = os.path.join(input_path, fileName)
    if not os.path.isfile(input_file):
        raise FileNotFoundError(f"{fileName} doesn't exist in the {input_path} directory!!")

    split_char = '-'
    outFileList = fileName.split('.')[0].split(split_char)
    outFileList[0] = "analysis"
    outFileName = split_char.join(outFileList)

    output_file_path = os.path.join(input_path, outFileName)
    
    file_descriptor = os.open(output_file_path, os.O_WRONLY | os.O_CREAT, 0o644)

    if file_descriptor == -1:
        raise FileNotFoundError(f"{output_file_path} doesn't exist")   
    
    # Read the input files
    with open(input_file, "r") as file:
        for line in file:
            attr = line.split(' ')
            uid = int(attr[0])
            context = attr[1]
            time = float(attr[2])
            for key, value in queueMap.items():
                if (context.endswith(context_map[key + "dequeue"]) and uid_enqueue.__contains__(uid) and uid_enqueue[uid]!=-1):
                    os.write(file_descriptor, bytes(f"Dequeue time for uid {uid} is {time}ns \n", 'utf-8'))
                    mean_delays[value] = mean_delays[value] + (time - uid_enqueue[uid])
                    std_delays[value] = std_delays[value] + (time - uid_enqueue[uid])**2
                    uid_enqueue[uid] = -1
                    # rbl_delays[value] = rbl_delays[value] + 1 if (get_tcr(headway) > ((time - uid_enqueue[uid])/1000000000)) else rbl_delays[value]
                    counters[value] = counters[value] + 1
                elif (context.endswith(context_map[key + "enqueue"]) and (not uid_enqueue.__contains__(uid))):
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
    if(len(sys.argv) < 3):
        raise TypeError("Insufficient arguments. At least two additional arguments are required.")

    file_path = sys.argv[1]
    script_dir = os.path.dirname(file_path)
    input_path = os.path.join(script_dir, "outputs")
    plot_path = os.path.join(script_dir, "plots")
    
    parameters = sys.argv[2]
    json_data = convert_to_json(parameters)
    # position_model = str(json_data['position_model'])
    distance_array = get_array(json_data['distance_array'])
    nodes_array = get_array(json_data['num_nodes_array'])
    fixed_nodes = 250
    data_rate_array = get_array(json_data["data_rate_array"])
    fixed_data_rate = 27
    pkt_size_array = get_array(json_data["pkt_size_array"])
    fixed_pkt_size = 500

    lamda0_array = get_array(json_data["lamda0_array"])
    fixed_lamda0 = int(json_data["critical_rate"])
    lamda1_array = get_array(json_data["lamda1_array"])
    fixed_lamda1 = int(json_data["general_rate"])
    bd = int(json_data["bd"])
    
    distance = distance_array[0]

    variable_array = [nodes_array, data_rate_array, pkt_size_array, lamda0_array, lamda1_array]
    fixed_values = [fixed_nodes, fixed_data_rate, fixed_pkt_size, fixed_lamda0, fixed_lamda1]    

    for idx, variable in enumerate(variable_array):
            mean_delays = [[], [], [], []]
            std_delays = [[], [], [], []]
            rbl_delays = [[], [], [], []]

            for jdx, var in enumerate(variable):
                temp_fixed_values = fixed_values.copy()
                temp_fixed_values[idx] = var
                input_file_template = f"testbd-n{temp_fixed_values[0]}-d{temp_fixed_values[1]}-p{temp_fixed_values[2]}-l0{temp_fixed_values[3]}-l1{temp_fixed_values[4]}.log"
                input_path_temp = input_path + f"/{folder_names[idx]}"
                temparr_mean, temparr_std, temparr_rbl = get_mean_std_mac_delay(input_path_temp, input_file_template)
                for x in range(4):
                    mean_delays[x].append(round(temparr_mean[x]/1000000, 5))
                    std_delays[x].append(round(temparr_std[x]/1000000, 5))
                    rbl_delays[x].append(temparr_rbl[x])

            # Remove this break after everything is in line
            break 


#### Plotting #####


    # if str(position_model).endswith('distance'):
    #     for distance in distance_array:
    #         nodes_array, headway_array = convert_headway_to_nodes(json_data, distance)
    #         mean_delays = [[], [], [], []]
    #         std_delays = [[], [], [], []]
    #         rbl_delays = [[], [], [], []]
            
    #         for idx, nodes in enumerate(nodes_array):
    #             input_file = input_file_template + str(nodes) +'-d' + str(distance) + ".log"
    #             temparr_mean, temparr_std, temparr_rbl = get_mean_std_mac_delay(input_path, input_file, nodes=nodes, headway=headway_array[idx], distance=distance)
    #             for x in range(4):
    #                 mean_delays[x].append(round(temparr_mean[x]/1000000, 5))
    #                 std_delays[x].append(round(temparr_std[x]/1000000, 5))
    #                 rbl_delays[x].append(temparr_rbl[x])
    #         xlabel, plt_data = 'Number of Nodes', nodes
    #         if position_model.startswith('platoon'):
    #             xlabel = 'Headway'
    #             plt_data = headway_array
            
    #         data_map = {}
    #         for x in range(4):
    #             if sum(mean_delays[x]) == 0:
    #                 continue
    #             data_map[f'mean_{inverse_map[x]}'] = mean_delays[x]
    #             data_map[f'std_{inverse_map[x]}'] = std_delays[x]
    #             data_map[f'rbl_{inverse_map[x]}'] = rbl_delays[x]
    #         row = ['mean', 'std', 'rbl']
    #         col = len(data_map)/len(row) + 1
    #         plot_figure_solo(data_map, row, col, plt_data, xlabel, plot_path, distance)
    
    # elif str(position_model).endswith('nodes'):
    #     for nodes in nodes_array:
    #         mean_delays = [[], [], [], []]
    #         std_delays = [[], [], [], []]
    #         rbl_delays = [[], [], [], []]
            
    #         for idx, headway in enumerate(headway_array):
    #             distance = headway*(nodes-1)
    #             input_file = input_file_template + str(nodes) +'-d' + str(distance) + ".log"
    #             temparr_mean, temparr_std, temparr_rbl = get_mean_std_mac_delay(input_path, input_file, nodes=nodes, headway=headway, distance=distance)
    #             for x in range(4):
    #                 mean_delays[x].append(round(temparr_mean[x]/1000000, 5))
    #                 std_delays[x].append(round(temparr_std[x]/1000000, 5))
    #                 rbl_delays[x].append(temparr_rbl[x])
    #         xlabel, plt_data = 'Headway', headway_array
            
    #         data_map = {}
    #         for x in range(4):
    #             if sum(mean_delays[x]) == 0:
    #                 continue
    #             data_map[f'mean_{inverse_map[x]}'] = mean_delays[x]
    #             data_map[f'std_{inverse_map[x]}'] = std_delays[x]
    #             data_map[f'rbl_{inverse_map[x]}'] = rbl_delays[x]
    #         row = ['mean', 'std', 'rbl']
    #         col = len(data_map)/len(row) + 1
    #         plot_figure_solo(data_map, row, col, plt_data, xlabel, plot_path, distance)
                
if __name__ == "__main__":
    try:
        main()
    except (TypeError, FileNotFoundError) as e:
        print(f"Error: {e}")
