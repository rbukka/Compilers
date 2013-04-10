//test while


int main()
{
  
  int i = 0;
  

  while (i<5){
    struct a{
      int b1;
      int b2;
    };
    struct a *b;
    struct a new = {1,2};
    b = &new;
    printf("%d %d\n", i, b->b1 + b->b2); 
    i = i+1;
  }

  return 1;
}


