#ifndef UTILS_H
#define UTILS_H

#define DEFAULT_IFACE	"enp3s0"
#define ETHER_TYPE		0x1234
#define BUF_SIZ			1500

int macCompare(u_int8_t *a, u_int8_t *b);
int write_buffer_to_file(char *file_name, char *buffer, u_int32_t size);
char *getMD5(char *filename);

#endif
