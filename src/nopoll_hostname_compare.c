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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static char nopoll_to_lower(char ch);
static int nopoll_get_delimeter_count(char * string, char *delimeter);
static int  nopoll_validate_ipv4(char *hostname);
static int nopoll_validate_ipv6(char *hostname);

static char nopoll_to_lower(char ch)
{
  if(ch >= 'A' && ch <= 'Z')
    return (char)('a' + ch - 'A');
  return ch;
}

static int nopoll_get_delimeter_count(char * string, char *delimeter) {
	int count = 0;
	while (*string != '\0') {
		if (*string == *delimeter) {
			count +=1;
		}
		string++;
	}
	return count;
} 
int nopoll_hostname_compare_without_wildcard(char *pattern, char *hostname) {
  	while (*pattern != '\0' && *hostname != '\0') {
		if( nopoll_to_lower(*pattern) != nopoll_to_lower(*hostname) ){
			return 0;
		}
		pattern++;
		hostname++;
	}
	if (*pattern == '\0' && *hostname == '\0') {
		return 1;
	}
	return 0;
}

int nopoll_hostname_compare_with_wildcard(char *pattern, char *hostname) {
	int cmp_result = 0;
	int split_count = 0;
	char *p_split_first, *p_split_second, *h_split_first, *h_split_second;

	/* Get string after two dots (.) for comparison (without wildcard)
	* as to long wildcard is not supported */
	p_split_first = strchr(pattern, '.');
	p_split_second = strchr(++p_split_first, '.');

	h_split_first = strchr(hostname,'.');
	h_split_second = strchr(++h_split_first, '.');

	if (p_split_second && h_split_second) {
		cmp_result = nopoll_hostname_compare_without_wildcard(++p_split_second, ++h_split_second);
		if (!cmp_result){
			return 0;
		}
	}
	else {
		printf("Should contain atleast two dots (.) in the string \n");
		return 0;
	}

	/* Get string till two dots (.) for wild card comparison */
	int p_index = p_split_second - pattern;
	int h_index = h_split_second - hostname;

	*(pattern + (p_index)) = '\0';
	*(hostname + (h_index)) = '\0';

	while ((*pattern!= '\0') && (*hostname != '\0')) {
		/* Continue reading if both the charecters are same */
		if ( nopoll_to_lower(*pattern) == nopoll_to_lower(*hostname)) {
			pattern++;
			hostname++;
		}
		/* check if hostname contains wildcard */
		else if (*hostname == '*') {

			if(*(++hostname) == '.') {
				/* If hostname contains only * in first octet then skip all charecters in pattern
				* Then do comparison of next octet*/
				if (split_count == 0) {
 					p_index = p_split_first - pattern;
 					pattern += p_index;
 					++hostname;
 				}
 				else {
 					p_index = p_split_second - pattern;
 					pattern += p_index;
 					if (*pattern == '\0') {
 						return 1;
 					}
				}
			}
			else {
				/* Will support partial wildcard comparison */
				char *h_part = strchr(hostname, '.');
				h_index = h_part - hostname;

				/* Skip all charecters in pattern and compare the charecters remained after '*' in hostname string */
				while (*(pattern + h_index) != '.'){
					pattern++;
				}

				while (h_index) {
					if ( nopoll_to_lower(*hostname) != nopoll_to_lower(*pattern)) {
						return 0;
					}
					pattern++;
					hostname++;
					h_index -= 1;
				}
			}
			/* After first octet comparison make this on, so that wildcard comparison for next octet will get rejected */
			split_count = 1;
		}
		else if ( nopoll_to_lower(*pattern) != nopoll_to_lower(*hostname)) {
				return 0;
		}
	}
	/* Outside while, check whether we have traversed both the strings completely or not */ 
	if ((*pattern == '\0') && (*hostname == '\0') ) {
			return 1;
	}
	else {
		return 0;
	}
}

static int nopoll_validate_ipv4(char *hostname){
	int octets = 0,octet_digits = 0;
	char delimeter[] = {"."};
	unsigned char *ip_val;

	unsigned char tmp[4];
	ip_val = tmp;
	*ip_val = 0;

	char *host_tmp = hostname;

	octets = nopoll_get_delimeter_count(host_tmp, delimeter);
	if (octets != 3){
		printf("Octets are more or less than 3, Invalid IP.\n");
		return 0;
	}

	const char *ip_ptr = strchr(host_tmp,'.');
	octet_digits = ip_ptr - host_tmp;
	if (octet_digits >3){
		printf("No. of digits are more than 3 in one octet, Invalid IP.\n");
		return 0;
	}

	while (*host_tmp != '\0'){

		if(!isdigit(*host_tmp)){
			return 0;
		}

		while ( (*host_tmp != *delimeter) && (*host_tmp != '\0')) {
			unsigned int val = (*ip_val) * 10 + (unsigned int)(*host_tmp - '0') ;
			*ip_val = val;
			host_tmp++;

			if(val >255){
				printf("val %u is greater than 255\n",val);
				return 0;
			}
		}
		
		if(*host_tmp != '\0'){
			*ip_val = 0;
			host_tmp++;
		}
	}
	return 1;
}

static int  nopoll_validate_ipv6(char *hostname){
	/*TODO;*/
	return 0;
}

int nopoll_hostname_validate_inet(int addr_fam, char *hostname) {

  switch(addr_fam) {
  case AF_INET:
    return (nopoll_validate_ipv4(hostname));
  case AF_INET6:
    return (nopoll_validate_ipv6(hostname));
  default:
    errno = EAFNOSUPPORT;
    return (-1);
  }
}
