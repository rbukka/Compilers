//test if and block

void testif(int i){
  if (i ==1) {
    printf("now i = 1\n");
    return;
  }else {
    printf("now i!=1\n");
    if (i == 2) {
      printf ("now i = 2\n");
      return;
    }else {
      printf("something else\n");
      return;
    }
  }

  printf("OK\n");
  return;
}

int main()
{
  
  testif(1);
  testif(2);
  testif(3);

  
}


