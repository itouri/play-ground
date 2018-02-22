#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

double gettimeofday_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

int simple_sum(int n) {
  int sum = 0 ;
  for (int i=0; i < n; i++) {
    sum += i;
  }
  return sum;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Please type the n\n");
    return 0;
  }
 int n = atoi(argv[1]);
  
  double t1, t2;
  t1 = gettimeofday_sec();
  simple_sum(n);  
  t2 = gettimeofday_sec();
  printf("%lf\n", t2-t1);
  return 0;
}
