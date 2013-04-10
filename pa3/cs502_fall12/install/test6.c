//test pointer and array

int *a[2];
int main()
{
  int x = 10;
  int y = 11;

  a[1] = &x;
  a[2] = &y;

  x = x+y;
  printf("%d\n", *(a[1]) + *(a[2]));
  return 1;
}


