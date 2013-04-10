//test struct
int main()
{
  struct a1 {
    int a;
    int b;
  };

  struct a2 {
    int a[10];
    struct a1 b;
    char c;
  };

  struct a1 l1 ={1,2};
  struct a2 l2;
  int i;

  l2.a[4] = 0;
  l2.b.a = 10;
  printf("%d %d %d %d\n", l1.a, l1.b, l2.a[4], l2.b.a); 
}


