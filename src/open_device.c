# include "open_device.h"
# include "proc.h"


void*
open_device(void *args)
{
	proc_t *proc = (proc_t *) args;

	char errbuf[PCAP_ERRBUF_SIZE];

	proc->dev = pcap_open_live(proc->device, BUFSIZ, 1, 50, errbuf);
	if (proc->dev == NULL) {
		fprintf(stderr, "pcap_open_live() failed due to [%s]\n", errbuf);
		goto fail;
	}

	switch (proc->mode) {
		case MODE_2:
			pcap_loop(proc->dev, 0, packet_handler_mode2, NULL);
			break;

		case MODE_3:
			pcap_loop(proc->dev, 0, packet_handler_mode3, NULL);
			break;
	}

fail:
	proc->active = false;
	pthread_exit(NULL);
}

