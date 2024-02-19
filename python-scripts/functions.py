import json, os, sys, math
import matplotlib.pyplot as plt
import random
import numpy as np
from constants import *

# Code for obtaining theoratical Reliability Values

def get_rel(t_cr, headway):
    x1_ac0 = 0.0566
    x2_ac0 = 0.1277
    x1_ac1 = 0.0156
    x2_ac1 = 0.057
    print(f"t_tr: {TRANSMISSION_TIME}, t_cr: {t_cr}, expo: {-1*(x1_ac0*headway + x2_ac0) * (t_cr - TRANSMISSION_TIME) * 1000}")
    if t_cr >= TRANSMISSION_TIME:
        rel_ac0 = 1 - np.exp(-1* (x1_ac0*headway + x2_ac0) * (t_cr - TRANSMISSION_TIME) * 1000) #Note: values must be in (ms)
        rel_ac1 = 1 - np.exp(-1* (x1_ac1*headway + x2_ac1) * (t_cr - TRANSMISSION_TIME) * 1000)
        return rel_ac0, rel_ac1 # TODO change it later
    else:
        return 0,0

# Code for Obtaining the communication and Critical Delays

# tn --> ( -1 / ( (̃d) * (2 + root(2)) ) )  * ln[ ((̃d)(a + 𝑙)(−2 − root(2)) − 𝑎𝑉′(𝑦∗)) / ( (̃d) * (−2 − root(2)) )^2 ]
# tn is the communication delay
def get_tn(y, d, a, l, x0, y_dash, ym):
    log_num = (-d * (a + l)*(-2-np.sqrt(2))) - a*V_bar(y, x0, y_dash, ym)
    log_den = (d * (-2-np.sqrt(2)))**2
    val = -1/(d*(2+np.sqrt(2)))
    tn = val * np.log(log_num/log_den)
    return tn

# This function provides the value of critical delay, considering tn as only 10% of the delay
def get_t_cr(tn, mean=0.1, std_dev=0.01):
    k = random.gauss(mu=mean, sigma=std_dev)
    return k*tn


# d_bar --> (̃d) = 𝑎𝑉′(𝑦∗)∕(𝑎 + 𝑙)
def get_d_bar(y, a, l, x0, y_dash, ym):
    num = a * V_bar(y, x0, y_dash, ym)
    den = a + l
    d_bar = num/den
    return d_bar

# Calculating value of Vo satisfying the equation --> 𝑦∗ = 𝑉^(-1)(𝑥`0)
def V_0(y, x0, y_dash, ym):
    v0 = (x0 /( np.tanh((y - ym)/y_dash) + np.tanh(ym/y_dash) )) 
    return v0

def V_bar(y, x0, y_dash, ym):
    val = (V_0(y, x0, y_dash, ym) * ( 1 - (np.tanh((y - ym)/y_dash)**2) ) * (1/y_dash))
    return val

def get_tcr(headway, a=5, l=0, x0=25, y_dash=10, ym=5):
    d_bar = get_d_bar(headway, a, l, x0, y_dash, ym)
    tn = get_tn(headway, d_bar, a, l, x0, y_dash, ym)
    t_cr = get_t_cr(tn)
    return t_cr


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

def get_float_array(str):
    arr = list(map(float, str.split(' ')))
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

def PrintlinesBD(arr):
    if(len(arr)==5):
        print(f'Running for nodes={arr[0]}, dataRate={arr[1]}Mbps, pktSize={arr[2]}bytes, lamda0={arr[3]}pkt/seconds, lamda1={arr[4]}pkt/seconds')
    else:
        print(f'Insufficient Arguments')


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


# Code for testing the functions
def test():
    headways = np.arange(2,11,1)
    pts = []
    for headway in headways:
        pts.append(get_rel(get_tcr(headway), headway))
        
    plt.plot(headways, pts)
    plt.xticks(headways)
    plt.savefig(os.path.join(os.path.dirname(__file__), 'test.png'))
    
# test()