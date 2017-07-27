#ifndef PACKET_H
#define PACKET_H

#define MAX_NAME_SIZE 10
#define MAX_FILE_NAME_SIZE 32

typedef struct BCAST_FILE_s {
    struct ether_header eth;					//14
    uint8_t type;								//1
    char name[MAX_NAME_SIZE];					//32
    char surname[MAX_NAME_SIZE];				//10
    char file_name[MAX_FILE_NAME_SIZE];			//10
    uint32_t file_size;							//4
    uint16_t file_fragment_count;				//2
    uint16_t fragment_index;					//2
    uint32_t fragment_size;						//4
    char fragment_data[0];
} __attribute__((__packed__)) BCAST_FILE_t[1], *BCAST_FILE;

typedef struct FILE_INFO_s {
	uint32_t file_size;
	uint16_t file_fragment_count;
    char file_name[MAX_FILE_NAME_SIZE];
}__attribute__((__packed__)) FILE_INFO_t[1], *FILE_INFO;

typedef struct FILE_ACK_s {
	u_int8_t type;
	u_int16_t length;
    char md5sum[32];
}__attribute__((__packed__)) FILE_ACK_t[1], *FILE_ACK;

int openSocket();
void sendPacket(u_int8_t *destMac, char *interface, int bufSize, char *packet);

#endif
