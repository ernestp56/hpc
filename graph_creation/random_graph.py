import pandas as pd
import numpy as np
import csv
import matplotlib.pyplot as plt
import networkx as nx
import random

def graph():
    N = 10 ** 2  # number of individuals
    kave = 5  # expected number of partners
    G = nx.fast_gnp_random_graph(N, kave / (N - 1))  # Erdős-Rényi graph

    print("{} {}".format(G.number_of_nodes(), G.number_of_edges()))
    #for edge in G.edges():
    #    print("{} {}".format(edge[0] + 1, edge[1] + 1))
    for (u, v) in G.edges():
        G.edges[u,v]['weight'] = random.randint(0,100)
    
    nx.draw(G, node_size = 50)
    plt.savefig("filename.png")
    
    A = nx.adjacency_matrix(G)
    
    print(A.todense())
    
def matrix():
    # Size of the matrix
    n = 10 ** 4
    weights = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,1000000]
    probability = [1/len(weights)] * len(weights)
    G = np.zeros((n,n))
    for i in range(n):
        for j in range(n):
            if (i == j):
                G[i][j] = 1000000
            else:
                G[i][j] = random.choices(weights,probability, k=1)[0]
    file_name = ''.join(('random_graph',str(n),'.txt'))
    np.savetxt(file_name, G, fmt='%d')
    
matrix()