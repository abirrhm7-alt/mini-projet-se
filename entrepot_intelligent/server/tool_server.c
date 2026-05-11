#include "../include/common.h"

typedef struct {
    int id;
    int busy;
    pthread_mutex_t mutex;
} Tool;

Tool tools[MAX_TOOLS];

void init_tools() {
    for (int i = 0; i < MAX_TOOLS; i++) {
        tools[i].id = i;
        tools[i].busy = 0;
        pthread_mutex_init(&tools[i].mutex, NULL);
    }
}

void* handle_client(void* arg) {
    int client_fd = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    printf("Robot connected\n");
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);
        
        if (bytes <= 0) {
            printf("Robot disconnected\n");
            break;
        }
        
        printf("Received: %s\n", buffer);
        
        if (strncmp(buffer, "REQUEST", 7) == 0) {
            int tool_id;
            sscanf(buffer, "REQUEST %d", &tool_id);
            
            pthread_mutex_lock(&tools[tool_id].mutex);
            if (tools[tool_id].busy == 0) {
                tools[tool_id].busy = 1;
                sprintf(response, "OK tool %d granted", tool_id);
                printf("Tool %d granted\n", tool_id);
            } else {
                sprintf(response, "DENIED tool %d busy", tool_id);
                printf("Tool %d busy\n", tool_id);
            }
            pthread_mutex_unlock(&tools[tool_id].mutex);
            send(client_fd, response, strlen(response), 0);
        }
        else if (strncmp(buffer, "RELEASE", 7) == 0) {
            int tool_id;
            sscanf(buffer, "RELEASE %d", &tool_id);
            
            pthread_mutex_lock(&tools[tool_id].mutex);
            tools[tool_id].busy = 0;
            pthread_mutex_unlock(&tools[tool_id].mutex);
            
            sprintf(response, "OK tool %d released", tool_id);
            send(client_fd, response, strlen(response), 0);
            printf("Tool %d released\n", tool_id);
        }
    }
    
    close(client_fd);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    init_tools();
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);
    
    printf("Server listening on port %d...\n", PORT);
    
    while (1) {
        int* client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        
        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, client_fd);
        pthread_detach(thread);
    }
    
    return 0;
}
