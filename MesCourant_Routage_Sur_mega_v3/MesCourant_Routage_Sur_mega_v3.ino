
// Version Mesure-Routeur V3  MRV3
//
//
// Version installée le 02 dec 23
//  OK au 14 dec 23
//
// Projet avec la carte inteface  CarteInterfaceCourantMEGA_V7
// Projet de mesure des courant de la maison et de delestage sur ballon eau chaude ou ballon pac
// Retour de calcul sur raspberry toutes les 30" pour les graphiques
// Sur mega
// odisseano@gmail.com

// EmonLib librairie pour acquisition des mesures sur capteur de courant
// Wire pour communication IIC avec l'afficheur
// rgb_lcd lib pour l'afficheur
// running average  lib de moyennage des valeurs
// dimmable_light.h controle en pwm des variateurs AC Dimmer de robot dyn



#include "EmonLib.h"  // Inclusion des lib
#include <Wire.h>
#include "rgb_lcd.h"
#include "math.h"
#include "RunningAverage.h"
#include "dimmable_light.h"

EnergyMonitor emon1;    //Tension general et courant général sur A0 et A1
EnergyMonitor emon2;    // solaire sur A2
EnergyMonitor emon3;    // Chauffe eau sur A3
EnergyMonitor emon4;    // PAC sur A4
EnergyMonitor emon5;    // ZOE sur A5

RunningAverage Usecteur(5); // creation des objets de moyennage sur 5 valeurs
RunningAverage Igeneral(5);
RunningAverage Isolaire(5);  
RunningAverage IChEau(5);
RunningAverage IPAC(5);
RunningAverage IZOE(5);

rgb_lcd lcd;

const int cons = 5;
const int tempo = 2000;
const int bouton_defil = 13;    // bouton sur entree num 13
//const int bouton_defil = 12;    // bouton sur entree num 12
const int led_ch_eau=6;
const int led_pac=7;

//float pot=0;
//float test_power=0;
int nb_boucle =  90;  // itération; envoie des données toutes les xxx  100mes/min      
int n = 0;        
int i = 0;
int val_bouton = 0;

float i_solaire =0;
float u_secteur = 0;
float i_general = 0;         
float i_PAC = 0;        
float i_chauffe_eau = 0;         
float i_ZOE = 0;         
float i_fct_maison = 0 ;
float realPower = 0;
float apparentPower = 0;
float powerFActor = 0;

const int syncPin = 2;
int com_varia=0;
int com_varia1 = 0;
int com_varia2 = 0;

DimmableLight light1(4);
DimmableLight light2(5);

// declaration des sous programmes
void lcp_print(void);
void trame();


void setup() 
{
pinMode(led_ch_eau, OUTPUT);
pinMode(led_pac, OUTPUT);
pinMode(bouton_defil, INPUT);    // en entree patte 13

digitalWrite(led_ch_eau,LOW);
digitalWrite(led_pac,LOW);

n = nb_boucle - 2;      // pour faire un envoie des données assez rapide au demarrage puisque la première trame est supprimée par le code de réception en python

lcd.begin(16, 2);
lcd.setCursor(0, 0);
lcd.print("Mesure Routeur ");
lcd.setCursor(0, 1);
lcd.print("Version 3");
delay(tempo);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("14 dec 2023     ");
lcd.setCursor(0, 1);
lcd.print("MRV3 sur MEGA");
delay(tempo);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("zobielamouche");
lcd.setCursor(0, 1);
lcd.print("odisseano@gmail ");
delay(tempo);
lcd.clear();

Serial.begin(9600);

emon1.voltage(A0, 207, 1.7);           //   Tension secteur                           calibré le 01 dec 23
emon1.current(A1, 61.5);               //   Courant general                           calibré le 13 oct 23                    
emon2.current(A2, 30);               //   Solaire                           pas de calibrage                  
emon3.current(A3, 30);               //   Courant chauffe eau               pas de calibrage            
emon4.current(A4, 60);               //   Courant PAC                       pas de calibrage                                  
emon5.current(A5, 25.5);               //   Courant ZOE                               calibré le 5 nov 23

Isolaire.clear();
Usecteur.clear();
Igeneral.clear();
IPAC.clear();
IChEau.clear();
IZOE.clear();
//IFct.clear();

DimmableLight::setSyncPin(syncPin);
DimmableLight::begin();

light1.setBrightness(com_varia1);
light2.setBrightness(com_varia2);

}


void loop() 
{

//pot = analogRead(A8)/100;  // pour les tests avec potentiometre

// Sur les entrées A0 et A1 mesure courant général et tension secteur  Production tension courant et puissances
//-------------------------------------------------------------------------------------------------------------------------
emon1.calcVI(20, 100);  // Solaire Calculate all. No.of half wavelengths (crossings), time-out  Periodicité des calculs
Usecteur.addValue(emon1.Vrms);      //acquisition de la valeur Vrms sur emon01 dans la fonction de moyennage
u_secteur = Usecteur.getAverage();   // on retourne le résultat de la moyenne dans u_secteur
Igeneral.addValue(emon1.Irms - 0.2);
i_general = Igeneral.getAverage();
if (i_general < 0.05)  { i_general = 0; }

realPower       = emon1.realPower/1000;             //realPower = realPower / 1000;                                              
apparentPower   = emon1.apparentPower/1000;         //apparentPower = apparentPower / 1000;                                                 
powerFActor     = emon1.powerFactor;


// Sur le entrées A2 mesure courant solaire
//-------------------------------------------------------------------------------------------------------------------------
emon2.calcVI(20, 100);  
Isolaire.addValue(emon2.Irms - 0.1);
i_solaire = Isolaire.getAverage();
if (i_solaire < 0.05)  { i_solaire = 0; }

// Sur l' entrée A3 mesure courant chauffe eau
//-------------------------------------------------------------------------------------------------------------------------
emon3.calcVI(20, 100);  
IChEau.addValue(emon3.Irms - 0.1);
i_chauffe_eau = IChEau.getAverage();
if (i_chauffe_eau < 0.05)   { i_chauffe_eau = 0; }

// Sur l' entrée A4 mesure courant PAC
//-------------------------------------------------------------------------------------------------------------------------
emon4.calcVI(20, 100); 
IPAC.addValue(emon4.Irms - 0.1);
i_PAC = IPAC.getAverage();
if (i_PAC < 0.05)  { i_PAC = 0; }

// Sur l' entrée A5 mesure courant voiture
//-------------------------------------------------------------------------------------------------------------------------
emon5.calcVI(20, 100);
IZOE.addValue(emon5.Irms - 0.1);
i_ZOE = IZOE.getAverage();
if (i_ZOE < 0.05)  { i_ZOE = 0; }

// Calcul du courant restant
//-------------------------------------------------------------------------------------------------------------------------
i_fct_maison = i_general - i_PAC - i_chauffe_eau - i_ZOE;
if (i_fct_maison < 0)  { i_fct_maison = 0; }

// Lecture de l'état du bouton et incrementation
//-------------------------------------------------------------------------------------------------------------------------
val_bouton = digitalRead(bouton_defil);   
if (val_bouton == 1)  { i++; }    //outVal = outVal + 1; 

// Calcul et commande des variateurs si surproduction
//-------------------------------------------------------------------------------------------------------------------------
//test_power = pot - 5;
//if (test_power<0)  { com_varia = com_varia + cons; }  //si courant injecté sur réseau edf en test avec pot

if (realPower<0)  { com_varia = com_varia + cons; }  //si courant injecté sur réseau edf
  else {com_varia = com_varia - cons;}
  
com_varia = constrain(com_varia, 0, 510);

com_varia1=com_varia;
com_varia2=com_varia-255;

if (com_varia1>5)  { digitalWrite(led_ch_eau,HIGH);}
  else { digitalWrite(led_ch_eau,LOW); }

if (com_varia2>5)  { digitalWrite(led_pac,HIGH);}
  else { digitalWrite(led_pac,LOW); }

com_varia1 = constrain(com_varia1, 0, 255);
com_varia2 = constrain(com_varia2, 0, 255);

light1.setBrightness(com_varia1);   //pilotage des variateurs
light2.setBrightness(com_varia2);


lcp_print();
n ++;
if (n > nb_boucle)
  {
  trame();
  n = 0;
  }
}

/**********************************************************************************************
/* Envoie sur trame pour raspberry
/**********************************************************************************************/
void trame()
{
lcd.setCursor(12, 1);
lcd.print("Send");
Serial.print(",");
Serial.print(i_solaire);
Serial.print(",");
u_secteur = u_secteur / 10;     // pour avoir la tension secteur dans la meme représentation que les courants
Serial.print(u_secteur);
Serial.print(",");
Serial.print(i_general);
Serial.print(",");
Serial.print(i_ZOE);
Serial.print(",");
Serial.print(i_chauffe_eau);
Serial.print(",");
Serial.print(i_PAC);
Serial.print(",");
Serial.print(i_fct_maison);
Serial.println(",");
}

/**********************************************************************************************/
/* affichage sur lcd
/**********************************************************************************************/
void lcp_print(void)
{

if ( i == 0)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("u secteur");
  lcd.setCursor(0, 1);
  lcd.print(u_secteur, 1);
  lcd.print(" Amp");
  }

if ( i == 1)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("i general");
  lcd.setCursor(0, 1);
  lcd.print(i_general, 1);
  lcd.print(" Amp");
  }

if ( i == 2)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("i solaire");
  lcd.setCursor(0, 1);
  lcd.print(i_solaire, 1);
  lcd.print(" Amp");
  }

if ( i == 3 )
  {  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("i chauffe eau");
  lcd.setCursor(0, 1);
  lcd.print(i_chauffe_eau, 1);
  lcd.print(" Amp");

  }

if ( i == 4)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("i PAC");
  lcd.setCursor(0, 1);
  lcd.print(i_PAC, 1);
  lcd.print(" Amp");
  }

if ( i == 5)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("i charge ZOE");
  lcd.setCursor(0, 1);
  lcd.print(i_ZOE, 1);
  lcd.print(" Amp");
  }

if ( i == 6)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Courant de fonctionnement");
  lcd.setCursor(0, 1);
  lcd.print(i_fct_maison, 1);
  lcd.print(" Amp");
  }

if ( i == 7)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("W reelle");
  lcd.setCursor(0, 1);
  lcd.print(realPower, 1);
  lcd.print(" K Watt");
  }
if ( i == 8)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("W Apparente");
  lcd.setCursor(0, 1);
  lcd.print(apparentPower, 1);
  lcd.print(" K Watt");
  }
if ( i == 9)
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Facteur W");
  lcd.setCursor(0, 1);
  lcd.print(powerFActor);
  }

if ( i == 10) { i=0; }

}

/*lcd.clear();              // pour test avec pot
  lcd.setCursor(0, 0);
  lcd.print(com_varia);
  lcd.print("         ");
  lcd.print(com_varia1);
  lcd.setCursor(0, 1);
  lcd.print(realPower);  //lcd.print(realPower, 1);
  lcd.print(" V      ");
  lcd.print(com_varia2);*/


/*
com_varia1++;
com_varia2 = com_varia2+10;
Serial.print(val);
Serial.print("   ");
Serial.println(com_varia2);
if (com_varia1>254) {com_varia1=0;}
if (com_varia2>254) {com_varia2=0;}*/



/*emon2.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
realPower       = emon2.realPower;        //extract Real Power into variable
apparentPower   = emon2.apparentPower;    //extract Apparent Power into variable
powerFActor     = emon2.powerFactor;      //extract Power Factor into Variable
supplyVoltage   = emon2.Vrms;             //extract Vrms into Variable
Irms            = emon2.Irms;             //extract Irms into Variable
100mes/min
*/
