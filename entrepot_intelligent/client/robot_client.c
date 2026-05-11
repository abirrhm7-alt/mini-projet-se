#include "../include/common.h"

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("[ROBOT] Connected to server\n");
    
    // Request tool 0
    send(client_fd, "REQUEST 0", 9, 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    printf("[ROBOT] %s\n", buffer);
    
    // Request tool 0 again (should be denied)
    send(client_fd, "REQUEST 0", 9, 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    printf("[ROBOT] %s\n", buffer);
    
    // Release tool 0
    send(client_fd, "RELEASE 0", 9, 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    printf("[ROBOT] %s\n", buffer);
    
    // Request again (should work now)
    send(client_fd, "REQUEST 0", 9, 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    printf("[ROBOT] %s\n", buffer);
    
    close(client_fd);
    return 0;
}
