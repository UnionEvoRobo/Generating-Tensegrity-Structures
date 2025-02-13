"""Graph class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 6.0
"""

import random
import networkx as nx
import networkx.algorithms.approximation as appr
from l_system import LSystem
from transformer import Transformer
from node import Node
from edge import Edge
import graphviz  # doctest: +NO_EXE


class Graph:
    """Graph class for the generation of complex tensegrity structures."""

    def __init__(self, rules, edge_types, bars):
        self.dot = graphviz.Digraph(comment='Tensegrity Object Graph')
        self.edge_types = edge_types
        self.rules = rules
        self.transformer = Transformer(self)
        self.edge_list = []
        self.protected_nodes=[]
        self.node_list = []
        self.bar_num=bars
        self.bracket_nodes = []
        self.node_number = 0
        self.mut=False

    def get_rule(self, key:LSystem):
        """Getter for individual rules in the rule dictionary

        Args:
            key (string): string containing the dictionary
            key for the desired rule

        Returns:
            string: string containing the desired rule
        """
        return self.rules.get_rule(key)

    def get_edge_types(self):
        """Getter for the list of different edge types

        Returns:
            list: list containing strings of the different
            possible edge types
        """
        return self.edge_types

    def get_nodes(self):
        """Getter for the list of the graphs nodes

        Returns:
            list: list containing all nodes in the graph
        """
        return self.node_list

    def get_edges(self):
        """Getter for the list of the graphs edges

        Returns:
            list: list containing all edges in the graph
        """
        return self.edge_list

    def order(self):
        """Gives the order, or number of nodes, of the graph

        Returns:
            int: number of nodes in the graph
        """
        return len(self.node_list)

    def size(self):
        """Gives the size, or number of edges, of the graph

        Returns:
            int: number of edges in the graph
        """
        return len(self.edge_list)

    def create_graph(self):
        """Generates a dictionary representing the initial graph to be
        fed into the transformer and then runs the dictionary (edge_dict)
        through the transformer however many times is desired.
        """
        while self.node_number < 4:
            self.add_node(None)
        self.add_edge("A", self.node_list[0], self.node_list[1])
        self.add_edge("B", self.node_list[1], self.node_list[2])
        self.add_edge("C", self.node_list[2], self.node_list[3])
        self.add_edge("D", self.node_list[3], self.node_list[0])
        self.add_edge("E", self.node_list[1], self.node_list[3])

    def transform(self, size, print_intermed, graph_num, def_l):
        """Passes the graph through the transformer a number of times

        Args:
            num_trans (int): the desired number of passes through
            the transformer
        """
        transformations = 0
        marg=.25
        while transformations < (size):
            self.transformer.transform()
            if print_intermed and self.rules.rule_dict!=def_l:
                self.draw_graph(graph_num+marg)
            marg+=.25
            transformations += 1
        self.generate_bracket_edges()


    def swap_rule(self, key, new_rule):
        """Changes the definition of the rule at the indicated key

        Args:
            key (string): string containing the dictionary
            key for the rule that will be changed

            new_rule (string): string containing the new rule
        """
        self.rules[key] = new_rule

    def add_node(self, bracket):
        """Creates and adds a new node to the graph

        Args:
            bracket (string): string indicating what
            the bracket property will be

        Returns:
            Node: returns the newly created node
        """
        node = Node(self.node_number)
        self.node_number += 1
        self.node_list.append(node)
        node.set_bracket(bracket)
        if node.get_bracket() is not None:
            self.bracket_nodes.append(node)
        return node

    def add_edge(self, label, start, end):
        """Creates and adds a new edge to the graph

        Args:
            label (string): _description_
            start (Node): the origin node of the edge
            end (Node): the destination node for the edge
        """
        edge = Edge(label, start, end)
        self.edge_list.append(edge)
        if start is not None:
            start.add_edge(end)
        if end is not None:
            end.add_edge(start)

    def generate_bracket_edges(self):
        """Creates new connecting edges between
            nodes with matching bracket types
        """
        for node1 in self.bracket_nodes:
            for node2 in self.bracket_nodes:
                if node1 != node2 and self.bracket_nodes.index(
                        node1) < self.bracket_nodes.index(
                            node2) and node1.get_bracket(
                            ) == node2.get_bracket():
                    edge = Edge(node1.get_bracket(), node1, node2)
                    if edge.is_unique(self.edge_list):
                        self.edge_list.append(edge)
                        node1.add_edge(node2)
                        node2.add_edge(node1)

    def adjacent(self, node1:Node, node2:Node):
        """Determines whether two nodes in the graph have an
        edge connecting them (are adjacent).

        Args:
            node1 (Node): One of the two nodes being checked
            node2 (Node): One of the two nodes being checked

        Returns:
            boolean: True if the two nodes are adjacent and false if not
        """
        return node1.is_adjacent(node2)

    def neighbors(self, node1):
        """Return a list of all nodes adjacent to node1.

        Args:
            node1 (Node): The node whose list of
            adjacent nodes is desired

        Returns:
            list: list of all adjacent nodes
        """
        return node1.get_adjacent()

    def remove_node(self, node1:Node):
        """Removes a node from the graph.

        Args:
            node1 (Node): The node that is being removed
        """
        if node1 not in self.protected_nodes:
            adj=node1.get_adjacent()
            for node2 in adj:
                self.remove_edge1(node1,node2)
            self.node_list.remove(node1)

    def remove_edge1(self, start_node, end_node):
        """Removes an edge from the graph

        Args:
            start_node (Node): The starting node of the desired edge
            end_node (Node): The end node of the desired edge

        Returns:
            boolean: True if the edge was deleted and false if not
        """
        for edge in self.edge_list:
            if (edge.contains(start_node) or start_node is None) and (edge.contains(end_node) or end_node is None):
                self.edge_list.remove(edge)
                if start_node is not None and end_node is not None:
                    start_node.remove_edge(end_node)
                    end_node.remove_edge(start_node)
                return True
        return False

    def remove_edge2(self, edge:Edge):
        """Removes an edge from the graph

        Args:
            start_node (Node): The starting node of the desired edge
            end_node (Node): The end node of the desired edge

        Returns:
            boolean: True if the edge was deleted and false if not
        """
        self.edge_list.remove(edge)
        edge.get_start().remove_edge(edge.get_end())
        edge.get_end().remove_edge(edge.get_start())

    def reassign_edge(self, edge:Edge, new_node:Node, side):
        if side>0:
            if not self.adjacent(new_node,edge.get_end()):
                edge.get_end().remove_edge(edge.get_start())
                edge.set_start(new_node)
                new_node.add_edge(edge.get_end())
                edge.get_end().add_edge(new_node)
            else:
                self.remove_edge2(edge)
        elif side<0:
            if not self.adjacent(new_node,edge.get_start()):
                edge.get_start().remove_edge(edge.get_end())
                edge.set_end(new_node)
                new_node.add_edge(edge.get_start())
                edge.get_start().add_edge(new_node)
            else:
                self.remove_edge2(edge)

    def clear_nodes(self):
        """Resets the degree of all nodes in the graph.
        """
        for node in self.node_list:
            node.clear_deg()

    def get_edge(self, node1, node2):
        """Getter for a specific edge in the graph

        Args:
            node1 (Node): One of the nodes that the desired edge must contain
            node2 (Node): One of the nodes that the desired edge must contain

        Returns:
            Edge: the edge in the graph containing both of the specified nodes.
            If not found, returns None
        """
        for i in self.edge_list:
            if i.contains(node1) and i.contains(node2):
                return i
        return None

    def make_tr3(self):
        """Create the graph of a starter 3 bar tensegrity"""
        while self.node_number < 6:
            self.add_node(None)

        self.add_edge(3, self.node_list[0], self.node_list[1])
        self.add_edge(3, self.node_list[1], self.node_list[2])
        self.add_edge(3, self.node_list[2], self.node_list[0])
        self.add_edge(1, self.node_list[3], self.node_list[4])
        self.add_edge(3, self.node_list[4], self.node_list[5])
        self.add_edge(5, self.node_list[5], self.node_list[3])
        self.add_edge(2, self.node_list[4], self.node_list[0])
        self.add_edge(4, self.node_list[5], self.node_list[1])
        self.add_edge(1, self.node_list[3], self.node_list[2])

    def make_tr4(self):
        """Create the graph of a starter 3 bar tensegrity"""
        while self.node_number < 8:
            self.add_node(None)

        self.add_edge('1', self.node_list[0], self.node_list[1])
        self.add_edge('3', self.node_list[1], self.node_list[2])
        self.add_edge('5', self.node_list[2], self.node_list[3])
        self.add_edge('7', self.node_list[3], self.node_list[0])
        self.add_edge('1', self.node_list[4], self.node_list[5])
        self.add_edge('1', self.node_list[5], self.node_list[6])
        self.add_edge('1', self.node_list[6], self.node_list[7])
        self.add_edge('1', self.node_list[7], self.node_list[4])
        self.add_edge('4', self.node_list[0], self.node_list[5])
        self.add_edge('1', self.node_list[0], self.node_list[6])
        self.add_edge('6', self.node_list[1], self.node_list[6])
        self.add_edge('1', self.node_list[1], self.node_list[7])
        self.add_edge('7', self.node_list[2], self.node_list[7])
        self.add_edge('1', self.node_list[2], self.node_list[4])
        self.add_edge('2', self.node_list[3], self.node_list[4])
        self.add_edge('1', self.node_list[3], self.node_list[5])

    def grow_while_connecting_rods(self, size, print_intermed, graph_num, def_l):
        #(self, size, print_intermed)
        """Grows an initial graph using the indicated l_system and size parameters

        Args:
            size (int): Number of iterations through the transformer
            print_intermed (boolean): Whether or not the method should
                                    print intermediate stages of graph growth

        Returns:
            boolean: boolean indicating whether or not the graph is growing
        """
        growing = 1

        print("growing...")
        oldnodes = self.order()
        oldedges = self.size()
        self.transform(size,graph_num,print_intermed,def_l)
        self.generate_bracket_edges()
        curnodes = self.order()
        curedges = self.size()
        if (oldnodes == curnodes and oldedges == curedges):
            growing = 0

        return growing

    def simplify_graph(self):
        """Removes all nodes that have less than 3 edges
        beginning and ending at its location.
        """

        ####    SEE IF THIS CAN BE IMPROVED!!!!!   ####

        loop = True
        while loop and len(self.node_list) != 0:
            goodbye_edges = []
            goodbye_nodes = []
            loop = False
            for node in self.node_list:
                if node.is_extraneous() and node not in self.protected_nodes:
                    loop = True
                    goodbye_nodes.append(node)
                    for edge in self.edge_list:
                        if edge.contains(node) and goodbye_edges.count(
                                edge) == 0:
                            goodbye_edges.append(edge)
            edge:Edge
            for edge in self.edge_list:
                if edge.get_start() is None or edge.get_end() is None:
                    if edge in goodbye_edges:
                        goodbye_edges.remove(edge)
                    self.remove_edge1(edge.get_start(), edge.get_end())
            for i in goodbye_nodes:
                self.remove_node(i)
            for i in goodbye_edges:
                self.remove_edge1(i.get_start(), i.get_end())

    def find_random_string_labels(self):
        """Reassigns the lables of strings in the graph
        """
        for i in self.node_list:
            self.assign_random_label_for_edges_to_node(i)

    def assign_random_label_for_edges_to_node(self, node):
        """Reassigns the lables of strings connected to the given node.
        """
        edges = self.get_node_edges(node)
        if edges is not None:
            stay_in = True
            for e in edges:
                if e.get_label() == node.get_label():
                    # rand_edge.change_label("-1")
                    i=random.randint(-1, (len(self.edge_types) - 1))
                    e.change_label(self.edge_types[i])
                    self.mut=True

            tries = 0
            while stay_in and tries <= 5 * len(edges):
                tries += 1
                rand_ex = random.randint(0, (len(edges) - 1))
                rand_edge = edges[rand_ex]
                if int(rand_edge.get_label() == "-1"):
                    # rand_edge.change_label("-1")
                    i=random.randint(0, (len(self.edge_types) - 1))
                    rand_edge.change_label(self.edge_types[i])
                    self.mut=True
                    stay_in = False

    def get_node_edges(self, node):
        """Get the list of all edges containing the given node

        Args:
            node (Node): the node being inspected

        Returns:
            list: the list of all edges in graph that contains the given node
        """
        ret = []
        if node != []:
            for i in self.neighbors(node):
                edge = self.get_edge(node, i)
                if edge is not None:
                    ret.append(edge)
            if not ret:
                return None
            return ret

    def get_string_labels(self):
        """Return a list of the labels of all edges
        """
        ret = []
        i:Edge
        for i in self.edge_list:
            ret.append(i.get_label())
        return ret

    def mutate_string_labels(self):
        """Change the label of a string in the graph at random
        """
        randnode = random.randint(0, (len(self.node_list) - 1))
        self.assign_random_label_for_edges_to_node(self.node_list[randnode])

    def find_matching_pairs_recursively(self):
        """Find pairs of nodes with matching bracket labels

        Returns:
            list: list of paired nodes
        """
        nodes = self.node_list
        all_pairs = []
        for i,inode in enumerate(nodes):
            my_matches = []
            for jnode in nodes:
                if inode!=jnode:
                    if not self.adjacent(inode, jnode):
                        my_matches.append(jnode)
            all_pairs.append(my_matches)
        found_matches = []

        if len(all_pairs) != len(nodes):
            print(
                f"uh oh! different allPairs ({len(all_pairs)}) than nodes ({len(nodes)})!"
            )
        found_matches = self.recurse_find_pairs(0, all_pairs, found_matches)
        for i in found_matches:
            for j in i:
                self.protected_nodes.append(j)
        self.purify_graph(found_matches)
        return found_matches

    def purify_graph(self,found):
        i=0
        while i<len(self.node_list):
            # if not self.node_list[i].is_matched():
            if self.node_list[i] not in self.protected_nodes:
                self.remove_node(self.node_list[i])
            else:
                i+=1
        i=0
        self.smallest_node()
        while i<len(self.edge_list):
            edge:Edge=self.edge_list[i]
            if (edge.get_end() not in self.node_list) and (edge.get_start() not in self.node_list):
                    self.remove_edge1(edge.get_start(),edge.get_end())
            elif (edge.get_end() not in self.node_list):
                    self.reassign_edge(edge, self.smallest_node(),-1)
            elif (edge.get_start() not in self.node_list):
                    self.reassign_edge(edge, self.smallest_node(),1)
            else:
                i+=1
        while len(found)>self.bar_num:
            popped=found.pop()
            for x in popped:
                self.remove_node(x)
                self.simplify_graph()
        self.smallest_node()

    def smallest_node(self):
        nodes=self.node_list
        n = len(nodes)
        for i in range(n - 1):
            min_idx = i
            for j in range(i + 1, n):
                if nodes[j].get_degree() < nodes[min_idx].get_degree():
                    min_idx = j
        return nodes[min_idx]



    def recurse_find_pairs(self, curindex:int, all_pairs:list, matches:list):
        """Find pairs of nodes with matching bracket labels

        Args:
            curindex (int): current recursion index
            allPairs (list): list of all known pairs
            matches (list): list of found matches

        Returns:
            list: list of paired nodes
        """
        nodes=self.node_list
        if curindex==len(all_pairs):
            return matches
        else:
            my_pairs:list=all_pairs[curindex]
            temp_matches=[]
            for j in matches:
                temp_matches.append(j)
            i:Node
            # my_pairs.reverse()
            for i in my_pairs:
                if not nodes[curindex].is_matched():
                    match=i
                    if not match.is_matched():
                        match.matched=True
                        nodes[curindex].matched=True
                        temp_matches.append((match, nodes[curindex]))
                        found_matches=self.recurse_find_pairs(curindex+1,all_pairs,temp_matches)
                        return found_matches
            return self.recurse_find_pairs(curindex+1,all_pairs,temp_matches)

    def grow_node_by_node_until_size(self, size):
        """Grows graph until the graph is
        of a specified order of good nodes

        Args:
            size (int): desired order of the graph

        Returns:
            int: indicates whether the graph was successfully grown
        """
        notgrowing = 0
        iters = 0
        stay = True
        while stay:
            oldnodes = self.order()
            oldedges = self.size()
            self.transformer.transform()
            curnodes = self.order()
            curedges = self.size()
            if (oldnodes == curnodes and oldedges == curedges):
                notgrowing = 1
            iters += 1
            if (self.number_of_good_nodes()
                    >= size) or (notgrowing == 1) or (self.order() >= 80):
                stay = False
        self.generate_bracket_edges()
        self.simplify_graph()
        if notgrowing:
            return -1

    def number_of_good_nodes(self):
        """Find how many nodes have a degree of three or more

        Returns:
            int: number of good nodes
        """
        count = 0
        for i in self.node_list:
            if i.get_degree() > 2:
                count += 1
        return count

        #here we're trying to find pairs of nodes to turn into elements

    def match_nodes(self):
        """Matches nodes with other nodes with a similar bracket label
        """
        unhappy = []
        unmatched = []
        now_unmatched = []
        tries = 0
        unmatched_are_the_same = 1
        stay = True
        while stay:
            print("matching nodes..")
            tries += 1
            for i in range(self.order()):
                if self.node_list[i].get_other is None:
                    unmatched.append(self.node_list[i])
                if self.node_list[i].happy() == 0:
                    unhappy.append(self.node_list[i])
            #search through unhappy nodes
            for n in unhappy:
                # only want to match 3-connected nodes
                # which are "unhappy"
                if ((n.get_degree() >= 3) and (n.get_other() is None)):
                    found = 0
                    #first see if you have a perfect match
                    for j in unhappy:
                        if ((i != j) and (n.get_bracket() == j.get_bracket())
                                and not (self.adjacent(n, j))
                                and (n.get_other() is None) and
                            (j.happy() == 0)):  #just unhappy, not necc empty
                            print("a perfect match")
                            n.set_other(j)
                            j.set_other(n)
                            found = True
                            break
                        if found:
                            break
            #otherwise find an imperfect match
                    if not found:
                        for j in unhappy:
                            if ((i != j) and (n.get_other() is None)
                                    and (j.get_other() is None)):
                                if not self.adjacent(i, j):
                                    print("an imperfect match")
                                    n.set_other(j)
                                    j.set_other(n)
            now_unmatched = []
            for i in self.node_list:
                if i.get_other is None:
                    now_unmatched.append(i)
            if len(unmatched) == len(now_unmatched):
                unmatched_are_the_same = 1
                for i in (enumerate(unmatched)):
                    if unmatched[i] != now_unmatched[i]:
                        unmatched_are_the_same = 0
                        break
            else:
                unmatched_are_the_same = 0
            if len(now_unmatched) != 0 and (unmatched_are_the_same
                                            == 0) and (tries < 5):
                stay = False

    def draw_graph(self,graph_name_num):
        """Reads through the node_list and edge_list and generates nodes
        and edges respectively using the information contained within
        the node and edge objects.
        """
        self.dot.clear()
        self.dot.comment=f'{self.rules}'
        # self.dot = graphviz.Digraph(comment='Tensegrity Object Graph')        i:Node
        for i in self.node_list:
            if i!=[]:
                self.dot.node(i.get_label())
        i:Edge
        for i in self.edge_list:
            self.dot.edge(str(i.get_start_label()),str(i.get_end_label()),str(i.get_label()))
        try:
            self.dot.render(f'doctest-output/graph_num_{graph_name_num}.gv').replace('\\', '/')
        except graphviz.backend.execute.ExecutableNotFound:
            print(f"Graph {graph_name_num} Made")
            # print(self.rules)