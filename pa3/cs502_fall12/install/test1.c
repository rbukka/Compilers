int glob1;
struct ttt {
  int k1;
  float k2;
};
struct ttt glob2;

main()
{
int i[2]={1,2},k = 10;
struct ttt h={3,4.0};
char *u="string";

if ( glob1 > 0 )
	glob1 = 0;
else
	glob1 = 1;
while ( k ) {
	h.k1 = h.k1 + k;
	if ( h.k1 > 10 ) {
		h.k1 = 0;
		h.k2 = 1.0;
	}
	k = k-1;
}
glob2.k1 = h.k1;
k=i[1]+t(1,3);
printf("glob2.k1=%d, k=%d\n",glob2.k1,k);
}

int t(int k, int j)
{
  return glob1+k+j;
}
