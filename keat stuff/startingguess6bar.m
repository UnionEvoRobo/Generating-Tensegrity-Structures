%Starting Guess Machine #1
%This Starting Guess Machine does not take into account that Alex knows
%what a 6 bar tensegrity looks like. In other words Alex has done some math
%based upon what he has seen of the 6 bar tensegrity to do his best to
%create a good starting guess from

function [xnod,ynod,znod,eleconn,eletyp] = startingguess6bar

%input number of struts
numstruts = 6;

%input length of said struts
strutleng = 5.;

%premaid element connectivities
eleconn = [1 2 3 4 5 6 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 6 6 7 7 8 8 9 10;
11 12 7 8 9 10 7 2 6 12 7 8 3 8 9 4 9 10 5 10 11 6 11 12 9 11 12 10 11 12];

eletyp = [0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1];

%create matrix for nodal locations
%note that Node 8 will be predesignated at origin (0,0,0)
% (x,y,z) where z is height
node_loc = zeros(12,3); 

%create random position of node 10 same plane as node 8
xyz10 = [randi([-3,3],1,2), 0];

%distance from ground node to ground node
gnodedis = sqrt((xyz10(1).^2)+(xyz10(2).^2));

%find loc of node 12 assuming distance from 8-12 is same as 8-10 and 10-12
xyz12 = [(gnodedis*cosd(60)),gnodedis*sind(60),0];

%replace the values in node loc
node_loc(10,:) = xyz10;
node_loc(12,:) = xyz12;

%random loc for nodes 4,6,2 all have z dimension of 0

%node 4 is at some random location within the distance defined by the 
%length of the strut
angle = randi([0,360]);
xyz4 = [strutleng*cosd(angle),strutleng*sind(angle),0];

%repeat for nodes 6 and 2 but know that connected node location is
%different than the 4-8 scenario

%node 6 which is connected to node 10
angle = randi([0,360]);
xyz6 = xyz10 + [strutleng*cosd(angle),strutleng*sind(angle),0];

%node 2 which is connected to node 12
angle = randi([0,360]);
xyz2 = xyz12 + [strutleng*cosd(angle),strutleng*sind(angle),0];

%update node_loc
node_loc(4,:) = xyz4;
node_loc(6,:) = xyz6;
node_loc(2,:) = xyz2;

%create nodal locations for 7,9,11 node that the z dimension will be random
%as well now these also are space equally apart from each other

xyz7 = [randi([-3,3],1,2), rand(1)];
xyz9 = [randi([-3,3],1,2), rand(1)];
nodedis79 = sqrt(sum((xyz9-xyz7).^2));
xyz11 = [nodedis79*cosd(60),nodedis79*sind(60),-(xyz9(3)-xyz7(3))]+xyz7;

%update node locations
node_loc(7,:)=xyz7;
node_loc(9,:)=xyz9;
node_loc(11,:)=xyz11;

%create node locations for 1,3,5
%node 1 connected to node 11
angle = randi([0,360]);
xyz1 = xyz11 + [strutleng*cosd(angle),strutleng*sind(angle),0];

%node 3 connected to node 7
angle = randi([0,360]);
xyz3 = xyz7 + [strutleng*cosd(angle),strutleng*sind(angle),0];

%node 5 connected to node 9
angle = randi([0,360]);
xyz5 = xyz9 + [strutleng*cosd(angle),strutleng*sind(angle),0];

%update node locations

node_loc(1,:)=xyz1;
node_loc(3,:)=xyz3;
node_loc(5,:)=xyz5;

%save node locations
for inod=1:12
    xnod(inod)=node_loc(inod,1);
    ynod(inod)=node_loc(inod,2);
    znod(inod)=node_loc(inod,3);
end
