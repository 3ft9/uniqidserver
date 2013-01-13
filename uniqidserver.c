/**
 * Unique ID server. Runs a TCP server. Connect to that server and it will send
 * you an ID and immediately close the connection.
 *
 * Usage: uniqidserver <port> <machineid>
 *
 * The machine ID should be a number between 0 and 255. The number is used when
 * generating IDs. This enables you to run multiple ID servers and ensures that
 * IDs created on different machines will never clash.
 *
 * IDs are as follows:
 *
 *   aabbccccccddd
 *
 * Where:
 *
 *   aaaa = first four chars of the secs since epoch in hex
 *   bb   = the machine ID in hex
 *   cccc = the rest of the secs since epoch in hex
 *   ddd  = the ms part of time time since epoch in hex
 *
 * This gives us IDs that are roughly ordered by time, even across servers, that
 * don't have a 00 at the end of every ID if only running one ID server (always
 * looked odd to me), and is horizontally scalable across up to 256 servers. To
 * add more servers simply increase the machine ID from one to two bytes.
 *
 * If the machine ID is not provided, is -1, or fails to convert to an integer
 * between 0 and 255, the bb part above will be excluded.
 * 
 * Source: https://github.com/3ft9/uniqidserver
 */
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
	int listenfd, connfd, port, machine_id;
	struct sockaddr_in servaddr, cliaddr;
	struct timeval tv;
	socklen_t clilen;
	char secs_buf[9], id_buf[14];
	char machine_id_hex[3];

	if (argc != 2 && argc != 3) {
		printf("Usage: uniqidserver <port> <machineid>\n");
		return 1;
	}

	port = atoi(argv[1]);
	machine_id = (argc == 2 ? -1 : atoi(argv[2]));

	if (machine_id < 0 || machine_id > 255) {
		fprintf(stderr, "WARN: Not using a machine ID\n");
		machine_id = -1;
	} else {
		// Convert the machine ID to hex.
		sprintf(machine_id_hex, "%02x", machine_id);
	}

	while (1) {
		listenfd = socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(port);
		bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		listen(listenfd, 1024);
		clilen = sizeof(cliaddr);

		while (1) {
			connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
			if (connfd) {
				if (gettimeofday(&tv, NULL) == -1) {
					sendto(connfd, "ERR", 3, 0, (struct sockaddr *)&cliaddr, clilen);
				} else {
					if (machine_id == -1) {
						// No machine ID.
						snprintf(id_buf, 14, "%08x%03x", (unsigned int)tv.tv_sec, tv.tv_usec);
					} else {
						// With machine ID.
						snprintf(secs_buf, 9, "%08x", (unsigned int)tv.tv_sec);
						snprintf(id_buf, 14, "%c%c%c%c%c%c%c%c%c%c%03x",
							secs_buf[0], secs_buf[1], secs_buf[2], secs_buf[3],
							machine_id_hex[0], machine_id_hex[1],
							secs_buf[4], secs_buf[5], secs_buf[6], secs_buf[7],
							tv.tv_usec);
					}
					sendto(connfd, id_buf, (machine_id == -1 ? 11 : 13), 0, (struct sockaddr *)&cliaddr, clilen);
				}
				close(connfd);
			}
		}

		close(listenfd);
	}

	return 0;
}
