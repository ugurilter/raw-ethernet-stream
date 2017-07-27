#include <net/if.h>
#include <sys/ioctl.h>
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

int openSocket(){
	struct ifreq ifopts;	/* set promiscuous mode */
	struct ifreq if_ip;		/* get ip addr */
	int sockopt;
	int sockfd;

	char ifName[IFNAMSIZ];

	strcpy(ifName, DEFAULT_IFACE);
	memset(&if_ip, 0, sizeof(struct ifreq));

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");
	}

	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

void sendPacket(u_int8_t *destMac, char *interface, int bufSize, char *packet){
	int sockfd;

	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	char sendbuf[bufSize];
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct sockaddr_ll socket_address;
	char ifName[IFNAMSIZ];

	strcpy(ifName, interface);

	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		perror("socket");
	}

	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
		perror("SIOCGIFINDEX");
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
		perror("SIOCGIFHWADDR");

	memset(sendbuf, 0, bufSize);
	memcpy(eh->ether_shost, ((uint8_t *)&if_mac.ifr_hwaddr.sa_data), 6);
	memcpy(eh->ether_dhost, destMac, 6);
	eh->ether_type = htons(0x1234);
	tx_len += sizeof(struct ether_header);

	int ctr, size = 35;
	for(ctr = 0; ctr < size; ctr++) sendbuf[tx_len++] = packet[ctr];

	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	socket_address.sll_halen = ETH_ALEN;		/* Octets in one ethernet addr	 */
	memcpy(socket_address.sll_addr, destMac, 6);

	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
		printf("Send failed\n");
}
