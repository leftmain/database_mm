#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
//#include <netdb.h>
#include "header.h"
#include "database.h"

#define print_time() \
	do { \
		clock_gettime(CLOCK_MONOTONIC, &t2); \
		fprintf(stderr, "Time: %.2lf\n", t2.tv_sec - t1.tv_sec \
				+ (t2.tv_nsec - t1.tv_nsec) / M); \
	} while (0)


int new_client(int, fd_set *);
int old_client(int, fd_set *, Database<Record> *);
void write_on_socket(int, char *, int);
int read_from_socket(int, char *, int);
void delete_n(char *);

//static FILE * fp_log;

int main(int argc, char ** argv) {
	Database<Record> d;
	const char * file = "a.txt";
	FILE * fp = nullptr;
	int res = 0;

	if (argc > 1) file = argv[1];
	if (!(fp = fopen(file, "r"))) {
		perror("fopen() error");
		return 1;
	}
	if ((res = d.read(fp)) != ALL_RIGHT) {
		switch (res) {
		case CANNOT_OPEN:
			fprintf(stderr, "Cannot open file %s\n", file);
			break;
		case CANNOT_READ:
			fprintf(stderr, "Cannot read file %s\n", file);
			break;
		case MEM_ERR:
			fprintf(stderr, "Memory error\n");
			break;
		case DATA_ERR:
			fprintf(stderr, "Data error\n");
			break;
		default:
			fprintf(stderr, "Unknown error\n");
		}
		fclose(fp);
		return 1;
	}
	fclose(fp);
//	d.print();

	struct sockaddr_in addr;
	int sock = 0;
	int opt = 1;

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("socket() error");
		return 1;
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind() error");
		close(sock);
		return 2;
	}
	if (listen(sock, 3) < 0) {
		perror("listen() error");
		close(sock);
		return 2;
	}

	fd_set active_set, read_set;
	FD_ZERO(&active_set);
	FD_SET(sock, &active_set);

//	if (!(fp_log = fopen("log_server.txt", "w"))) {\
		perror("fopen() error");\
		return 1;\
	}

	struct timespec t1, t2;
	bool flag = true;
	while (true) {
		read_set = active_set;
		if (select(FD_SETSIZE, &read_set, 0, 0, 0) < 0) {
			perror("select() error");
			close(sock);
			return 3;
		}
		if (flag) {
			clock_gettime(CLOCK_MONOTONIC, &t1);
			flag = false;
		}
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &read_set)) {
				if (i == sock) {
					if (new_client(sock, &active_set) < 0) {
						perror("accept error");
						close(sock);
						return 3;
					}
				} else {
					res = old_client(i, &active_set, &d);
					if (res < 0) {
						perror("accept error");
						close(sock);
						return 3;
					} else if (res == 1) {
						print_time();
						write_on_socket(i, nullptr, -1);
						close(sock);
//						fclose(fp_log);
						return 0;
					}
				}
			}
		}
	}
	print_time();

	close(sock);
//	fclose(fp_log);
	return 0;
}

void write_on_socket(int fd, char * buf, int len) {
	write(fd, &len, sizeof(int));
	if (len > 0) write(fd, buf, len);
}

int read_from_socket(int fd, char * buf, int len) {
	if (read(fd, &len, sizeof(int)) != (int)sizeof(int)) {
		perror("read_from_socket() read_length error");
		return -1;
	}
	int l = len;
	int i = 0;
	int nbytes = 0;
	for (i = 0; len > 0; i += nbytes, len -= nbytes) {
		nbytes = read(fd, buf + i, len);
		if (nbytes < 0) {
			perror("read_from_socket() read() error");
			return -1;
		} else if (nbytes == 0) {
			perror("read truncated");
			return -1;
		}
	}
	buf[i] = 0;
#ifdef DEBUG
	delete_n(buf);
	printf("read\n\tbytes: %d\n\t_msg_: %s\n\t_len_: %d\n", i, (i > 0) ? buf : "empty", l);
#endif
	return l;
}

int new_client(int sock, fd_set * active_set) {
	struct sockaddr_in client;
	socklen_t size = sizeof(client);
	int new_sock = accept(sock, (struct sockaddr *)&client, &size);
	if (new_sock < 0) return new_sock;
//	fprintf(fp_log, "Server connect\n\thost: %s\n\tport: %d\n", \
			inet_ntoa(client.sin_addr), \
			(unsigned)ntohs(client.sin_port));
#ifdef DEBUG
	printf("Server connect\n\thost: %s\n\tport: %d\n", \
			inet_ntoa(client.sin_addr), \
			(unsigned)ntohs(client.sin_port));
#endif
	FD_SET(new_sock, active_set);
	return 0;
}

int old_client(int client, fd_set * active_set, Database<Record> * d) {
	char buf[LEN];
	int msg = -1;
	if (read_from_socket(client, buf, LEN) < 0) {
		write(client, &msg, sizeof(int));
		close(client);
		FD_CLR(client, active_set);
	} else {
		Command c;
		int res = c.parse(buf);
//		if (res) d->apply_command(c, STDOUT_FILENO);
		if (res) d->apply_command(c, client);
		else if (c.get_type() == STOP) return 1;
		else if (c.get_type() == QUIT) {
			write(client, &msg, sizeof(int));
			close(client);
			FD_CLR(client, active_set);
		}
//		write_on_socket(client, "end\n", 4);
		write_on_socket(client, nullptr, 0);
	}
	return 0;
}

void delete_n(char * s) {
	for (; *s; s++) if (*s == '\n') { *s = 0; return; }
}

