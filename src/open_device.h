#ifndef __OPEN_DEVICE_H__
#define __OPEN_DEVICE_H__

# include <stdio.h>
# include <pcap.h>

typedef enum { MODE_2 = 2, MODE_3 = 3 } capture_mode_t;

void* open_device(void *args);

void packet_handler_mode2(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char *pkt_data);
void packet_handler_mode3(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char *pkt_data);

#endif
