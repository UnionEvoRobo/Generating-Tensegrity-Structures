from node import Node

"""def recurse_find_pairs(curindex, all_pairs:list, matches:list, nodes):
    if curindex==len(all_pairs):
        print("end")
        return matches
    else:
        my_pairs=all_pairs[curindex]
        temp_matches=[]
        for j in matches:
            temp_matches.append(j)
            for i in j:
                if nodes[curindex]==i:
                    print('found')
        i:Node
        my_pairs.reverse()
        for i in my_pairs:
            if not nodes[curindex].is_matched():
                match=i
                if not match.is_matched():
                    match.matched=True
                    nodes[curindex].matched=True
                    temp_matches.append((match, nodes[curindex]))
                    found_matches=recurse_find_pairs(curindex+1,all_pairs,temp_matches,nodes)
                    return found_matches
        return recurse_find_pairs(curindex+1,all_pairs,temp_matches,nodes)"""


def recurse_find_pairs(curindex:int, all_pairs:list, matches:list,nodes):
    """Find pairs of nodes with matching bracket labels

    Args:
        curindex (int): current recursion index
        allPairs (list): list of all known pairs
        matches (list): list of found matches

    Returns:
        list: list of paired nodes
    """
    if curindex==len(all_pairs):
        return matches
    else:
        my_pairs:list=all_pairs[curindex]
        temp_matches=[]
        for j in matches:
            temp_matches.append(j)
            # for i in j:
            #     if nodes[curindex]==i:
            #         print('found')
        i:Node
        # my_pairs.reverse()
        for i in my_pairs:
            if not nodes[curindex].is_matched():
                match=i
                if not match.is_matched():
                    match.matched=True
                    nodes[curindex].matched=True
                    temp_matches.append((match, nodes[curindex]))
                    found_matches=recurse_find_pairs(curindex+1,all_pairs,temp_matches,nodes)
                    return found_matches
        return recurse_find_pairs(curindex+1,all_pairs,temp_matches,nodes)
                    
nodes=[]
for i in range (88):
    node=Node(i)
    nodes.append(node)

ll2=[[nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[8],nodes[9],nodes[10],nodes[11],nodes[12],nodes[13],nodes[14],nodes[15],nodes[16],nodes[17],nodes[18],nodes[19],nodes[20],nodes[21],nodes[22],nodes[23],nodes[24],nodes[25],nodes[26],nodes[27],nodes[28],nodes[29],nodes[30],nodes[31],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[39],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[47],nodes[48],nodes[49],nodes[50],nodes[51],nodes[52],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[63],nodes[64],nodes[65],nodes[66],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[77],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86],nodes[87]],
    [nodes[0],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[8],nodes[9],nodes[10],nodes[11],nodes[12],nodes[13],nodes[14],nodes[15],nodes[16],nodes[17],nodes[18],nodes[19],nodes[20],nodes[21],nodes[22],nodes[23],nodes[24],nodes[25],nodes[26],nodes[27],nodes[28],nodes[29],nodes[30],nodes[31],nodes[32],nodes[33],nodes[35],nodes[36],nodes[37],nodes[38],nodes[39],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[47],nodes[48],nodes[49],nodes[50],nodes[51],nodes[52],nodes[53],nodes[54],nodes[55],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[66],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[77],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[84],nodes[85],nodes[86],nodes[87]],
    [nodes[0],nodes[1],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[8],nodes[9],nodes[10],nodes[11],nodes[12],nodes[13],nodes[14],nodes[15],nodes[16],nodes[17],nodes[18],nodes[19],nodes[20],nodes[21],nodes[22],nodes[23],nodes[24],nodes[25],nodes[26],nodes[27],nodes[28],nodes[29],nodes[30],nodes[31],nodes[32],nodes[33],nodes[34],nodes[35],nodes[37],nodes[38],nodes[39],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[47],nodes[48],nodes[49],nodes[50],nodes[51],nodes[52],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[66],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[77],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[4],nodes[5],nodes[6],nodes[7],nodes[8],nodes[9],nodes[10],nodes[11],nodes[12],nodes[13],nodes[14],nodes[15],nodes[16],nodes[17],nodes[18],nodes[19],nodes[20],nodes[21],nodes[22],nodes[23],nodes[24],nodes[25],nodes[26],nodes[27],nodes[28],nodes[29],nodes[30],nodes[31],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[39],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[47],nodes[48],nodes[49],nodes[50],nodes[51],nodes[52],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[64],nodes[65],nodes[66],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[73],nodes[74],nodes[75],nodes[76],nodes[77],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[85],nodes[86],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[5],nodes[6],nodes[7],nodes[8],nodes[9],nodes[10],nodes[11],nodes[12],nodes[13],nodes[14],nodes[15],nodes[16],nodes[17],nodes[18],nodes[19],nodes[20],nodes[21],nodes[22],nodes[23],nodes[24],nodes[25],nodes[26],nodes[27],nodes[28],nodes[29],nodes[30],nodes[31],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[39],nodes[40],nodes[42],nodes[43],nodes[45],nodes[46],nodes[47],nodes[48],nodes[49],nodes[50],nodes[51],nodes[52],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[66],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[77],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[6],nodes[7],nodes[8],nodes[9],nodes[10],nodes[11],nodes[12],nodes[13],nodes[14],nodes[15],nodes[16],nodes[17],nodes[18],nodes[19],nodes[20],nodes[21],nodes[22],nodes[23],nodes[24],nodes[25],nodes[26],nodes[27],nodes[28],nodes[29],nodes[30],nodes[31],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[39],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[47],nodes[48],nodes[49],nodes[50],nodes[51],nodes[52],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[66],nodes[67],nodes[68],nodes[69],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[77],nodes[78],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]],
    [nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[18],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[52],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[6],nodes[7],nodes[9],nodes[13],nodes[16],nodes[23],nodes[27],nodes[31],nodes[39],nodes[47],nodes[66],nodes[77],nodes[87]],
    [nodes[0],nodes[1],nodes[2],nodes[3],nodes[4],nodes[5],nodes[8],nodes[10],nodes[11],nodes[12],nodes[14],nodes[15],nodes[17],nodes[19],nodes[20],nodes[21],nodes[22],nodes[24],nodes[25],nodes[26],nodes[28],nodes[29],nodes[30],nodes[32],nodes[33],nodes[34],nodes[35],nodes[36],nodes[37],nodes[38],nodes[40],nodes[41],nodes[42],nodes[43],nodes[44],nodes[45],nodes[46],nodes[48],nodes[49],nodes[50],nodes[51],nodes[53],nodes[54],nodes[55],nodes[56],nodes[57],nodes[58],nodes[59],nodes[60],nodes[61],nodes[62],nodes[63],nodes[64],nodes[65],nodes[67],nodes[68],nodes[69],nodes[70],nodes[71],nodes[72],nodes[73],nodes[74],nodes[75],nodes[76],nodes[78],nodes[79],nodes[80],nodes[81],nodes[82],nodes[83],nodes[84],nodes[85],nodes[86]]]

nodes1=[]
for i in range (88):
    node=Node(i)
    nodes1.append(node)

ll1=[[nodes1[3],nodes1[5]],[nodes1[3],nodes1[4]],[nodes1[4],nodes1[5]],[nodes1[0],nodes1[1]],[nodes1[1],nodes1[2]],[nodes1[0],nodes1[2]]]
matched=recurse_find_pairs(0,ll1,[],nodes1)
for i in matched:
    edge=[]
    for j in i:
        edge.append(j.get_label())
    print(edge)
