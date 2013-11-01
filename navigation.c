//
//  navigation.c
//  Tastenblinky
//
//  Created by Ruedi Heimlicher on 30.10.2013.
//
//

#include <stdio.h>
#include <avr/pgmspace.h>

#include "menu.c"

//#include "dogm.c"
//extern void write_byte_lcd(byte x, byte y, byte z);
extern volatile uint8_t  navigation[8][8][8];
extern volatile uint8_t  cursor[8][8][8];

void initnav(void)
{
   uint8_t i=0, k=0, l=0;
   for (i=0;i<8;i++)
   {
      for (k=0;k<8;k++)
      {
         for (l=0;l<8;l++)
         {
            navigation[i][k][l] = 0xFF;
           // cursor[i][k][l] = 0xFF;
         }

      }

   }
   
   // page 0 // fixpage
   //zeile 0: Titelzeile
   navigation[0][0][0] = 0; //
   navigation[0][0][1] = 12; // laufminute
   navigation[0][0][2] = 13; // ':'
   navigation[0][0][3] = 15; // laufsekunde
   
   // zeile 1: Zeit
   navigation[0][1][0] = 0; // "Zeit"
   navigation[0][1][1] = 6; // Gesamtzeit zahl
   navigation[0][1][3] = 10; // "ON"
   navigation[0][1][4] = 12; // ON-Zeit zahl
   
   
   // Zeile 2
   navigation[0][2][0] = 0; // "Stop"
   navigation[0][2][1] = 6; // stopzeit zahl
   navigation[0][2][2] = 10; // "Bat"
   navigation[0][2][3] = 12; // Batteriespannung zahl

   
   
   // page 1
   //zeile 0: Titelzeile
   navigation[1][0][0] = 0; //
   // zeile 1: Modell-Zeile. "Model" an 0, nummer an 6, Bezeichnung an 8

   navigation[1][1][0] = 0; //
   navigation[1][1][1] = 6;
   navigation[1][1][2] = 8;
   
   // zeile 2: Menu-Zeile. "Kanal" an 0, "Mix" an 7
   navigation[1][2][0] = 0;
   navigation[1][2][1] = 8;

   // page 1: Kanal
   // zeile 0: "Nr" an 0, Nummer an 2, "Fkt" an 4, Funktion an 7, "Ri" an 13, richtung an 15
    navigation[2][0][0] = 0;
    navigation[2][0][1] = 2;
    navigation[2][0][2] = 4;
    navigation[2][0][3] = 7;
    navigation[2][0][4] = 13;
    navigation[2][0][5] = 15;
   // zeile 1: "Mode" an 0, nummer an 5, Bezeichnung an 7
   navigation[2][1][0] = 0;
   navigation[2][1][1] = 5;
   navigation[2][1][2] = 7;
   // zeile 2: Menu-Zeile "Level" an 0, "Expo" an 7
   navigation[2][2][0] = 0;
   navigation[2][2][1] = 7;
   
   // page 3: Kanal Level
   // zeile 0:
   
   // zeile 1:
   
   // zeile 2:

   
   // page 3: Kanal Expo
   // zeile 0:
   
   // zeile 1:
   
   // zeile 2:
   
   // page 4: Mix
   // zeile 0:
   
   // zeile 1:
   
   // zeile 2:


}