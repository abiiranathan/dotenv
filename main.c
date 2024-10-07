#define DOTENV_IMPLEMENTATION
#include "dotenv.h"

int main() {
  load_dotenv(".env");

  printf("PORT: %s\n", getenv("PORT"));
  printf("HOST: %s\n", getenv("HOST"));
  printf("ADDR: %s\n", getenv("ADDR"));

  return 0;
}
