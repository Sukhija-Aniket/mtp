#!/bin/bash

# Declarations and variables
declare -A params
num_nodes_array=(10 20 30 40 50 60 70 80 90 100) # Number of nodes to simulate on
headway_array=(2 3 4 5 6 7 8 9 10) # Distance between two consecutive nodes to simulate on
position_model='platoon-ps1'
general_type='constant'
critical_type='poisson'
general_rate=30
total_distance=2000

# Functions
print_usage() {
  echo "Usage: $(basename "$0") FILENAME [OPTIONS]"
  echo "Options:"
  echo "--help                                                  Displays this help message"
  echo "--general_rate=VALUE (default=30)                       Specify the rate of routine/general packets"
  echo "--total_distance=VALUE (default=2000)                   Specify the total distance of road during simulation"
  echo "--t=VALUE (default=10)                                  Specify time in seconds for simulation to  run"
  echo "--general_type={poisson, constant, default=constant}    Specify the distribution for generating general packets"
  echo "--critical_type={poisson, constant, default=poisson}    Specify the distribution for generating critical packets"
  echo "--position_model={uniform, platoon, default=uniform}    Specify the position model used during simulation"
  #TODO: Add a lot of echo options that will help the user to understand how to use the script.
  echo "Example:"
  echo "$(basename "$0") myfile.txt --general_type=constant --critical_type=poisson --general_rate=5"
}

get_script_dir() {
  local fileName="$1"
  found_file=$(find "$current_dir" -type f -name "$fileName" -print -quit 2>/dev/null)
  if [ -n "$found_file" ]; then
    found_directory=$(dirname "$found_file")

    echo "$found_directory"
  else
    echo "File '$fileName' could not be found, exiting..."
    exit 1
  fi 
}

handle_argument() {
  local arg="$1"
  local key="${arg%=*}"
  local value="${arg#*=}"

  # Remove leading "--" from the key
  key="${key#--}"

  # Store the option in the associative array
  params["$key"]=$value
}

# Setting paths
fileName="$1"
if [ -z "$fileName" ]; then 
  echo "Error, Please provide a fileName, exiting..."
  print_usage
  exit 1
fi
if [[ ! "$fileName" =~ \.cc$ ]]; then
  echo "File name must have a .cc extension, exiting..."
  exit 1
fi
shift

base_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
script_directory=$(get_script_dir "$fileName") 
python_script_path="${script_directory}/python-scripts"
python_script_mean_delay="${python_script_path}/mean-delay-vs-node.py"
python_script_process_generation="${python_script_path}/randomProcessGeneration.py"
python_script_process_runner="${python_script_path}/processRunner.py"
cd ${script_directory} || exit

# deleting the previously created inputs, outputs and plots
cd inputs/
rm *
cd ../

base_fileName="${fileName%.*}"
cd outputs/
rm enqueue_dequeue_trace*
rm "${base_fileName}"*
cd ../

cd plots/
files_to_delete = $(find -type f -not -name "*save*")
if [ -n "$files_to_delete" ]; then 
  echo ${files_to_delete}
  rm $files_to_delete 
fi
cd ../

# Setting input Parameters
params["num_nodes_array"]=$num_nodes_array # These are default params
params["headway_array"]=$headway_array
params["position_model"]=$position_model
params["general_type"]=$general_type
params["general_rate"]=$general_rate
params["critical_type"]=$critical_type
params["total_distance"]=$total_distance

while [[ "$#" -gt 0 ]]; do
  case "$1" in
    --*=*)
      handle_argument "$1"
      ;;
    *)
      echo "Invalid Param format: $1, exiting..."
      exit 1
      ;;
  esac
  shift
done

json_data="{"
for key in "${!params[@]}"; do
  json_data+="\"$key\":\"${params[$key]}\","
done
json_data="${json_data%,}"  
json_data+="}"

# Generating, Running and analyzing data & Processes
echo "Running File Generation Process"
python3 $python_script_process_generation $json_data 

echo "Running the Actual ns3 process"
python3 $python_script_process_runner $fileName $json_data

echo "Running the Process for output & Plot extraction"
python3 $python_script_mean_delay $json_data




# # Running the simulation for different nodes
# for node in "${num_nodes_array[@]}"
# do
#     # echo "-------------------------Number of Nodes: $node -----------------------------"
#     # echo "-------------------------Generating Random Processes-------------------------"
#     # Script to generate the parameters of simulation using some distributions
#     if [[ ${#processFlags} -eq 2 ]]; then
#       flags=$node
#       python3 $python_script_process_generation $flags
#     else
#       flags="$node $processFlags"
#       python3 $python_script_process_generation $flags
#     fi 
#     echo "-------------------------Running ${fileName} for ${node} nodes -----------------------------"
#     # Run the simulations
    
#     ../../../../ns3 run "$fileName --n=$node"
#     echo "------------------------- Simulation successfully done for ${node} nodes -------------------------"
# done

# output_path="${script_directory}/outputs"

# # Preparing arguments for plot script
# last_element="${num_nodes_array[${#num_nodes_array[@]}-1]}"
# flags="--plot"

# cd ${script_directory}

# echo "-------------------------Running the plot script ----------------------------------"
# python3 $python_script_mean_delay $last_element $flags

# echo "------------------------- Job Completed!! -----------------------------------------"
