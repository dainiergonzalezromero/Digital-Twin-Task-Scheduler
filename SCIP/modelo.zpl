param cantp := read "datos.dat" as "1n" use 1;
param canttau := read "datos.dat" as "1n" use 1 skip (1+cantp);

set P := {1 .. cantp};
param Mem[P] := read "datos.dat" as "<1n> 2n" skip 1 use cantp;
param UP[P] := read "datos.dat" as "<1n> 3n" skip 1 use cantp;
param Cost[P] := read "datos.dat" as "<1n> 4n" skip 1 use cantp;

set Tau := {0 .. canttau};
param C[Tau] := read "datos.dat" as "<1n> 2n" skip (2+cantp) use (canttau+1);
param T[Tau] := read "datos.dat" as "<1n> 3n" skip (2+cantp) use (canttau+1);
param D[Tau] := read "datos.dat" as "<1n> 4n" skip (2+cantp) use (canttau+1);
param r[Tau] := read "datos.dat" as "<1n> 5n" skip (2+cantp) use (canttau+1);
param M[Tau] := read "datos.dat" as "<1n> 6n" skip (2+cantp) use (canttau+1);
param Proc[Tau] := read "datos.dat" as "<1n> 7n" skip (2+cantp) use (canttau+1);

param cantprec := read "datos.dat" as "1n" skip(3+cantp+canttau) use 1;
param prec[Tau*Tau] := read "datos.dat" as "<1n,2n> 3n" skip (4+cantp+canttau) use cantprec default 0;

param cantdelta := read "datos.dat" as "1n" skip(4+cantp+canttau+cantprec) use 1;
param Delta[P*P] := read "datos.dat" as "<1n,2n> 3n" skip (5+cantp+canttau+cantprec) use cantdelta default 0;
#var E[P] binary;
var L[Tau*P] binary;
var f[Tau] integer;
var s[Tau] integer;
var psi[Tau*Tau*P*P] binary;
var delt[Tau*Tau*P] binary;
param alfa := 1;
param beta := 1;
param gamma:= 1;
param bigM := 10000;

var finalizacion >= 0;
var costo >= 0;
var comunicacion >= 0;

#Objective
minimize fobj:
 sum <i> in Tau: alfa*f[i] + sum <p> in P: sum <i> in Tau: gamma * Cost[p]*L[i,p] + beta * sum <h> in Tau: sum <i> in Tau: sum <u> in P: sum <v> in P: Delta[u,v] * psi[h,i,u,v];

#Constraints
#subto epsilon: 
#sum <p> in P: E[p] >= 1;
 
subto tau_server: forall <i> in Tau with Proc[i]>0:
L[i,Proc[i]] == 1;

subto deadline: forall <i> in Tau:
f[i] <= r[i] + D[i];

subto nonpreempt: forall <i> in Tau:
f[i] == s[i] + C[i]-1;

subto util: forall <p> in P:
sum <i> in Tau: L[i,p] * (C[i] / T[i]) <= UP[p];

subto mem: forall <p> in P:
sum <i> in Tau: L[i,p] * M[i] <= Mem[p];

subto assign: forall <i> in Tau:
sum <p> in P: L[i,p] == 1;

subto nonoverlap: forall <i,j,p> in Tau*Tau*P with i != j:
s[i] >= f[j] - bigM * (1 - delt[j,i,p]);

subto nonoverlap2: forall <i,j,p> in Tau*Tau*P with i != j:
s[j] >= f[i] - bigM * (1 - delt[i,j,p]);

subto nonoverlap3: forall <h,i,p> in Tau*Tau*P with i != h: 
L[i,p]+L[h,p]-1 <= delt[i,h,p]+delt[h,i,p];

subto psi_coupling: forall <h,i,u,v> in Tau*Tau*P*P:
psi[h,i,u,v] <= L[h,u];

subto psi_coupling2: forall <h,i,u,v> in Tau*Tau*P*P:
psi[h,i,u,v] <= L[i,v];

subto psi_coupling3: forall <h,i,u,v> in Tau*Tau*P*P:
L[h,u] + L[i,v] - 1 <= psi[h,i,u,v];

subto psi_limit: forall <h,i> in Tau*Tau:
sum <u,v> in P*P: psi[h,i,u,v] <= 1;

subto precedence_start: forall <h,i,u,v> in Tau*Tau*P*P with h != i and u != v and prec[h,i] == 1:
 s[i] >= f[h] + Delta[u,v] - bigM * (1 - psi[h,i,u,v]); 

subto precedence_start_same_proc: forall <h,i> in Tau*Tau with h != i and prec[h,i] == 1:
  s[i] >= f[h];

subto domains: forall <i> in Tau:
s[i] >= 0;

#subto domains2: forall <i> in Tau:
#f[i] >= s[i] + C[i];

#finalizacion = sum <i> in Tau: alfa*f[i] ;
#costo = sum <p> in P: sum <i> in Tau: Cost[p]*L[i,p];
#comunicacion =  beta * sum <h> in Tau: sum <i> in Tau: sum <u> in P: sum <v> in P: Delta[u,v] * psi[h,i,u,v];

