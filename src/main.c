# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <string.h>

# include <pcap.h>
# include <pthread.h>

# include <net/ethernet.h>         /*  struct ether_header  */
# include <netinet/ip.h>           /*  struct ip  */

# include <sys/stat.h>             /*  mkfifo  */

# include <unistd.h>
# include "msleep.h"

# include "extract.h"
# include "hashmap.h"
# include "database.h"

# include "fifo.h"
# include "open_device.h"
# include "proclist.h"

static void initialize();
static void *start_tracker(void *args);

int main(int argc, char* argv[])
{
	initialize();
	fifo_listen();
}


/*
 * accumulation
 *
 * when a packet is captured add the data to accum_main
 * then have a thread take the data and write it to the database
 */

enum { ACCUM_MAC, ACCUM_IP, NUMBER_OF_ACCUM };

static hashmap_t accum_main[NUMBER_OF_ACCUM];
static hashmap_t accum_back[NUMBER_OF_ACCUM];


static pthread_mutex_t lock;
# define  WITH_LOCK(...)                                 \
    do {                                                 \
        pthread_mutex_lock(&lock);                       \
        __VA_ARGS__                                      \
        pthread_mutex_unlock(&lock);                     \
    } while (0);



typedef struct {
	const char* device;
	const capture_mode_t mode;
} thread_args;


static void
initialize()
{
	// init mutex
	pthread_mutex_init(&lock, NULL);

	// init hashmaps
	for (uint8_t i = 0; i < NUMBER_OF_ACCUM; i++) {
		hashmap_init(&accum_main[i]);
		hashmap_init(&accum_back[i]);
	}

	// open database
	db_open();

	// make fifo file
	mkfifo(fifofile, 0666);

	// start tracker
	pthread_t thread;
	pthread_create(&thread, NULL, start_tracker, NULL);
}



static void
write_to_db(node_t *node)
{
	db_write(node->key);
}

static void
print_kv(node_t *node)
{
	printf( "%s :~: %zu : %zu\n"
	      , node->key.key
	      , node->key.up_value
	      , node->key.down_value );
}


/*
 * tracker
 *
 * take collected data and write to database
 */
static void*
start_tracker(void *args)
{
	hashmap_t acc;       // tmp to swap accumulators

	while (true) {
		sleep(25);       // wait between writes to database

		/*  swap main and backup accumulators  */
		for (uint8_t i = 0; i < NUMBER_OF_ACCUM; i++) {
			acc = accum_main[i];
			accum_main[i] = accum_back[i];
			accum_back[i] = acc;
		}

		msleep(250);      // a buffer, in case a thread is still writing

		/*  write to database  */
		hashmap_foreach(&accum_back[ACCUM_MAC], write_to_db);
		hashmap_foreach(&accum_back[ACCUM_IP],  write_to_db);

		// clear tables
		for (uint8_t i = 0; i < NUMBER_OF_ACCUM; i++)
			hashmap_delete(&accum_back[i]);
	}
}




void
packet_handler_mode2( unsigned char *param
                    , const struct pcap_pkthdr *header
                    , const unsigned char *pkt_data )
{
	const struct ether_header *eth = (struct ether_header *) pkt_data;
	const size_t size = header->len;

	kv_t kv_src, kv_dest;

	extract_mac_address(kv_src.key,  eth->ether_shost);
	kv_src.up_value   = size;
	kv_src.down_value = 0;

	extract_mac_address(kv_dest.key, eth->ether_dhost);
	kv_dest.up_value   = 0;
	kv_dest.down_value = size;

	WITH_LOCK(
		hashmap_insert(&accum_main[ACCUM_MAC], kv_src);
		hashmap_insert(&accum_main[ACCUM_MAC], kv_dest);
	)
}

void
packet_handler_mode3( unsigned char *param
                    , const struct pcap_pkthdr *header
                    , const unsigned char *pkt_data )
{
	const struct ip *ih = (struct ip *) (pkt_data + sizeof(struct ether_header));
	const size_t size = header->len;

	kv_t kv_src, kv_dest;

	extract_ip_address(kv_src.key,  ih->ip_src.s_addr);
	kv_src.up_value   = size;
	kv_src.down_value = 0;

	extract_ip_address(kv_dest.key, ih->ip_dst.s_addr);
	kv_dest.up_value   = 0;
	kv_dest.down_value = size;

	WITH_LOCK(
		hashmap_insert(&accum_main[ACCUM_IP], kv_src);
		hashmap_insert(&accum_main[ACCUM_IP], kv_dest);
	)
}

