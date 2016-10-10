#include <stdio.h>

/* inserted in GitHub October, 7th */
/* insere par nedit BRANCHE A */
 
/*
 * Calcul de modulo 5 sans utliser l'operateur "%" ni "/" 
 * (pas de division dans processeur ARM)
 *
 * On fait une division par 5 en multipliant par une fraction
 *   - qui approche 0.2
 *   - qui est > 0.2
 *   - dont le diviseur une puissance de 2
 *
 * La division par une puissance de 2 est une division entiere, 
 * elle se fait par decalage de bits.
 * 
 *                        i * M
 * i modulo 5 =  i  -  -------------- * 5
 *                       2 exp N
 *
 * avec M = 2 exp N / 5
 * (valeur approchée par excès)
 * 
 * Deux dangers : 
 *    -  i * M    dépasse la capacité du processeur  (d'ou l'utilisation de "unsigned" pour doubler le max)
 *    -  l'erreur sur "i * M" (du fait que M est un arrondi)
 *       Les plus petits "i" qui provoquent l'erreur sont ceux dont le modulo est 4
 *       Si "i * M" est surévalué d' 1 cinquieme de "2 exp N" (autrement dit si l'erreur est de   au moins M)
 *       le resultat de la division donne 1 de trop
 */

int main (int argc, char *argv[])
{
  unsigned int i,modulo;
  int lastok;
  int ok=0;
  for (i=0;i<256000; i++)
  {
//      test=i-((i*3355444)>>24)*5;
//      test = i - ((i*1677722)>>23)*5;  // 1677721.6

    // Valeurs qui permettent de calculer le plus de modulo : 
    modulo = i - ((i*52429)>>18)*5; // 2 exp 18  / 5 = 52428.8

    // (ok jusqu'à i=81919)
    // 81919 * 52429 = 0xFFFF7333
    // On note que l'erreur du à l'arrondi (52429 au lieu de 52428.8) est de 0.2 * 81919 = 16383.8
    // reste inferieure a 52429, donc pas de pb.

    if (modulo == i%5)
    {
      lastok=i;
      ok++;
    }
    else
      break;
  }
  printf("Nombre de OK = %d.   Dernier ok = %d\n", ok, lastok);
}


#ifdef MAIL_LAURENT
ici la valeur (V) de 2 exp 24 / 5   est la valeur approchee par defaut
Pour i multiple de 5, La multiplication par la fraction donne par exemple 
       40 * V / 2 exp 24 =  7.99 et des poussieres
ce qui donne 7 en division entiere
==> "test" vaut 5

main()
{
  int i,test;
  for (i=0;i<256; i++)
  {
    test=0;
    if (i!=0)
    {
      test=i-((i*3355443)>>24)*5;
    }
    if ((i==0) || (test==5))
    {
      printf("valeur=%d %08x is modulo 5",i,i-((i*3355443)>>24)*5);
      if ((i&1)==0)
      {
        printf(" and modulo 10");
        if (((i>>1)&1)==0)
        {
          printf(" and modulo 20");
        }
      }
      printf("\n");
    }
  }
}
#endif

#ifdef PREMIERE_SOLUTION
int main (int argc, char *argv[])
{
  int x;
  int m;
  int aquotient; // Approximation of the quotient x/m
  int ax;        // Approximation of x
  int result;


  x=atoi(argv[1]);
  // m=atoi(argv[2]);
  m=5;
  printf("Calcul de %d modulo %d\n", x, m);

  // approximation de "x / 5"  par   "x / 4   -   x / 16  +  x / 64  - 1 / 512  - 1 / 1024"
  aquotient =                        (x >> 2) -  (x >> 4) + (x >> 6) - (x >> 9) - (x>>10);

  // appromixation de x, par un nombre multiple de 5
  ax = aquotient * m;

  printf("aquotient = %d, ax= %d\n", aquotient, ax);
  
  if (ax <= x)
  {
    while (ax+m <= x) 
    {
      ax += m;
    }
  }
  else
  {
    while (ax > x)
    {
      ax -= m;
    } 
  }

  result = x-ax;

  printf("%d modulo %d = %d... ", x, m, result);
  if (result == x%m)
  {
    printf("exact !\n");
  }
  else
  {
    printf("faux !\n");
  }
}
#endif


#ifdef IDEE_OCTET_PAR_OCTET
pas fini...
pourrait etre combiné avec la solution " i - i * M / 2expN * 5"
(en travaillant sur les 16 eb de poids fort d'abord ??)


int main (int argc, char *argv[])
{
  int x;
  int m;
  int aquotient; // Approximation of the quotient x/m
  int a;        // Approximation of x
  int result;

  int xh;        // octet de poids fort
  int xhm;       // 256 * (modulo de l'octet de poids fort)

  x=atoi(argv[1]);
  // m=atoi(argv[2]);
  m=5;
  printf("Calcul de %d modulo %d\n", x, m);


  xh = x >> 8;

si xh < m, sauter la premiere partie (en plus, bug pour valeurs 1 a 4)

  // approximation de "x / 5"  par   "x / 4   -   x / 16  +  x / 64  - 1 / 512  - 1 / 1024"
  // aquotient =                        (x >> 2) -  (x >> 4) + (x >> 6) - (x >> 9) - (x>>10);
  aquotient =                        (xh >> 2) -  (xh >> 4);

  // appromixation de x, par un nombre multiple de 5
  a = aquotient * m;

  printf("xh=%d  aquotient = %d, a= %d\n", xh, aquotient, a);
  
  /*  Si l'approximation donne un nombre plus petit, utiliser ce while */
  
  while (a+m <= xh) 
  {
    a += m;
  }
  

  /*  Si l'approximation donne un nombre plus grand, utiliser ce while */
  /*
  while (a > xh)
  {
    a -= m;
  } 
  */

  result = xh-a;

  printf("%d modulo %d = %d... ", xh, m, result);
  if (result == xh%m)
  {
    printf("exact !\n");
  }
  else
  {
    printf("faux !\n");
  }

deuxieme partie : 
x2 = x & 255 + m * 256

refaire 1ere partie avec x2
}
#endif
