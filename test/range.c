/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#include <semver.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STRNSIZE(s) (s), sizeof(s)-1

int test_read(const char *expected, const char *str, size_t len) {
  size_t offset = 0;
  unsigned slen;
  char buffer[1024];
  semver_range_t range = {0};

  printf("test: `%.*s`", (int) len, str);
  if (semver_range_read(&range, str, len, &offset)) {
    puts(" \tcouldn't parse");
    return 1;
  }
  if (offset != len) {
    puts(" \tcouldn't parse fully base");
    return 1;
  }
  slen = (unsigned) semver_range_write(range, buffer, 1024);
  printf(" \t=> \t`%.*s`", slen, buffer);
  if (memcmp(expected, buffer, (size_t) slen > len ? slen : len) != 0) {
    printf(" != `%s`\n", expected);
    semver_range_dtor(&range);
    return 1;
  }
  printf(" == `%s`\n", expected);
  semver_range_dtor(&range);
  return 0;
}

int main(void) {
  puts("x-range:");
  if (test_read(">=0.0.0 || 1.2.3", STRNSIZE("* || 1.2.3"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.0.0 <2.0.0 || >=2.0.0 <3.0.0", STRNSIZE("1.x || 2.x"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.2.0 <1.3.0 || 3.0.0", STRNSIZE("1.2.x || 3.0.0"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=0.0.0", STRNSIZE(""))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.0.0 <2.0.0 || >=2.0.0 <3.0.0 || >=3.0.0 <4.0.0", STRNSIZE("1 || 2 || 3"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.2.0 <1.3.0 || >=5.0.0", STRNSIZE("1.2 || >=5"))) {
    return EXIT_FAILURE;
  }

  puts("\nhyphen:");
  if (test_read(">=1.2.3 <=2.3.4 || >=5.0.0", STRNSIZE("1.2.3 - 2.3.4 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.2.0 <=2.3.4 || >=5.0.0", STRNSIZE("1.2 - 2.3.4 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.2.3 <2.4.0 || >=5.0.0", STRNSIZE("1.2.3 - 2.3 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.2.3 <3.0.0 || >=5.0.0", STRNSIZE("1.2.3 - 2 || >=5"))) {
    return EXIT_FAILURE;
  }

  puts("\ntidle:");
  if (test_read(">=1.2.3 <1.3.0 || >=5.0.0", STRNSIZE("~1.2.3 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.2.0 <1.3.0 || >=5.0.0", STRNSIZE("~1.2 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=1.0.0 <2.0.0 || >=5.0.0", STRNSIZE("~1 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=0.2.3 <0.3.0 || >=5.0.0", STRNSIZE("~0.2.3 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=0.2.0 <0.3.0 || >=5.0.0", STRNSIZE("~0.2 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=0.0.0 <1.0.0 || >=5.0.0", STRNSIZE("~0 || >=5"))) {
    return EXIT_FAILURE;
  }

  puts("\ncaret:");
  if (test_read(">=1.2.3 <2.0.0 || >=5.0.0", STRNSIZE("^1.2.3 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=0.2.3 <0.3.0 || >=5.0.0", STRNSIZE("^0.2.3 || >=5"))) {
    return EXIT_FAILURE;
  }
  if (test_read(">=0.0.3 <0.0.4 || >=5.0.0", STRNSIZE("^0.0.3 || >=5"))) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
