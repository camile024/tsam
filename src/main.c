
/* 
 * File:   main.c
 * Author: Kamil Cupial <kac12>
 *
 * Created on 23 January 2017, 17:01
 */
#include "libs.h"
#include "defines.h"
#include "main.h"
#include "files.h"
#include "sockets.h"

/* Server settings and logfile */
FILE* f_logs;
char ip[16] = "";
char port[6] = "";
char sid[4] = "";
char username[40] = "";
char password[100] = "";

/* Directories */
char currdir[250] = "";
char FILENAME_SETTINGS[250] = "tsam_settings";
char FILENAME_LOGS[250] = "logs/tsam_logs";
    


int main(int argc, char** argv) {
    getcwd(currdir, sizeof(currdir));
    //prepareFilenames();
    f_logs = fopen(FILENAME_LOGS, "a");
    if (f_logs == NULL){
        printf("Couldn't create logfile (%s).\n", FILENAME_LOGS);
        return(EXIT_FAILURE);
    }
    plog("===== Puck's Teamspeak Activity Monitor. Version: "VERSION" =====\n");
    getSettings();
    plog("[LOG] Current hour: %s. Waiting for the next hour.\n","notImplemented");
    updateData();
    cleanExit();
    return (EXIT_SUCCESS);
}



/*
 * Saves logs and exits
 */
void cleanExit(){
    fclose(f_logs);
    exit(EXIT_SUCCESS);
}




/* Updates activity logs */
int updateData(){
    char buffer[BUFFER_SIZE];
    plog("===== ACTIVITY DUMP. TIME: %s =====\n", "UNDEFINED");
    plog("============================================\n");
    
    int error = net_getinfo(buffer);
    if (error == 0)
        error = createSnapshot(buffer);
    if (error < 0){
        plog("===== ERROR: ACTIVITY DUMP FAILED =====\n");
    } else {
        plog("===== ACTIVITY DUMP SUCCESS =====\n");
    }
    plog("============================================\n");
}