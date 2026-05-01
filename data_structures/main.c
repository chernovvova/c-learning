#include <stdalign.h>
#include <stdio.h>

struct Bad {
  char c;
  double d;
  int i;
};

struct Good {
  double d;
  int i;
  char c;
};

int main() {
  printf("%zu\n", alignof(struct Bad));
  printf("%zu\n", alignof(struct Good));
}
