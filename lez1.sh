#!/bin/bash
DATA=data.txt
#1. Stampa “T” (per True) o “F” (per False) a seconda che il valore di data rappresenti un file o una cartella esistente
[ -f $DATA ] && echo "T" || echo "F"   # il primo comando ha successo (codice di ritorno 0) se il file esiste, viene eseguito il primo comando
[ -d $DATA ] && echo "T" || echo "F"
#Stampa “file”, “cartella” o “?” a seconda che il valore rappresenti un file(esistente), una cartella (esistente) o una voce non presente nel file-system

if [ -f "$DATA" ]; then
    echo "file"
elif [ -d "$DATA" ]; then
    echo "cartella"
else
    echo "?"
fi
# Stampa il risultato di una semplice operazione aritmetica (es: ‘1 < 2’) contenuta nel file indicato dal valore di DATA, oppure “?” se il file non esiste

if [ -f "$DATA" ]; then
   echo $(( $(cat ./data.txt) ))
else
    echo "?"
fi

#  Scrivere uno script che dato un qualunque numero di argomenti li restituisca in output in ordine inverso.
nArg=$#
list=("$@") #in questo modo gli argomenti sono una lista, altrimenti facendo list=$@ otterrei solamente la stringa degli argomenti

for ((i=nArg-1; i>=0; i--)); do
    echo "${list[$i]}"
done


# Scrivere uno script che mostri il contenuto della cartella corrente in ordine
# inverso rispetto all’output generato da “ls” (che si può usare ma senza opzioni).
# Per semplicità, assumere che tutti i file e le cartelle non abbiano spazi nel nome.
list=($(ls)) #necessarie le parentesi esterne per avere list di tipo array, senza viene considerato una stringa 

nFile="${#list[@]}" #sintasi per ottenere il numero di elementi di una lista 0-based 
nFile=$(ls | wc -l) #numero di elementi 1-based
for ((i = nFile - 1 ; i>=0 ; i--)); do 
    echo "${list[i]}"
    
done
