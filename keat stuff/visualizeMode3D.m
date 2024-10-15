% generate the 3D mesh plot

% input:
% nv: number of nodes on the horizontal plane
% nvele: number of elements on the horizontal plane
% vcor: nodal coordintes on the bottom plane
% vconn: connectivities of all elements in 3D
% nz: number of nodes in the z direction
% z: z-coordinates in a single strand 

% output:
% 3D plot 
function [] = visualizeMode3D(ne, xnod, ynod, znod, CONN, eletyp, AZ, ...
              EL,xmin,xmax,ymin,ymax,zmin,zmax)

plot3(xnod(1,:),ynod(1,:),znod(1,:),'.')
view(AZ,EL)
%axis equal
axis([xmin xmax ymin ymax zmin zmax])
xlabel('x-coordinate')
ylabel('y-coordinate')
zlabel('z-coordinate')
for i = 1:ne
        if eletyp(i) == 0
            color = 'b';
        elseif eletyp(i) == 1
            color = 'g';
        else
            color = 'r';
        end
    
    Xa = xnod(CONN(1,i));
    Xb = xnod(CONN(2,i));
    Ya = ynod(CONN(1,i));
    Yb = ynod(CONN(2,i));
    Za = znod(CONN(1,i));
    Zb = znod(CONN(2,i));
    line([Xa,Xb],[Ya,Yb],[Za,Zb],'color',color,'LineWidth',0.1)
end
