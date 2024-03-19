#!/bin/bash

# Declarations and variables
declare -A params
# TODO: Uncomment when running again!
num_nodes_array=(50 100 150 200 250 300 350 400 450 500) # Number of nodes to simulate on
data_rate_array=(3 4.5 6 9 12 18 24 27)  # Data Rate as per OFDM standards for 10MHz channel width (fixed don't change)
packet_size_array=(50 100 200 400 800 1600) # Packet Size in bytes
critical_rate_array=(10 15 20 25 30 35 40 45 50 55)
general_rate_array=(10 15 20 25 30 35 40 45 50 55)
headway_array=(2 3 4 5 6 7 8 9 10) # Distance between two consecutive nodes to simulate on
distance_array=(100) # Total Distance to consider

position_model='platoon-distance'
num_nodes=100
general_type='constant'
critical_type='poisson'
general_rate=30
critical_rate=30
data_rate=27
packet_size=500
velocity_lead_node=25
bd=0
plot=0

# Functions
print_usage() {
  echo "Usage: $(basename "$0") FILENAME [OPTIONS]"
  echo "Options:"
  echo -e "\t--help                                                  Displays this help message"
  echo -e "\t--plot                                                  Runs only code for obtaining plots"
  echo -e "\t--general_rate=VALUE (default=30)                       Specify rate of routine/general packets"
  echo -e "\t--time=VALUE (default=10)                                  Specify time in seconds for simulation to  run"
  echo -e "\t--general_type={poisson, constant, gaussian, default=constant}    Specify  distribution for generating general packets"
  echo -e "\t--critical_type={poisson, constant, gaussian, default=poisson}    Specify distribution for generating critical packets"
  echo -e "\t--general_rate={poisson, constant, gaussian, default=30}"
  echo -e "\t--critical_rate={poisson, constant, gaussian, default=30}"
  echo -e "\t--position_model={platoon-nodes, platoon-distance, uniform-distance, default=platoon-distance}    Specify position model used during simulation"
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
    echo "File '$fileName' could not be found, exiting..."  >&2
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
  print_usage
  exit 1
fi

if [[ "$#" -eq 1 && "$1" == "--help" ]]; then
  print_usage
  exit 0
fi

if [[ ! "$fileName" =~ \.cc$ ]]; then
  echo "File name must have a .cc extension, exiting..."
  exit 1
else
  BASENAME=${fileName%.cc}
  echo $BASENAME
  if [[ $BASENAME == *bd ]]; then
    position_model='uniform-distance'
    distance_array=(700)
    critical_type='constant'
    bd=1
    echo "bd file provided so changing default values"
  fi
fi
shift

# Step 2: Setting input Parameters
params["distance_array"]=${distance_array[@]} # Fishy
params["headway_array"]=${headway_array[@]}
params["num_nodes_array"]=${num_nodes_array[@]} # These are default params
params["critical_rate_array"]=${critical_rate_array[@]}
params["general_rate_array"]=${general_rate_array[@]}
params["data_rate_array"]=${data_rate_array[@]}
params["packet_size_array"]=${packet_size_array[@]}

params["position_model"]=$position_model
params["num_nodes"]=$num_nodes
params["critical_type"]=$critical_type
params["general_type"]=$general_type
params["critical_rate"]=$critical_rate
params["general_rate"]=$general_rate
params["data_rate"]=$data_rate
params["packet_size"]=$packet_size
params["bd"]=$bd


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

# Step 2: Clearing old outputs
touch nohup.out
echo -n > nohup.out

# Step 3: Moving to base Directory
base_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
script_directory=$(get_script_dir "$fileName")
if [[ -n "${script_directory}" ]]; then
    cd ${script_directory}
else
  exit
fi

file_path="${script_directory}/${fileName}"
python_script_path="${base_directory}/python-scripts"
python_script_mean_delay="${python_script_path}/mean-delay-vs-node.py"
python_script_process_generation="${python_script_path}/randomProcessGeneration.py"
python_script_process_runner="${python_script_path}/processRunner.py"
python_script_analysis="${python_script_path}/analysis.py"

# deleting the previously created inputs, outputs and plots

base_fileName="${fileName%.*}"
if [ $plot -ne 1 ]; then
  mkdir -p inputs
  cd inputs/
  # rm -rf *
  cd ../

  mkdir -p outputs
  cd outputs/
  # rm -rf *
  cd ../

  mkdir -p plots
  cd plots/
  files_to_delete=$(find -type f -not -name "*save*")
  if [ -n "$files_to_delete" ]; then
    rm -rf $files_to_delete
  fi
  cd ../

  mkdir -p pcaps
  cd pcaps/
  # rm -rf *
  cd ../

  mkdir -p practical
  cd practical/
  # rm -rf *
  cd ../
fi

json_data="{"
for key in "${!params[@]}"; do
  json_data+="\"$key\":\"${params[$key]}\","
done
json_data="${json_data%,}"
json_data+="}"

# echo ${json_data}

# Generating, Running and analyzing data & Processes (moving to base directory)
cd $base_directory
if [ $plot -ne 1 ]; then
  echo "Running File Generation Process"
  python3 "$python_script_process_generation" "$file_path" "$json_data"

  # testing of the script for Actual NS3 Process
  echo "Running the Actual ns3 process"
  python3 "$python_script_process_runner" "$file_path" "$json_data"

fi

echo "Running the Process for output & Plot extraction"
if [ $bd -ne 1 ]; then
  echo "error in bash"
  python3 "$python_script_mean_delay" "$file_path" "$json_data"
else
  echo "$json_data" 
  python3 "$python_script_analysis" "$file_path" "$json_data"
fi
