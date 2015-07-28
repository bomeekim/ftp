/*
 * main.c
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
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int send_flag = 0; //false

typedef struct {
    
    char name[256];
    char path[256];
    double size;
    
} FileInfo;


void ls_and_sort(char *dir)
{
    struct dirent **dir_entry;
    struct stat statbuf;
    
    int n, i;
    
    n = scandir(dir, &dir_entry, 0, alphasort);
    
    for (i = 0; i < n; i++) {
        stat(dir_entry[i]->d_name, &statbuf);
        printf("%9d %9d\t%s\n", statbuf.st_size, statbuf.st_mode, dir_entry[i]->d_name);
    }
}


int main(int argc, char *argv[])
{
    //FileInfo info;
    
    char *dir;
    
    if (argc < 2) {
        dir = ".";
    }
    else {
        dir = argv[1];
    }
    
    ls_and_sort(dir);
    
    return 0;
}



