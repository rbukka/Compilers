/*test call graph*/
int g1;
int g2;
int g3;

void init1( int i1)
{
   g1 = i1;
}

void init2( int i2){
  g2 = i2;
}

void init3( int i3){
  g3 = i3;
}

int add(){
  init1(2);
  init2(5);
  return g1+g2;
}

int sub(){
  return g1-g3;
}
 
void main(){
  int i;
  scanf("%d", &i);
  if (i < 10){
    init1(i);
    printf("%d\n",add());
  }
  
  init2(i+1);
  init3(i+1);
  printf("glob1=%d\n",sub());
}

