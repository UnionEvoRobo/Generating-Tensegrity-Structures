%--------------------------------------------------------------------------
%Program to create a movie of the mode shape over time. Run it without
%clearing variables after first running position3f.m and Stiff.m.
%--------------------------------------------------------------------------

%identify the modal vector by column
col=8;

%input number of times to cycle the short movie
ncycle=20;

%input number of frames per half cycle
nframe=10;

%input viewing angle
az=-37.5;           %horizontal rotation of viewpoint(-37.5 deg is default)
el=30.;             %vertical elevation of viewpoint(30 deg is default)

%extract the modal vector
mvector=X(:,col);

%generate two vectors of evenly spaced scale factors
scalef=linspace(-1,1,nframe);
scaleb=linspace(1,-1,nframe);

%initializations
xplot=zeros(1,nnod);
yplot=zeros(1,nnod);
zplot=zeros(1,nnod);

%determine axis limits
xmin=min(xnod);
xmax=max(xnod);
ymin=min(ynod);
ymax=max(ynod);
zmin=min(znod);
zmax=max(znod);

%adjust these limit values to insure equal grids along each edge
diff(1)=xmax-xmin;
diff(2)=ymax-ymin;
diff(3)=zmax-zmin;
diffmax=max(diff);
xmax=xmin+diffmax;
ymax=ymin+diffmax;
zmax=zmin+diffmax;

%plot the mode shape over time for the specified number of cycles

for i=1:ncycle

    for j=2:nframe
        
        for inod=1:nnod
            xplot(inod) = xnod(inod) + mvector(3*(inod-1)+1)*scalef(j);
            yplot(inod) = ynod(inod) + mvector(3*(inod-1)+2)*scalef(j);
            zplot(inod) = znod(inod) + mvector(3*(inod-1)+3)*scalef(j);
        end        
        visualizeMode3D(nele,xplot,yplot,zplot,conn,eletyp,az,el,...
                        xmin,xmax,ymin,ymax,zmin,zmax)
        pause(0.05)
        
    end

    for j=2:nframe
        
        for inod=1:nnod
            xplot(inod) = xnod(inod) + mvector(3*(inod-1)+1)*scaleb(j);
            yplot(inod) = ynod(inod) + mvector(3*(inod-1)+2)*scaleb(j);
            zplot(inod) = znod(inod) + mvector(3*(inod-1)+3)*scaleb(j);
        end
        visualizeMode3D(nele,xplot,yplot,zplot,conn,eletyp,az,el,...
                        xmin,xmax,ymin,ymax,zmin,zmax)
        pause(0.05)
        
    end
    
end

