#!/bin/bash

clear

if [ $# -ne 1 ]; then
	echo Errore, inserisci il nome del file!
	exit 1
fi

name=$1

files_c=$(find ../STM32cubeIDE_Workspace -regex .*$name.c)
files_h=$(find ../STM32cubeIDE_Workspace -regex .*$name.h)

for i in $files_c; do

    echo $i
	cp source_file/$name.c $i
    echo
    
done

for i in $files_h; do

    echo $i
	cp source_file/$name.h $i
    echo
    
done


