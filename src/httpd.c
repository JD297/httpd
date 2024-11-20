#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <unistd.h>

#define PORT 8080

void print_usage()
{	printf("Usage: %s [FILE]\n", TARGET);
	printf("Hyper Text Transfer Protocol server\n");
	printf("\n");
	printf("JD297 httpd source code <https://github.com/jd297/%s>\n", TARGET);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		print_usage();
		exit(EXIT_FAILURE);
	}

	int sockfd = sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		err(EXIT_FAILURE, "socket");
	}

	struct sockaddr_in host_addr;

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(PORT);
	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int host_addrlen = sizeof(host_addr);

	if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) == -1) {
		err(EXIT_FAILURE, "bind");
	}

	if (listen(sockfd, SOMAXCONN) != 0) {
		err(EXIT_FAILURE, "listen");
	}

	for (size_t i = 0; i < 3; i++) {
		int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);

		if (newsockfd  == -1) {
			err(EXIT_FAILURE, "accept");
		}

		// SERVER CLIENT INTERACTION HERE
		struct stat sb;

		char *filename = argv[1];

		if (stat(filename, &sb) == -1) {
			err(EXIT_FAILURE, "stat");
		}

		int fd = open(filename, O_RDONLY);

		if (fd == -1) {
			err(EXIT_FAILURE, "open");
		}

		char *buf = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

		if (buf == -1) {
			err(EXIT_FAILURE, "mmap");
		}

		char header[4096];

		sprintf(header, "HTTP/1.1 200 OK\n\rServer: %s\n\r\Connection: close\n\rContent-Type: text/plain\n\rContent-Length: %ld\n\r\n\r", TARGET, sb.st_size);

		if (send(newsockfd, header, strlen(header) - 1, MSG_CONFIRM) == -1) {
			err(EXIT_FAILURE, "send (header)");
		}

		if (send(newsockfd, buf, sb.st_size, MSG_CONFIRM) == -1) {
			err(EXIT_FAILURE, "send (content)");
		}

		if (munmap(buf, sb.st_size) == -1) {
			err(EXIT_FAILURE, "munmap");
		}
		// INTERACTION END

		if (shutdown(newsockfd, SHUT_RDWR) == -1) {
			err(EXIT_FAILURE, "shutdown");
		}

		close(newsockfd);
	}

	close(sockfd);

	return 0;
}
