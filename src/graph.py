"""Graph class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 7.0
"""

import random
from l_system import LSystem
from transformer import Transformer
from node import Node
from edge import Edge
import graphviz


class Graph:
    """Graph class for the generation of complex tensegrity structures."""

    def __init__(self, rules, edge_types, bars):
        self.dot = graphviz.Digraph(comment='Tensegrity Object Graph')
        self.edge_types=edge_types
        self.rules:LSystem=rules
        self.transformer = Transformer(self)
        self.edge_list = []
        self.protected_nodes=[]
        self.node_list = []
        self.bar_num=bars
        self.bracket_nodes = []
        self.node_number = 0
        self.mut=False

    def get_rule(self, key):
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

    def add_edge(self, label, start:Node, end:Node):
        """Creates and adds a new edge to the graph

        Args:
            label (string): label for the edge
            start (Node): the origin node of the edge
            end (Node): the destination node for the edge
        """
        if start is not None or end is not None:
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
        node1:Node
        node2:Node
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

    def neighbors(self, node1:Node):
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
            edge:Edge
            for edge in self.edge_list:
                if edge.contains(node1):
                    if edge.get_end()==node1 and edge.get_start() in self.protected_nodes:
                        self.reassign_edge(edge,-1)
                    elif edge.get_start()==node1 and edge.get_end() in self.protected_nodes:
                        self.reassign_edge(edge,1)
                    else:
                        self.remove_edge(edge)
            self.node_list.remove(node1)

    def remove_edge(self, edge:Edge):
        """Removes an edge from the graph

        Args:
            start_node (Node): The starting node of the desired edge
            end_node (Node): The end node of the desired edge

        Returns:
            boolean: True if the edge was deleted and false if not
        """
        self.edge_list.remove(edge)
        if edge.get_start() is not None:
            edge.get_start().remove_edge(edge.get_end())
        if edge.get_end() is not None:
            edge.get_end().remove_edge(edge.get_start())

    def reassign_edge(self, edge:Edge, side):
        """Reassign one end of an edge to a new strut endpoint
        
        Args:
            edge (Edge): edge being changed
            side (int): which end of the edge is being reassigned
        """
        x=0
        adj=True
        new_node=self.smallest_node()
        endpoint=edge.get_endpoint(-side)
        other_end=edge.get_endpoint(side)
        while adj:
            if x>len(endpoint.get_adjacent()):
                adj=False
                self.remove_edge(edge)
            elif self.adjacent(new_node,endpoint) or new_node==endpoint or new_node==endpoint.get_other():
                new_node=random.choice(self.protected_nodes)
                x+=1
            else:
                adj=False
                endpoint.remove_edge(other_end)
                edge.set_start(new_node)
                new_node.add_edge(endpoint)
                endpoint.add_edge(new_node)

    def clear_nodes(self):
        """Resets the degree of all nodes in the graph.
        """
        node:Node
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
        i:Edge
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
        """Grows an initial graph using the indicated l_system and size parameters

        Args:
            size (int): Number of iterations through the transformer
            print_intermed (boolean): Whether or not the method should
                                    print intermediate stages of graph growth

        Returns:
            boolean: boolean indicating whether or not the graph is growing
        """
        growing = True
        print("growing...")
        oldnodes = self.order()
        oldedges = self.size()
        transformations = 0
        # marg=.25
        #run graph through transformer he indicated number of times
        while transformations < (size):
            self.transformer.transform()
            # if print_intermed and self.rules.rule_dict!=def_l:
            #     # self.draw_graph(graph_num+marg)
            # marg+=.25
            transformations += 1
        self.generate_bracket_edges()
        curnodes = self.order()
        curedges = self.size()
        if (oldnodes == curnodes and oldedges == curedges):
            growing = False
        return growing

    def simplify_graph(self):
        """
        Removes all nodes that have less than 3 edges
        beginning and ending at its location."""
        loop=True
        #loop indefinitely or until graph is empty
        while loop and self.order()!=0:
            n_to_del=[]
            loop=False
            node:Node
            for node in self.node_list:
                #remove unprotected nodes with a degree less than 3
                if node not in self.protected_nodes and node.is_extraneous():
                    loop=True
                    n_to_del.append(node)
            for i in n_to_del:
                if self.order()>self.bar_num*2:
                    self.remove_node(i)
            #break loop if less than the requisite number of nodes for an x-bar structure
            if self.order()<=self.bar_num*2:
                loop=False

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
                    i=random.randint(-1, (len(self.edge_types) - 1))
                    e.change_label(self.edge_types[i])
                    self.mut=True
            tries = 0
            while stay_in and tries <= 5 * len(edges):
                tries += 1
                rand_ex = random.randint(0, (len(edges) - 1))
                rand_edge = edges[rand_ex]
                if int(rand_edge.get_label() == "-1"):
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
        #get a non-adjacency matrix of all nodes in graph
        for inode in nodes:
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
        found_matches.reverse()
        self.protect(found_matches)
        self.purify_graph(found_matches)
        self.simplify_graph()
        return found_matches

    def protect(self, found_matches):
        """Identifies endpoints of struts to protect 
        them from being deleted in simplification

        Args:
            found_matches (list): list of strut endpoints
        """
        i=0
        while i<self.bar_num and i<len(found_matches):
            match=found_matches[i]
            for j in match:
                self.protected_nodes.append(j)
            i+=1

    def purify_graph(self,found:list):
        """Removes all nodes that are not included in a final strut

        Args:
            found (list): list of all potential strut endpoints
        """
        #remove all unprotected nodes
        i=0
        while i<len(self.node_list):
            if self.node_list[i] not in self.protected_nodes:
                self.remove_node(self.node_list[i])
            else:
                i+=1
        #remove or reassign all edges that had endpoints removed in previous loop
        i=0
        while i<len(self.edge_list):
            edge:Edge=self.edge_list[i]
            if (edge.get_end() not in self.node_list) and (edge.get_start() not in self.node_list):
                self.remove_edge(edge)
            elif edge.get_end() not in self.node_list:
                self.reassign_edge(edge,-1)
            elif edge.get_start() not in self.node_list:
                self.reassign_edge(edge,1)
            else:
                i+=1
        #remove extraneous struts and their endpoints
        while len(found)>self.bar_num:
            popped=found.pop()
            for x in popped:
                if x in self.node_list and x not in self.protected_nodes:
                    self.remove_node(x)
                    self.simplify_graph()

    def smallest_node(self):
        """Get the strut endpoint with the least edges on it

        Returns:
            Node: least connected strut endpoint in graph
        """
        nodes=self.protected_nodes
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
        my_pairs:list=all_pairs[curindex]
        my_pairs.reverse()
        temp_matches=[]
        for j in matches:
            temp_matches.append(j)
        i:Node
        #iterate through potential matches
        i=0
        while nodes[curindex].get_other() is None and i<len(my_pairs):
            match:Node=my_pairs[i]
            #if the potential match is so far unmatched
            #OR it is the last potential match
            if match.get_other() is None or match==my_pairs[-1]:
                old=match.get_other()
                match.set_other(nodes[curindex])
                nodes[curindex].set_other(match)
                #if match was previously paired, unpair it
                if old is not None:
                    if (old,match) in temp_matches:
                        temp_matches.remove((old,match))
                    else:
                        temp_matches.remove((match,old))
                temp_matches.append((match, nodes[curindex]))
                found_matches=self.recurse_find_pairs(curindex+1,all_pairs,temp_matches)
                return found_matches
            i+=1
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
        stay = (self.number_of_good_nodes()<size) or (self.order()<80)
        oldnodes = self.order()
        oldedges = self.size()
        #loop indefinitely
        while stay:
        # while stay:
            num_edge=self.size()
            edges:list=self.edge_list
            r=1
            #iterate through rules
            while r<len(self.edge_types) and stay:
                i=0
                num_edge = self.size()
                rule=self.rules.get_rule(r)
                #iterate thru edges
                while i<num_edge and stay:
                    #if edge type matches
                    if edges[i].get_label()==r:
                        #apply rule
                        self.transformer.apply_rule(edges[i],rule)
                        self.remove_edge(edges[i])
                    #terminate if
                    if (self.number_of_good_nodes()>=size) or (self.order()>=80) or iters>=100:
                        stay=False
                    i+=1
                r+=1
            curnodes = self.order()
            curedges = self.size()
            if (oldnodes == curnodes and oldedges == curedges):
                notgrowing = True
            #connect stubs
            self.generate_bracket_edges()
            iters += 1
        #remove extraneous nodes
        self.simplify_graph()
        if notgrowing:
            return False
        return True

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

    def draw_graph(self,graph_name_num,comment):
        """Reads through the node_list and edge_list and generates nodes
        and edges respectively using the information contained within
        the node and edge objects.
        """
        self.dot.clear()
        self.dot.comment=comment
        n:Node
        for n in self.node_list:
            self.dot.node(n.get_label())
        i:Edge
        for i in self.edge_list:
            if i.get_start_label()!="NONE" and i.get_end_label()!="NONE":
                self.dot.edge(str(i.get_start_label()),str(i.get_end_label()),str(i.get_label()))
        try:
            self.dot.render(f'doctest-output/graph_num_{graph_name_num}.gv').replace('\\', '/')
        except graphviz.backend.execute.ExecutableNotFound:
            print(f"Graph {graph_name_num} Made")
            