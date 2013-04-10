/* basic switch*/
int getnumber(){
  return 1;
}

int main(){
  int i,j,k;
  int a;
  a = getnumber();
  switch(a){
  case 0: 
    i=a;
    j=a;
    k=a;
    break;
  case 1:
    i = a;
    k = a;
    break;
  case 2:
    i = a;
    j= a;
    break;
  default:
    i = a;
    break;
  };

  printf("%d %d %d\n", i,j,k);
  return 1;
}
