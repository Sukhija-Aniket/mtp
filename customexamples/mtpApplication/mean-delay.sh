#!/bin/bash
my_array=(10 20 30 40 50 60 70 80 90 100) # Number of nodes to simulate on

fileName="${1:-static-node-delay-calculation.cc}" # default file is static-node-delay-calculaton.cc
if [[ ! "$fileName" =~ \.cc$ ]]; then
  echo "File name must have a .cc extension."
  exit 1
fi

# Calculating the area of topology
length=2000
width=2
area=$(expr $length \* $width)

# Define the desired paths
script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
python_script_path="${script_directory}/python-scripts"
python_script_mean_delay="${python_script_path}/mean-delay-vs-node.py"
python_script_process_generation="${python_script_path}/randomProcessGeneration.py"

# delete the previously created trace files
cd outputs/
rm enqueue_dequeue_trace*
rm static-node-delay-calc*
cd ..
# Back to directory where script is present

cd ${script_directory}

# Running the simulation for different nodes
for node in "${my_array[@]}"
do
    echo "-------------------------Number of Nodes: $node -----------------------------"
    echo "-------------------------Generating Random Processes-------------------------"
    # Script to generate the parameters of simulation using some distributions
    python3 $python_script_process_generation $node
    echo "-------------------------Running ${fileName} for ${node} nodes -----------------------------"
    # Run the simulations
    ../../../../ns3 run "$fileName --n=$node"
    echo "------------------------- Simulation successfully done for ${node} nodes -------------------------"
done


output_path="${script_directory}/outputs"

# Preparing arguments for plot script
last_element="${my_array[${#my_array[@]}-1]}"
flags="--plot"

cd ${script_directory}

echo "-------------------------Running the plot script ----------------------------------"
python3 $python_script_mean_delay $last_element $area $flags

echo "------------------------- Job Completed!! -----------------------------------------"
