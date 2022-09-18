/*===========================================================================*=
   CFPT - Projet : projet_exam1
   Auteur        : Guillaume Guillet
   Date cr�ation : 08.11.2016
  =============================================================================
   Descriptif:
   Ce programme simule le panneau de controle d'un ascenseur avec 3 �tage.
=*===========================================================================*/
#include <REG51F380.h>     // registres 8051f380
//#include <Delay.h>
// ==== Fonctions prototypes ==================================================
void ClockInit ();         // init. clock syst�me
void PortInit ();          // init. config des ports
// ==== Equivalences ==========================================================

//MACRO - port
#define POUT  P1    // P1, port de sortie
#define PIN   P2    // P2, Port d'entr�e

//MACRO - etats
#define ETATS_FERMER 0
#define ETATS_OUVERT 1
#define ETATS_FERMETURE 2
#define ETATS_OUVERTURE 3
#define ETATS_MOUVEMENT 4

//SBIT - entree

//SBIT - sortie

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*                                  MAIN                                       */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
void main ()
{
   //--- Variables locales ----------------------------------------------------
   unsigned char compteur1 = 0;
   unsigned char compteur2 = 0;
   unsigned char compteur3 = 0;
   unsigned char compteur4 = 0;
   unsigned char entree = 0;
   //--------------------------------------------------------------------------

   PCA0MD &= ~0x40;     // WDTE = 0 (disable watchdog timer)
   ClockInit ();        // init. clock syst�me
   PortInit ();         // init. config des ports

   POUT = 0;

   while (1)
   {
       entree = PIN&0xF0;

      POUT = (POUT&0x0F)|entree;
   } //--- End while (1)

} /*%%%%%%%%%%%%%%%%%%%%%%%%%%%% END MAIN %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*---------------------------------------------------------------------------*-
   ClockInit ()
  -----------------------------------------------------------------------------
   Descriptif: Initialisation du mode de fonctionnement du clock
   Entr�e    : --
   Sortie    : --
-*---------------------------------------------------------------------------*/
void ClockInit ()
{
                 //       ++-- Oscillateur interne
                 //       ||
    CLKSEL = 0x00;   // 00000000   clock system = clock interne

                     // +--------- clock interne autoris�
                     // |     ++-- f = 12 MHZ
                     // |     ||
    OSCICN = 0x83;   // 10000011

} // ClockInit ----------------------------------------------------------------

/*---------------------------------------------------------------------------*-
   PortInit ()
  -----------------------------------------------------------------------------
   Descriptif: Initialisation du mode de fonctionnement des ports
   Entr�e    : --
   Sortie    : --
-*---------------------------------------------------------------------------*/
void PortInit ()
{
   // Type d'entr�e Analogique (0) / Num�rique (1)
   P0MDIN  = 0xFF;      // port P0 entr�e num�rique (valeur par d�faut)
   P1MDIN  = 0xFF;      // port P1 entr�e num�rique (valeur par d�faut)
   P2MDIN  = 0xFF;      // port P2 entr�e num�rique (valeur par d�faut)
   P3MDIN  = 0xFF;      // port P3.0 entr�e num�rique (valeur par d�faut)

   // Type de sortie Open Drain (0) / Push-Pull (1)
   P0MDOUT = 0x00;      // port P0 en entr�e num�rique open drain (valeur par d�faut)
   P1MDOUT = 0xFF;      // port P1 en sortie num�rique (push-pull)
   P2MDOUT = 0x00;      // port P2 en entr�e num�rique open drain (valeur par d�faut)
   P3MDOUT = 0x00;      // port P3 en entr�e num�rique open drain (valeur par d�faut)

   // R�servation des bits de port Libre (0) / R�serv� (1)
   P0SKIP  = 0x00;      // port P0 libre (valeur par d�faut)
   P1SKIP  = 0xFF;      // port P1 r�serv�
   P2SKIP  = 0xFF;      // port P2 r�serv�
						// Remarque pour P2 pas de skip possible pour bits P2.4 � P.7
						//          pour P3.0 pas de skip possible

   // Configuration du crossbar (valeur par d�faut 0x00)
                        // +----------- 1, Autorise comparateur 0 ligne CP1A
                        // |+---------- 1, Autorise comparateur 0 ligne CP1
						// ||+--------- 1, Autorise comparateur 0 ligne CP0A
                        // |||+-------- 1, Autorise comparateur 0 ligne CP0
                        // ||||
                        // |||| +------ 1, Autorise System clock ligne SYSCLK
						// |||| |+----- 1, Autorise le SMBus lignes SDA, SCL
                        // |||| ||+---- 1, Autorise le SPI lignes SCK, MISO, MOSI, NSS
                        // |||| |||+--- 1, Autorise l'UART
                        // |||| ||||
   XBR0   |= 0x00;      // nnnn'nnnn autorise le fonctionnement du crossbar

                        // +----------- 1, D�sactive r�sistance de weak pull-up
                        // |+---------- 1, Autorise le fonctionnement du crossbar
						// ||+--------- 1, Autorise le Timer 1
                        // |||+-------- 1, Autorise le Timer 0
                        // ||||
                        // |||| +------ Pas utilis�e
                        // |||| |+++--- 0..5, PCA nombre de lignes CEX0..CEX4 autoris�es
                        // |||| ||||          exemple, 3 sont autoris�es CEX0, CEX1, CEX2
   XBR1   |= 0x40;      // nnnn'nnnn

} // PortInit ----------------------------------------------------------------
