function  [xnod,ynod,znod,eleconn,eletyp] = bar6guess
numstruts = 6;
strutleng = 7;

eleconn = [1 2 3 4 5 6 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 6 6 7 7 8 8 9 10;
11 12 7 8 9 10 7 2 6 12 7 8 3 8 9 4 9 10 5 10 11 6 11 12 9 11 12 10 11 12];

eletyp = [0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1];
node_loc = zeros((numstruts*2),3);
lowerlim = -3.5;
upperlim = 3.5;
node_loc(10,:)=rand(1,3).*(upperlim-lowerlim)+lowerlim;
node_loc(12,:)=rand(1,3).*(upperlim-lowerlim)+lowerlim;
node_loc(11,:)=rand(1,3).*(upperlim-lowerlim)+lowerlim;
node_loc(7,:)=rand(1,3).*(upperlim-lowerlim)+lowerlim;
node_loc(9,:)=rand(1,3).*(upperlim-lowerlim)+lowerlim;
for  nodes = 1:6
    node_loc(nodes,[1 2]) = rand(1,2).*(upperlim-lowerlim)+lowerlim;
end
node_loc(4,:)=node_loc(8,:)+ node_loc(4,:) + [0,0, ...
        sqrt((strutleng).^2 - (node_loc(4,1)).^2 - (node_loc(4,2)).^2)];
node_loc(1,:)=node_loc(11,:)+ node_loc(1,:) + [0,0, ...
        sqrt((strutleng).^2 - (node_loc(1,1)).^2 - (node_loc(1,2)).^2)];
node_loc(2,:)=node_loc(12,:)+ node_loc(2,:) + [0,0, ...
        sqrt((strutleng).^2 - (node_loc(2,1)).^2 - (node_loc(2,2)).^2)];
node_loc(3,:)=node_loc(7,:)+ node_loc(3,:) + [0,0, ...
        sqrt((strutleng).^2 - (node_loc(3,1)).^2 - (node_loc(3,2)).^2)];
node_loc(5,:)=node_loc(9,:)+ node_loc(5,:) + [0,0, ...
        sqrt((strutleng).^2 - (node_loc(5,1)).^2 - (node_loc(5,2)).^2)];
node_loc(6,:)=node_loc(10,:)+ node_loc(6,:) + [0,0, ...
        sqrt((strutleng).^2 - (node_loc(6,1)).^2 - (node_loc(6,2)).^2)];
   
%checks that everything is the proper length
truthiness = [0;0;0;0;0;0];
if (sqrt(sum((node_loc(4,:)-node_loc(8,:)).^2)) > 4.9 || ...
        sqrt(sum((node_loc(4,:)-node_loc(8,:)).^2)) < 5.1)
    truthiness(4) = 1;
end
if (sqrt(sum((node_loc(3,:)-node_loc(7,:)).^2)) > 4.9 || ...
        sqrt(sum((node_loc(3,:)-node_loc(7,:)).^2)) < 5.1)
    truthiness(3) = 1;
end
if (sqrt(sum((node_loc(2,:)-node_loc(12,:)).^2)) > 4.9 || ...
        sqrt(sum((node_loc(2,:)-node_loc(12,:)).^2)) < 5.1)
    truthiness(2) = 1;
end
if (sqrt(sum((node_loc(1,:)-node_loc(11,:)).^2)) > 4.9 || ...
        sqrt(sum((node_loc(1,:)-node_loc(11,:)).^2)) < 5.1)
    truthiness(1) = 1;
end
if (sqrt(sum((node_loc(5,:)-node_loc(9,:)).^2)) > 4.9 || ...
        sqrt(sum((node_loc(5,:)-node_loc(9,:)).^2)) < 5.1)
    truthiness(5) = 1;
end
if (sqrt(sum((node_loc(6,:)-node_loc(10,:)).^2)) > 4.9 || ...
        sqrt(sum((node_loc(6,:)-node_loc(10,:)).^2)) < 5.1)
    truthiness(6) = 1;
end
%node_loc;
%truthiness;
for inod=1:12
    xnod(inod)=node_loc(inod,1);
    ynod(inod)=node_loc(inod,2);
    znod(inod)=node_loc(inod,3);
end

end