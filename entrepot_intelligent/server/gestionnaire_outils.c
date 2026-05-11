#include "../include/common.h"

typedef struct {
    int id;
    int busy;
    int owner;
    pthread_mutex_t mutex;
} Tool;

Tool tools[MAX_TOOLS];
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_msg(const char* msg) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    pthread_mutex_lock(&log_mutex);
    printf("[%02d:%02d:%02d] %s\n", t->tm_hour, t->tm_min, t->tm_sec, msg);
    pthread_mutex_unlock(&log_mutex);
}

void init_tools() {
    for (int i = 0; i < MAX_TOOLS; i++) {
        tools[i].id = i;
        tools[i].busy = 0;
        tools[i].owner = -1;
        pthread_mutex_init(&tools[i].mutex, NULL);
    }
    log_msg("Tools initialized");
}

int request_tool(int tool_id, int client_id) {
    if (tool_id < 0 || tool_id >= MAX_TOOLS) return 0;
    
    pthread_mutex_lock(&tools[tool_id].mutex);
    if (tools[tool_id].busy == 0) {
        tools[tool_id].busy = 1;
        tools[tool_id].owner = client_id;
        pthread_mutex_unlock(&tools[tool_id].mutex);
        return 1;
    }
    pthread_mutex_unlock(&tools[tool_id].mutex);
    return 0;
}

int release_tool(int tool_id, int client_id) {
    if (tool_id < 0 || tool_id >= MAX_TOOLS) return 0;
    
    pthread_mutex_lock(&tools[tool_id].mutex);
    if (tools[tool_id].owner == client_id) {
        tools[tool_id].busy = 0;
        tools[tool_id].owner = -1;
        pthread_mutex_unlock(&tools[tool_id].mutex);
        return 1;
    }
    pthread_mutex_unlock(&tools[tool_id].mutex);
    return 0;
}

int request_two_tools(int t1, int t2, int client_id) {
    // Order to prevent deadlock
    if (t1 > t2) {
        int temp = t1;
        t1 = t2;
        t2 = temp;
    }
    
    // Request first tool
    if (!request_tool(t1, client_id)) {
        return 0;
    }
    
    // Request second tool
    if (!request_tool(t2, client_id)) {
        release_tool(t1, client_id);
        return 0;
    }
    
    return 1;
}

void* handle_client(void* arg) {
    int client_fd = *(int*)arg;
    int client_id = client_fd;
    free(arg);
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    char msg[100];
    sprintf(msg, "Robot %d connected", client_id);
    log_msg(msg);
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);
        
        if (bytes <= 0) {
            sprintf(msg, "Robot %d disconnected", client_id);
            log_msg(msg);
            break;
        }
        
        buffer[bytes] = '\0';
        
        if (strncmp(buffer, "REQUEST", 7) == 0 && strncmp(buffer, "REQUEST_TWO", 11) != 0) {
            int tool_id;
            sscanf(buffer, "REQUEST %d", &tool_id);
            
            if (request_tool(tool_id, client_id)) {
                sprintf(response, "OK tool %d granted", tool_id);
                sprintf(msg, "Robot %d got tool %d", client_id, tool_id);
            } else {
                sprintf(response, "DENIED tool %d busy", tool_id);
                sprintf(msg, "Robot %d denied tool %d", client_id, tool_id);
            }
            log_msg(msg);
            send(client_fd, response, strlen(response), 0);
        }
        else if (strncmp(buffer, "REQUEST_TWO", 11) == 0) {
            int t1, t2;
            sscanf(buffer, "REQUEST_TWO %d %d", &t1, &t2);
            
            if (request_two_tools(t1, t2, client_id)) {
                sprintf(response, "OK tools %d and %d granted", t1, t2);
                sprintf(msg, "Robot %d got tools %d and %d", client_id, t1, t2);
            } else {
                sprintf(response, "DENIED tools %d and %d busy", t1, t2);
                sprintf(msg, "Robot %d denied tools %d and %d", client_id, t1, t2);
            }
            log_msg(msg);
            send(client_fd, response, strlen(response), 0);
        }
        else if (strncmp(buffer, "RELEASE", 7) == 0) {
            int tool_id;
            sscanf(buffer, "RELEASE %d", &tool_id);
            
            if (release_tool(tool_id, client_id)) {
                sprintf(response, "OK tool %d released", tool_id);
                sprintf(msg, "Robot %d released tool %d", client_id, tool_id);
            } else {
                sprintf(response, "ERROR tool %d not yours", tool_id);
                sprintf(msg, "Robot %d failed to release tool %d", client_id, tool_id);
            }
            log_msg(msg);
            send(client_fd, response, strlen(response), 0);
        }
        else if (strncmp(buffer, "STATUS", 6) == 0) {
            sprintf(response, "Tools: ");
            for (int i = 0; i < MAX_TOOLS; i++) {
                char status[30];
                pthread_mutex_lock(&tools[i].mutex);
                sprintf(status, "%d:%s ", i, tools[i].busy ? "BUSY" : "FREE");
                strcat(response, status);
                pthread_mutex_unlock(&tools[i].mutex);
            }
            send(client_fd, response, strlen(response), 0);
        }
        else if (strncmp(buffer, "EXIT", 4) == 0) {
            break;
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
    
    log_msg("Server started");
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
