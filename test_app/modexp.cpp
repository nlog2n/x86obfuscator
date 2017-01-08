
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

// Referenced "Modular Exponentiation" article on wikipedia for following algorithm
//  base^exp % mod
int modExp (int base, int exp, int mod)
{
   int c = 1, expNot = 0;
   do
   {
      expNot++;
      c = (base * c) % mod;
   }
   while (expNot < exp);
   return c;
}


int main(int argc, char * const argv[])
{
   if(argc != 4)
   {
      cout << "Usage: encrypt <message int> <public key int> <exponent int>\n";
      return 0;
   }
        int x = atoi(argv[1]);
        int N = atoi(argv[2]);
        int e = atoi(argv[3]);

        int y = modExp(x,e,N);

        cout << y << "\n";

   return 0;
}
