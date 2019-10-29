#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include "version.h"
#include "hamming_distance.h"

FILE *meta_fifo = NULL;

#define SYNC_SIZE 10
uint8_t nxdn_sync[] = { 3, 0, 3, 1, 3, 3, 1, 1, 2, 1 };
#define FRAME_SIZE 192

#define SYNCTYPE_UNKNOWN 0
// unused for now
//#define SYNCTYPE_DATA 1
#define SYNCTYPE_VOICE 2

#define BUF_SIZE 128
#define RINGBUFFER_SIZE 1024

uint8_t buf[BUF_SIZE];
uint8_t ringbuffer[RINGBUFFER_SIZE];
int ringbuffer_write_pos = 0;
int ringbuffer_read_pos = 0;

// modulo that will respect the sign
unsigned int mod(int n, int x) { return ((n%x)+x)%x; }

int ringbuffer_bytes() {
    return mod(ringbuffer_write_pos - ringbuffer_read_pos, RINGBUFFER_SIZE);
}

int get_synctype(uint8_t potential_sync[SYNC_SIZE]) {
    if (symbol_hamming_distance(potential_sync, nxdn_sync, SYNC_SIZE) <= 3) {
        return SYNCTYPE_VOICE;
    }
    return SYNCTYPE_UNKNOWN;
}

#define RF_CHANNEL_TYPE_RCCH 0
#define RF_CHANNEL_TYPE_RTCH 1
#define RF_CHANNEL_TYPE_RDCH 2
#define RF_CHANNEL_TYPE_RTCH_C 3

#define CAC_TYPE_INBOUND_LONG 1
#define CAC_TYPE_INBOUND_SHORT 3
#define CAC_TYPE_OUTBOUND_CAC 0

// SF = superframe
#define USC_TYPE_SACCH_NON_SF 0
#define USC_TYPE_UDCH 1
#define USC_TYPE_SACCH_SF 2
#define USC_TYPE_SACCH_SF_IDLE 3

#define DIRECTION_INBOUND 0;
#define DIRECTION_OUTBOUND 1;

typedef struct {
    uint8_t rf_channel_type;
    uint8_t functional_channel_type;
    uint8_t option;
    uint8_t direction;
} lich;

bool decode_lich(uint8_t input[8], lich* output) {
    uint8_t lich = 0;
    int i;
    for (i = 0; i < 8; i++) {
        lich = (lich << 1) | ((input[i] & 0b10) >> 1);
    }

    uint8_t parity = 0;
    // 1 bit even parity
    // Range of parity shall be 4 most significant bits.
    for (i = 0; i < 4; i++) {
        parity ^= (lich >> (7 - i)) & 0b1;
    }

    if (parity != (lich & 0b1)) return false;

    output->rf_channel_type =         (lich >> 6) & 0b11;
    output->functional_channel_type = (lich >> 4) & 0b11;
    output->option =                  (lich >> 2) & 0b11;
    output->direction =               (lich > 1) & 0b1;
}

void print_usage() {
    fprintf(stderr,
        "nxdn_decoder version %s\n\n"
        "Usage: nxdn_decoder [options]\n\n"
        "Available options:\n"
        " -h, --help          show this message\n"
        " -f, --fifo          send metadata to this file\n"
        " -v, --version       print version and exit\n",
        VERSION
    );
}

int main(int argc, char** argv) {
    int c;
    static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"fifo", required_argument, NULL, 'f'},
        {"version", no_argument, NULL, 'v'},
        { NULL, 0, NULL, 0 }
    };
    while ((c = getopt_long(argc, argv, "hf:vc:", long_options, NULL)) != -1) {
        switch (c) {
            case 'f':
                fprintf(stderr, "meta fifo: %s\n", optarg);
                meta_fifo = fopen(optarg, "w");
                break;
            case 'v':
                print_version();
                return 0;
            case 'h':
                print_usage();
                return 0;
        }
    }

    int r = 0;
    bool sync = false;
    int sync_missing = 0;

    while ((r = fread(buf, 1, BUF_SIZE, stdin)) > 0) {
        int i;
        for (i = 0; i < r; i++) {
            ringbuffer[ringbuffer_write_pos] = buf[i];
            ringbuffer_write_pos += 1;
            if (ringbuffer_write_pos >= RINGBUFFER_SIZE) ringbuffer_write_pos = 0;
        }

        // 16 bit is LICH raw size, divided by 2 due to dibitization
        while (!sync && ringbuffer_bytes() > SYNC_SIZE + 8) {
            uint8_t potential_sync[SYNC_SIZE];

            for (i = 0; i < SYNC_SIZE; i++) potential_sync[i] = ringbuffer[(ringbuffer_read_pos + i) % RINGBUFFER_SIZE];

            if (get_synctype(potential_sync) != SYNCTYPE_UNKNOWN) {
                uint8_t raw_lich[8];
                for (i = 0; i < 8; i++) raw_lich[i] = ringbuffer[(ringbuffer_read_pos + i) % RINGBUFFER_SIZE];
                lich lich;
                if (decode_lich(raw_lich, &lich)) {
                    fprintf(stderr, "nxdn sync and valid LICH at pos %i; rf channel type: %i; functional channel type: %i\n", ringbuffer_read_pos, lich.rf_channel_type, lich.functional_channel_type);
                    sync = true; sync_missing = 0;
                    break;
                }
            }

            ringbuffer_read_pos++;
            if (ringbuffer_read_pos >= RINGBUFFER_SIZE) ringbuffer_read_pos = 0;
        }

        while (sync && ringbuffer_bytes() > FRAME_SIZE) {
            uint8_t potential_sync[SYNC_SIZE];
            for (i = 0; i < SYNC_SIZE; i++) potential_sync[i] = ringbuffer[(ringbuffer_read_pos + i) % RINGBUFFER_SIZE];

            int synctype = get_synctype(potential_sync);
            if (synctype != SYNCTYPE_UNKNOWN) {
                sync_missing = 0;
            } else {
                sync_missing += 1;
            }


            if (sync_missing >= 5) {
                fprintf(stderr, "lost sync at %i\n", ringbuffer_read_pos);
                sync = false;
            }

            // advance to the next frame. as long as we have sync, we know where the next frame begins
            ringbuffer_read_pos = mod(ringbuffer_read_pos + 144, RINGBUFFER_SIZE);
        }

    }
}