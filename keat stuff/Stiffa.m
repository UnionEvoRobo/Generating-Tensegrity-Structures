%This program uses the results from the form-finding code 'Position3f.m,
%without clearing variables, to perform the following tasks towards the
%design of an n-bar tensegrity:
%    Part I - Coodinate transforms the vertex coodinates such that three
%             user-defined contact points lie on the plane z=0 with the
%             tensegrity situated on the +z side
%    Part II- Calculates the static sag (i.e. z-displacements) due to
%             the weights of the struts using a non-linear large
%             displacement algorithm that acccounts for the pre-tension
%             in the springs
%    Part III-Calculates the natural frequncies of the tensegrity
%             while accounting for the pre-tension in the spring. This
%             is a small displacement algorithm.
%
%Note: You can re-run this code without having to rerun position3f.m

%--------------------------------------------------------------------------
%additional inputs to the code
%--------------------------------------------------------------------------

%PART I INPUTS
%input the three vertices that define the contact patch and the z=0 plane
if (nbar==6)
    nodz= [ 1 5 12 ];
end

if (nbar==3)
    nodz= [ 1 3 5 ];
end


%input viewing angle
az=-37.5;        %horizontal rotation of viewpoint(-37.5 deg is default)
el=30.;          %vertical elevation of viewpoint(30 deg is default)

%PART II INPUTS
%input the weight [lb] and mass of the strut
Wstrut=0.1267;
if (nbar==3)
    Wstrut=0.132;
end
mstrut=Wstrut/(32.2*12);      %must be consistent with units on length!!!

%convergence parameters for non-linear analysis
niter=50;     %max number of iterations (set to 0 to skip sag calculation)
stopflag=0;   %to set to 1 to skip over sag calculation
        
%PART III INPUTS
%there are none

%always start this code with the vertex coordinates produced by 
%the code 'position3f.m'

xnod=xsav;
ynod=ysav;
znod=zsav;

%--------------------------------------------------------------------------
%--------------------------------------------------------------------------
%Part I - Transform vertex coordinates such that contact patch corresponds
%         to z=0
%--------------------------------------------------------------------------
%--------------------------------------------------------------------------

%Locate the contact patch on the 15-bar that has the largest area
%--------------------------------------------------------------------------

if (nbar==15)
    
    %loop over all possible contact patches
    
    areamax=0.;
    
    for inod=1:30
    
        for jnod=1:30
            
            if (jnod~=inod)
                
                for knod=1:30
                
                    if (knod~=jnod && knod~=inod)
                        nn1=inod;
                        nn2=jnod;
                        nn3=knod;
                    
                        %define a unit vector perpendicular to the contact 
                        %patch

                        delx=xnod(nn2)-xnod(nn1);
                        dely=ynod(nn2)-ynod(nn1);
                        delz=znod(nn2)-znod(nn1);
                        emag=sqrt(delx^2 +dely^2 +delz^2);
                        ea(1)=delx/emag;
                        ea(2)=dely/emag;
                        ea(3)=delz/emag;

                        delx=xnod(nn3)-xnod(nn1);
                        dely=ynod(nn3)-ynod(nn1);
                        delz=znod(nn3)-znod(nn1);
                        emag=sqrt(delx^2 +dely^2 +delz^2);
                        eb(1)=delx/emag;
                        eb(2)=dely/emag;
                        eb(3)=delz/emag;
    
                        en=cross(eb,ea);
                        enmag=sqrt( en(1)^2 + en(2)^2 + en(3)^2 );
                        en=en/enmag;

                        %do the vertices lie completely to one side of the 
                        %contact patch?

                        sump=0;
                        sumn=0;
                        cflag=0;

                        for inod=1:nnod
                            delx=xnod(inod)-xnod(nn1);
                            dely=ynod(inod)-ynod(nn1);
                            delz=znod(inod)-znod(nn1);
                            delvec=[delx dely delz];
                            dotsav = dot(delvec,en);
                            if (dotsav>=0)
                                sump=dotsav+sump;
                            else
                                sumn=dotsav+sumn;
                            end
                            if (sump>.0001 && abs(sumn)>.0001)
                                cflag=1;
                                break
                            end
                        end
                        
                        if (cflag==0)
                            delx=xnod(nn2)-xnod(nn1);
                            dely=ynod(nn2)-ynod(nn1);
                            delz=znod(nn2)-znod(nn1);
                            aa=sqrt(delx^2 +dely^2 +delz^2);
                            
                            delx=xnod(nn3)-xnod(nn1);
                            dely=ynod(nn3)-ynod(nn1);
                            delz=znod(nn3)-znod(nn1);
                            bb=sqrt(delx^2 +dely^2 +delz^2);
                            
                            delx=xnod(nn3)-xnod(nn2);
                            dely=ynod(nn3)-ynod(nn2);
                            delz=znod(nn3)-znod(nn2);
                            cc=sqrt(delx^2 +dely^2 +delz^2);

                            %apply Heron's formula to calculate area of the
                            %trianglular contact patch
                            
                            s = (aa+bb+cc)/2;
                            
                            area = sqrt(s*(s-aa)*(s-bb)*(s-cc));
                            
                            if (area > areamax)
                                nodz = [nn1 nn2 nn3];
                                areamax=area;
                            end
                        end
                    end
                end
            end
        end
    end
end

%--------------------------------------------------------------------------
%Having identified the 3 vertices of the contact patch, transform all 
%vertex coordinates such that the contact patch lies on the z=0 plane and 
%the tensegrity lies to the +z side of the contact patch
%--------------------------------------------------------------------------

% define unit vectors in the plane of and perpendicular to the contact
% patch

nn1=nodz(1);
nn2=nodz(2);
delx=xnod(nn2)-xnod(nn1);
dely=ynod(nn2)-ynod(nn1);
delz=znod(nn2)-znod(nn1);
emag=sqrt(delx^2 +dely^2 +delz^2);
ea(1)=delx/emag;
ea(2)=dely/emag;
ea(3)=delz/emag;

nn1=nodz(1);
nn2=nodz(3);
delx=xnod(nn2)-xnod(nn1);
dely=ynod(nn2)-ynod(nn1);
delz=znod(nn2)-znod(nn1);
emag=sqrt(delx^2 +dely^2 +delz^2);
eb(1)=delx/emag;
eb(2)=dely/emag;
eb(3)=delz/emag;
    
en=cross(eb,ea);
enmag=sqrt( en(1)^2 + en(2)^2 + en(3)^2 );
en=en/enmag;

ec=cross(ea,en);
ecmag=sqrt( ec(1)^2 + ec(2)^2 + ec(3)^2 );
ec=ec/ecmag;

%do the vertices lie to the +en side of the contact patch or to 
%the -en side?

dotsum=0;

for inod=1:nnod
    delx=xnod(inod)-xnod(nn1);
    dely=ynod(inod)-ynod(nn1);
    delz=znod(inod)-znod(nn1);
    delvec=[delx dely delz];
    dotsum = dot(delvec,en) + dotsum;
end

%define the unit vectors associated with the new coordinate system

if (dotsum>0)
    e1=ec;
    e2=ea;
    e3=en;
    doffix=1;
else
    e1=ea;
    e2=ec;
    e3=-en;
    doffix=2;
end

%coordinate transform the vertex coordinates to the new coordinate system 
%with origin located at the first node on the contact patch

for inod=1:nnod
    nodold = [xnod(inod) ynod(inod) znod(inod)];
    xtemp(inod)=dot(e1,nodold);
    ytemp(inod)=dot(e2,nodold);
    ztemp(inod)=dot(e3,nodold);
end

for inod=1:nnod
    xnew(inod)=xtemp(inod)-xtemp(nn1);
    ynew(inod)=ytemp(inod)-ytemp(nn1);
    znew(inod)=ztemp(inod)-ztemp(nn1);
end

%reassign vertex coordinates

xnod=xnew;
ynod=ynew;
znod=znew;

%plot the tensegrity in the new coordinate system

visualizeMesh3D(nele,xnew,ynew,znew,conn,eletyp,az,el)
pause(1.0)


%--------------------------------------------------------------------------
%--------------------------------------------------------------------------
%Part II - Calculate static sag (i.e. vertex displacements) due to the 
%          weights of the struts. This is a non-linear large displacement
%          analysis.
%--------------------------------------------------------------------------
%--------------------------------------------------------------------------

%initialize displacement boundary conditions

dispbc=zeros(2,6);

nn1=nodz(1);
nn2=nodz(2);
nn3=nodz(3);

%ndbc=9;
%dispbc = [nn1 nn1 nn1 nn2 nn3 nn2; 
%          1   2   3   3   3   doffix];
%dispbc = [nn1 nn1 nn1 nn2 nn2 nn2 nn3 nn3 nn3 ; 
%          1   2   3   1   2   3   1   2   3  ];
ndbc = 6;
dispbc = [nn1 nn1 nn1 nn2 nn2 nn3 ; 
          1   2   3   2   3   3  ];

%--------------------------------------------------------------------------
%calculate incremental displacements and update vertex coordinates until
%convergence 
%--------------------------------------------------------------------------

%initializations
%stopflag=0;   %Comment out to skip sag calculation
iter=0;
delta=zeros(niter,nele);
SE=zeros(1,niter);
SEold=0;
xtemp=zeros(1,nnod);
ytemp=zeros(2,nnod);
ztemp=zeros(3,nnod);
E=zeros(iter,3*nnod);
condKg=zeros(1,niter);
unorm=zeros(1,niter);
deltanorm=zeros(1,niter);
phinorm=zeros(1,niter);

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

    end
    
    %----------------------------------------------------------------------
    %set up external force vector due to strut weights
    %----------------------------------------------------------------------
    
    if (iter==1)
        Rg=zeros(nnod*3,1);
        for inod=1:nnod
            Rg((inod-1)*3+3,1)=-Wstrut/2;
        end
    end
    
    %----------------------------------------------------------------------
    %assign unbalanced load vector
    %----------------------------------------------------------------------
    
    if (iter==1)
        phi=Rg;
    else
        phi=Fg-Rg;
    end
        
    %----------------------------------------------------------------------
    %impose displacement boundary conditions
    %----------------------------------------------------------------------

    for inod=1:ndbc
        row=3*(dispbc(1,inod)-1) + dispbc(2,inod);
        Kg(row,row)=Kg(row,row)*10^4;
        phi(row)=0;
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
    %multiply unbalanced load vector by -1
    %----------------------------------------------------------------------
    
    phinorm(iter)=norm(phi,Inf);
    
    if (iter==1)
        scale2 = 1.0;
    else
        scale2 = -1.0;
    end
    
    phi = phi * scale2;
       
    %----------------------------------------------------------------------
    %solve for the displacement increment
    %----------------------------------------------------------------------
    
    uinc = Kg\phi
    
    %calculate the norm of the incremental displacement vector
    
    unorm(iter)=norm(uinc,Inf);
    
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
        
        if ( SEtemp < SEold || iter==1)
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
    %check stopping criteria
    %----------------------------------------------------------------------
    
    if (iter==niter)
        stopflag=1
    end
    
    if (nu<.001)
        stopflag=2
    end

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

%--------------------------------------------------------------------------
%calculate max static sag (i.e. z-displacment)
%--------------------------------------------------------------------------

%calculate total displacements

utot=xnod-xnew;
vtot=ynod-ynew;
wtot=znod-znew;

%calculate largest sag (i.e. z-displacment)

sagmax=min(wtot)

%plot final deformed shape

visualizeMesh3D(nele,xnod,ynod,znod,conn,eletyp,az,el)


%--------------------------------------------------------------------------
%--------------------------------------------------------------------------
%Part III - Calculate natural frequencies
%--------------------------------------------------------------------------
%--------------------------------------------------------------------------

%calculate pre-tension (PT) in each of the springs and the associated
%stiffness term (kp)

kp=zeros(1,nele);

for iele=1:nele
            
    if (eletyp(iele)==1)
        
        nn1=conn(1,iele);
        nn2=conn(2,iele);
        
        %calculate relative displacement in local coordinates
    
        Lc=sqrt( (xnod(nn2)-xnod(nn1))^2 + ...
               (ynod(nn2)-ynod(nn1))^2 + (znod(nn2)-znod(nn1))^2 );
        del = Lc - Lfsav(iele);

        %calculate the pre-tension in the spring
        
        PT = k * del + f0;

        %calculate the associated stiffness term which accounts for preload
        
        kp(iele) = PT/Lc;

    end
  
end

%--------------------------------------------------------------------------
%generate a random point in the vicinity of the tensegrity that does not
%lie along the axis of any of the struts or springs. This point
%will be used to define the local element coordinate systems.
%--------------------------------------------------------------------------
    
xminr=min(xnod);
xmaxr=max(xnod);
yminr=min(ynod);
ymaxr=max(ynod);
zminr=min(znod);
zmaxr=max(znod);

rflag=0;

while rflag==0
    
    pt(1) = xminr + (xmaxr-xminr)*rand(1);
    pt(2) = yminr + (ymaxr-yminr)*rand(1);
    pt(3) = zminr + (zmaxr-zminr)*rand(1);
    
    eflag=0;
    ele=0;
    
    while eflag==0
        ele=ele+1;
        nn1=conn(1,ele);
        nn2=conn(2,ele);

        delx=xnod(nn2)-xnod(nn1);
        dely=ynod(nn2)-ynod(nn1);
        delz=znod(nn2)-znod(nn1);
        emag=sqrt(delx^2 +dely^2 +delz^2);
        e(1)=delx/emag;
        e(2)=dely/emag;
        e(3)=delz/emag;

        delx=pt(1)-xnod(nn2);
        dely=pt(2)-ynod(nn2);
        delz=pt(3)-znod(nn2);
        emag=sqrt(delx^2 +dely^2 +delz^2);
        ep(1)=delx/emag;
        ep(2)=dely/emag;
        ep(3)=delz/emag;
        
        if (abs(dot(e,ep)) > 0.98)
            eflag=1;
        end
        
        if (ele==nele && eflag==0)
            eflag=1;
            rflag=1;
        end               
    end
    
end

%--------------------------------------------------------------------------
%assemble the global stiffness and mass matrices
%--------------------------------------------------------------------------

%initializations
Fg=zeros(3*nnod,1);
Kg=zeros(3*nnod,3*nnod);
Mg=zeros(3*nnod,3*nnod);
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
    e1(1)=delx/emag;
    e1(2)=dely/emag;
    e1(3)=delz/emag;
    
    delx=pt(1)-xnod(nn2);
    dely=pt(2)-ynod(nn2);
    delz=pt(3)-znod(nn2);
    emag=sqrt(delx^2 +dely^2 +delz^2);
    ep(1)=delx/emag;
    ep(2)=dely/emag;
    ep(3)=delz/emag;
    
    e2=cross(ep,e1);
    e3=cross(e1,e2);

    %------------------------------------------------------------------
    %calculate the rotation matrix
    %------------------------------------------------------------------
    
    T=[ e1(1) e1(2) e1(3) 0     0     0;
        e2(1) e2(2) e2(3) 0     0     0;
        e3(1) e3(2) e3(3) 0     0     0;
        0     0     0     e1(1) e1(2) e1(3);
        0     0     0     e2(1) e2(2) e2(3); 
        0     0     0     e3(1) e3(2) e3(3) ];
         
    %------------------------------------------------------------------
    %determine the element stiffness matrix in global coordinates
    %------------------------------------------------------------------

    if (eletyp(iele)==1)
        s = k;
        d = kp(iele);
    else
        s = kstrut;
        d = 0;          %think about this
    end


    %calculate element stiffnes matrix in local coordinates (see paper by
    %Bossens et al)
    
    KeL = [ s  0  0 -s  0  0;
            0  d  0  0 -d  0;
            0  0  d  0  0 -d;
           -s  0  0  s  0  0;
            0 -d  0  0  d  0;
            0  0 -d  0  0  d ];
    
    %transform this stiffness matrix to global coordinates
    
    KeG = T' * KeL * T;
    
    %------------------------------------------------------------------
    %determine the element (consistent) mass matrix in global coordinates
    %------------------------------------------------------------------
    
    if (eletyp(iele)==1)
        m = 0;
    else
        m = mstrut;
    end
    
    MeL = m/6 * [2 0 0 1 0 0;
                 0 2 0 0 1 0;
                 0 0 2 0 0 1;
                 1 0 0 2 0 0;
                 0 1 0 0 2 0;
                 0 0 1 0 0 2];
                 
    %no need to coordinate transform the element mass matrix
    
    MeG = MeL;
    
    %------------------------------------------------------------------
    %assemble element equations into global equations using the 
    %direct stiffness method
    %------------------------------------------------------------------
    
    r1=3*nn1-3;
    r2=3*nn2-3;
    c1=r1;
    c2=r2;

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

    %assemble the element mass matrix in the global mass matrix
    for j=1:3
        for i=1:3
            Mg(r1+i,c1+j)=MeG(i,j)     + Mg(r1+i,c1+j);
            Mg(r2+i,c2+j)=MeG(3+i,3+j) + Mg(r2+i,c2+j);
            Mg(r1+i,c2+j)=MeG(i,3+j)   + Mg(r1+i,c2+j);
            Mg(r2+i,c1+j)=MeG(3+i,j)   + Mg(r2+i,c1+j);
        end
    end

%end of element loop    
end

%----------------------------------------------------------------------
%impose displacement boundary conditions
%----------------------------------------------------------------------

for inod=1:ndbc
    row=3*(dispbc(1,inod)-1) + dispbc(2,inod);
    Kg(row,row)=Kg(row,row)*10^4;
end

%--------------------------------------------------------------------------
%calculate natural frequncies in cycles per sec
%--------------------------------------------------------------------------

[X,Ws]=eig(Kg,Mg);
w=sqrt(Ws)/(2*pi);



