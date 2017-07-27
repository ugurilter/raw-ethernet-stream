#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Utils.h"
#include "Packet.h"

int main(){
	int counter;
	float percentage = 0;
	u_int8_t BCAST_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	FILE_INFO fileInfo = malloc(sizeof(FILE_INFO_t));
	uint8_t buf[BUF_SIZ];

	int sockfd = openSocket();

	BCAST_FILE packet = (BCAST_FILE) buf;

	memset(buf, 0, BUF_SIZ);												// clear buffer
	recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);							// receive 1 packet first

	/* Save file information to FILE_INFO */
	memcpy(fileInfo->file_name, packet->file_name, MAX_FILE_NAME_SIZE);
	fileInfo->file_size = packet->file_size;
	fileInfo->file_fragment_count = packet->file_fragment_count;

	/* Create an array full of 0s */
	counter = packet->file_fragment_count;
	int packetCheck[counter];
	memset(packetCheck, 0, counter*4);

	/* Create a buffer for whole data and clear */
	char *myFile = malloc(fileInfo->file_size);
	memset(myFile, 0, fileInfo->file_size);

	while(1){
		memset(buf, 0, BUF_SIZ);											// clear buffer
		recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);						// receive a packet
		if (!macCompare(packet->eth.ether_dhost, BCAST_MAC)) continue;		// if not broadcast discard !
		if(packet->type != 6) continue;										// if type is not 6 discard !

		int index = packet->fragment_index - 1;								// get fragment index
		if(counter){
			if(packetCheck[index] == 0){									// if this is the first time this fragment is captured
				int offset;

				if(packet->fragment_index != packet->file_fragment_count) offset = (index * packet->fragment_size);
				else offset = packet->file_size - packet->fragment_size;	// if packet is the last packet then the size if different

				memcpy(
					&myFile[offset],
					&(packet->fragment_data),
					(u_int32_t) packet->fragment_size
				);
				packetCheck[index] = 1;
				counter--;
				percentage++;
				float p = percentage / packet->file_fragment_count;
				if(counter % 100 == 0) printf("%.2f %%\n", p*100);
			}
		}
		else {
			break;
		}
	}

	printf("All fragments are captured! Now saving to file...");
	fflush(stdout);
	write_buffer_to_file(fileInfo->file_name, myFile, fileInfo->file_size);

	char *md5 = getMD5(fileInfo->file_name);

	printf("\nSending Ack: md5sum[%s]", md5);

	FILE_ACK fileAck = (FILE_ACK) malloc(sizeof(FILE_ACK_t));

	fileAck->type = 0x07;
	fileAck->length = packet->file_size;
	strcpy(fileAck->md5sum, md5);

	sendPacket(packet->eth.ether_shost, DEFAULT_IFACE, BUF_SIZ, (char *) fileAck);

	close(sockfd);

	free(fileInfo);
	free(myFile);

	return 0;
}




