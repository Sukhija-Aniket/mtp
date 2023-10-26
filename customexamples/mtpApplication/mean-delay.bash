#!/bin/bash

# Create an array of integers
my_array=(10)

length=2000
width=2
area=$(expr $length \* $width)

pwd="$(pwd)"

simulation_file="/home/ashu3103/ns-allinone-3.36.1/ns-3.36.1/scratch/mtp/customexamples/mtpApplication/abc.cc"
python_script_path="$pwd/python-scripts"
python_script_file="$python_script_path/mean-delay-vs-node.py"
output_path="$pwd/outputs"

# Set the length of the random string you want
length=10
# Generate random bytes from /dev/urandom and format them
plot_file="$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $length | head -n 1)"

# Create a randomly new file in outputs directory
# cd ./outputs
# touch $plot_file
# cd ..

# Get to the ns3 directory
cd ../../../../
cd ./

echo $(pwd)
# for nNode in "${my_array[@]}"; do
#     # Run static simulation file
#     echo "Starting the simulation for mean-delay calculation for $nNode nodes"

#     outfile="static-node-delay-calc-n$nNode"
#     outfileLog="$outfile.log"
#     executable="\"$simulation_file"

#     ./ns3 run $simulation_file

#     # Get the mean delay
#     # result=$(python3 $python_script_file $outfileLog)
#     # echo "Mean Delay for $nNode nodes is $result ms"
# done

./ns3 run $simulation_file
# Delete the randomly generated new file in outputs directory
# cd ./outputs
# rm $plot_file
# cd ..

# Access
#  and print individual elements
# echo "First element: ${my_array[0]}"
# echo "Second element: ${my_array[1]}"
# echo "Third element: ${my_array[2]}"

# Loop through and print all elements
# for element in "${my_array[@]}"; do
#   echo "Element: $element"
# done