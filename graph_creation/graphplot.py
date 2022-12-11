import pandas as pd
import numpy as np
import csv
import matplotlib.pyplot as plt


index = ['50','100','150','200','250','300','350','400','450','500']
#index = ['500']
fig = plt.figure(figsize=(20,6))

for i in index:
    input_file = ''.join(("output",i))

    name_file = ''.join(("hpc4ds/output", input_file,".csv"))
    data = pd.read_csv(name_file, header=None, sep = " ")
    data.columns = ["Processors","Elements" ,"Nodes" ,"loc_n" ,"time_cost"]

    time_serial = data['time_cost'][data['Processors'] == 1][0]
    data['speedup'] = time_serial/data['time_cost']
    data['efficiency'] = data['speedup']/data['Processors']

    plt.subplot(1,3,1)
    plt.plot(data['Processors'], data['speedup'],marker='o', linestyle='dashed',linewidth=2, markersize=5)
    plt.xlabel("Processors")
    plt.ylabel("Speedup")
    plt.title('Speedup vs. number of processors')
    plt.legend(['50 nodes', '100 nodes', '150 nodes','200 nodes','250 nodes','300 nodes','350 nodes','400 nodes','450 nodes','500 nodes'])

    plt.subplot(1,3,2)
    plt.plot(data['Processors'], data['time_cost'],marker='o', linestyle='dashed',linewidth=2, markersize=5)
    plt.xlabel("Processors")
    plt.ylabel("Time cost")
    plt.title('Time cost vs. number of processors')
    plt.legend(['50 nodes', '100 nodes', '150 nodes','200 nodes','250 nodes','300 nodes','350 nodes','400 nodes','450 nodes','500 nodes'])

    plt.subplot(1,3,3)
    plt.plot(data['Processors'], data['efficiency'],marker='o', linestyle='dashed',linewidth=2, markersize=5)
    plt.xlabel("Processors")
    plt.ylabel("Efficiency")
    plt.title('Efficiency vs. number of processors')
    plt.legend(['50 nodes', '100 nodes', '150 nodes','200 nodes','250 nodes','300 nodes','350 nodes','400 nodes','450 nodes','500 nodes'])


#fig_file = ''.join(("./graph_output/", input_file,".png"))
plt.savefig("hpc4ds/graph_output/combined.png")
#plt.savefig(fig_file)