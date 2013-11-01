// ********************************************************************
// Demoprogramm
// Inhalt: LCD (DOG-M 081/162/163) mit (ST7036 Controller) ansteuern,
// 5V/3V Variante, mit selbst definierten Zeichen im RAM, SPI-Ansteuerung seriell
// LCD-Controller: ST7036 kompatibel
// Stand: 18.04.2008, Autor: Matthias Kahnt, Url: pro-51.eltra-tec.de
// SDCC-Version: M-IDE-51/SDCC 2.8.0
// AT89C51ED2: --code-loc 0x0000 --iram-size 256 --xram-loc 0x0000 --xram-size 0x0700
// AT89S8253:  --code-loc 0x0000 --iram-size 256
// ********************************************************************
// Für 8051-Microcontroller, z.B. AT89C51ED2 oder AT89S8253
// Frequenz: 11.0592 MHz
// Code-Speicherbedarf: ca. 1.3kByte, keine XRAM-Verwendung
// Das Programm verwendet nur 8051/52 Standard-SFRs und wurde
// mit dem AT89C51ED2 auf dem PRO-51 System getestet.
// ********************************************************************
// Die Programmfunktionen dürfen für private Nutzung verwendet und
// verändert werden. Für gewerbliche Nutzung ist die Zustimmung
// des Autors erforderlich. Die Nutzung erfolgt auf eigene Gefahr.
// Für eventuell entstehende Schäden wird keine Haftung übernommen.
// ********************************************************************

#include <string.h>      // Stingfunktionen
#include <avr/eeprom.h>

// Typendefinition
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long dword;

char menubuffer[16];
char titelbuffer[16];


// Hardware PINs, bitte anpassen
#define DOG_PORT        PORTD
#define DOG_DDR        DDRD   

#define LCD_ON    4
#define LCD_RS    5
#define LCD_SCL   6
#define LCD_DATA  7

//#define LCD_CSB (CS-Signal: wird im Bsp. nicht verwendet)

// Displaytyp, anpassen
#define SPALTEN  16
#define ZEILEN   3
#define SPANNUNG 5

extern volatile uint16_t loopcount1;
extern volatile uint8_t modelnummer;

extern volatile uint8_t            blinkline;
extern volatile uint8_t            blinkcol;
extern volatile char            blinkzeichen;

extern volatile uint8_t            laufsekunde;
extern volatile uint8_t            laufminute;

// Tabelle für 8 selbst definierte Sonderzeichen
char  sonderzeichen[] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F, // 00-07 Zeichen an Adresse 0
  0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x1F, // 08-15 Zeichen an Adresse 1
  0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F, // 16-23 Zeichen an Adresse 2
  0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F, // 32-39 Zeichen an Adresse 3
  0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,0x1F, // 40-47 Zeichen an Adresse 4
  0x00,0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F, // 48-55 Zeichen an Adresse 5
  0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F, // 56-63 Zeichen an Adresse 6
  0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F  // 64-71 Zeichen an Adresse 7
};

const uint8_t  sonderzeichenA[] PROGMEM =
{
   0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03, // 00-07 Zeichen an Adresse 0 // Senkrechter Balken rechts
   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18, // 08-15 Zeichen an Adresse 1 // Senkrechter Balken links
   0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B, // 16-23 Zeichen an Adresse 2 // 2 senkrechte Balken
   0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F, // 32-39 Zeichen an Adresse 3
   0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,0x1F, // 40-47 Zeichen an Adresse 4
   0x00,0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F, // 48-55 Zeichen an Adresse 5
   0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F, // 56-63 Zeichen an Adresse 6
   0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F  // 64-71 Zeichen an Adresse 7
};

const uint8_t  sonderzeichenB[] PROGMEM =
{
   0x00,0x00,0x10,0x08,0x00,0x04,0x02,0x01, // 00-07 Zeichen an Adresse 0 // Senkrechter Balken rechts
   0x00,0x00,0x00,0x10,0x08,0x04,0x02,0x01, // 08-15 Zeichen an Adresse 1 // Senkrechter Balken links
   0x00,0x00,0x00,0x00,0x10,0x08,0x06,0x01, // 16-23 Zeichen an Adresse 2 // 2 senkrechte Balken
   0x00,0x00,0x00,0x00,0x00,0x10,0x0C,0x03, // 32-39 Zeichen an Adresse 3
   0x01,0x00,0x02,0x00,0x04,0x08,0x00,0x10, // 40-47 Zeichen an Adresse 4
   0x00,0x00,0x00,0x01,0x02,0x04,0x08,0x10, // 48-55 Zeichen an Adresse 5
   0x00,0x00,0x00,0x00,0x01,0x02,0x0C,0x10, // 56-63 Zeichen an Adresse 6
   0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x18  // 64-71 Zeichen an Adresse 7
};

const uint8_t  sonderzeichenC[] PROGMEM =
{
   0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 00-07 Zeichen an Adresse 0 // Senkrechter Balken rechts
   0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00, // 08-15 Zeichen an Adresse 1 // Senkrechter Balken links
   0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00, // 16-23 Zeichen an Adresse 2 // 2 senkrechte Balken
   0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x00, // 32-39 Zeichen an Adresse 3
   0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00, // 40-47 Zeichen an Adresse 4
   0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00, // 48-55 Zeichen an Adresse 5
   0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00, // 56-63 Zeichen an Adresse 6
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07  // 64-71 Zeichen an Adresse 7
};

const uint8_t  sonderzeichenD[]  PROGMEM =
{
   0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 00-07 Zeichen an Adresse 0 // Senkrechter Balken rechts
   0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00, // 08-15 Zeichen an Adresse 1 // Senkrechter Balken links
   0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00, // 16-23 Zeichen an Adresse 2 // 2 senkrechte Balken
   0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x00, // 32-39 Zeichen an Adresse 3
   0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00, // 40-47 Zeichen an Adresse 4
   0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00, // 48-55 Zeichen an Adresse 5
   0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00, // 56-63 Zeichen an Adresse 6
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07  // 64-71 Zeichen an Adresse 7
};





// Tabelle der DOG-M Initialisierung
char lcd_init_code[] = {
  0x31,0x1C,0x51,0x6A,0x74, // 00-04 DOG-M 081 5V
  0x31,0x14,0x55,0x6D,0x7C, // 05-09 DOG-M 081 3V
  0x39,0x1C,0x52,0x69,0x74, // 10-14 DOG-M 162 5V
  0x39,0x14,0x55,0x6D,0x78, // 15-19 DOG-M 162 3V
  0x39,0x1D,0x50,0x6C,0x77, // 20-24 DOG-M 163 5V
  0x39,0x15,0x55,0x6E,0x72  // 25-29 DOG-M 163 3V
};

// Tabelle der DOG-M Zeilenanfangsadressen
char  lcd_zeilen_adresse[] =
{
  0x00,0x00,0x00, // 00-02 DOG-M 081
  0x00,0x40,0x40, // 03-05 DOG-M 162
  0x00,0x10,0x20  // 06-08 DOG-M 163
};

// ********************************************************************
// Verzögerungsschleife für kurze Zeit
// Zeit: Wartezeit in [zeit]
//       1 = 20us
//       2 = 28us
//       3 = 36us u.s.w. (gilt für 11.0952MHz)
//     255 = ca. 2ms
// ********************************************************************

// ********************************************************************
// Verzögerungsschleife für lange Zeit
// Zeit: Wartezeit in [zeit]
//       1 = ca. 10ms
//       2 = ca. 20ms
//       3 = ca. 30ms u.s.w.
//     255 = ca. 2,5s (gilt für 11.0952MHz)
// ********************************************************************
void delay(uint8_t zeit)
{
uint8_t zaehler;
  for (zaehler = zeit; zaehler; zaehler--)
  {
    _delay_us(255);  // dauert ca. 2ms
    _delay_us(255);  // dauert ca. 2ms
    _delay_us(255);  // dauert ca. 2ms
    _delay_us(255);  // dauert ca. 2ms
    _delay_us(255);  // dauert ca. 2ms
  }
}

// ***********************************************************************
// Schreiben eines Zeichens an das LCD-Modul
// seriell Soft-SPI Mode, 3/4 Draht Interface
// Übergabe: lcd_byte : Auszugebendes Zeichen/Steuerzeichen
//           lcd_mode : 0 - Daten
//                      1 - Steuerzeichen
// ***********************************************************************
void write_lcd(uint8_t lcd_byte, uint8_t lcd_mode)
{
byte stelle;
  // Pause bevor nächstes Zeichen gesendet werden darf
  _delay_us(5); // Pause mind. 26,3us
  // LCD_CSB = 0;
  if (lcd_mode)
  {
     
  DOG_PORT &= ~(1<<LCD_RS ); // Steuerzeichen
}
    else
{
   
   DOG_PORT |=  (1<<LCD_RS );        // Daten
}
  // Byte senden seriell senden, H-Bit zuerst (Bit 8 7 6 5 4 3 2 1 0)
  for ( stelle = 0x80; stelle; stelle >>= 1 )
{
   //LCD_DATA = lcd_byte & stelle;
   if (lcd_byte & stelle)
   {
      DOG_PORT |=  (1<< LCD_DATA);// bit ist 1
   }
   else
   {
      DOG_PORT &=  ~(1<< LCD_DATA); // bit ist 0
   }
   
    DOG_PORT &= ~(1<< LCD_SCL);
    DOG_PORT |=  (1<< LCD_SCL);
  }
  // LCD_CSB = 1;
}

// ***********************************************************************
// Definieren eines Sonderzeichen
// Ubergabe: lcd_addr : Adresse
//           lcd_zeichen: Zeiger auf das 1. Byte der Zeichendefinition
// ***********************************************************************
void write_lcd_cg(byte lcd_addr, byte *lcd_zeichen)
{
byte lcd_i;
  write_lcd(0x38,1); // Function Set DL=1 N=1 DH=0 IS2=0 IS1=0 IS Table 0
  for(lcd_i = 0; lcd_i < 8; lcd_i++) {
    write_lcd(0x40 + lcd_addr * 8 + lcd_i,1); // CG RAM Adresse Set (01aa azzz)
    write_lcd(*lcd_zeichen,0);  // Data Write 8x Pixelzeile
    lcd_zeichen++;
  }
  write_lcd(0x39,1); // Function Set DL=1 N=1 DH=0 IS2=0 IS1=1 IS Table 1
}

// ***********************************************************************
// Definieren eines Sonderzeichen aus PROGMEM
// Ubergabe: lcd_addr : Adresse
//           lcd_zeichen: Zeiger auf das 1. Byte der Zeichendefinition
// ***********************************************************************
void write_ee_lcd_cg(byte lcd_addr, byte *lcd_zeichen)
{
   byte lcd_i;
   write_lcd(0x38,1); // Function Set DL=1 N=1 DH=0 IS2=0 IS1=0 IS Table 0
   for(lcd_i = 0; lcd_i < 8; lcd_i++)
   {
      write_lcd(0x40 + lcd_addr * 8 + lcd_i,1); // CG RAM Adresse Set (01aa azzz)
      write_lcd(pgm_read_byte(lcd_zeichen),0);  // Data Write 8x Pixelzeile
      lcd_zeichen++;
   }
   write_lcd(0x39,1); // Function Set DL=1 N=1 DH=0 IS2=0 IS1=1 IS Table 1
}




// ***********************************************************************
// Definieren von 8 Sonderzeichen
// Ubergabe:  lcd_zeichen: Zeiger auf das Zeichenfeld (8x8 Byte)
// ***********************************************************************
void write_lcd_cg_block(byte *sonderzeichenblock) {
byte lcd_i;
  for(lcd_i = 0; lcd_i < 8; lcd_i++) {
    write_lcd_cg(lcd_i, &sonderzeichenblock[lcd_i * 8]);
  }
} 

// ***********************************************************************
// Definieren von 8 Sonderzeichen aus PROGMEM
// Ubergabe:  lcd_zeichen: Zeiger auf das Zeichenfeld (8x8 Byte)
// ***********************************************************************
void write_ee_lcd_cg_block(byte *sonderzeichenblock)
{
   byte lcd_i;
   for(lcd_i = 0; lcd_i < 8; lcd_i++) {
       write_ee_lcd_cg(lcd_i, &sonderzeichenblock[lcd_i * 8]);
   }
} 




// ***********************************************************************
// Löschen des LCD-Display
// und Kursor auf Position 0,0 setzen
// ***********************************************************************
void clear_lcd(void) {
  write_lcd(0x01,1);
  delay(20);
} 

// ***********************************************************************
// Ausgabe eines ASCII-Zeichen positioniert auf dem LCD-Modul
// ¨bergabe: lcd_x : Spalte (0...SPALTEN-1)
//           lcd_y : Zeile  (0...ZEILEN-1)
//           lcd_ascii : ASCII-Zeichen
// *********************************************************************** 
void write_byte_lcd(byte lcd_x, byte lcd_y, byte lcd_ascii)
{
byte lcd_offset = 0;
  if (lcd_x > (SPALTEN - 1)) lcd_x = 0;
  if (lcd_y > (ZEILEN  - 1)) lcd_y = 0;
  switch (lcd_y)
   {
    case 0:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3];     break; // Zeile 1
    case 1:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 1]; break; // Zeile 2 
    case 2:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 2]; break; // Zeile 3
  };
  write_lcd(0x80 + lcd_x + lcd_offset,1); // Kursorposition setzen
  write_lcd(lcd_ascii,0);                 // Ausgabe des ASCII-Zeichen an der Kursorposition
}

// ***********************************************************************
// Ausgabe einer Zeichenkette positioniert auf dem LCD-Modul
// Ubergabe: lcd_x : Spalte (0...SPALTEN-1)
//           lcd_y : Zeile  (0...ZEILEN-1)
//           lcd_zeichen : Adresse der auszugebenden format.  Zeichenkette
//           clr_line    : Löschen bis Zeilenende
//                         1 - Löschen
//                         0 - kein Löschen
// ***********************************************************************
void printf_lcd(byte lcd_x, byte lcd_y, char* lcd_zeichen, uint8_t clr_line) {
byte lcd_i;
byte lcd_offset = 0;
  if (lcd_x > (SPALTEN - 1)) lcd_x = 0;
  if (lcd_y > (ZEILEN  - 1)) lcd_y = 0;
  switch (lcd_y) {
    case 0:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3];     break; // Zeile 1
    case 1:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 1]; break; // Zeile 2 
    case 2:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 2]; break; // Zeile 3
  }
  write_lcd(0x80 + lcd_x + lcd_offset,1); // Kursorposition setzen
  // Ausgabe der Zeichenkette ab der Kursorposition
  lcd_offset = strlen(lcd_zeichen); // Länge der Zeichenkette
  if (lcd_offset > SPALTEN) lcd_offset = SPALTEN;
  for(lcd_i = lcd_offset; lcd_i; lcd_i--) {
    write_lcd(*lcd_zeichen,0); lcd_zeichen++;
  }
  if (clr_line) {
    // Löschen bis Zeilenende
    for(lcd_i = SPALTEN - lcd_offset - lcd_x; lcd_i; lcd_i--)
      write_lcd(' ',0);
  }
}


void printf_menu_lcd(byte lcd_x, byte lcd_y, char* lcd_zeichen)
{
   byte lcd_i;
   byte lcd_offset = 0;
   if (lcd_x > (SPALTEN - 1)) lcd_x = 0;
   if (lcd_y > (ZEILEN  - 1)) lcd_y = 0;
   switch (lcd_y) {
      case 0:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3];     break; // Zeile 1
      case 1:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 1]; break; // Zeile 2
      case 2:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 2]; break; // Zeile 3
   }
   write_lcd(0x80 + lcd_x + lcd_offset,1); // Kursorposition setzen
   // Ausgabe der Zeichenkette ab der Kursorposition
   
    write_lcd(0x7e,0);
   lcd_offset = strlen(lcd_zeichen); // Länge der Zeichenkette
   if (lcd_offset > SPALTEN) lcd_offset = SPALTEN;
   
   for(lcd_i = lcd_offset; lcd_i; lcd_i--) {
      write_lcd(*lcd_zeichen,0); lcd_zeichen++;
   }
   write_lcd(8,0);
  
}

// ***********************************************************************
// Schreiben einer Zahl.
// Ubergabe: lcd_x : Spalte (0...SPALTEN-1) Position der Einer
//           lcd_y : Zeile  (0...ZEILEN-1)

//           lcd_zahl :    uint16_t Zahl
// ***********************************************************************
void write_zahl_lcd(uint8_t lcd_x, uint8_t lcd_y, uint16_t ausgabezahl,uint8_t stellen)
{
   uint16_t zahl = ausgabezahl;
   write_byte_lcd(lcd_x,lcd_y,(zahl%10 + '0')); // einer immer schreiben
   if (stellen == 1)
   {
      return;
   }
   
   zahl /= 10;
   if (zahl)
   {
      write_byte_lcd(lcd_x-1,lcd_y,(zahl%10 + '0')); // zehner schreiben
   }
   
   else if (ausgabezahl>99)
   {
      write_byte_lcd(lcd_x-1,lcd_y,'0'); // es gibt hunderter, 0 schreiben
      
   }
   else
   {
      write_byte_lcd(lcd_x-1,lcd_y,32); // keine zehner
   }
   if (stellen == 2)
   {
      return;
   }

   
   
   zahl /= 10;
   if (zahl)
   {
      write_byte_lcd(lcd_x-2,lcd_y,(zahl%10 + '0'));
   }
   else if (ausgabezahl>999)
   {
      write_byte_lcd(lcd_x-2,lcd_y,'0'); // es gibt tausender, 0 schreiben
   }
   else
   {
      write_byte_lcd(lcd_x-2,lcd_y,32); // leerzeichen
      
   }
   if (stellen == 3)
   {
      return;
   }


   zahl /= 10;
   if (zahl)
   {
      write_byte_lcd(lcd_x-3,lcd_y,(zahl%10 + '0'));
   }
   else if (ausgabezahl>9999)
   {
      write_byte_lcd(lcd_x-3,lcd_y,'0'); // es gibt zehntausender, 0 schreiben
   }
   else
   {
      write_byte_lcd(lcd_x-3,lcd_y,32); // leerzeichen
   
   }
   if (stellen == 4)
   {
      return;
   }
   zahl /= 10;
   if (zahl)
   {
      write_byte_lcd(lcd_x-4,lcd_y,(zahl%10 + '0'));
   }
   else if (ausgabezahl>99999)
   {
      write_byte_lcd(lcd_x-4,lcd_y,'0'); // es gibt zehntausender, 0 schreiben
   }
   else
   {
      write_byte_lcd(lcd_x-4,lcd_y,32); // leerzeichen
      
   }
   if (stellen == 5)
   {
      return;
   }


}



void write_zahl2_lcd(uint8_t lcd_x, uint8_t lcd_y, uint16_t ausgabezahl)
{
   uint16_t zahl = ausgabezahl;
   write_byte_lcd(lcd_x,lcd_y,(zahl%10 + '0')); // einer immer schreiben
    
   zahl /= 10;
   write_byte_lcd(lcd_x-1,lcd_y,(zahl%10 + '0')); // zehner schreiben
   
   
   
}

// ***********************************************************************
// Blinkposition/Cursor auf dem LCD-Displays setzen
// Ubergabe: lcd_x : Spalte (0...SPALTEN-1)
//           lcd_y : Zeile  (0...ZEILEN-1)
//           lcd_blink :   0 - Blinken/Cursor aus
//                         1 - Blinken an
//                         2 - Cursor  an
// ***********************************************************************
void blink_lcd(byte lcd_x, byte lcd_y, byte lcd_blink) {
byte lcd_offset = 0;
  write_lcd(0x0C,1); // KURSOR ausschalten
  if (lcd_x > (SPALTEN - 1)) lcd_x = 0;
  if (lcd_y > (ZEILEN  - 1)) lcd_y = 0;
  switch (lcd_y) {
    case 0:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3];     break; // Zeile 1
    case 1:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 1]; break; // Zeile 2 
    case 2:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 2]; break; // Zeile 3
  };
  write_lcd(0x80 + lcd_x + lcd_offset,1); // Blinkposition setzen
  if (lcd_blink == 1) write_lcd(0x0D,1);  // Blinken ein
  if (lcd_blink == 2) write_lcd(0x0E,1);  // Cursor ein
}

// ***********************************************************************
// Blinken an Position
// Ubergabe: lcd_x : Spalte (0...SPALTEN-1)
//           lcd_y : Zeile  (0...ZEILEN-1)
//           lcd_blink :   0 - Blinken/Cursor aus
//                         1 - Blinken an
//                         2 - Cursor  an
// ***********************************************************************
void blinktext_lcd(byte lcd_x, byte lcd_y, char blinkzeichen)
{
   
   byte lcd_offset = 0;
   //write_lcd(0x0C,1); // KURSOR ausschalten
   if (lcd_x > (SPALTEN - 1)) lcd_x = 0;
   if (lcd_y > (ZEILEN  - 1)) lcd_y = 0;
   switch (lcd_y) {
      case 0:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3];     break; // Zeile 1
      case 1:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 1]; break; // Zeile 2
      case 2:  lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3 + 2]; break; // Zeile 3
   };

   //lcd_offset = lcd_zeilen_adresse[(ZEILEN - 1) * 3]+lcd_y;
   write_lcd(0x80 + lcd_x + lcd_offset,1); // Blinkposition setzen
   
   if (loopcount1%2)
   {
      
      write_byte_lcd(lcd_x,lcd_y,blinkzeichen);
   }
   else
   {
       write_byte_lcd(lcd_x,lcd_y,' ');
   }
    
}

// ***********************************************************************
// Grundinitialisierung des LCD-Moduls in SPI-Mode (seriell)
// ***********************************************************************
#pragma save
#pragma disable_warning 126
void init_lcd(void)
{
   byte offset;
//   LCD_ON  = 1; // LCD Beleuchtung einschalten
   delay(5); // ca. 50ms Wartezeit nach dem Einschalten
   offset = (ZEILEN - 1) * 10;     // Offset = 00, 10, 20
   if (SPANNUNG != 5) offset += 5; // Offset = 05, 15, 25
   // Grundinitialisierung (SPI, wie im 8-Bit parallel-Mode)
   write_lcd(lcd_init_code[offset],1);     // Function Set
   write_lcd(lcd_init_code[offset],1);     // Function Set (gleiches Byte nochmal senden)
   write_lcd(lcd_init_code[offset + 1],1); // Bias Set
   write_lcd(lcd_init_code[offset + 2],1); // Power Control + Kontrast Set C5,C4
   write_lcd(lcd_init_code[offset + 3],1); // Follower Control
   write_lcd(lcd_init_code[offset + 4],1); // Kontrast Set C3,C2,C1,C0
   write_lcd(0x0C,1); // Display Set
   write_lcd(0x06,1); // Entry Mode Set
   clear_lcd();       // Display löschen
}
#pragma restore


void displaynav(const uint8_t page,const uint8_t col,const uint8_t line)
{
   write_byte_lcd(14,0, 'P');
   write_byte_lcd(15,0, page + '0');
   write_byte_lcd(14,1, 'L');
   write_byte_lcd(15,1, col + '0');
   write_byte_lcd(14,2, 'C');
   write_byte_lcd(15,2, line + '0');
   
}

void displaypage(uint8_t page,uint8_t line,uint8_t col)
{
   switch (page)
   {
      case 0:
      {
         // zeile 0
         strcpy_P(titelbuffer, (PGM_P)pgm_read_word(&(DefaultTable[0])));
         printf_lcd(0,0,titelbuffer,1); //
         write_zahl2_lcd(navigation[page][0][1],0,laufminute);
         write_byte_lcd((navigation[page][0][2]),0,':'); //
         write_zahl2_lcd(navigation[page][0][3],0,laufsekunde);
         // zeile 1
         //Gesamtzeit
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(DefaultTable[1])));
         printf_lcd((navigation[page][1][0]),1,menubuffer,1); //

         // Einschaltzeit
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(DefaultTable[2])));
         printf_lcd((navigation[page][1][3]),1,menubuffer,0); //

         // zeile 2
         // Stoppuhr
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(DefaultTable[3])));
         printf_lcd((navigation[page][2][0]),2,menubuffer,1); //

         // Batteriespannung
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(DefaultTable[4])));
         printf_lcd((navigation[page][2][2]),2,menubuffer,0); //

         // modelnummer
         //write_zahl_lcd(navigation[page][0][1],1,modelnummer,1);
         
           /*
         printf_lcd((navigation[page][0][2]),1,"M",0); //
         
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(DefaultTable[2])));
         
         printf_lcd((navigation[page][2][0]),2,menubuffer,0); //
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(DefaultTable[3])));
         printf_lcd((navigation[page][2][1]),2,menubuffer,0); //
       
          blinkline=1;
          blinkcol=navigation[page][1][1];
          blinkzeichen = modelnummer + '0';
          */
         
      }break;

      case 1:
      {

         strcpy_P(titelbuffer, (PGM_P)pgm_read_word(&(TitelTable[0])));	//Untermenu im PROGMEM
         printf_lcd(0,0,"Hallo",1); // Einschaltmeldung
         
         strcpy_P(titelbuffer, (PGM_P)pgm_read_word(&(TitelTable[0])));
         printf_lcd(0,0,titelbuffer,0); //
 
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(TitelTable[1])));
         printf_lcd((navigation[page][1][0]),1,menubuffer,1); //

         // modelnummer
         write_zahl_lcd(navigation[page][1][1],1,modelnummer,1);
         printf_lcd((navigation[page][1][2]),1,"MS 0",0); //
         
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(TitelTable[2])));
         
         printf_lcd((navigation[page][2][0]),2,menubuffer,1); //
         strcpy_P(menubuffer, (PGM_P)pgm_read_word(&(TitelTable[3])));
         printf_lcd((navigation[page][2][1]),2,menubuffer,0); //
      /*
         blinkline=1;
         blinkcol=navigation[page][1][1];
         blinkzeichen = modelnummer + '0';
        */ 

      }break;
         
      case 2:
      {
         
      }break;
   }// switch
   
 
}

