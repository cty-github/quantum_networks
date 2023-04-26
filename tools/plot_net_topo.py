import networkx as nx
import matplotlib.pyplot as plt

graph = nx.Graph()
pos = {}
labels = {}

with open('./data/net_topo_example.txt') as file:
# with open('./tools/example.txt') as file:
    lines = file.readlines()
    for line in lines:
        elements = line.split()
        if len(elements) == 0:
            continue
        elif elements[0] == '#':
            continue
        elif elements[0] == 'node':
            graph.add_node(int(elements[2]))
            pos[int(elements[2])] = [float(elements[3]), float(elements[4])]
            labels[int(elements[2])] = f"{elements[2]}"
        elif elements[0] == 'edge':
            graph.add_edge(int(elements[1]), int(elements[2]))
        else:
            pass

print(graph.number_of_nodes())
print(graph.number_of_edges())

nx.draw_networkx_nodes(graph, pos, node_size=200)
nx.draw_networkx_edges(graph, pos)
nx.draw_networkx_labels(graph, pos, labels, font_size=10)
plt.show()
