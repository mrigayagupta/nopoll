/***************************************************************************
 *
 * Copyright (C) 1998 - 2017, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include <nopoll_hostname_validation.h>

static int nopoll_hostcheck(char *pattern, char *hostname);

static int nopoll_hostcheck(char *pattern, char *hostname){
  char *pattern_wildcard = NULL;

  size_t p_length = strlen(pattern);
  size_t h_length = strlen(hostname);
  /* pattern or hostname should not end with "." */
  if( pattern[p_length-1] == '.' || hostname[h_length-1] == '.')
  {
    return MATCH_FAIL;
  }

  pattern_wildcard = strchr(pattern, '*');

  if (pattern_wildcard == NULL) {
    return nopoll_hostname_compare_without_wildcard(pattern,hostname) ? MATCH_SUCCESS : MATCH_FAIL;
  }
  if (nopoll_hostname_validate_inet(AF_INET, hostname) > 0) {
    return MATCH_FAIL;
  }
  /*else if(nopoll_hostname_validate_inet(AF_INET6, hostname) > 0)
    return MATCH_FAIL;*/
  return nopoll_hostname_compare_with_wildcard(pattern,hostname) ? MATCH_SUCCESS : MATCH_FAIL;
}

int nopoll_match_hostname(const char *pattern, const char *hostname)
{
  char *match_pattern;
  char *host;
  int result = MATCH_FAIL;
  match_pattern = nopoll_strdup(pattern);
  host = nopoll_strdup(hostname);

  if(match_pattern && host) {
    result = nopoll_hostcheck(match_pattern,host);
    nopoll_free(host);
    nopoll_free(match_pattern);
    return result;
  }
  return result;
}

