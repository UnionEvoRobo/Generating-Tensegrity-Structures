function [xnod,ynod,znod,eleconn,eletyp] = bar15guess

%Number of struts
numstruts = 15;

%Strut length
strutleng = 5;

%Element Connectivities
eleconn = ...
    [1  3  5  7  9  11 13 15 17 19 21 23 25 27 29 25 25 25 27 27 29 23 ...
     23 30 30 21 21 24 24 24 24 24 24 24 19 19 19 28 28 15 15 17 17 22 ...
     22 22 22 22 13 13 26 11 11 11 20 20 20 18 18 18 9  9  14 14 14 3  ...
     3  3  14 14 7  7  12 12 12 12 16 16 10 10 10 10 10 5  5  5  1  1  ...
     8  8  4  4  6; ...
     2  4  6  8  10 12 14 16 18 20 22 24 26 28 30 29 27 28 29 30 23 30 ...
     26 21 24 24 28 15 28 22 26 17 18 20 26 22 28 26 22 22 18 26 20 13 ...
     16 18 20 26 20 16 20 20 18 14 18 16 14 14 9  12 12 16 12 16 7  14 ...
     12 8 10  8  16 10 16 10 8  5  18 10 5  2  6  4  8  1  8  4  4  8  ...
     4  6 6   2  2];

%Element type
eletyp = [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ...
    1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ...
    1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1];

%Create a matrix of node locations
node_loc = zeros(numstruts*2,3);
upperlim = 3.5;
lowerlim = -3.5;

%Create random node locations for 1 3 5 7 8 9 11 13 15 21 23 25 27 29
for nodes = 1:2:29
    node_loc(nodes,:) = rand(1,3).*(upperlim-lowerlim)+lowerlim;
end

%Create random xy node locations for 2 4 6 8 10 12 14 16 18 20 22 24 26 28
% and 30
for nodez = 2:2:30
    node_loc(nodez,:) = [rand(1,2).*(upperlim-lowerlim)+lowerlim,0];
end

%Calculate appropriate Z location for the even numbered nodes
for node = 2:2:30
    node_loc(node,:)= node_loc(node-1,:)+ node_loc(node,:) + [0,0, ...
        sqrt((strutleng).^2 - (node_loc(node,1)).^2 - ...
        (node_loc(node,2)).^2)];
end

%checks that everything is the proper length
truthiness = zeros(numstruts,1);
for truenode = 2:2:30
    if (sqrt(sum((node_loc(truenode,:)-node_loc(truenode-1,:)).^2)) ...
            > 4.9 || sqrt(sum((node_loc(truenode,:)-node_loc(8,:)).^2)) ...
            < 5.1)
    truthiness(truenode/2) = 1;
    end
end

%Print Node Location and Proper length check list
node_loc
truthiness

%convert for Keat's Program
for inod=1:(numstruts*2)
    xnod(inod)=node_loc(inod,1);
    ynod(inod)=node_loc(inod,2);
    znod(inod)=node_loc(inod,3);
end
