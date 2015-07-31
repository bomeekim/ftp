/*
 * client.c
 * 
 * Copyright 2015 bomikim <bomikim@nemustech.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF_LEN 2048
#define MAX_LINE 61440 //60KByte

void parsing(char *recv_message, char *file_list)
{
    file_list = strtok(recv_message, "#");
    
    printf("[Client] 서버 파일 목록\n");
    
    while (file_list != NULL)
    {
        printf("%9s\n", file_list);
        file_list = strtok(NULL, "#");
    }
    printf("\n");
}

void receive_file_from_server(int sock, char *recv_message, int recv_msg_len, char *file_name)
{
    FILE *fp;
    char buffer[MAX_LINE];
    int recv_size;
    unsigned long file_size;
    unsigned long remain_size;
    
    snprintf(buffer, sizeof(buffer), "123%s", file_name);
    
    fp = fopen(buffer, "wb");
    
    memset(buffer, 0, sizeof(buffer));
    recv_size = read(sock, &file_size, sizeof(file_size)); 
    
    if(recv_size == sizeof(file_size))
    {
        file_size = ntohl(file_size);
        printf("[Client] received file size : %lu\n", file_size);
    }
    else
    {
        printf("[Error] file size error...!\n");
    }
    
    remain_size = file_size;
    
    do
    {
        recv_size = read(sock, recv_message, recv_msg_len);
        
        if(recv_size < 0)
        {
            printf("[Error] read error...!\n");
            break;
        }
        
        remain_size -= recv_size;
        
        fwrite(recv_message, sizeof(char), recv_size, fp);
        //printf("\r[Client] receive : %d%%   ", (int)((((double) file_size-remain_size)/file_size) * 100.0));
        printf("\r%d%%   ", (int)((((double) file_size-remain_size)/file_size) * 100.0));
        
        fflush(stdout);
        
    }while(remain_size > 0);
    
    printf("\n");
    
    fclose(fp);
}

int main(int argc, char **argv)
{
    int sock;
    
    char send_message[BUF_LEN];
    char recv_message[BUF_LEN];
    char file_list[BUF_LEN];
    char file_name[BUF_LEN];
    char dir[BUF_LEN];
    char command[BUF_LEN];
    
    struct sockaddr_in serv_addr;
    
    memset(send_message, 0, sizeof(send_message));
    memset(recv_message, 0, sizeof(recv_message));
    
    if(argc != 3)
    {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    
    if(sock == -1)
    {
        printf("Client : Can't open stream socket\n");
        exit(0);
    }
    
    memset(&serv_addr, 0x00, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("Client : Can't connect to server\n");
        exit(0);
    }
    
    
    //start a ftp program
    printf("\n >>>>>>>>>>>>> Welcome to nemus ftp program <<<<<<<<<<<<<\n\n");
    
    while(1)
    {
        
        memset(send_message, 0, sizeof(send_message));
        memset(recv_message, 0, sizeof(recv_message));
        
        fputs("[Client] 전송할 메시지 입력 (ls, up, dw, quit) : ", stdout);
        fgets(command, BUF_LEN, stdin);
        
        if(!strcmp(command, "quit\n")) 
        {
            printf("[Client] 서버와 연결을 종료합니다. \n");
            printf("\n >>>>>>>>>>>>> End to nemus ftp program <<<<<<<<<<<<<\n\n");
            break;
        }
        else if(!strcmp(command, "ls\n"))
        {
            fputs("[Client] 디렉토리 입력 : ", stdout);
            fgets(dir, BUF_LEN, stdin);
            
            command[strlen(command)-1] = '\0';
            dir[strlen(dir)-1] = '\0';
            
            snprintf(send_message, sizeof(send_message), "%s%s", command, dir);
            
            write(sock, send_message, strlen(send_message));
            read(sock, recv_message, sizeof(recv_message));
            
            parsing(recv_message, file_list);
        }
        else if(!strcmp(command, "up\n"))
        {
            fputs("[Client] 파일이름 입력 : ", stdout);
        }
        else if(!strcmp(command, "dw\n"))
        {
            fputs("[Client] 파일이름 입력 : ", stdout);
            fgets(file_name, BUF_LEN, stdin);
            
            command[strlen(command)-1] = '\0';
            file_name[strlen(file_name)-1] = '\0';
            
            snprintf(send_message, sizeof(send_message), "%s%s", command, file_name);
            
            write(sock, send_message, strlen(send_message)); //command, file name send
            
            receive_file_from_server(sock, recv_message, sizeof(recv_message), file_name);
        }
    }
    close(sock);
    
    return 0;
}

