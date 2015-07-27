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
//소켓 프로그래밍에 사용될 헤더파일 선언
 
#define BUF_LEN 256
//메시지 송수신에 사용될 버퍼 크기를 선언
 
 
 FILE* readFP;
 FILE* writeFP;
 
int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    int clnt_addr_size;
    char message[BUF_LEN];
    
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    
    if(argc != 2)
    {
        printf("usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {// 소켓 생성
        printf("Server : Can't open stream socket\n");
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
        printf("Server : Can't bind local address.\n");
        exit(0);
    }
 
    if(listen(serv_sock, 5) == -1)
    {//연결 요청 대기 상태로 진입
        printf("Server : Can't listening connect.\n");
        exit(0);
    }
 
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_addr, &clnt_addr_size); //연결 요청 수락
    
    if(clnt_sock == -1)
    {   
        printf("Server : Can't accept. \n");
        exit(0);
    }
    
    readFP = fdopen(clnt_sock, "r");
    writeFP = fdopen(clnt_sock, "w");
    
    printf("Connected from %s : %d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    
    while(!feof(readFP))
    {
        fgets(message, BUF_LEN, readFP);
        printf("Sent message from client : %s \n", message);
        
        fputs(message, writeFP);
        printf("Send message to client : %s \n", message);
    }

    fclose(writeFP);
    fclose(readFP);
    
    return 0;
}
