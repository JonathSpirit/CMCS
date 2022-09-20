#include <REG51F380.H>

#define PORT_OUT     P1          // P1, port de sortie
#define PORT_IN      P2          // P2, Port d'entrée

#define MSK_J1       0x80        
#define MSK_J2       0x01
#define MSK_RESET    0x02

// Peripheral specific initialization functions,
// Called from the Init_Device() function
void Port_IO_Init()
{
    // P0.0  -  Unassigned,  Open-Drain, Digital
    // P0.1  -  Unassigned,  Open-Drain, Digital
    // P0.2  -  Unassigned,  Open-Drain, Digital
    // P0.3  -  Unassigned,  Open-Drain, Digital
    // P0.4  -  Unassigned,  Open-Drain, Digital
    // P0.5  -  Unassigned,  Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    // P1.0  -  Unassigned,  Push-Pull,  Digital
    // P1.1  -  Unassigned,  Push-Pull,  Digital
    // P1.2  -  Unassigned,  Push-Pull,  Digital
    // P1.3  -  Unassigned,  Push-Pull,  Digital
    // P1.4  -  Unassigned,  Push-Pull,  Digital
    // P1.5  -  Unassigned,  Push-Pull,  Digital
    // P1.6  -  Unassigned,  Push-Pull,  Digital
    // P1.7  -  Unassigned,  Push-Pull,  Digital

    // P2.0  -  Unassigned,  Open-Drain, Digital
    // P2.1  -  Unassigned,  Open-Drain, Digital
    // P2.2  -  Unassigned,  Open-Drain, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital
    // P2.4  -  Unassigned,  Open-Drain, Digital
    // P2.5  -  Unassigned,  Open-Drain, Digital
    // P2.6  -  Unassigned,  Open-Drain, Digital
    // P2.7  -  Unassigned,  Open-Drain, Digital

    // P3.0  -  Unassigned,  Open-Drain, Digital

    P1MDOUT   = 0xFF;
    XBR1      = 0x40;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    Port_IO_Init();
}

void main () 
{
   unsigned char inters;   
   unsigned char position;
   unsigned char switch1;
   unsigned int temps;

   Init_Device();
   
   position=0x01;
   temps=800;

   while (1)
   {
      inters = PORT_IN;
      
      if (inters&MSK_RESET) { //RESET
         switch1=0;
         temps=800;
         position=0x01;
      }
      
      if (switch1==0) { //left
         if (position==0x80) {
            if (inters&MSK_J1) {
               if (temps>=300)
               {
                  temps=temps-50;
               }
               switch1=1;
               position=position>>1;
            }
            else
            {
               switch1=2;
               position=0x0F;
            }
         }
         else
         {
            if (inters&MSK_J1)
            {
               if (temps>=300)
               {
                  temps=temps-50;
               }
               switch1=2;
               position=0x0F;
            }
            else
            {
               position=position<<1;
            }
         }
      }
      else
      {
         if (switch1==1) { //right
            if (position==0x01) {
               if (inters&MSK_J2) {
                  switch1=0;
                  position=position<<1;
               }
               else
               {
                  switch1=2;
                  position=0xF0;
               }
            }
            else
            {
               if (inters&MSK_J2)
               {
                  switch1=2;
                  position=0xF0;
               }
               else
               {
                  position=position>>1;
               }
            }
         }
      }
      
      PORT_OUT = position;
   }// End while (1)
 
}
