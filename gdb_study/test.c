#include <stdio.h>

int AddToVal(int x, int y)
{
  int sum = 0;
  int i = 0;

  for (i = x; i <= y; ++i)
  {
    sum += i;
  }

  return sum;
}

void Print(int sum)
{
  printf("result = %d\n", sum);
}

int main()
{
  int sum = AddToVal(0, 100);
  Print(sum);

  printf("hello a\n");
  printf("hello b\n");
  printf("hello c\n");
  printf("hello d\n");
  printf("hello e\n");
  printf("hello f\n");

  return 0;
}
