/* test passing address*/
void init(float *a, int *b, float a1, int b1){
  *a = a1;

}


void main(){
  float a, a1;
  int b, b1;
  int c[10];

  scanf("%f", &a1);
  scanf("%d", &b1);
  c[0] = b1;
  init(&a, &b, a1, c[0]);

  printf("%f %d\n", a, b);
}
  
