int glob1;

void init()
{
   glob1 = 1;
}
main()
{
  int k;

printf("Please input 1st value: ");
scanf("%d", &k);

init();
if ( k > 0 ) glob1 = 0;


printf("glob1=%d\n",glob1);
}

