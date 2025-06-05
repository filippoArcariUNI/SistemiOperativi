#!/bin/bash 
if [[ $# != 10 ]]; then 
    echo "error"
else 
    args=("$@")  # Salva gli argomenti in un array
    for (( i=$#-1; i>=0; i-- )); do
        echo "${args[i]}"
    done
fi


