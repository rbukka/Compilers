/* test expression and sequential code*/
void foo1(int x1);
int foo2(int x2);

void foo1(int x1){
  int x,j,k;
  x = foo2(x1);
  j = (x+x1)/k;
}


int foo2(int x2){
  int x3;
  x3 = x3+x2;
  return x3;
  }

int main()
{
  int a,b,c;
  int r1,r2,r3;
  a = 10;
  b = a+10;
  r1 = (a>=b) || (b<=c) && (c!=a);
  r2 = !r1;
  r3 = ((a+b)*10-(a-c)*200)*2;
  foo1(r1+r2-r3+c);
  printf("%d %d %d\n", r1, r2, r3);
  return 0;
}

