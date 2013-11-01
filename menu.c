//
//  menu.c
//  Tastenblinky
//
//  Created by Ruedi Heimlicher on 27.10.2013.
//
//

#include <stdio.h>
#include <avr/pgmspace.h>



char DefaultTitel[]  PROGMEM ="RC-Sender";
char Laufzeit[]  PROGMEM ="Zeit";
char Einschaltzeit[]  PROGMEM ="ON";
char Stopuhr[]  PROGMEM ="Stop";
char Batterie[]  PROGMEM ="Bat";
PGM_P DefaultTable[] PROGMEM ={DefaultTitel, Laufzeit,Einschaltzeit ,Stopuhr,Batterie};



char Titel[]  PROGMEM ="Einstellungen";
char Modelwahl[]  PROGMEM ="Model:";
char Kanalwahl[]  PROGMEM ="Kanal";
char Mixwahl[]  PROGMEM ="Mix";
PGM_P TitelTable[] PROGMEM ={Titel, Modelwahl, Kanalwahl, Mixwahl};

char Werkstatt0[] PROGMEM = "Status\0";
char Werkstatt1[] PROGMEM = "Plan\0";
char Werkstatt2[] PROGMEM = "WS 2\0";
char Werkstatt3[] PROGMEM = "WS 3\0";
char Werkstatt4[] PROGMEM = "WS 4\0";
char Werkstatt5[] PROGMEM = "WS 5\0";
char Werkstatt6[] PROGMEM = "WS 6\0";
char Werkstatt7[] PROGMEM = "WS 7\0";
//PGM_P WerkstattTable[] PROGMEM = {Werkstatt0, Werkstatt1, Werkstatt2, Werkstatt3, Werkstatt4, Werkstatt5, Werkstatt6, Werkstatt7};




char Raum0[] PROGMEM = "Heizung\0";
char Raum1[] PROGMEM = "Werkstatt\0";
char Raum2[] PROGMEM = "WoZi\0";
char Raum3[] PROGMEM = "Buero\0";
char Raum4[] PROGMEM = "Labor\0";
char Raum5[] PROGMEM = "OG 1\0";
char Raum6[] PROGMEM = "OG 2\0";
char Raum7[] PROGMEM = "Estrich\0";
PGM_P RaumTable[] PROGMEM = {Raum0, Raum1, Raum2, Raum3, Raum4, Raum5, Raum6, Raum7};

/*
char Titel[] PROGMEM = "HomeCentral\0";
char Name[] PROGMEM = "Ruedi Heimlicher\0";
char Adresse[] PROGMEM = "Falkenstrasse 20\0";
char Ort[] PROGMEM = "8630 Rueti\0";
PGM_P P_StartTable[] PROGMEM = {Titel, Name, Adresse, Ort};
*/