# dotenv.h

A single header library for parsing .env files with basic interpolation syntax.

```conf
PORT=8090
HOST=localhost
DEBUG =1
CONFIG_PATH=config.json
PATH="C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"
ADDR=${HOST}:${PORT}
```

```c
#define DOTENV_IMPLEMENTATION
#include <stdio.h>
#include "dotenv.h"

int main() {
    load_dotenv(".env");

    printf("PORT: %s\n", getenv("PORT"));
    printf("HOST: %s\n", getenv("HOST"));
    printf("DEBUG: %s\n", getenv("DEBUG"));
    printf("CONFIG_PATH: %s\n", getenv("CONFIG_PATH"));
    printf("PATH: %s\n", getenv("PATH"));
    printf("ADDR: %s\n", getenv("ADDR"));
    return 0;
}
```

## Features
- Basic interpolation syntax `${VAR}`.
- Support for double quoted strings.
- Support comments with `#`.