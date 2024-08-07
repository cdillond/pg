#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/random.h>

bool is_digit(char c)
{
  return '0' <= c && c <= '9';
}

bool is_lower(char c)
{
  return 'a' <= c && c <= 'z';
}

bool is_upper(char c)
{
  return 'A' <= c && c <= 'Z';
}

bool is_printable(char c)
{
  // exclude control, whitespace, and DEL chars
  return 32 < c && c < 127;
}

bool is_special(char c)
{
  return is_printable(c) && !is_lower(c) && !is_upper(c) && !is_digit(c);
}

#define DEFAULT_LEN 12
#define MIN_LEN 4
#define MAX_LEN 1024

int main(int argc, char **argv)
{
  unsigned int len;
  bool newline = false;
  /*
    the final 2 bytes of the buffer will be used to improve the distribution of
    substituted characters. realistically, only 1 byte should be fine, since
    the chances of a generating a string longer than 255 characters that doesn't
    include at least 1 character from each category is quite small, but it
    doesn't hurt to be safe.
  */
  unsigned char buf[MAX_LEN + 2];

  if (argc < 2)
  {
    len = DEFAULT_LEN;
  }
  else if (sscanf(argv[1], "%ud", &len) != 1)
  {
    if (*argv[1] == 'n')
    {
      len = DEFAULT_LEN;
      newline = true;
    }
    else
    {
      fputs("unable to parse arguments\n", stderr);
      return 1;
    }
  }

  if (!newline && argc > 2)
  {
    if (*argv[2] == 'n')
      newline = true;
  }

  if (len > MAX_LEN || len < MIN_LEN)
  {
    fprintf(stderr, "length argument must be in the interval [%d, %d]\n", MIN_LEN, MAX_LEN);
    return 1;
  }

  getrandom(buf, len+2, 0);

  // ensure all chars in buf are acceptable.
  for (int i = 0; i < len; i++)
  {
    if (!is_printable(buf[i]))
    {
      buf[i] %= ('~' - '!');
      buf[i] += '!';
    };
  }

  bool seen_lower, seen_upper, seen_num, seen_special, seen_all;
  seen_all = false;
  // n is the randomly chosen index within buf to be replaced if
  // not all char categories were included on the first pass.
  unsigned int n = *(uint16_t *)(&buf[len]);
  n %= len;
  while (!seen_all)
  {
    seen_lower = seen_upper = seen_num = seen_special = false;

    for (int i = 0; i < len && !seen_all; i++)
    {
      char c = buf[i];

      if (is_lower(c))
        seen_lower = true;
      else if (is_upper(c))
        seen_upper = true;
      else if (is_digit(c))
        seen_num = true;
      else if (is_special(c))
        seen_special = true;

      seen_all = (seen_lower && seen_upper && seen_num && seen_special);
    }

    if (!seen_all)
    {

      if (!seen_lower)
      {
        buf[n] %= ('z' - 'a');
        buf[n] += 'a';
        n++;
        n %= len;
      }
      if (!seen_upper)
      {
        buf[n] %= ('Z' - 'A');
        buf[n] += 'A';
        n++;
        n %= len;
      }
      if (!seen_num)
      {
        buf[n] %= ('9' - '0');
        buf[n] += '0';
        n++;
        n %= len;
      }
      if (!seen_special)
      {
        buf[n] %= ('~' - '!');
        buf[n] += '!';
        n++;
        n %= len;
      }
    }
  }

  buf[len] = '\0';
  if (!newline)
    fputs((const char *)buf, stdout);
  else
    puts((const char *)buf);

  return 0;
}
