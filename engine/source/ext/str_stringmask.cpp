#include "ext/strings/str_stringmask.h"

NAMESPACE_BEGIN(str)

// This is an adapted version of Richard M. Stallman's glob_match().
// WARNING: GPL license

static bool glob_match(const char *pattern, const char *text);

// Like glob_match, but match PATTERN against any final segment of TEXT.
static bool glob_match_after_star(const char *pattern, const char *text)
{
  const char *p = pattern, *t = text;
  char c, c1;

  while ((c = *p++) == '?' || c == '*')
    if (c == '?' && *t++ == '\0')
      return 0;

  if (c == '\0')
    return 1;

  if (c == '\\')
    c1 = *p;
  else
    c1 = c;

  for (;;)
  {
    if ((c == '[' || *t == c1) && glob_match(p - 1, t))
      return true;
    if (*t++ == '\0')
      return false;
  }
}

static bool glob_match(const char *pattern, const char *text)
{
  const char *p = pattern, *t = text;
  char c;

  while ((c = *p++) != '\0')
    switch (c)
    {
    case '?':
      if (*t == '\0')
        return false;
      else
        ++t;
      break;

    case '\\':
      if (*p++ != *t++)
        return false;
      break;

    case '*':
      return glob_match_after_star(p, t);

    case '[':
      {
        char c1 = *t++;

        if (!c1)
          return false;

        bool invert = ((*p == '!') || (*p == '^'));
        if (invert)
          p++;

        c = *p++;
        for (;;)
        {
          char cstart = c, cend = c;

          if (c == '\\')
          {
            cstart = *p++;
            cend = cstart;
          }
          if (c == '\0')
            return false;

          c = *p++;
          if (c == '-' && *p != ']')
          {
            cend = *p++;
            if (cend == '\\')
              cend = *p++;
            if (cend == '\0')
              return false;
            c = *p++;
          }
          if (c1 >= cstart && c1 <= cend)
            goto match;
          if (c == ']')
            break;
        }
        if (!invert)
          return false;
        break;

      match:
        /* Skip the rest of the [...] construct that already matched.  */
        while (c != ']') {
          if (c == '\0')
            return false;
          c = *p++;
          if (c == '\0')
            return false;
          else if (c == '\\')
            ++p;
        }
        if (invert)
          return false;
        break;
      }

    default:
      if (c != *t++)
        return false;
    }

  return *t == '\0';
}

bool MatchString(const char* text, const char* pattern)
{
  return glob_match(pattern, text);
}

NAMESPACE_END(str)
