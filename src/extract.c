# include <stdio.h>

# include "extract.h"

void
extract_mac_address(char* string, const uint8_t macaddr[MAC_ADDR_LEN])
{
	static const char* hex_values = "0123456789abcdef";

# define  LEFT_MASK(value)      ((value & 0xF0) >> 4)
# define  RIGHT_MASK(value)     ((value & 0x0F)     )

	for (uint8_t i = 0; i < MAC_ADDR_LEN; i++) {
		const uint8_t index = i * 3;
		const uint8_t value = macaddr[i];

		string[index + 0] = hex_values[LEFT_MASK(value)];
		string[index + 1] = hex_values[RIGHT_MASK(value)];
		string[index + 2] = ':';
	}
	string[17] = '\0';
}

static char*
add_number_to_string(char *c, uint32_t number)
{
	uint8_t index = 0;
	char digits[3];      // ip addresses have at most 3-digit sections
	do {
		digits[index++] = (number % 10) + '0';
		number /= 10;
	} while (number);

	while (index)
		*c++ = digits[--index];

	return c;
}

void
extract_ip_address(char* string, const uint32_t ipaddr)
{
	char *c = string;

# define  M1(value)    ((value & 0xFF000000) >> 24)
# define  M2(value)    ((value & 0x00FF0000) >> 16)
# define  M3(value)    ((value & 0x0000FF00) >>  8)
# define  M4(value)    ((value & 0x000000FF)      )

	const uint32_t ipaddrs[4] = { M4(ipaddr), M3(ipaddr), M2(ipaddr), M1(ipaddr) };

	for (uint8_t i = 0; i < 4; i++) {
		c = add_number_to_string(c, ipaddrs[i]);
		*c++ = '.';
	}
	*--c = '\0';
}

