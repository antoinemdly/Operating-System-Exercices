#!/bin/bash

SOURCE_DIR=$1
DEST_DIR=$2
RES=$3

# verifie si le dossier donner en argument existe.
# si il n'existe pas, la fonction va le creer
check_dir() {
    if [ -d "$1" ]; then
        :
    else
        mkdir "$1"
    fi
}

aide() {
    echo "Utilisation : "
    echo ""
    echo "  ./script.sh <source> <destination> <resolution (optional)(1234x1234) >"
    echo ""
    echo "Exemple :"
    echo ""
    echo "  ./script.sh /Download/img /Desktop/images 1920x1080"
}

# function qui check si un fichier est en effet une images en utilisant la fonction file
check_valid_img() {
    file -i -b "$1" | grep -q "image/"
}

# check combien d'argument l'utilisateur a donné
# si il y a moins de deux argument ou plus que trois, on affiche l'aide et on quit le script
if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
    aide
    exit 1
fi

# check si RES existe
# si oui, on check si l'argument est au bon format
if [ -n "$RES" ]; then
    # on check si RES est du bon format :
    # ^ signifie le debut du string et $ sigifie la fin (celui qu'on defini comme le 'test')
    # [0-9]+ signifie un ou plus nombre entre 0 et 9
    # suivi d'un x puis encore [0-9]+
    # Donc on check si $RES est de la forme 1234x1234
    if [[ "$RES" =~ ^[0-9]+x[0-9]+$ ]]; then
        # l'entree est correct
        :
    else
        # l'entree n'est pas correct on quit avec l'aide
        aide
        exit 1
    fi
fi

# on check si le dossier d'arriver exist. Si non, on le cree
check_dir $2

# loop sur tous les fichier dans le dossier de depart
for file in "$SOURCE_DIR"/*; do
    # check si le fichier est une image
    if $(check_valid_img "$file"); then
        # on recupere le nom du fichier uniquement
        filename=$(basename "$file")
    
        # on enleve tous les caractere non souhaite donc les espaces, les gimmets, etc...
        new_filename=$(echo $filename | tr -d " " | tr -d "'" | tr -d "/" | tr -d '"')

        # check si un troisème argument a été donné
        # autrement dit, si il faut changer la resolution des images ou pas
        if [ -n "$RES" ]; then
            # si il a ete donnee, on converti l'image a la resolution souaither
            # et on copie l'image directement dans le dossier d'arriver avec l'extension .png
            convert -resize $RES "$file" $2/"${new_filename%.*}.png"
            echo "$new_filename was copied and reformated with a resolution of $RES"

            else

            # meme chose ici mais on ne converti pas la resolution si l'utilisateur ne l'a pas demander
            convert "$file" $2/"${new_filename%.*}.png"
            echo "$new_filename was copied and changed to png"
        fi

    fi
done

# grep -q pattern renvoie 0 si il trouve le pattern