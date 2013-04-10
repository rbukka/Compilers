/*global*/

int a, b,c;


void init()
{
   a = 1;
}


main()
{
  int k = 10;

  printf("Please input 1st value: ");
  scanf("%d", &k);

  while (k>5){
    init();
    b = a+1;
    if (b<5) c = b+5;
    printf("%d, %d\n", a, b, c, k);
    k = k-1;
  }
  init();
  printf("%d %d %d\n",a, b,c);
}

