#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int KMP(char *text, char *pattern) {
    int occur = 0, m = strlen(text), n = strlen(pattern), seq[n + 1], j;

    if (*pattern == '\0' || n == 0 || *text == '\0' || n > m) return occur;

    for (int i = 0; i < n + 1; i++) seq[i] = 0;

    for (int i = 1; i < n; i++) {
        j = seq[i];

        while (j > 0 && pattern[j] != pattern[i]) j = seq[j];

        if (j > 0 || pattern[j] == pattern[i]) seq[i + 1] = j + 1;
    }

    for (int i = 0, j = 0; i < m; i++) {
        if (*(text + i) == *(pattern + j)) {
            if (++j == n) occur++;
        }
        else if (j > 0) {
            j = seq[j];
            i--;
        }
    }
    return occur;
}

int processing(char *filename, long file_len, char *pattern, long patt_len) {
    int result = 0;
    char buff[1024];
    FILE *fd = fopen(filename, "r");

    for (int i = 0; i < file_len/1024 + 1; i++) {
        fgets(buff, 1024, fd);
        result += KMP(buff, pattern);
        fseek(fd, -patt_len+1, SEEK_CUR);
    }
    fclose(fd);
    return result;
}

void decryption(char *filename, long file_len) {
    FILE *fd1 = fopen(filename, "r");
    strcat(filename, "_enc");
    FILE *fd2 = fopen(filename, "w");
    char keyword[10] = "QRHSTOZPKU";
    char c1, c2;

    for (int i = 0; i < file_len; i++) {
        c1 = fgetc(fd1);
        c2 = (c1 - keyword[i%10] + 26)%26;
        c2 += 'A';
        fputc(c2, fd2);
    }
    fclose(fd1);
    fclose(fd2);
}

void client_connection(int sock) {
    FILE *fd;
    int answer = 0;
    char buff[1024], filename[64];
    long file_len, patt_len, received, rec_all;

    if (recv(sock, &patt_len, 4, 0) == -1) {
        printf("(client #%d) error during connection\n\n", sock-3);
        close(sock);
        return;
    }
    patt_len = ntohl(patt_len);
    printf("(client #%d) received pattern length: %ld\n\n", sock-3, patt_len);

    int r = 0;
    char pattern[patt_len + 1];
    do {
        r += recv(sock, buff, patt_len, 0);
    } while(r < patt_len);
    for (int i = 0; i < patt_len; i++) pattern[i] = buff[i];
    pattern[patt_len] = '\0';
    printf("(client #%d) received pattern: %s\n\n", sock-3, pattern);

    if (recv(sock, &file_len, 4, 0) == -1) {
        printf("(client #%d) error during connection\n\n", sock-3);
        close(sock);
        return;
    }
    file_len = ntohl(file_len);
    printf("(client #%d) received file length: %ld\n\n", sock-3, file_len);

    rec_all = 0;
    sprintf(filename, "%d", sock);
    fd = fopen(filename, "w");
    while (rec_all < file_len) {
        memset(buff, 0, 1024);
        received = recv(sock, buff, 1023, 0);
        buff[1023] = '\0';
        if (received < 0) {
            printf("(client #%d) error during connection\n\n", sock-3);
            close(sock);
            return;
        }
        rec_all += received;
        fputs(buff, fd);
    }
    fclose(fd);
    printf("(client #%d) file received\n\n", sock-3);

    decryption(filename, file_len);
    printf("(client #%d) file decrypted\n\n", sock-3);

    answer = processing(filename, file_len, pattern, patt_len);
    sleep(10);
    printf("answer for client #%d: %d\n\n", sock-3, answer);
    answer = htonl(answer);

    send(sock, &answer, 4, 0);
    printf("(client #%d) answer sent\n\n", sock-3);

    close(sock);
}

int main(int argc, char **argv) {
    int sockL, sockC;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    sockL = socket(PF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockL, (struct sockaddr*) &addr, addr_len) < 0) {
        printf("bind failed\n");
        return 1;
    }

    listen(sockL, 10);
    int clients_num = 0;
    while (1) {
        sockC = accept(sockL, (struct sockaddr*) &addr, &addr_len);
        if (sockC < 0) {
            printf("accept failed\n\n");
            continue;
        }
        clients_num++;
        if (fork() == 0) {
            printf("connection with client #%d accepted\n\n", sockC-3);
            client_connection(sockC);
            close(sockC);
            printf("connection with client #%d closed\n\n", sockC-3);
            clients_num--;
            exit(0);
        }
        else {
            if (clients_num < 10) {
                printf("listening...\n\n");
                continue;
            }
            else {
                printf("too many clients, waiting...)\n\n");
                wait(NULL);
            }
        }
    }

    return 0;
}