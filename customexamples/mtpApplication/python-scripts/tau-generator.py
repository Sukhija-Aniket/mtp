import numpy as np
import random

def func_tn(v):
    log_num = (-d * (a + l)*(-2-np.sqrt(2))) - a*V(y)
    log_den = (d * (-2-np.sqrt(2)))**2
    val = -1/(d*(2+np.sqrt(2)))
    tn = val * np.log(log_num/log_den)
    return tn

def func_tcr(tn, mean=0.1, std_dev=0.01):
    k = random.gauss(mu=mean, sigma=std_dev)
    return k*tn

def d_bar():
    num = a * V_bar(y)
    den = a + l

    d_bar = num/den
    return d_bar

def V_inverse(x):
    pass

def V(y):
    V_0 * (np.tanh(y - ym/y_dash) + np.tanh(ym/y_dash))

def V_bar(y):
    pass


d=d_bar()
a, ym, y_dash = 5, 5, 10 # Reference [12]
x=25
l=[0, 2] # MOVM, FVD
y_star=V_inverse(x)
V_0 = 1





