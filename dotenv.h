#ifndef DOTENV_H
#define DOTENV_H

#define _GNU_SOURCE

// Load environment variables from a .env file
// Supports the following syntax:
//   KEY=VALUE
//   KEY="VALUE"
//   KEY=${OTHER_KEY} where OTHER_KEY is an environment variable already set.
void load_dotenv(const char* path);


#ifdef DOTENV_IMPLEMENTATION
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum length of a line in the .env file
#ifndef MAX_LINE_LENGTH
#define MAX_LINE_LENGTH 4096
#endif

// Helper function to sanitize and interpolate values
static bool interpolate(const char* value, char result[MAX_LINE_LENGTH]) {
  size_t result_len = 0;
  const char* ptr = value;
  memset(result, 0, MAX_LINE_LENGTH);

  while (*ptr && result_len < MAX_LINE_LENGTH - 1) {
    if (*ptr == '$' && *(ptr + 1) == '{') {
      const char* start = ptr + 2;
      const char* end = strchr(start, '}');

      if (end) {
        size_t var_name_len = end - start;
        char var_name[MAX_LINE_LENGTH] = {0};
        snprintf(var_name, sizeof(var_name), "%.*s", (int)var_name_len, start);

        const char* var_value = secure_getenv(var_name);
        if (var_value) {
          size_t var_value_len = strlen(var_value);
          if (result_len + var_value_len < MAX_LINE_LENGTH) {
            snprintf(result + result_len, MAX_LINE_LENGTH - result_len, "%s", var_value);
            result_len += var_value_len;
          }
        } else {
          fprintf(stderr, "Warning: Variable %s not found\n", var_name);
        }
        ptr = end + 1;
      } else {
        result[result_len++] = *ptr++;
      }
    } else {
      result[result_len++] = *ptr++;
    }
  }
  return result_len > 0;
}

void load_dotenv(const char* path) {
  FILE* file = fopen(path, "r");
  if (!file) {
    fprintf(stderr, "Failed to open file: %s\n", path);
    return;
  }

  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), file)) {
    char* start = line;
    char* end = line + strlen(line) - 1;

    // Remove leading whitespace
    while (isspace(*start))
      start++;

    // Remove trailing whitespace
    while (end > start && isspace(*end))
      *end-- = '\0';

    // Skip empty lines and comments
    if (*start == '\0' || *start == '#')
      continue;

    char* equals = strchr(start, '=');
    if (equals) {
      // Trim spaces before and after the equals sign
      char* key_end = equals - 1;
      while (key_end > start && isspace(*key_end))
        key_end--;
      *(key_end + 1) = '\0';
      char* key = start;

      char* value = equals + 1;
      while (*value && isspace(*value))
        value++;

      // Remove quotes from value if present
      if (*value == '"' && value[strlen(value) - 1] == '"') {
        value++;
        value[strlen(value) - 1] = '\0';
      }

      // Interpolate value if it has pattern ${VAR_NAME}
      if (strchr(value, '$') && strchr(value, '{') && strchr(value, '}')) {
        char interpolated_value[MAX_LINE_LENGTH];
        bool success = interpolate(value, interpolated_value);
        if (success) {
          setenv(key, interpolated_value, 1);
        } else {
          fprintf(stderr, "Failed to interpolate value: %s\n", value);
        }
      } else {
        setenv(key, value, 1);
      }
    } else {
      fprintf(stderr, "Invalid line: %s\n", line);
    }
  }

  fclose(file);
}

#endif  // DOTENV_IMPLEMENTATION

#endif  // DOTENV_H
