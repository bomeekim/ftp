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

/*
daytime 클라이언트 프로그램이 실행되기 위해서는
서버쪽에서 xinetd의 daytime이 구동하고 있어야한다.
coder YoWu
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF_LEN 256


int main(int argc, char **argv)
{
    int sock;
    int str_len;
    int select_number;
    char message[30];
    
    struct sockaddr_in serv_addr;
    
    
    
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
    
    //str_len = read(sock, message, sizeof(message)-1);
    
    /*if(str_len == -1)
    {
        printf("Client : Can't read the message\n");
        exit(0);
    }*/
    
    //start a ftp program
    printf("Welcom to nemus ftp program\n");
    
    while(1)
    {
        printf("\n*-------------------------------------------*\n");
        printf("          M      E      N      U            \n\n");
        printf("                 1. File List\n");
        printf("                 2. Upload\n");
        printf("                 3. Download\n");
        printf("                 4. Exit\n\n");
        
        printf("Select menu number : ");
        scanf("%d\n", &select_number);
        
        printf("*-------------------------------------------*\n");
        
        switch(select_number){
            case 1:
                //파일 조회
                break;
                
            case 2:
                fputs("Upload file name with extension : ", stdout);
                fgets(message, BUF_LEN, stdin);
                //file_up_load_process(sock, message);
                break;
                
            case 3:
                fputs("Download file name with extension : ", stdout);
                fflush(stdout);
                fgets(message, BUF_LEN, stdin);
                message[strlen(message)-1] = '\0';
                write(sock, message, strlen(message)+1);
                
                
                printf("Sent message from client to server : %s \n", message);
                
                //file_download_process(sock, message);
                break;
                
            case 4:
                printf("Close connection\n");
                close(sock);
                break;
                
        }
        
        
    }
    
    
    close(sock);
    
    return 0;
}

