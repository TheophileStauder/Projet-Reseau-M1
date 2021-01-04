# Projet-Reseau-M1
IMPORTANT : par défaut le serveur écoute sur le port 80 , ce port est privé ( comme tous les ports inférieur à 1024) pour écouter sur ce port il
faut donc executer avec sudo pour donner les permissions ( "sudo ./websnarf")
Si vous ne voulez pas run avec sudo faites : "./websnarf -p numéroPort"


Run "make" to compile the project
Then you can run "./websnarf -h" to read the man

Options disponibles : 
-h l'help manual 
-p [numeroport] pour spécifier un numéro de port ( par défaut 80 )
-f [filename] pour spécifier le nom du fichier de sauvegarde des logs ( par défaut logs.txt )
-t [temps] pour spécifier le temps du timeout ( temps de connexion entre le serveur et le client )
-m [maxSizeMessage] pour spécifier le nombre de caractères de la requête qu'on va logger
