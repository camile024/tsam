
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
#include "compressor.h"
#include "utilities.h"
#include "channel.h"

/* Server settings and program settings */
FILE* f_logs;
char ip[16] = "";
char port[6] = "";
char sid[4] = "";
char username[40] = "";
char password[100] = "";
char delay[8] = "";
char oldtime[5] = "";
char peakstart[5]= "";
char peakend[5] = "";
char recordstart[5] = "";
char recordend[5] = "";
char ignoretemp[5] = "";


/* Directories */
char currdir[FILENAME_SIZE] = "";
char FILENAME_SETTINGS[FILENAME_SIZE] = "tsam_settings";
char FILENAME_LOGS[FILENAME_SIZE] = "logs";
char FILENAME_SNAPS[FILENAME_SIZE] = "data";
char FILENAME_COMP[FILENAME_SIZE] = "tsam_compressed";

char FILENAME_SCRIPT_START[FILENAME_SIZE] = "./tsam_onStartup.sh";
char FILENAME_SCRIPT_ERROR[FILENAME_SIZE] = "./tsam_onError.sh";  
char FILENAME_SCRIPT_SNAPSHOT[FILENAME_SIZE] = "./tsam_onSnapshot.sh";







int main(int argc, char** argv) {
    prepareFilenames();
    prepareDirectories(argv);
    if ((argc > 1) && (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-a") == 0)){
        strcat(FILENAME_LOGS, "/log-comp");
        openLogs();
        getSettings();
    } else {
        strcat(FILENAME_LOGS, "/log");
        openLogs();
    }
    system(FILENAME_SCRIPT_START);
    plog("\n\n=================================================================\n");
    plog("======== Puck's Teamspeak Activity Monitor. Version: "VERSION" =======\n");
    plog("=================================================================\n");
    
    if ((argc > 1) && (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-a") == 0)){
        startCompressor(argc, argv);
    } else {
        getSettings();
        startUpdater();
    }
    return (EXIT_SUCCESS);
}


/* Opens log file */
void openLogs(){
    struct tm* t_last = getCurrentTime();
    char logdate[30] = "";
    char usedfilename[FILENAME_SIZE];
    sprintf(logdate, "_%d-%02d-%d", t_last->tm_mday, t_last->tm_mon+1, t_last->tm_year+1900);
    strcpy(usedfilename, FILENAME_LOGS);
    strcat(usedfilename, logdate);
    f_logs = fopen(usedfilename, "a");
    if (f_logs == NULL){
        printf("Couldn't create logfile (%s).\n", usedfilename);
        cleanExit();
    }
}




/* Runs the software as updating app */
void startUpdater(){
    struct tm* t_last = getCurrentTime();
    if (testConnection() < 0) {
        cleanExit();
    }
    plog("[LOG] Current time: %02d:%02d. Waiting for the next snapshot.\n", t_last->tm_hour, t_last->tm_min);
    int sleeptime = atoi(delay);
    while (1){
        sleep(sleeptime);
        updateData();
        fclose(f_logs);
        openLogs();
        char removal[100];
        sprintf(removal, "find %s/ -mtime +%s -exec rm {} \\;", FILENAME_SNAPS, oldtime);
        system(removal);
        plog("[LOG] Current time: %02d:%02d. Waiting for the next snapshot.\n", t_last->tm_hour, t_last->tm_min);
    }
    cleanExit();
}



/*
 * Saves logs and exits
 */
void cleanExit(){
    struct tm* cur = getCurrentTime();
    system(FILENAME_SCRIPT_ERROR);
    if (f_logs != NULL){
        plog("Terminating... (%02d:%02d %02d-%02d-%d)\n", cur->tm_hour, cur->tm_min, cur->tm_mday, cur->tm_mon+1, cur->tm_year+1900);
        fclose(f_logs);
    }
    exit(EXIT_SUCCESS);
}




/* Updates activity logs */
int updateData(){
    char buffer[BUFFER_SIZE];
    struct tm* curtime = getCurrentTime();
    plog("\n============================================\n");
    plog("======== ACTIVITY DUMP. TIME: %02d:%02d ========\n", curtime->tm_hour, curtime->tm_min);
    plog("============================================\n");
    if (withinRecord()){
        if (withinPeak(curtime->tm_hour)){
            plog("--- Peak hour ---\n");
        } else {
            plog("--- Non-peak ---\n");
        }
        int error = net_getinfo(buffer);
        if (error == 0)
            error = createSnapshot(buffer);
        if (error < 0){
            plog("======== ERROR: ACTIVITY DUMP FAILED =======\n");
            system(FILENAME_SCRIPT_ERROR);
        } else {
            plog("=========== ACTIVITY DUMP SUCCESS ==========\n");
            system(FILENAME_SCRIPT_SNAPSHOT);
        }
    } else {
        plog("========== ABORT: NON-RECORD HOUR ==========\n");
    }
}