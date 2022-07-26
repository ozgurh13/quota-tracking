#ifndef __EXTRACT_H__
#define __EXTRACT_H__

# include <stdint.h>

# define  MAC_ADDR_LEN          6            // number of array cells

void extract_mac_address(char* string, const uint8_t macaddr[MAC_ADDR_LEN]);
void extract_ip_address(char* string, const uint32_t ipaddr);

#endif
