/*
 * server.c
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
#include <dirent.h>
#include <sys/stat.h>
//소켓 프로그래밍에 사용될 헤더파일 선언
 
#define BUF_LEN 2048
#define MAX_LINE 61440 //60KByte
//메시지 송수신에 사용될 버퍼 크기를 선언
 
 int serv_sock;
 int clnt_sock;
 
 
void ls_and_sort(char *dir, char *send_message, int size)
{
    struct dirent **dir_entry;
    struct stat statbuf_name;
    
    int n, i;
    
    n = scandir(dir, &dir_entry, 0, alphasort);
    
    for (i = 0; i < n; i++) {
        stat(dir_entry[i]->d_name, &statbuf_name);
        
        if((strlen(send_message)+strlen(dir_entry[i]->d_name)+2) <= BUF_LEN)
        {
            strncat(send_message, dir_entry[i]->d_name, strlen(dir_entry[i]->d_name));
            strncat(send_message, "#", 1);
        }
        else
        {
            printf("[Error] send message buffer size error!\n");
        }
    }
}

void send_file_to_client(char *file_name, char *send_message, int send_msg_len)
{
    FILE *fp;
    struct stat file_size;
    int len;
    unsigned long send_size;
    
    fp = fopen(file_name, "rb");
    
    if(fp == NULL)
    {
        printf("[Error] file open error...!!\n");
        return;
    }
    
    // 파일 사이즈 전송
    if (0 > stat(file_name, &file_size))
    {
        printf("[Error] No file ...!! \n");
        return;
    }
    else
    {
        send_size = htonl(file_size.st_size);
        printf("[Server] file size : %lu\n", send_size);
    }
    
    write(clnt_sock, &send_size, sizeof(send_size));
    
    // 파일전송
    while((len = fread(send_message, sizeof(char), send_msg_len, fp)) > 0 )
    {
        printf("len : %d\n", len);
        write(clnt_sock, send_message, len);
        printf("[Server] 보낸 길이 : %d\n", len);
    }
    
    fclose(fp);
}
    
int main(int argc, char *argv[])
{
    socklen_t clnt_addr_size;
    
    char send_message[BUF_LEN];
    char recv_message[BUF_LEN];
 
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    
    memset(send_message, 0, sizeof(send_message));
    memset(recv_message, 0, sizeof(recv_message));
    
    if(argc != 2)
    {
        printf("[usage] %s <port>\n", argv[0]);
        exit(1);
    }
    
    if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {// 소켓 생성
        printf("[Error] Can't open stream socket\n");
        exit(0);
    }
    
    memset(&serv_addr, 0x00, sizeof(serv_addr));
    //server_Addr 을 NULL로 초기화
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    //server_addr 셋팅
 
    if(bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
    {//소켓에 주소할당
        printf("[Error] Can't bind local address.\n");
        exit(0);
    }
 
    if(listen(serv_sock, 5) == -1)
    {//연결 요청 대기 상태로 진입
        printf("[Error] Can't listening connect.\n");
        exit(0);
    }
 
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size); //연결 요청 수락
    
    if(clnt_sock == -1)
    {   
        printf("[Error] Can't accept. \n");
        exit(0);
    }
    
    printf("\n >>>>>>>>>>>>> Start to nemus ftp server program <<<<<<<<<<<<<\n\n");
    printf("[Server] Connected from %s : %d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    
    while((read(clnt_sock, recv_message, BUF_LEN)) != 0)
    {
        if(!strncmp(recv_message, "quit", 4))
        {
            printf("[Server] 클라이언트와와 연결을 종료합니다. \n");
            printf("\n >>>>>>>>>>>>> End to nemus ftp server program <<<<<<<<<<<<<\n\n");
            break;
        }
        else if(!strncmp(recv_message, "ls", 2))
        {
            char dir[BUF_LEN];
            
            strncpy(dir, recv_message+2, strlen(recv_message));
            
            ls_and_sort(dir, send_message, sizeof(send_message));
            write(clnt_sock, send_message, strlen(send_message));
            
            memset(recv_message, 0, sizeof(recv_message));
        }
        else if(!strncmp(recv_message, "up", 2))
        {
        }
        else if(!strncmp(recv_message, "dw", 2))
        {
            char file_name[BUF_LEN];
            
            strncpy(file_name, recv_message+2, strlen(recv_message));
            
            send_file_to_client(file_name, send_message, sizeof(send_message));
        }
        
        memset(send_message, 0, sizeof(send_message));
    }
    
    close(clnt_sock);
    
    return 0;
}
