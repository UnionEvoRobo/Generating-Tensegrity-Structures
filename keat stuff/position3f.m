%Program to determine equilibrium vertex coordinates of a tensegrity 
%starting from a semi-random deformed position
%(note: this draft addresses the special case of a 6-bar)
%(note: this version creates a movie of the evolving geometry)
%(note: this version includes a function that creates a starting guess for
%       the 6-bar geometry)
%(note: this version stochastically varies the location of a seventh 
%       displacement constraint)
%(note: this version employs new starting guess functions for the 6-bar 
% and 15-bar)

%clear variables
clear

%--------------------------------------------------------------------------
%inputs
%--------------------------------------------------------------------------

%number of bars (nbar) determines number of vertices (nnod) and 
%elements (nele)
nbar=6;

if( nbar==6)
    nnod=12;
    nele=30;
elseif (nbar==15)
    nnod=30;
    nele=93;
end

%input mode
%  mode=0 using starting guess for geometry provided below
%       1 use Alex's function to create a starting guess for the 6-bar
%       2 use alternative starting guess function for the 6-bar
%       3 use starting guess function for the 15-bar

mode=1;

if (mode==0)   
  %initial guess for vertex coordinates
  xnod=[-2.0  2.0  -2.0  2.0  -3.0 -3.0   6.0  6.0   0.   0.    0.   0. ];
  ynod=[ 0.   0.    0.   0.   -2.0  2.0  -2.0  2.0   7.0  7.0  -3.0 -3.0];
  znod=[ 6.0  6.0  -3.0 -3.0   0.   0.    0.   0.   -2.0  2.0  -2.0  2.0];
  %element connectivities
  conn=   ...                                                                 
[1 3 5 7 9  11 6 1  6  9 3 9 8  10 8 4 8 4 1 5  1  5  11 5 12 2 12 4  11 4;
 2 4 6 8 10 12 1 10 10 3 6 6 10 2  2 8 9 9 5 12 12 11 3  3 2  7 7  11 7  7];
  %element type (0=strut; 1=spring)
  eletyp=[0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1];
elseif mode==1    
  %call Alex's function for the 6-bar
  [xnod,ynod,znod,conn,eletyp] = startingguess6bar;
elseif mode==2
  %alternative starting guess for the 6-bar
  [xnod,ynod,znod,conn,eletyp] = bar6guess;
else
    %starting guess for the 15 bar
  [xnod,ynod,znod,conn,eletyp] = bar15guess;
end

%spring stiffness (k), preload (f0) and free length (Lf)
k=0.120;
kstrut=k*10^2;
f0=0.05;
Lf=1.63;

%convergence parameters
niter=2000;    %max number of iterations
uincmax=0.1;  %max allowable displacement increment
steptol=10^(-5);   %tolerance for relative change in delta
                   %(exponent corresponds to number of significant digits)

%plot control parameters
% movie=0 do not plot any meshes
%       1 plot initial mesh only, pause, then proceed with analysis, then
%         plot final mesh
%       2 plot the entire sequence of meshes in real time, pausing briefly
%         between meshes
movie=1;
nframe=2000;     %number of movie frames
az=-37.5;        %horizontal rotation of viewpoint(-37.5 deg is default)
el=30;           %vertical elevation of viewpoint(30 deg is default)

%--------------------------------------------------------------------------
%plot the initial geometry
%--------------------------------------------------------------------------

if (movie>0)
    visualizeMesh3D(nele,xnod,ynod,znod,conn,eletyp,az,el)
    pause(1.0)
end

%--------------------------------------------------------------------------
%assign free lengths to all elements (springs and struts)
%--------------------------------------------------------------------------

Lfsav=zeros(1,nele);

for iele=1:nele
    if (eletyp(iele)==1)
        Lfsav(iele)=Lf;
    else
        nn1=conn(1,iele);
        nn2=conn(2,iele);
        Lfsav(iele)=sqrt( (xnod(nn2)-xnod(nn1))^2 + ...
                    (ynod(nn2)-ynod(nn1))^2 + (znod(nn2)-znod(nn1))^2 );
    end
end
%--------------------------------------------------------------------------
%determine the spring elements attached to each vertex
%--------------------------------------------------------------------------

nspring=zeros(1,nnod);

for inod=1:nnod
    ispr=0;
    for jele=1:nele
        if (eletyp(jele)==1)
            nn1=conn(1,jele);
            nn2=conn(2,jele);
            if (nn1==inod || nn2==inod)
                ispr=ispr+1;
                vspring(inod,ispr)=jele;
            end
        end
    end
    nspring(inod)=ispr;
end

%--------------------------------------------------------------------------
%initialize displacement boundary conditions
%--------------------------------------------------------------------------

sflag=0;
ele=0;

%fix the nodes of the first strut in the element list

while sflag==0
    ele=ele+1;
    if (eletyp(ele)==0)
        nn1=conn(1,ele);
        nn2=conn(2,ele);
        dispbc = [nn1 nn1 nn1 nn2 nn2 nn2 0;
                  1   2   3   1   2   3   0];
        elefix=ele;
        nodfix(1)=nn1;
        nodfix(2)=nn2;
        sflag=1;
    end
end

%randomly assign a third node to be constrained, being sure to exclude the
%endpoints of the fixed strut

rflag=0;

while rflag==0
    inod=randi(nnod);
    if ( inod~=nodfix(1) && inod~=nodfix(2) )
        nodfix(3)=inod;
        rflag=1;
    end
end

%designate a dof of the third fixed node to be constrained and check that 
%constraining this dof will prevent rigid body motion

DOF = [1 0 0;
       0 1 0;
       0 0 1];

doffix=1;
rflag=0;

while rflag==0
    
    nn1=conn(1,elefix);
    nn2=conn(2,elefix);
    delx=xnod(nn2)-xnod(nn1);
    dely=ynod(nn2)-ynod(nn1);
    delz=znod(nn2)-znod(nn1);
    esmag=sqrt(delx^2 +dely^2 +delz^2);
    es(1)=delx/esmag;
    es(2)=dely/esmag;
    es(3)=delz/esmag;

    nn1=conn(2,elefix);
    nn2=nodfix(3);
    delx=xnod(nn2)-xnod(nn1);
    dely=ynod(nn2)-ynod(nn1);
    delz=znod(nn2)-znod(nn1);
    epmag=sqrt(delx^2 +dely^2 +delz^2);
    ep(1)=delx/epmag;
    ep(2)=dely/epmag;
    ep(3)=delz/epmag;
    
    en=cross(ep,es);
    er=cross(en,es);
    ec=DOF(doffix,:);
    
    if ( abs(dot(ec,es))>0.98 || abs(dot(ec,er))>0.98 )
        doffix=doffix+1;
    else
        rflag=1;
    end

end
    
%fill in last column of array 'dispbc'

dispbc(1,7)=nodfix(3);
dispbc(2,7)=doffix;
    
%--------------------------------------------------------------------------
%calculate incremental displacements and update vertex coordinates until
%convergence 
%--------------------------------------------------------------------------

%initializations
stopflag=0;
iter=0;
delta=zeros(niter,nele);
SE=zeros(1,niter);
SEold=0;
xtemp=zeros(1,nnod);
ytemp=zeros(1,nnod);
ztemp=zeros(1,nnod);
E=zeros(iter,3*nnod);
condKg=zeros(1,niter);
unorm=zeros(1,niter);
deltanorm=zeros(1,niter);

while stopflag==0
    
%--------------------------------------------------------------------------
%loop over each element to generate the global force vector, the 
%global stiffness matrix and the global preload vector
%--------------------------------------------------------------------------

    iter=iter+1;

    %initializations
    Fg=zeros(3*nnod,1);
    Kg=zeros(3*nnod,3*nnod);
    Pg=zeros(3*nnod,1);
    
    for iele=1:nele

        nn1=conn(1,iele);
        nn2=conn(2,iele);

        %------------------------------------------------------------------
        %calculate the direction cosines
        %------------------------------------------------------------------
    
        delx=xnod(nn2)-xnod(nn1);
        dely=ynod(nn2)-ynod(nn1);
        delz=znod(nn2)-znod(nn1);
        emag=sqrt(delx^2 +dely^2 +delz^2);
        l=delx/emag;
        m=dely/emag;
        n=delz/emag;
    
        %------------------------------------------------------------------
        %calculate the transpose of the transformation matrix
        %(note: only relevant terms in TT were retained)
        %------------------------------------------------------------------
    
        TT=[ l 0 0 0 0 0;
             m 0 0 0 0 0;
             n 0 0 0 0 0;
             0 0 0 l 0 0;
             0 0 0 m 0 0;
             0 0 0 n 0 0];
     
        %------------------------------------------------------------------
        %determine the element force vector in global coordinates
        %------------------------------------------------------------------
        
        if (eletyp(iele)==1)

            %calculate relative displacement in local coordinates
    
            Lc=sqrt( (xnod(nn2)-xnod(nn1))^2 + (ynod(nn2)-ynod(nn1))^2 +...
                 (znod(nn2)-znod(nn1))^2 );
            delta(iter,iele) = Lc - Lfsav(iele);

            %calculate the spring force
            
            Fspring2 = k * delta(iter,iele) + f0;
            
            %calculate strain energy if this is the first iteration
            
            if (iter==1)
                SEold = 0.5*k*delta(iter,iele)^2 + ...
                        f0*delta(iter,iele) + SEold;
            end
            
        else
            
            %sum spring forces at the second node of the strut

            Fstrut = 0.;

            %loop over each spring that is attached to the strut
            
            for i=1:nspring(nn2)
                
                ele=vspring(nn2,i);
                n1=conn(1,ele);
                n2=conn(2,ele);
                
                %calculate relative displacement in local coordinates
                
                Lc=sqrt( (xnod(n2)-xnod(n1))^2 + (ynod(n2)-ynod(n1))^2 +...
                         (znod(n2)-znod(n1))^2 );
                del = Lc - Lfsav(ele);

                %calculate the spring force
            
                Fspring = k * del + f0;

                %adjust sign of spring force based on spring connectivity
                
                if (n1==nn2)
                    Fspring = -Fspring;
                end
                
                %calculate direction cosines for the spring
                
                delx=xnod(n2)-xnod(n1);
                dely=ynod(n2)-ynod(n1);
                delz=znod(n2)-znod(n1);
                emag=sqrt(delx^2 +dely^2 +delz^2);
                ls=delx/emag;
                ms=dely/emag;
                ns=delz/emag;
                
                %calculate component of the spring force that acts along 
                %the strut and sum
                
                Fstrut = Fspring*(l*ls+m*ms+n*ns) + Fstrut;
                
            end
            
            %force on strut is negative of the resultant force on the
            %attached springs as per Newton's Third Law
            
            Fspring2 = -Fstrut;
                        
        end
        
        %calculate the norm of the 'delta vector'
        
        deltacur = delta(iter,:);
        deltanorm(iter) = norm(deltacur,Inf);
            
        %create the nodal force vector in local coordinates

        FeL = [ -Fspring2 0 0 Fspring2 0 0 ]';
    
        %transform the nodal force vector to global coordinates
    
        FeG=TT*FeL;
   
        %------------------------------------------------------------------
        %determine the element stiffness matrix in global coordinates
        %------------------------------------------------------------------

        l2=l^2;
        m2=m^2;
        n2=n^2;
        lm=l*m;
        ln=l*n;
        mn=m*n;
        KeG=[ l2  lm  ln -l2 -lm -ln;
              lm  m2  mn -lm -m2 -mn;
              ln  mn  n2 -ln -mn -n2;
             -l2 -lm -ln  l2  lm  ln;
             -lm -m2 -mn  lm  m2  mn;
             -ln -mn -n2  ln  mn  n2 ];
        
        if (eletyp(iele)==1)
            KeG=KeG*k;
        else
            KeG=KeG*kstrut;
        end
                
        %------------------------------------------------------------------
        %detemine the element preload vector in global coordinates
        %------------------------------------------------------------------
    
        %set up the element preload vector in local coordinates
    
        if (eletyp(iele)==1)
            PeL=[ -f0 0 0 f0 0 0 ]';
        else 
            PeL=[ 0 0 0 0 0 0 ]';
        end
   
        %transform the element preload vector to global coordinates
    
        PeG=TT*PeL;
    
        %------------------------------------------------------------------
        %assemble element equations into global equations using the 
        %direct stiffness method
        %------------------------------------------------------------------
    
        r1=3*nn1-3;
        r2=3*nn2-3;
        c1=r1;
        c2=r2;

        %assemble the element force vector in the global force vector  
        for i=1:3
            Fg(r1+i)=FeG(i)   + Fg(r1+i);
            Fg(r2+i)=FeG(3+i) + Fg(r2+i);
        end   

        %assemble the element stiffness matrix in the global stiffness 
        %matrix
        for j=1:3
            for i=1:3
                Kg(r1+i,c1+j)=KeG(i,j)     + Kg(r1+i,c1+j);
                Kg(r2+i,c2+j)=KeG(3+i,3+j) + Kg(r2+i,c2+j);
                Kg(r1+i,c2+j)=KeG(i,3+j)   + Kg(r1+i,c2+j);
                Kg(r2+i,c1+j)=KeG(3+i,j)   + Kg(r2+i,c1+j);
            end
        end

        %assemble the element preload vector in the global preload vector    
        for i=1:3
            Pg(r1+i)=PeG(i)   + Pg(r1+i);
            Pg(r2+i)=PeG(3+i) + Pg(r2+i);
        end   
    
    end
    
    %----------------------------------------------------------------------
    %modify 7th displacement boundary condition if deemed appropriate
    %based on relative change in displacement increment
    %----------------------------------------------------------------------

    %randomly re-assign the third node to be constrained, being sure to 
    %exclude both the endpoints of the fixed strut and the current third
    %node

    if (iter>2)
        Fgchange=abs( (Fgnorm(iter-1)-Fgnorm(iter-2))/Fgnorm(iter-2) );
    end
    
    if ( iter>2 && Fgchange<0.20 )
    
        rflag=0;

        while rflag==0
            inod=randi(nnod);
            if ( inod~=nodfix(1) && inod~=nodfix(2) )
                if ( inod~=nodfix(3) )
                    nodfix(3)=inod;
                    rflag=1;
                end
            end
        end
    
        rflag=0;
        
        doffix=doffix+1;
        if (doffix==4)
            doffix=1;
        end
        
        while rflag==0
    
            nn1=conn(1,elefix);
            nn2=conn(2,elefix);
            delx=xnod(nn2)-xnod(nn1);
            dely=ynod(nn2)-ynod(nn1);
            delz=znod(nn2)-znod(nn1);
            esmag=sqrt(delx^2 +dely^2 +delz^2);
            es(1)=delx/esmag;
            es(2)=dely/esmag;
            es(3)=delz/esmag;

            nn1=conn(2,elefix);
            nn2=nodfix(3);
            delx=xnod(nn2)-xnod(nn1);
            dely=ynod(nn2)-ynod(nn1);
            delz=znod(nn2)-znod(nn1);
            epmag=sqrt(delx^2 +dely^2 +delz^2);
            ep(1)=delx/epmag;
            ep(2)=dely/epmag;
            ep(3)=delz/epmag;
    
            en=cross(ep,es);
            er=cross(en,es);
            ec=DOF(doffix,:);
    
            if ( abs(dot(ec,es))>0.98 || abs(dot(ec,er))>0.98 )
                doffix=doffix+1;
                if (doffix==4)
                    doffix=1;
                end
            else
                rflag=1;
            end

        end
    
        %fill in last column of array 'dispbc'

        dispbc(1,7)=nodfix(3);
        dispbc(2,7)=doffix;
        
    %end of if statement
    end
    
    %----------------------------------------------------------------------
    %impose displacement boundary conditions
    %----------------------------------------------------------------------

    for inod=1:7
        row=3*(dispbc(1,inod)-1) + dispbc(2,inod);
        Kg(row,row)=Kg(row,row)*10^4;
        Fg(row)=0;
        Pg(row)=0;
    end
    
    %----------------------------------------------------------------------
    %modify Kg to insure positive definiteness using the method suggested
    %by Zhang (2014)
    %----------------------------------------------------------------------
    
    %calculate the condition number of Kg
    
    condKg(iter)=cond(Kg);
    
    %calculate the eigenvalues of Kg
    
    E(iter,:)=eig(Kg);

    %calculate diagonal scaling factor based on relation of lowest
    %eigenvalue to machine epsilon
    
    if ( E(iter,1) < eps^0.1 )
        Emin=0;
        i=0;
        while Emin < eps^0.1
            i=i+1;
            Emin=E(iter,i);
        end
        scale1=1.01*Emin;
    else
        scale1=0.;
    end
    
    %modify diagonal elements of Kg to improve positive definitenesss

    for i=1:3*nnod
        Kg(i,i)=1.01*abs(scale1) + Kg(i,i);
    end
            
    %----------------------------------------------------------------------
    %down-scale the external load vector
    %----------------------------------------------------------------------
    
    Fgnorm(iter)=norm(Fg,Inf);
    
%    if (iter==1)
%        Fgmax=Fgnorm(iter)/20;
%    end
    
%    if (Fgnorm(iter)>Fgmax)
%        scale2 = -Fgmax/Fgnorm(iter);
%    else
        scale2 = -1.0;
%    end 
    
    Fg = Fg * scale2;        
    
    %----------------------------------------------------------------------
    %solve for the displacement increment
    %----------------------------------------------------------------------
    
    uinc = Kg\Fg;
    
    %calculate the norm of the incremental displacement vector
    
    unorm(iter)=norm(uinc,Inf);
    
    %re-scale the displacement increment to limit the max step size
    
    if (unorm(iter)>uincmax)
        uinc = uincmax/unorm(iter) * uinc;
    end

    %----------------------------------------------------------------------
    %update nodal coordinates
    %----------------------------------------------------------------------

    update=0.;
    nutemp=1.0;
    
    %line search using bisection
    
    while update==0
    
        %temporarily update vertex coodinates
        
        for i=1:nnod
            xtemp(i) = xnod(i) + nutemp*uinc((i-1)*3+1);
            ytemp(i) = ynod(i) + nutemp*uinc((i-1)*3+2);
            ztemp(i) = znod(i) + nutemp*uinc((i-1)*3+3);
        end
 
        %loop over all spring elements to calculate strain energy
        
        SEtemp=0.;
        
        for iele=1:nele
            
            if (eletyp(iele)==1)
        
                nn1=conn(1,iele);
                nn2=conn(2,iele);
        
                %calculate relative displacement in local coordinates
    
                Lc=sqrt( (xtemp(nn2)-xtemp(nn1))^2 + ...
                   (ytemp(nn2)-ytemp(nn1))^2 + (ztemp(nn2)-ztemp(nn1))^2 );
                del = Lc - Lfsav(iele);

                %calculate strain energy
            
                SEtemp = 0.5*k*del^2 + f0*del + SEtemp;
                
            end    
            
        end

        %check if strain energy got smaller since the last iteration; if
        %not, halve the parameter 'nu' and repeat strain energy calculation
        
        if ( SEtemp < SEold )
            nu=nutemp;
            for i=1:nnod
                xnod(i) = xnod(i) + nu*uinc((i-1)*3+1);
                ynod(i) = ynod(i) + nu*uinc((i-1)*3+2);
                znod(i) = znod(i) + nu*uinc((i-1)*3+3);
            end
            SE(iter)=SEtemp;
            SEold=SEtemp;
            update=1;
        else
            nutemp=nutemp/2;
            if (nutemp<.001)
                update=1;
                stopflag=2
            end
        end
        
    %end of line search
    end
    
    %----------------------------------------------------------------------
    %update geometry plot
    %----------------------------------------------------------------------

    if (movie==2 && iter<=nframe)
        visualizeMesh3D(nele,xnod,ynod,znod,conn,eletyp,az,el)
        M(k) = getframe;
        pause(0.5)
    end
    
    %----------------------------------------------------------------------
    %check stopping criteria
    %----------------------------------------------------------------------
    
    if (iter==niter)
        stopflag=1
    end
    
    if (nu<.001)
        stopflag=2
    end

    %only use this stopping criterion with the 6 bar
    if(nbar==6)
        if (iter>1)
           rel=abs(deltanorm(iter)-deltanorm(iter-1))/abs(deltanorm(iter));
           if (rel<steptol)
               stopflag=3
           end
        end
    end

    %check final strut lengths

    for iele=1:nele
        if (eletyp(iele)==0)
            nn1=conn(1,iele);
            nn2=conn(2,iele);
            Lstrut(iter,iele)=sqrt( (xnod(nn2)-xnod(nn1))^2 + ...
                    (ynod(nn2)-ynod(nn1))^2 + (znod(nn2)-znod(nn1))^2 );
        end
    end    

%--------------------------------------------------------------------------
%end of iterative while loop
%--------------------------------------------------------------------------
 
end

iter

if (movie==1)
    visualizeMesh3D(nele,xnod,ynod,znod,conn,eletyp,az,el)
end

%save final coordinates
xsav=xnod;
ysav=ynod;
zsav=znod;







