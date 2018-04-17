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

#ifndef __NOPOLL_HOSTNAME_COMPARE_H__
#define __NOPOLL_HOSTNAME_COMPARE_H__

int nopoll_hostname_compare_without_wildcard(char *pattern, char *hostname);
int nopoll_hostname_compare_with_wildcard(char *pattern, char *hostname);
int nopoll_hostname_validate_inet(int addr_fam, char *hostname);

#endif /* __NOPOLL_HOSTNAME_COMPARE_H__*/