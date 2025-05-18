# 2.2

la commande 'echo' permet d'écrire l'argument de la fonction dans le terminal, dans la sortie standard.

Les paramètres de cette commande sont "-n" qui permet d'écrire toute la sortie sur la même ligne, ne pas imprimer le caractère de retour à la ligne.

# 2.3

On peut utiliser la commande "touch" avec le nom du fichier qu'on veut créer.

> touch filename

On peut aussi créer un fichier avec la commande "nano" qui est un editeur de texte

> nano newfile

# 2.5

a)

Les trois vont afficher le nombre de mots dans le fichier foo.txt

wc -w < foo.txt -> foo.txt est envoyer comme entrée pour la fonction "wc -w"

cat foo.txt | wc -w -> cat foo.txt va lire le fichier et affiche ce qu'il contient. "|" va ensuite prendre la sortie de la commande "cat" et la prend comme entrée pour la commande "wc"

wc -w foo.txt -> il y aura la même sortie ici mais il n'y a pas de redirection de sortie, on donne directement le fichier en argument.

b)

head foo.txt -n 6 -> La commande va lire les 6 (-n) premières lignes du fichier foo.txt

tail foo.txt -n 6 -> La commande va lire les 6 (-n) dernières lignes du fichier foo.txt

c)

oui il y a une différence

si foo.txt n'existe pas, out1.txt sera vide car foo.txt n'existe pas et out2.txt contiendra l'erreur de la fonction sort : "sort: No such file or directory". 2> redirige le flux d'erreur vers le fichier out2.txt

si foo.txt exist, alors out1.txt contiendra la sortie de la fonction sort donc le contenu de foo.txt trié. Et out2.txt sera vide car il n'y a pas d'erreur.