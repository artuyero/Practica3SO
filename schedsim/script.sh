#/!bin/bash

#comprobacion carpeta resultados
if [ -d resultados  ]
then
	rm -r resultados
fi
mkdir resultados

echo "Introduce el numero de CPUs: "
read nCPUs

while [ $nCPUs -lt 1 ] && [ $nCPUs -gt 8 ]; do
	echo "El valor de CPUs no es valido, Introduce un entero entre 1 y 8: "
	read nCPUs
done
echo "Has seleccionado $nCPUs CPUs "


echo "Introduce la ruta del fichero: "
read ruta

while [ ! -f $fichero ]; do
	echo "la ruta no es valida, Introduce otra: "
	read ruta
done


echo "has seleccionado el fichero $ruta "

listaTipos=("FCFS" "PRIO" "RR" "SJF")
for nombretipo in "${listaTipos[@]}"; do

	for ((cpus = 1; $cpus <= $nCPUs; cpus++)) do
	./schedsim -i "$fichero" -n "$cpus" -s "$nombretipo" 
				
		for ((i=0;$i<$cpus;i++)) do
			mv CPU_$i.log resultados/"$nombretipo"-CPU-$i.log
		done
	done

	cd ../gantt-gplot

	for ((i=0; $i < $maxCPUs ;i++)) do
		../gantt-gplot/generate_gantt_chart ../schedsim/resultados/"$nombretipo"-CPU-"$i".log
	done
	cd ../schedsim
done
