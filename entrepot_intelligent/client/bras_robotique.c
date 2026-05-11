#include "../include/common.h"

typedef struct {
    int id;
    int socket_fd;
    int running;
    int tool1;
    int tool2;
} Robot;

Robot robot;

void* thinking_thread(void* arg) {
    while (robot.running) {
        sleep(rand() % 3 + 1);
        printf("[Robot %d] Thinking...\n", robot.id);
    }
    return NULL;
}

void* communication_thread(void* arg) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    while (robot.running) {
        // Ask for tools every 5 seconds
        sleep(5);
        
        // Request two tools
        sprintf(buffer, "REQUEST_TWO %d %d", robot.tool1, robot.tool2);
        send(robot.socket_fd, buffer, strlen(buffer), 0);
        
        memset(response, 0, BUFFER_SIZE);
        recv(robot.socket_fd, response, BUFFER_SIZE, 0);
        printf("[Robot %d] Server: %s\n", robot.id, response);
    }
    return NULL;
}

void* assembly_thread(void* arg) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    while (robot.running) {
        // Request tools
        sprintf(buffer, "REQUEST_TWO %d %d", robot.tool1, robot.tool2);
        send(robot.socket_fd, buffer, strlen(buffer), 0);
        
        memset(response, 0, BUFFER_SIZE);
        recv(robot.socket_fd, response, BUFFER_SIZE, 0);
        
        if (strncmp(response, "OK", 2) == 0) {
            printf("[Robot %d] Got tools! Assembling...\n", robot.id);
            sleep(rand() % 4 + 2);
            
            // Release tools
            sprintf(buffer, "RELEASE %d", robot.tool1);
            send(robot.socket_fd, buffer, strlen(buffer), 0);
            recv(robot.socket_fd, response, BUFFER_SIZE, 0);
            
            sprintf(buffer, "RELEASE %d", robot.tool2);
            send(robot.socket_fd, buffer, strlen(buffer), 0);
            recv(robot.socket_fd, response, BUFFER_SIZE, 0);
            
            printf("[Robot %d] Assembly done! Tools released.\n", robot.id);
        }
        
        sleep(rand() % 5 + 2);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Usage: %s <robot_id> <tool1> <tool2>\n", argv[0]);
        printf("Example: %s 1 0 1\n", argv[0]);
        return 1;
    }
    
    robot.id = atoi(argv[1]);
    robot.tool1 = atoi(argv[2]);
    robot.tool2 = atoi(argv[3]);
    robot.running = 1;
    
    srand(time(NULL) + robot.id);
    
    // Create socket
    robot.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    connect(robot.socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("[Robot %d] Connected to server\n", robot.id);
    
    // Create threads
    pthread_t think_t, comm_t, assembly_t;
    pthread_create(&think_t, NULL, thinking_thread, NULL);
    pthread_create(&comm_t, NULL, communication_thread, NULL);
    pthread_create(&assembly_t, NULL, assembly_thread, NULL);
    
    // Wait for user to stop
    printf("[Robot %d] Running. Press Enter to stop.\n", robot.id);
    getchar();
    
    robot.running = 0;
    send(robot.socket_fd, "EXIT", 4, 0);
    close(robot.socket_fd);
    
    pthread_cancel(think_t);
    pthread_cancel(comm_t);
    pthread_cancel(assembly_t);
    
    printf("[Robot %d] Stopped.\n", robot.id);
    return 0;
}
