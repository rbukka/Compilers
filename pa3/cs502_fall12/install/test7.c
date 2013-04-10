//test array pointer 2

int add(int a[], int b[]){

  return a[0]+b[0];
}


int main()
{
  int a[2] = {1,2};
  int b[2] = {1,2};
  int i = 0;
  printf("Total value %d \n ", add(a, b));
  
  return 1;
}


