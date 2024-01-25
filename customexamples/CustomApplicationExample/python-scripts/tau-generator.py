import numpy as np
import random

# Default values to use
a, ym, y_dash = 5, 5, 10 
x0=25 
l = 0
headways = np.arange(2,11,1) # Using headways to obtain values for t_cr for headway ranging from 2 to 10.


# tn --> ( -1 / ( (̃d) * (2 + root(2)) ) )  * ln[ ((̃d)(a + 𝑙)(−2 − root(2)) − 𝑎𝑉′(𝑦∗)) / ( (̃d) * (−2 − root(2)) )^2 ]
# tn is the communication delay
def get_tn(y, d):
    log_num = (-d * (a + l)*(-2-np.sqrt(2))) - a*V_bar(y)
    log_den = (d * (-2-np.sqrt(2)))**2
    val = -1/(d*(2+np.sqrt(2)))
    tn = val * np.log(log_num/log_den)
    return tn

# This function provides the value of critical delay, considering tn as only 10% of the delay
def get_tcr(tn, mean=0.1, std_dev=0.01):
    k = random.gauss(mu=mean, sigma=std_dev)
    return k*tn


# d_bar --> (̃d) = 𝑎𝑉′(𝑦∗)∕(𝑎 + 𝑙)
def get_d_bar(y):
    num = a * V_bar(y)
    den = a + l
    d_bar = num/den
    return d_bar


# Calculating value of Vo satisfying the equation --> 𝑦∗ = 𝑉^(-1)(𝑥`0)
def V_0(y):
    v0 = (x0 /( np.tanh((y - ym)/y_dash) + np.tanh(ym/y_dash) )) 
    return v0

def V_bar(y):
    val = (V_0(y) * ( 1 - (np.tanh((y - ym)/y_dash)**2) ) * (1/y_dash))
    return val

def get_rel(t_cr, t_tr, headway): # for linear reliability
    x1_ac0 = 0.0566
    x2_ac0 = 0.1277
    x1_ac1 = 0.0156
    x2_ac1 = 0.057
    if t_cr >= t_tr:
        rel_ac0 = 1 - np.exp((x1_ac0*headway + x2_ac0) * (t_cr - t_tr))
        rel_ac1 = 1 - np.exp((x1_ac1*headway + x2_ac1) * (t_cr - t_tr))
        return rel_ac0, rel_ac1
    else:
        return 0,0

# Main Program to get t_cr values
for i,x in enumerate(headways):
    d_bar=get_d_bar(x)
    tn = get_tn(x, d_bar)
    t_cr = get_tcr(tn)
    
    print(f"\nValue of tn: {tn}")
    print(f'values for headway {x} = {t_cr}\n')
    
    
# Main program to get reliability values
Phy_header_size = 42 # 42 bits
Mac_header_size = 112 # 112 bits
E_p = 500 * 8 # 500 bytes
R_b = 1 * 1024 * 1024 # 1Mbps
R_d = 3 * 1024 * 1024 # 3Mbps
delta = 0 # TODO fix it today
t_tr = Phy_header_size/R_b + (Mac_header_size + E_p)/R_d + delta
for i,x in enumerate(headways):
    d_bar = get_d_bar(x)
    tn = get_tn(x, d_bar)
    t_cr = get_tcr(tn)
    
    rel_ac0, rel_ac1 = get_rel(t_cr,t_tr, x)
    print(f"\nValue of headway {x}, t_cr: {t_cr}")
    print(f"Value for headway {x}, rel_ac0: {rel_ac0}, rel_ac1: {rel_ac1}\n")



    
    






