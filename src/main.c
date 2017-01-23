
/* 
 * File:   main.c
 * Author: Kamil Cupial <kac12>
 *
 * Created on 23 January 2017, 17:01
 */

#include "main.h"
#include "libs.h"
#define VERSION "0.9"
#define FILENAME_SETTINGS "tsam_settings"
#define FILENAME_LOGS "tsam_logs"
#define BUFFER_SIZE 3000

FILE* f_logs;
char ip[16] = "";
char port[6] = "";
char sid[4] = "";
char username[40] = "";
char password[100] = "";

    
    
void log(char* format, ...){
    va_list va_list;
    va_start(va_list, format);
    vfprintf(f_logs, format, va_list);
    va_end(va_list);
    va_start(va_list, format);
    vfprintf(stdout, format, va_list);
    va_end(va_list);
}

void cleanExit(){
    fclose(f_logs);
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    f_logs = fopen(FILENAME_LOGS".txt", "a");
    if (f_logs == NULL){
        printf("Couldn't create logfile. Press any key to quit...\n");
        scanf("");
        return(EXIT_FAILURE);
    }
    log("===== Puck's Teamspeak Activity Monitor. Version: "VERSION" =====\n");
    getSettings();
    log("[LOG] Current hour: %s. Waiting for the next hour.\n","notImplemented");
    updateData();
    cleanExit();
    return (EXIT_SUCCESS);
}

void checkEOF(FILE *file){
    if (feof(file)){
        log("[ERR] Unexpected end of file error. Terminating.\n");
        cleanExit();
    }
}

void getValue(char* buffer, char* value){
    char c = '\0';
    int position = 0;
    int position2 = 0;;
    for (position = 0; buffer[position] != '='; position++);
    while(buffer[++position] != '\n'){
        value[position2++] = buffer[position];
    }
    value[position2] = '\0';
}

void getField(FILE* file, char* buffer, char* fieldname){
    char field[15];
    while(strcmp(field, fieldname) != 0 && fgets(buffer, BUFFER_SIZE-1, file) != 0){
        memcpy(field, buffer, strlen(fieldname));
        field[strlen(fieldname)] = '\0';
    }
    checkEOF(file);
}

int updateData(){
    FILE f_channels, f_temp;
    log("===== ACTIVITY_DUMP. TIME: %s =====\n", "UNDEFINED");
    log("============================================\n");
}

void getSettings(){
    FILE *f_settings;
    log("Loading server/login info from "FILENAME_SETTINGS".\n");
    f_settings = fopen(FILENAME_SETTINGS, "r");
    if (f_settings == NULL){
        log("[ERR] Settings file not found. Dumping and terminating...\n");
    }
    char buffer[BUFFER_SIZE] = "";
    char field[15] = "";
    getField(f_settings, buffer, "ip=");
    getValue(buffer, ip);
    getField(f_settings, buffer, "port=");
    getValue(buffer, port);
    getField(f_settings, buffer, "sid=");
    getValue(buffer, sid);
    getField(f_settings, buffer, "login=");
    getValue(buffer, username);
    getField(f_settings, buffer, "password=");
    getValue(buffer, password);
    log("Host: %s:%s\nServerID: %s\nUsername: %s\nPassword: %s\n", ip, port, sid, username, password);
    fclose(f_settings);
}