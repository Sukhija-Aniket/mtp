import os, subprocess, sys
from functions import convert_to_json, convert_to_cli, convert_headway_to_nodes

app_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ns3_directory = app_dir.split('/scratch')[0]
accepted_keys = ['t']

# Functions
def run_ns3_process(fileName, paramString, nodes):
    command = ns3_directory + '/ns3 run "' + fileName + paramString + f' --n={nodes}"'

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
    nodes, headways, printlines = convert_headway_to_nodes(json_data)
    cli_args = convert_to_cli(json_data, accepted_keys)

    for i,nodes in enumerate(nodes):
        print(printlines[i])
        run_ns3_process(ns3_executable, cli_args, nodes)
