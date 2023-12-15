# !/usr/bin/python3
# coding: utf-8

import os
n_tiret=80
os.system('sudo clear')
print("-" * n_tiret)
print("                        \      /\      /                ")
print("                         \    /  \    /                 ")
print("                          \  /    \  /                  ")
print("                           \/      \/                   ")
print("-" * n_tiret)
print("        importation_courant_v4.py sur ttyACM0--_03_11_2023           ")
print("                                 V4                                  ")
print("  Programme acquisition arduino importation et creation fichiers csv ")
print("-" * n_tiret)
from datetime import datetime
print(datetime.now().strftime("le %d %m %y a %H h %M"))
print("-" * n_tiret)
print("Liste des ARDUINO sur USB")
commande_linux = os.system('ls /dev/ttyACM*')
print("-" * n_tiret)
print("Forcage du port usb0 pour ARDUINO en lecture/ecriture")
commande_linux = os.system('sudo chmod -R 777 /dev/ttyACM0')
print("-" * n_tiret)
print(" ")

from datetime import datetime
print(datetime.now().strftime("Demarrage du programme le %d %m %y a %H h %M"))
print(" ")
import serial
ser = serial.Serial('/dev/ttyACM0', 9600)
vidage = ser.readline()
while 1 :
    sortie = ser.readline()
    print("Entree en boucle")
    from datetime import datetime
    print("-" * n_tiret)
    print(datetime.now().strftime("Reception d'un paquet de donnees le %d %m %y a %H h %M"))
    print("I_solaire,U_secteur,I_general,I_chf_eau,I_zoe,I_PAC")
    print (sortie) 
    sortie = str(sortie)
    sortie = sortie[:-5]    #suppression des caratères de fin de la trame captée
    sortie = sortie[2:]      #suppression des caratères de debut de la trame captée
    print (sortie) 
    print("-" * n_tiret)

    NomFichier_csv = '/var/www/html/ifr.csv'
    Fichier = open (NomFichier_csv,'a')
    Fichier.write(datetime.now().strftime("%y/%m/%d/%H/%M"))
    Fichier.write (sortie)
    Fichier.write ("\n'")        #permet le saut à la ligne sur le fichier csv
    Fichier.close()

    #print(" ")
    print("Creation des fichiers jour semaine mois vide avec les legendes")
    fichier_jour = '/var/www/html/jour.csv'
    Fichier = open (fichier_jour,'w')
    Fichier.write("Date,I_solaire,U_secteur,I_general,I_PAC,I_zoe,I_chf_eau,I_maison")
    Fichier.close()
    fichier_semaine = '/var/www/html/semaine.csv'
    Fichier = open (fichier_semaine,'w')
    Fichier.write("Date,I_solaire,U_secteur,I_general,I_PAC,I_zoe,I_chf_eau,I_maison")
    Fichier.close()
    fichier_mois = '/var/www/html/mois.csv'
    Fichier = open (fichier_mois,'w')
    Fichier.write("Date,I_solaire,U_secteur,I_general,I_PAC,I_zoe,I_chf_eau,I_maison")
    Fichier.close()
    print("                                                          Fichiers crees")

    print("Creation du fichier Jour_tmp")
    from collections import deque
    def tail(path, n):
        with open(path, 'r') as f:
            data = ''.join(deque(f, n))
            return data
    FILEIN = "/var/www/html/ifr.csv"
    FILEOUTPUT = "/var/www/html/jour_tmp.csv"
    data = tail(FILEIN, 144) # Les 144 dernières lignes du fichier
    with open(FILEOUTPUT, 'w') as f:
        f.write(data)
    print("                                         Fichier Jour Temporaire cree")

    print("Creation du fichier semaine_tmp")
    from collections import deque
    def tail(path, n):
        with open(path, 'r') as f:
            data = ''.join(deque(f, n))
            return data
    FILEIN = "/var/www/html/ifr.csv"
    FILEOUTPUT = "/var/www/html/semaine_tmp.csv"
    data = tail(FILEIN, 1008) # Les 1008 dernières lignes du fichier
    with open(FILEOUTPUT, 'w') as f:
        f.write(data)
    print("                                         Fichier semaine temporaire cree")

    print("Creation du fichier mois_tmp")
    from collections import deque
    def tail(path, n):
        with open(path, 'r') as f:
            data = ''.join(deque(f, n))
            return data
    FILEIN = "/var/www/html/ifr.csv"
    FILEOUTPUT = "/var/www/html/mois_tmp.csv"
    data = tail(FILEIN, 4320) # Les 4320 dernières lignes du fichier
    with open(FILEOUTPUT, 'w') as f:
        f.write(data)
    print("                                         Fichier mois temporaire cree")
    #print(" ")
	
    print("Ecriture du temporaire dans fichier jour.cvs")
    fichier = open("/var/www/html/jour_tmp.csv", "r" ) 
    contenu = fichier.read()
    Fichier.close()
    fichier_jour = '/var/www/html/jour.csv'	#definition de jour.csv 
    Fichier = open (fichier_jour,'a')
    Fichier.write ("\n"+contenu)
    Fichier.close()
	
    print("Ecriture du temporaire dans fichier semaine.cvs")
    fichier = open("/var/www/html/semaine_tmp.csv", "r" ) 
    contenu = fichier.read()
    Fichier.close()
    fichier_semaine = '/var/www/html/semaine.csv'	#definition de semaine.csv 
    Fichier = open (fichier_semaine,'a')
    Fichier.write ("\n"+contenu)
    Fichier.close()
	
    print("Ecriture du temporaire dans fichier mois.cvs")
    fichier = open("/var/www/html/mois_tmp.csv", "r" ) 
    contenu = fichier.read()
    Fichier.close()
    fichier_mois = '/var/www/html/mois.csv'	#definition de mois.csv 
    Fichier = open (fichier_mois,'a')
    Fichier.write ("\n"+contenu)
    Fichier.close()
    print(" ")
    print("                                                Fin pour cette reception")
    print("-" * n_tiret)
    print(" ")
    print(" ")
