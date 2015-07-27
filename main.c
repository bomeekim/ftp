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

bool send_flag = false;

typedef struct {
    
    char name[256];
    char path[256];
    double size;
    
} FileInfo;

/*
 * void ls(char *dir)
{
    printf("ls\n");
    
    DIR *dir_info;
    struct dirent *dir_entry;
    
    dir_info = opendir((char*) dir);
    
    if (dir_info != NULL) {
        while (1) {
            dir_entry = readdir(dir_info);
            
            if (dir_entry == NULL)
                break;
                
            printf("%s\n", dir_entry->d_name);
        }
        closedir(dir_info);
    }
}
*/

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
    char *send_file_name;
    
    if (argc < 2) {
        dir = ".";
    else if (strcmp(argv[1], "-d")) {
    
    }
    else {
        dir = argv[1];
    }
    
    ls_and_sort(dir);
    
    return 0;
}



