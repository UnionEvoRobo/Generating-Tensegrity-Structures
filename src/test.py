from node import Node

node1=Node(1)
node2=Node(2)
node3=Node(3)
node2.set_other(node3)
node3.set_other(node2)
node1.set_other(node2)
node2.set_other(node1)
try:
    print(node2.get_other().get_label())
except AttributeError:
    print(None)
try:
    print(node1.get_other().get_label())
except AttributeError:
    print(None)
try:
    print(node3.get_other().get_label())
except AttributeError:
    print(None)
