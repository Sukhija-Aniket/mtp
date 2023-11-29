#!/bin/bash

# Declarations and variables
declare -A params
num_nodes_array=(10 20 30 40 50 60 70 80 90 100) # Number of nodes to simulate on
headway_array=(25 30 35 40 45 50 55 60 65 70 75) # Distance between two consecutive nodes to simulate on
distance_array=(100 300 500 700 900) # Total Distance to consider
position_model='platoon-nodes' 
general_type='constant'
critical_type='poisson'
general_rate=30
plot=0

# Functions
print_usage() {
  echo "Usage: $(basename "$0") FILENAME [OPTIONS]"
  echo "Options:"
  echo -e "\t--help                                                  Displays this help message"
  echo -e "\t--plot                                                  Runs only code for obtaining plots"
  echo -e "\t--general_rate=VALUE (default=30)                       Specify the rate of routine/general packets"
  echo -e "\t--t=VALUE (default=10)                                  Specify time in seconds for simulation to  run"
  echo -e "\t--general_type={poisson, constant, gaussian, default=constant}    Specify the distribution for generating general packets"
  echo -e "\t--critical_type={poisson, constant, gaussian, default=poisson}    Specify the distribution for generating critical packets"
  echo -e "\t--position_model={platoon-nodes, platoon-distance, uniform, default=platoon-nodes}    Specify the position model used during simulation"
  echo -e "\t--headway_array=(start,stop,step) start and stop both are inclusive."
  echo -e "\t--distance_array=(start,stop,step) start and stop both are inclusive."
  echo -e "\t--num_nodes_array=(start,stop,step) start and stop both are inclusive."
  
  #TODO: Add a lot of echo options that will help the user to understand how to use the script.
  echo -e "\nExample:"
  echo -e "\t$(basename "$0") wave-project.cc --general_type=constant --critical_type=poisson --general_rate=5"
}

get_script_dir() {
  local fileName="$1"
  found_file=$(find "." -type f -name "$fileName" -print -quit 2>/dev/null)
  if [ -n "$found_file" ]; then
    found_directory=$(realpath $(dirname "$found_file"))

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

handle_array() {
  local arg="$1"
  local key="${arg%=*}"
  local value="${arg#*=}"

   key="${key#--}"

  if [[ "$value" =~ ^[0-9]+,[0-9]+,[0-9]+$ ]]; then
    IFS=',' read -a values <<< "$value"

    local start="${values[0]}"
    local end="${values[1]}"
    local step="${values[2]}"

    local array=()
    for((i=start; i<=end;i+=step)); do
      array+=("$i")
    done

    params["$key"]="$(IFS=" "; echo "${array[@]}")"
  else
    echo "Invalid Argument: $value, exiting..."
    exit 1
  fi

}

# Step 1: Checking for Execution
fileName="$1"
if [ -z "$fileName" ]; then
  echo "Error, Please provide a .cc file as input, exiting..."
  exit 1
fi

if [[ "$#" -eq 1 && "$1" == "--help" ]]; then
  print_usage
  exit 0
fi

if [[ ! "$fileName" =~ \.cc$ ]]; then
  echo "File name must have a .cc extension, exiting..."
  exit 1
fi
shift

# Step 2: Clearing old outputs
touch nohup.out
echo -n > nohup.out

# Step 3: Moving to script Directory
base_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
script_directory=$(get_script_dir "$fileName")
python_script_path="${script_directory}/python-scripts"
python_script_mean_delay="${python_script_path}/mean-delay-vs-node.py"
python_script_process_generation="${python_script_path}/randomProcessGeneration.py"
python_script_process_runner="${python_script_path}/processRunner.py"
cd ${script_directory} || exit

# Step 4: Setting input Parameters
params["num_nodes_array"]=${num_nodes_array[@]} # These are default params
params["headway_array"]=${headway_array[@]}
params["position_model"]=$position_model
params["general_type"]=$general_type
params["general_rate"]=$general_rate
params["critical_type"]=$critical_type
params["distance_array"]=${distance_aray[@]} # Fishy



while [[ "$#" -gt 0 ]]; do
  case "$1" in
      --plot)
      plot=1
      ;;
      --*=*,*,*)
      handle_array "$1"
      ;;
    --*=*)
      handle_argument "$1"
      ;;
      *)
      echo "Invalid Param format: $1, exiting..."
      exit 1
  esac
  shift
done

# deleting the previously created inputs, outputs and plots

base_fileName="${fileName%.*}"
if [ $plot -ne 1 ]; then
  mkdir -p inputs
  cd inputs/
  rm -rf *
  cd ../

  mkdir -p outputs
  cd outputs/
  rm -rf enqueue_dequeue_trace*
  rm -rf "${base_fileName}"*
  cd ../

  mkdir -p plots
  cd plots/
  files_to_delete=$(find -type f -not -name "*save*")
  if [ -n "$files_to_delete" ]; then
    rm -rf $files_to_delete
  fi
  cd ../
fi

json_data="{"
for key in "${!params[@]}"; do
  json_data+="\"$key\":\"${params[$key]}\","
done
json_data="${json_data%,}"
json_data+="}"

# Generating, Running and analyzing data & Processes
if [ $plot -ne 1 ]; then 
  echo "Running File Generation Process"
  python3 "$python_script_process_generation" "$json_data"

  #TODO: testing of the script for Actual NS3 Process
  echo "Running the Actual ns3 process"
  python3 "$python_script_process_runner" "$fileName" "$json_data"

fi

echo "Running the Process for output & Plot extraction"
python3 "$python_script_mean_delay" "$json_data"
