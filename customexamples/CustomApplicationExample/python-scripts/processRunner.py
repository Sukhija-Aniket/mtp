import os, subprocess, sys
from functions import convert_to_json, convert_to_cli, convert_headway_to_nodes, get_array, Printlines

app_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ns3_directory = app_dir.split('/scratch')[0]
accepted_keys = ['t']

# Functions
def run_ns3_process(fileName, paramString, nodes, distance):
    command = ns3_directory + '/ns3 run "' + fileName + paramString + f' --n={nodes} --d={distance}"'

    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}, exiting...")
        sys.exit(1)

# Main Script
if len(sys.argv) == 1:
    print("Usage: python script_name.py ns3_executable [parameters]")
    sys.exit(1)
elif len(sys.argv) >= 2:
    ns3_executable = sys.argv[1]
    parameters = sys.argv[2] if (len(sys.argv) == 3) else '\{\}'
    json_data = convert_to_json(parameters)
    position_model = json_data['position_model']
    cli_args = convert_to_cli(json_data, accepted_keys)
    distance_array = get_array(json_data['distance_array'])
    nodes_array = get_array(json_data['num_nodes_array'])
    headway_array = get_array(json_data['headway_array'])
    
    if str(position_model).endswith('distance'):
        for distance in distance_array:
            nodes_array, headway_aray = convert_headway_to_nodes(json_data, distance)
            for idx, nodes in enumerate(nodes_array):
                Printlines(headway=headway_array[idx], nodes=nodes, distance=distance)
                run_ns3_process(ns3_executable, cli_args, nodes, distance)
        
    elif str(position_model).endswith('nodes'):
        for nodes in nodes_array:
            for headway in headway_array:
                Printlines(headway=headway, nodes=nodes)
                distance=headway*(nodes-1)
                run_ns3_process(ns3_executable, cli_args, nodes, distance)
                
    