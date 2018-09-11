/*
 * string_if.c
 *
 *  Created on: Aug 9, 2018
 *      Author: mehmet.alici
 */

#include <string.h>
#include <stdio.h>

void changeValueBuffer(char* post, char* name, float value){

    char buffer[5];
    char* pch;
    pch = strstr (post, "name");
    sprintf(buffer, "%g", value);
    strncpy(pch + 7, name, 3);
    pch = strstr(post, "unitPrice");
    strncpy(pch+11, buffer, 5);
}
