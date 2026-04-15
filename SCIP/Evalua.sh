#!/bin/bash

echo "Simulacion DT instancias"

echo "10 tareas"

cp dats/10dat_1.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/10dat_2.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/10dat_3.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/10dat_4.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/10dat_5.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


echo "20 tareas"

cp dats/20dat_1.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/20dat_2.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/20dat_3.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/20dat_4.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/20dat_5.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


echo "30 tareas"

cp dats/30dat_1.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/30dat_2.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/30dat_3.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/30dat_4.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto


cp dats/30dat_5.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

echo "40 tareas"

cp dats/40dat_1.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

cp dats/40dat_2.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

cp dats/40dat_3.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

cp dats/40dat_4.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

cp dats/40dat_5.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

echo "50 tareas"

cp dats/50dat_1.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

cp dats/50dat_2.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

cp dats/50dat_3.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

cp dats/50dat_4.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

cp dats/50dat_5.dat instancia.dat
zimpl -o prueba modelo.zpl
cplex < exacto

