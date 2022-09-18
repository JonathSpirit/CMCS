/*===========================================================================*=
   CFPT - Projet : projet_exam1
   Auteur        : Guillaume Guillet
   Date création : 08.11.2016
  =============================================================================
   Descriptif:
   Ce programme simule le panneau de controle d'un ascenseur avec 3 étage.
=*===========================================================================*/
#include <REG51F380.h>     // registres 8051f380
//#include <Delay.h>
// ==== Fonctions prototypes ==================================================
void ClockInit ();         // init. clock système
void PortInit ();          // init. config des ports
// ==== Equivalences ==========================================================

//MACRO - port
#define POUT  P1    // P1, port de sortie
#define PIN   P2    // P2, Port d'entrée

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
   ClockInit ();        // init. clock système
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
   Entrée    : --
   Sortie    : --
-*---------------------------------------------------------------------------*/
void ClockInit ()
{
                 //       ++-- Oscillateur interne
                 //       ||
    CLKSEL = 0x00;   // 00000000   clock system = clock interne

                     // +--------- clock interne autorisé
                     // |     ++-- f = 12 MHZ
                     // |     ||
    OSCICN = 0x83;   // 10000011

} // ClockInit ----------------------------------------------------------------

/*---------------------------------------------------------------------------*-
   PortInit ()
  -----------------------------------------------------------------------------
   Descriptif: Initialisation du mode de fonctionnement des ports
   Entrée    : --
   Sortie    : --
-*---------------------------------------------------------------------------*/
void PortInit ()
{
   // Type d'entrée Analogique (0) / Numérique (1)
   P0MDIN  = 0xFF;      // port P0 entrée numérique (valeur par défaut)
   P1MDIN  = 0xFF;      // port P1 entrée numérique (valeur par défaut)
   P2MDIN  = 0xFF;      // port P2 entrée numérique (valeur par défaut)
   P3MDIN  = 0xFF;      // port P3.0 entrée numérique (valeur par défaut)

   // Type de sortie Open Drain (0) / Push-Pull (1)
   P0MDOUT = 0x00;      // port P0 en entrée numérique open drain (valeur par défaut)
   P1MDOUT = 0xFF;      // port P1 en sortie numérique (push-pull)
   P2MDOUT = 0x00;      // port P2 en entrée numérique open drain (valeur par défaut)
   P3MDOUT = 0x00;      // port P3 en entrée numérique open drain (valeur par défaut)

   // Réservation des bits de port Libre (0) / Réservé (1)
   P0SKIP  = 0x00;      // port P0 libre (valeur par défaut)
   P1SKIP  = 0xFF;      // port P1 réservé
   P2SKIP  = 0xFF;      // port P2 réservé
						// Remarque pour P2 pas de skip possible pour bits P2.4 à P.7
						//          pour P3.0 pas de skip possible

   // Configuration du crossbar (valeur par défaut 0x00)
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

                        // +----------- 1, Désactive résistance de weak pull-up
                        // |+---------- 1, Autorise le fonctionnement du crossbar
						// ||+--------- 1, Autorise le Timer 1
                        // |||+-------- 1, Autorise le Timer 0
                        // ||||
                        // |||| +------ Pas utilisée
                        // |||| |+++--- 0..5, PCA nombre de lignes CEX0..CEX4 autorisées
                        // |||| ||||          exemple, 3 sont autorisées CEX0, CEX1, CEX2
   XBR1   |= 0x40;      // nnnn'nnnn

} // PortInit ----------------------------------------------------------------
