import sys
import os
import subprocess
import numpy as np
import pandas as pd
from functions import convert_to_json, convert_to_cli, convert_headway_to_nodes, get_array, Printlines, PrintlinesBD, plot_figure, plot_means_of_means, get_tcr, create_file, create_file_means_of_means

app_dir = os.path.dirname(__file__)
accepted_keys = ['time', 'pcap']

context_map = {}
queueMap = {
    'BE': 0,
    'BK': 1,
    'VI': 2,
    'VO': 3,
}

inverse_map = ['BE', 'BK', 'VI', 'VO']

folder_names = ["variable_nodes", "variable_data_rate", "variable_packet_size", "variable_critical_rate", "variable_general_rate"]

for key, pair in queueMap.items():
    context_map[key + "dequeue"] = f"WifiNetDevice/Mac/Txop/{key}Queue/Dequeue"
    context_map[key + "enqueue"] = f"WifiNetDevice/Mac/Txop/{key}Queue/Enqueue"

def get_mean_std_mac_delay(input_path, fileName, nodes=None, headway=None, distance=None):

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
    outFileList = fileName.split('.log')[0].split(split_char)
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
                    # Note: Comment below value in case you do not wish to calculate reliability
                    rbl_delays[value] = rbl_delays[value] + 1 if (get_tcr(headway) > ((time - uid_enqueue[uid])/1000000000)) else rbl_delays[value]
                    counters[value] = counters[value] + 1
                    uid_enqueue[uid] = -1
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

def runRandomProcessScript(executable, params):
    command = 'python3 ' + app_dir + '/randomProcessGeneration.py ' + executable + ' ' + f"'{params}'"
    print(command)
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}, exiting...")
        sys.exit(1)

def runProcessRunner(executable, params):
    command = "python3 " + app_dir + "/processRunner.py " + executable + " " + f"'{params}'"
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}, exiting...")
        sys.exit(1)

def main():
    if len(sys.argv) < 3:
        print("Usage: python script_name.py repititions ns3_executable [parameters]")
        sys.exit(1)
    elif len(sys.argv) >= 3:
        file_path = sys.argv[2]
        script_dir = os.path.dirname(file_path)
        input_path = os.path.join(script_dir, "outputs")
        plot_path = os.path.join(script_dir, "plots")
        data_path = os.path.join(script_dir, "practical")
        input_file_template = f"{os.path.basename(file_path).split('.')[0]}-n"

        repititions = sys.argv[1]
        ns3_executable = os.path.basename(sys.argv[2])
        script_directory = os.path.dirname(sys.argv[2])
        outpath_path = os.path.join(script_directory, "outputs")
        parameters = sys.argv[3] if (len(sys.argv) == 4) else '{\}'
        json_data = convert_to_json(parameters)
        position_model = json_data.get('position_model')
        cli_args = convert_to_cli(json_data, accepted_keys)
        distance_array = get_array(json_data.get('distance_array'))
        distance = distance_array[0]
        num_nodes_array = get_array(json_data.get('num_nodes_array'))
        headway_array = get_array(json_data.get('headway_array'))
        bd = int(json_data.get("bd"))

        assert str(position_model).endswith('platoon-distance') == True
        assert bd == 1

        for i in range(int(repititions)):
            # Run the random Generation file
            runRandomProcessScript(sys.argv[2], parameters)
            # Run processRunner
            runProcessRunner(sys.argv[2], parameters)

            # Do the analysis
            nodes_array, headway_array = convert_headway_to_nodes(json_data, distance)
            mean_delays = [[], [], [], []]
            std_delays = [[], [], [], []]
            rbl_delays = [[], [], [], []]
            
            for idx, nodes in enumerate(nodes_array):
                input_file = input_file_template + str(nodes) +'-d' + str(distance) + ".log"
                temparr_mean, temparr_std, temparr_rbl = get_mean_std_mac_delay(input_path, input_file, nodes=nodes, headway=headway_array[idx], distance=distance)
                for x in range(4):
                    mean_delays[x].append(round(temparr_mean[x]/1000000, 5))
                    std_delays[x].append(round(temparr_std[x]/1000000, 5))
                    rbl_delays[x].append(temparr_rbl[x])

            xlabel, plt_data = 'Number of Nodes', nodes
            if position_model.startswith('platoon'):
                xlabel = 'Headway'
                plt_data = headway_array
            
            data_map = {}
            for x in range(4):
                if sum(mean_delays[x]) == 0:
                    continue
                data_map[f'mean_{inverse_map[x]}'] = mean_delays[x]
                data_map[f'std_{inverse_map[x]}'] = std_delays[x]
                data_map[f'rbl_{inverse_map[x]}'] = rbl_delays[x]
            row = ['mean', 'std', 'rbl']
            col = len(data_map)/len(row) + 1
            create_file_means_of_means(data_map, row, plt_data, data_path)

        # Plot the graphs
        plot_means_of_means(data_path, row, xlabel, plt_data)


if __name__ == "__main__":
    main()