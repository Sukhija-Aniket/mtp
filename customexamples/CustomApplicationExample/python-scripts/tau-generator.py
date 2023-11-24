import numpy as np
import random

a, ym, y_dash = 5, 50, 100 # Reference [12]
x=25
l = 0

values = [
    0.004777,
    0.005130,
    0.004951,
    0.004368,
    0.004375,
    0.004246,
    0.003597,
    0.003182,
    0.002529,
    0.001498,
    0.000671
]


headways = np.arange(20,110,10)

def func_tn_simp(y,d):
    log_num = 5*(1+np.sqrt(2))
    log_den = V_bar(y) * ((2+np.sqrt(2))**2)
    val = -1/(d*(2+np.sqrt(2)))
    tn = val * np.log(log_num/log_den)
    return tn

def func_tn(y, d):
    log_num = (-d * (a + l)*(-2-np.sqrt(2))) - a*V_bar(y)
    log_den = (d * (-2-np.sqrt(2)))**2
    val = -1/(d*(2+np.sqrt(2)))
    tn = val * np.log(log_num/log_den)
    return tn

def func_tcr(tn, mean=0.1, std_dev=0.01):
    k = random.gauss(mu=mean, sigma=std_dev)
    return k*tn

def d_bar(y):
    num = a * V_bar(y)
    den = a + l
    d_bar = num/den
    return d_bar

# def V(y):
#     val = ((y_dash/y) * (np.tanh((y - ym)/y_dash) + np.tanh(ym/y_dash)))
#     return val
    

def V_0(y):
    return (x/(np.tanh((y - ym)/y_dash) + np.tanh(ym/y_dash))) 

def V_bar(y):
    val = (V_0(y) * (1-(np.tanh((y - ym)/y_dash)**2)) * 1/y_dash)
    return val
    


arr = []
for i,x in enumerate(headways):
    d=d_bar(x)
    tn_simp = func_tn_simp(x, d)
    tn = func_tn(x,d)
    print(tn, tn_simp)
    t_cr = func_tcr(tn)
    print(f'values for headway {x} = {t_cr}, {values[i]}')
    temp = values[i]/t_cr
    arr.append(temp)

for x in arr:
    print(x)

print(np.mean(arr))
    
    






