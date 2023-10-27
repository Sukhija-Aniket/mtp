import random
import os
import sys
import numpy as np
import matplotlib.pyplot as plt

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

cwd = os.getcwd()
parentDir = os.path.dirname(cwd)

area = int(sys.argv[2])

input_path = os.path.join(os.path.dirname(cwd), "outputs")
input_file_template = "static-node-delay-calc-n"

context_map = {
    "enqueue": "/$ns3::WifiNetDevice/Mac/Txop/Queue/Enqueue",
    "dequeue": "/$ns3::WifiNetDevice/Mac/Txop/Queue/Dequeue"
}

def get_mean_mac_delay(fileName, nodes=None):
    mean_delay = 0
    counter = 0
    uid_enqueue = {}

    input_file = os.path.join(input_path, fileName)
    if not os.path.isfile(input_file):
        raise FileNotFoundError(f"{fileName} doesn't exist in the {input_path} directory!!")
    
    output_file_path = os.path.join(input_path, "mean-delay-calculation.log")

    if not (nodes==None):
        output_file_path = os.path.join(input_path, f"enqueue_dequeue_trace_n{nodes}")

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
    if(len(sys.argv) < 3):
        raise TypeError("Insufficient arguments. At least one additional argument is required.")
    
    if(sys.argv[1].isdigit()):
        step = 10
        num_nodes = np.arange(step, int(sys.argv[1])+step, step)
        mean_delay = []
        for i in range(len(num_nodes)):
            input_file = input_file_template + str(num_nodes[i]) + ".log"
            mean_delay.append(round(get_mean_mac_delay(input_file, num_nodes[i])/1000000, 3))
        print(mean_delay)

        if(len(sys.argv)>=4 and sys.argv[3]=='--plot'):
            plt.plot(num_nodes, mean_delay)
            plt.scatter(num_nodes, mean_delay)
            plt.xlabel("Number of Nodes")
            plt.ylabel("Mean AMC delay (in ms)")
            plt.savefig(os.path.join(input_path, "mean-delay-vs-nodes.png"))
            plt.show()

    else:
        if(len(sys.argv)>=4 and sys.argv[3]=='--plot'):
            raise Exception("Invalid flag")
        
        mean_delay = get_mean_mac_delay(sys.argv[1])
        print(mean_delay)


if __name__ == "__main__":
    try:
        main()
    except (TypeError, FileNotFoundError) as e:
        print(f"Error: {e}")
