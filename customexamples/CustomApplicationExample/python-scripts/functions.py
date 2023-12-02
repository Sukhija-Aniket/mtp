import json, os, sys, math
import matplotlib.pyplot as plt
import random

def convert_to_json(json_string):
    try:
        json_data = json.loads(json_string)
        return json_data
    except json.JSONDecodeError as e:
        print(f"Error decoding JSON: {e}, exiting...")
        sys.exit(1)


def getPlatoonRate(params):
    # Get probablity that 2-wheelers(non-communicating nodes) will intercept the platoon
    alpha, gamma, y_star, x_dot0, p = params.get('alpha'), params.get('gamma'), params.get('convergence_value'), params.get('velocity_lead_node'), params.get('tunable_param')
    term = alpha + gamma*(y_star/x_dot0)
    prob = math.exp(term)/(1 + math.exp(term))
    critical_rate = round(float(p) * prob, 3)
    return critical_rate


def getCriticalRate(headway, json_data):
    position_model = str(json_data['position_model'])
    platoon_params = updateParams(headway, json_data)
    if position_model.startswith('platoon'):
        rate = getPlatoonRate(platoon_params)
        return rate
    else:
        rate = json_data.get('critical_rate', 100)
        return rate

def updateParams(headway, json_data):
    platoon_params = {
        'alpha': float(json_data.get('alpha', -1.933)),
        'gamma': float(json_data.get('gamma', 0.652)),
        'convergence_value': int(headway),
        'velocity_lead_node': int(json_data.get('velocity_lead_node', 120)),
        'tunable_param': int(json_data.get('tunable_param', 500)),
    }
    return platoon_params

def convert_to_cli(json_data, accepted_keys):
    cli_arguments = ""
    for key, value in json_data.items():
        if key in accepted_keys:
            cli_arguments += f' --{key}={value}'
    return cli_arguments

def get_array(str):
    arr = list(map(int, str.split(' ')))
    return arr

def Printlines(headway=None, nodes=None, distance=None):
    if headway and nodes:
        distance = (nodes-1)*headway
    elif headway and distance:
        nodes = int(distance/headway + 1)
    
    if headway:
        print(f'Running for headway={headway}, nodes={nodes}, distance={distance}')
    else:
        print(f'Running for nodes={nodes}, distance={distance}')


def convert_headway_to_nodes(json_data, distance=100):
    position_model =  str(json_data['position_model'])
    num_nodes = []
    headway = None
    dist = int(distance)
    position_model = str(json_data['position_model'])
    if position_model.startswith('platoon'):
        headway = list(map(int, json_data['headway_array'].split(' ')))
        for x in headway:
            nodes = int(dist/x + 1)
            num_nodes.append(nodes)
    else:
        num_nodes = list(map(int, json_data['num_nodes_array'].split(' ')))

    return num_nodes, headway

def plot_figure(data_map, row, col, xvalue, xlabel, plot_path=None, distance=100):
    fontsize = 6
    col = int(col)
    plt.figure(figsize=(50, 20))
    f, ax = plt.subplots(len(row), col, gridspec_kw={'hspace':0.3, 'wspace':0.3})
    for i,x in enumerate(row):
        cnt= 0
        for key, values in data_map.items():
            if key.startswith(x):
                ax[i][cnt].plot(xvalue, values, label=str(key.split('_')[1]))
                ax[i][cnt].scatter(xvalue, values)
                ax[i][cnt].legend(fontsize=fontsize)
                ax[len(row)-1][cnt].set_xlabel(xlabel,fontsize=fontsize)
                cnt += 1
                ax[len(row)-1][cnt].set_xlabel(xlabel=xlabel, fontsize=fontsize)

        for key, values in data_map.items():
            if key.startswith(x):
                label = str(key.split('_')[1])
                ax[i][col-1].plot(xvalue, values, label=label)
                ax[i][col-1].scatter(xvalue, values)
        ax[i][0].set_ylabel(f"{x} mac delays (in ms)", fontsize=fontsize)
        ax[i][col-1].legend(fontsize=fontsize)
    for i in range(len(row)):
        for j in range(col):
            ax[i][j].tick_params(axis='x', labelsize=fontsize)
            ax[i][j].tick_params(axis='y', labelsize=fontsize)

    plt.savefig(os.path.join(plot_path, f"mtp-plot-mac-delay-{distance}.png"))

def plot_figure_solo(data_map, row, col, xvalue, xlabel, plot_path=None, distance=100):
    fontsize = 6
    for key, values in data_map.items():      
        plt.figure()      
        plt.plot(xvalue, values, label=str(key.split('_')[1]))
        plt.scatter(xvalue, values)
        plt.legend(fontsize = fontsize)
        plt.xlabel(xlabel)
        plt.ylabel(f"{key.split('_')[0]} mac delays (in ms)")
        plt.savefig(os.path.join(plot_path, f"mtp-plot-mac-delay-{key}-{distance}"))

    for x in row:
        plt.figure()
        for key, values in data_map.items():
            key = str(key)
            if key.startswith(x):
                plt.plot(xvalue, values, label=str(key.split('_')[1]))
                plt.scatter(xvalue, values)
                plt.legend(fontsize = fontsize)
                plt.xlabel(xlabel)
                plt.ylabel(f"{key.split('_')[0]} mac delays (in ms)")
        plt.savefig(os.path.join(plot_path, f"mtp-plot-mac-delay-{x}-{distance}"))



def func_tcr(tn, mean=0.1, std_dev=0.01):
    k = random.gauss(mu=mean, sigma=std_dev)
    return k*tn
