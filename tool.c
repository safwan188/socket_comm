#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/un.h>
// ... other includes ...
#include <sys/time.h>
#define DATA_SIZE (100 * 1024 * 1024) // 100 MB
#define CHECKSUM_SIZE 32 // MD5 checksum size
#define BUFFER_SIZE 1024
void calculate_md5_checksum(unsigned char *data, size_t data_size, char *checksum) {
    FILE *f = fopen("temp_data.bin", "wb");
    if (f == NULL) {
        perror("File open error");
        exit(EXIT_FAILURE);
    }

    fwrite(data, 1, data_size, f);
    fclose(f);

    char cmd[128];
    snprintf(cmd, sizeof(cmd), "md5sum temp_data.bin");
    FILE *pipe = popen(cmd, "r");
    if (pipe == NULL) {
        perror("popen error");
        exit(EXIT_FAILURE);
    }

    if (fgets(checksum, 33, pipe) == NULL) {
        perror("fgets error");
        exit(EXIT_FAILURE);
    }

    pclose(pipe);
    remove("temp_data.bin");
}
unsigned long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
void test_ipv4_tcp_client(const char *ip, int port);
void test_ipv4_tcp_server(int port);
        typedef enum {
    TEST_IPV4_TCP,
    TEST_IPV4_UDP,
    TEST_IPV6_TCP,
    TEST_IPV6_UDP,
    TEST_UDS_DGRAM,
    TEST_UDS_STREAM,
    TEST_MMAP,
    TEST_PIPE
} test_type_t;
#define BUFFER_SIZE 1024

void test_ipv6_tcp_client(const char *ip, int port) {
    int sock_fd;
    struct sockaddr_in6 server_addr;
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(port);

    if (inet_pton(AF_INET6, ip, &server_addr.sin6_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    // Generate 100MB chunk of data
    unsigned char *data = malloc(DATA_SIZE);
    if (data == NULL) {
        perror("Data allocation error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }

    // Calculate the checksum for the data
    unsigned char checksum[CHECKSUM_SIZE];
    calculate_md5_checksum(data, DATA_SIZE, checksum);

    // Transmit the data and measure the time it takes
    unsigned long start_time = get_time_ms();
    int bytes_sent = 0;
    while (bytes_sent < DATA_SIZE) {
        int sent = send(sock_fd, data + bytes_sent, BUFFER_SIZE, 0);
        if (sent < 0) {
            perror("Error sending data");
            break;
        }
        bytes_sent += sent;
    }
    unsigned long end_time = get_time_ms();

    printf("ipv6_tcp,%lu\n", end_time - start_time);

    close(sock_fd);
    free(data);
}

void test_ipv6_tcp_server(int port) {
    int server_fd, client_fd;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 1) < 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    addr_len = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept error");
        exit(EXIT_FAILURE);
    }

    // Receive 100MB chunk of data
    char *data = malloc(DATA_SIZE);
    int bytes_received = 0;
    while (bytes_received < DATA_SIZE) {
        int received = recv(client_fd, data + bytes_received, BUFFER_SIZE, 0);
        if (received <= 0) {
            perror("Error receiving data");
            break;
        }
        bytes_received += received;
    }

    // Check the checksum of the received data
    // ... calculate checksum and compare ...

    close(client_fd);
    close(server_fd);
    free(data);
}



void test_uds_stream_client() {
    const char *socket_path ="/tmp/my_dgram_socket";
    int sock_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    struct timespec start_time, end_time;
    size_t sent_data_size = 0;

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    // Generate data and checksum
    unsigned char *data = malloc(DATA_SIZE);
    if (data == NULL) {
        perror("Data allocation error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }

// Calculate the checksum for the data
    unsigned char checksum[CHECKSUM_SIZE];
    calculate_md5_checksum(data, DATA_SIZE, checksum);
    // Generate a checksum for the data (omitted in this example for simplicity)

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    while (sent_data_size < DATA_SIZE) {
        size_t remaining_data_size = DATA_SIZE - sent_data_size;
        ssize_t sent_len = send(sock_fd, data + sent_data_size, remaining_data_size, 0);
        if (sent_len < 0) {
            perror("Send error");
            exit(EXIT_FAILURE);
        }
        sent_data_size += sent_len;
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    unsigned long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_nsec - start_time.tv_nsec) / 1000000;
    printf("uds_stream,%lu\n", elapsed_time);

    close(sock_fd);
    free(data);
}

void test_uds_stream_server() {
    const char *socket_path ="/tmp/my_dgram_socket";
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];
    size_t received_data_size = 0;


    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    addr_len = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept error");
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");

    while (received_data_size < DATA_SIZE) {
        ssize_t recv_len = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (recv_len < 0) {
            perror("Receive error");
            exit(EXIT_FAILURE);
        }
        received_data_size += recv_len;
    }

    printf("Received %zu bytes of data\n", received_data_size);

    close(client_fd);
    close(server_fd);
}

void test_uds_dgram_client() {
    const char *socket_path ="/tmp/my_uds_socket";
    int sock_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    struct timespec start_time, end_time;
    size_t sent_data_size = 0;

    if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);

    // Generate data and checksum
    unsigned char *data = malloc(DATA_SIZE);
    if (data == NULL) {
        perror("Data allocation error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }

// Calculate the checksum for the data
    unsigned char checksum[CHECKSUM_SIZE];
    calculate_md5_checksum(data, DATA_SIZE, checksum);
    // Generate a checksum for the data (omitted in this example for simplicity)

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    while (sent_data_size < DATA_SIZE) {
        size_t remaining_data_size = DATA_SIZE - sent_data_size;
        size_t send_len = remaining_data_size < BUFFER_SIZE ? remaining_data_size : BUFFER_SIZE;
        sendto(sock_fd, data + sent_data_size, send_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        sent_data_size += send_len;
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    unsigned long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_nsec - start_time.tv_nsec) / 1000000;


    close(sock_fd);
    free(data);
}



void test_pipe_client(const char *pipe_filename) {
    int pipe_fd = open(pipe_filename, O_WRONLY);
    if (pipe_fd < 0) {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    unsigned char *buffer = malloc(DATA_SIZE);
    if (buffer == NULL) {
        perror("Data allocation error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < DATA_SIZE; i++) {
        buffer[i] = rand() % 256;
    }

    size_t sent_data_size = 0;
    while (sent_data_size < DATA_SIZE) {
        ssize_t sent_len = write(pipe_fd, buffer + sent_data_size, DATA_SIZE - sent_data_size);
        if (sent_len < 0) {
            perror("write error");
            exit(EXIT_FAILURE);
        }
        sent_data_size += sent_len;
    }

    printf("Sent %zu bytes of data\n", sent_data_size);

    close(pipe_fd);
}

void test_pipe_server(const char *pipe_filename) {
    mkfifo(pipe_filename, 0666);


    int pipe_fd = open(pipe_filename, O_RDONLY);
    if (pipe_fd < 0) {
        perror("open error");
        exit(EXIT_FAILURE);
    }


    char *buffer = malloc(DATA_SIZE);
    size_t received_data_size = 0;

    while (received_data_size < DATA_SIZE) {
        ssize_t recv_len = read(pipe_fd, buffer + received_data_size, DATA_SIZE - received_data_size);
        if (recv_len < 0) {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        received_data_size += recv_len;
    }

    printf("Received %zu bytes of data\n", received_data_size);

    close(pipe_fd);
    unlink(pipe_filename);
}

void test_uds_dgram_server() {
    const char *socket_path ="/tmp/my_uds_socket";
    int sock_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path, sizeof(server_addr.sun_path) - 1);
    unlink(socket_path);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Receive 100MB chunk of data
    char *data = malloc(DATA_SIZE);
    int bytes_received = 0;
    while (bytes_received < DATA_SIZE) {
        int received = recvfrom(sock_fd, data + bytes_received, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr, &addr_len);
        if (received <= 0) {
            perror("Error receiving data");
            break;
        }
        bytes_received += received;
    }

    // Check the checksum of the received data
    // ... calculate checksum and compare ...

    close(sock_fd);
    free(data);
}
void test_mmap_server(const char *filename) {
    int fd;
    void *file_ptr;

    if ((fd = open(filename, O_CREAT | O_RDWR, 0666)) < 0) {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, DATA_SIZE) < 0) {
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    file_ptr = mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_ptr == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    // Wait for data to be written to the file
    sleep(5);

    printf("Received %d bytes of data\n", DATA_SIZE);

    munmap(file_ptr, DATA_SIZE);
    close(fd);
    unlink(filename);
}
void test_mmap_client(const char *filename) {
    int fd;
    void *file_ptr;

    if ((fd = open(filename, O_RDWR, 0666)) < 0) {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    file_ptr = mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_ptr == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    unsigned char *data = malloc(DATA_SIZE);
    if (data == NULL) {
        perror("Data allocation error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }
    unsigned long start_time = get_time_ms();
    // Write data to the file
    memcpy(file_ptr, data, DATA_SIZE);

    munmap(file_ptr, DATA_SIZE);
    unsigned long end_time = get_time_ms();

    printf("mmap,%lu\n", end_time - start_time);
    close(fd);
    free(data);
}
void test_ipv6_udp_client(const char *ip, int port) {
    int sock_fd;
    struct sockaddr_in6 server_addr;
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(port);

    if (inet_pton(AF_INET6, ip, &server_addr.sin6_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Generate 100MB chunk of data
    unsigned char *data = malloc(DATA_SIZE);
    if (data == NULL) {
        perror("Data allocation error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }

    // Calculate the checksum for the data
    unsigned char checksum[CHECKSUM_SIZE];
    calculate_md5_checksum(data, DATA_SIZE, checksum);

    // Transmit the data and measure the time it takes
    unsigned long start_time = get_time_ms();
    int bytes_sent = 0;
    while (bytes_sent < DATA_SIZE) {
        int sent = sendto(sock_fd, data + bytes_sent, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (sent < 0) {
            perror("Error sending data");
            break;
        }
        bytes_sent += sent;
    }
    unsigned long end_time = get_time_ms();

    printf("ipv6_udp,%lu\n", end_time - start_time);

    close(sock_fd);
    free(data);
}

void test_ipv6_udp_server(int port) {
    int server_fd;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Receive 100MB chunk of data
    char *data = malloc(DATA_SIZE);
    int bytes_received = 0;
    while (bytes_received < DATA_SIZE) {
        addr_len = sizeof(client_addr);
        int received = recvfrom(server_fd, data + bytes_received, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (received <= 0) {
            perror("Error receiving data");
            break;
        }
        bytes_received += received;
    }

    // Check the checksum of the received data
    // ... calculate checksum and compare ...

    close(server_fd);
    free(data);
}


void test_ipv4_udp_client(const char *ip, int port) {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Generate 100MB chunk of data
    unsigned char *data = malloc(DATA_SIZE);
    if (data == NULL) {
        perror("Data allocation error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }

    // Calculate the checksum for the data
    unsigned char checksum[CHECKSUM_SIZE];
    calculate_md5_checksum(data, DATA_SIZE, checksum);

    // Transmit the data and measure the time it takes
    unsigned long start_time = get_time_ms();
    int bytes_sent = 0;
    while (bytes_sent < DATA_SIZE) {
        int sent = sendto(sock_fd, data + bytes_sent, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (sent < 0) {
            perror("Error sending data");
            break;
        }
        bytes_sent += sent;
    }
    unsigned long end_time = get_time_ms();

    printf("ipv4_udp,%lu\n", end_time - start_time);

    close(sock_fd);
    free(data);
}

void test_ipv4_udp_server(int port) {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Receive 100MB chunk of data
    char *data = malloc(DATA_SIZE);
    int bytes_received = 0;
    while (bytes_received < DATA_SIZE) {
        addr_len = sizeof(client_addr);
        int received = recvfrom(server_fd, data + bytes_received, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (received <= 0) {
            perror("Error receiving data");
            break;
        }
        bytes_received += received;
    }

    // Check the checksum of the received data
    // ... calculate checksum and compare ...

    close(server_fd);
    free(data);
}

int main(int argc, char *argv[]) {
    // Check if the user provided enough arguments
    if (argc < 4) {
        fprintf(stderr, "Usage: %s -c IP PORT -p <type> <param> or %s -s PORT -p <type> <param> -q\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }
    char *pipe_filename;
    char * file;
    int is_client = 0;
    const char *ip = NULL;
    int port = 0;
    test_type_t test_type = TEST_IPV4_TCP;
    char param[256] = {0};
    int quiet_mode = 0;

    // Parse command-line arguments
    int opt;
    char **p=argv;
    while ((opt = getopt(argc, argv, "c:s:p:q")) != -1) {
        switch (opt) {
            case 'c':
                is_client = 1;
                ip = optarg;
                port = atoi(argv[optind]);
                break;
            case 's':
                is_client = 0;
                port = atoi(optarg);
                break;
            case 'p':
                if (strcmp(optarg, "ipv4") == 0) {
                    if (optind < argc && strcmp(argv[optind], "tcp") == 0) {
                        test_type = TEST_IPV4_TCP;
                        optind++; // Consume "tcp" argument
                    } else if (optind < argc && strcmp(argv[optind], "udp") == 0) {
                        test_type = TEST_IPV4_UDP;
                        optind++; // Consume "udp" argument
                    }
                } else  if (strcmp(optarg, "ipv6") == 0) {
                    if (optind < argc && strcmp(argv[optind], "tcp") == 0) {
                        test_type = TEST_IPV6_TCP;
                        optind++; // Consume "tcp" argument
                    } else if (optind < argc && strcmp(argv[optind], "udp") == 0) {
                        test_type = TEST_IPV6_UDP;
                        optind++; // Consume "udp" argument
                    }
                } else  if (strcmp(optarg, "uds") == 0) {
                    if (optind < argc && strcmp(argv[optind], "dgram") == 0) {
                            test_type = TEST_UDS_DGRAM;
                        optind++; // Consume "DGRAM" argument
                    } else if (optind < argc && strcmp(argv[optind], "stream") == 0) {
                        test_type = TEST_UDS_STREAM;
                        optind++; // Consume "STREAM" argument
                    }
                }
                else  if (strcmp(optarg, "mmap") == 0) {
                    if (optind < argc) {
                        test_type = TEST_MMAP;
                        file=argv[optind];
                        optind++; // Consume "DGRAM" argument
                    }
                }
                else  if (strcmp(optarg, "pipe") == 0) {

                    if (optind < argc) {
                        test_type = TEST_PIPE;
                        file = argv[optind];
                        optind++; // Consume the filename argument
                    } else {
                        fprintf(stderr, "Pipe filename is missing\n");
                        exit(EXIT_FAILURE);
                    }
                }
                // ... other test types ...
                break;
            case 'q':
                quiet_mode = 1;
                break;
            default:
                fprintf(stderr, "Invalid arguments\n");
                exit(EXIT_FAILURE);
                break;
        }
    }

    // Run the appropriate test based on the command-line arguments
    switch (test_type) {
        case TEST_IPV4_TCP:
            if (is_client) {
                test_ipv4_tcp_client(ip, port);

            } else {
                test_ipv4_tcp_server(port);
            }
            break;
        case TEST_IPV4_UDP:
            if (is_client) {
                test_ipv4_udp_client(ip, port);

            } else {
                test_ipv4_udp_server(port);
            }
            break;
        case TEST_IPV6_UDP:
            if (is_client) {
                test_ipv6_udp_client(ip, port);

            } else {
                test_ipv6_udp_server(port);
            }
            break;
        case TEST_IPV6_TCP:
            if (is_client) {
                test_ipv6_tcp_client(ip, port);

            } else {
                test_ipv6_tcp_server(port);
            }
            break;
        case TEST_UDS_STREAM:
            if (is_client) {
                test_uds_stream_client();

            } else {
                test_uds_stream_server();
            }
            break;
        case TEST_UDS_DGRAM:
            if (is_client) {
                test_uds_dgram_client();

            } else {
                test_uds_dgram_server();
            }
            break;
        case TEST_MMAP:
            if (is_client) {
                test_mmap_client(file);

            } else {
                test_mmap_server(file);
            }
            break;
        case TEST_PIPE:
            if (is_client) {
                test_pipe_client(file);
            } else {
                test_pipe_server(file);
            }
            break;


         default:
            fprintf(stderr, "Invalid test type\n");
           exit(EXIT_FAILURE);
            break;
    }

    return 0;
}




void test_ipv4_tcp_client(const char *ip, int port) {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    // Generate 100MB chunk of data

    unsigned char *data = malloc(DATA_SIZE);
    if (data == NULL) {
        perror("Data allocation error");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }

// Calculate the checksum for the data
    unsigned char checksum[CHECKSUM_SIZE];
    calculate_md5_checksum(data, DATA_SIZE, checksum);
    // Generate a checksum for the data (omitted in this example for simplicity)

    // Transmit the data and measure the time it takes
    unsigned long start_time = get_time_ms();
    int bytes_sent = 0;
    while (bytes_sent < DATA_SIZE) {
        int sent = send(sock_fd, data + bytes_sent, BUFFER_SIZE, 0);
        if (sent < 0) {
            perror("Error sending data");
            break;
        }
        bytes_sent += sent;
    }
    unsigned long end_time = get_time_ms();

    printf("ipv4_tcp,%lu\n", end_time - start_time);

    close(sock_fd);
    free(data);
}

void test_ipv4_tcp_server(int port) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 1) < 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    addr_len = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addr_len)) < 0) {
        perror("Accept error");
        exit(EXIT_FAILURE);
    }

    // Receive 100MB chunk of data
    char *data = malloc(DATA_SIZE);
    int bytes_received = 0;
    while (bytes_received < DATA_SIZE) {
        int received = recv(client_fd, data + bytes_received, BUFFER_SIZE, 0);
        if (received <= 0) {
            perror("Error receiving data");
            break;
        }
        bytes_received += received;
    }

    // Check the checksum of the received data
    // ... calculate checksum and compare ...

    close(client_fd);
    close(server_fd);
    free(data);
}