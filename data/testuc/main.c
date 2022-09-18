#include <REG51F380.H>

unsigned char setP0(unsigned char a)
{
   P0 = a;
   return 0;
}

unsigned char setP1(unsigned char a)
{
   P1 = a;
   return 0;
}

unsigned char setP2(unsigned char a)
{
   P2 = a;
   return 0;
}

void main()
{
   P0MDOUT = 0xFF;
   P1MDOUT = 0xFF;
   P2MDOUT = 0x00;
   
   while (1)
   {
      setP0(~P1);
      setP1(~P2);
   }
}