import random
import os
import sys
import numpy as np

'''
    --------------------------------------------README--------------------------------------------

    Python Script to calculate the mean delay of enqueue and dequeue in the MAC layer.

    # Param
        - Takes a file name (should be in "{relative path to ns3 folder}/ns-allinone-3.36.1/ns-3.36.1/scratch/mtp/customexamples/mtpApplication/outputs" directory) or a list of file names
    
    # Return
        - Returns the mean delay calculated
    
    The results (traces of packet for enqueue and dequeue with timestamps) will be written to a file in outputs folder

'''

cwd = os.getcwd()

input_path = os.path.join(os.path.dirname(cwd), "outputs")
output_file_path = os.path.join(input_path, "mean-delay-calculation.log")

context_map = {
    "enqueue": "/$ns3::WifiNetDevice/Mac/Txop/Queue/Enqueue",
    "dequeue": "/$ns3::WifiNetDevice/Mac/Txop/Queue/Dequeue"
}

def get_mean_mac_delay(fileName):
    input_file = os.path.join(input_path, fileName)
    if not os.path.isfile(input_file):
        raise FileNotFoundError(f"{fileName} doesn't exist in the {cwd} directory!!")
    
    mean_delay = 0
    counter = 0
    uid_enqueue = {}

    file_descriptor = os.open(output_file_path, os.O_WRONLY | os.O_CREAT, 0o644)

    if file_descriptor == -1:
        raise FileNotFoundError(f"{output_file_path} doesn't exist")

    with open(input_file, "r") as file:
        for line in file:
            attr = line.split(' ')
            uid = int(attr[0])
            context = attr[1]
            time = float(attr[2])
            if (context.endswith(context_map["dequeue"]) and uid_enqueue[uid]):
                os.write(file_descriptor, bytes(f"Dequeue time for uid {uid} is {time}ns \n", 'utf-8'))
                mean_delay = mean_delay + (time - uid_enqueue[uid])
                counter = counter + 1   
            elif(context.endswith(context_map["enqueue"])):
                os.write(file_descriptor, bytes(f"Enqueue time for uid {uid} is {time}ns \n", 'utf-8'))
                uid_enqueue[uid] = time
    

    if counter==0:
        return -1

    os.write(file_descriptor, bytes(f"Mean Delay: {mean_delay/counter}ns \n", 'utf-8'))
    os.close(file_descriptor)
    return mean_delay/counter

def main():
    if(len(sys.argv) < 2):
        raise TypeError("Insufficient arguments. At least one additional argument is required.")
    
    mean_delay = get_mean_mac_delay(sys.argv[1])
    return mean_delay


if __name__ == "__main__":
    try:
        x = main()
        print(x/1000000)
    except (TypeError, FileNotFoundError) as e:
        print(f"Error: {e}")
