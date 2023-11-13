import json, os, sys, math

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


def getCriticalRate(num_nodes, json_data):
    position_model = str(json_data['position_model'])
    platoon_params = updateParams(num_nodes, json_data)
    if position_model.startswith('platoon'):
        rate = getPlatoonRate(platoon_params)
        return rate
    else:
        rate = json_data.get('critical_rate', 100)
        return rate

def updateParams(num_nodes, json_data):
    platoon_params = {
        'alpha': float(json_data.get('alpha', -1.933)),
        'gamma': float(json_data.get('gamma', 0.652)),
        'convergence_value': int(convert_nodes_to_headway(num_nodes, int(json_data.get('total_distance', 2000)))),
        'velocity_lead_node': int(json_data.get('velocity_lead_node', 25)),
        'tunable_param': int(json_data.get('tunable_param', 500)),
    }
    return platoon_params

def convert_to_cli(json_data, accepted_keys):
    cli_arguments = ""
    for key, value in json_data.items():
        if key in accepted_keys:
            cli_arguments += f' --{key}={value}'
    return cli_arguments 


def convert_headway_to_nodes(json_data):
    num_nodes = []
    dist = json_data['total_distance']
    position_model = str(json_data['position_model'])
    if position_model.startswith('platoon'):
        num_nodes = list(map(int, json_data['num_nodes_array'].split(' ')))
    else:
        data = json_data['headway_array']
        for x in data:
            nodes = int(dist/x + 1)
            num_nodes.append(nodes)
    
    return num_nodes

def convert_nodes_to_headway(num_nodes, dist=2000):
    num_nodes = int(num_nodes)
    num_nodes -= 1
    x = int(dist/num_nodes)
    return x