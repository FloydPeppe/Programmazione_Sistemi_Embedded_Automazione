#!/bin/bash

source_dir="/home/peppe/Desktop/Progetto_Embedded/Tesina"

rm "$source_dir"/html/img/* >& /dev/null

pdftoppm -jpeg "$source_dir"/Tesina_Progetto_Embedded.pdf "$source_dir"/html/img/

