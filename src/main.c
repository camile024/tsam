
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

/* Server settings and program settings */
FILE* f_logs;
char ip[16] = "";
char port[6] = "";
char sid[4] = "";
char username[40] = "";
char password[100] = "";
char delay[8] = "";
char oldtime[5] = "";
char peakstart[3]= "";
char peakend[3] = "";
char quietstart[3] = "";
char quietend[3] = "";
char soundsuccess[2] = "";
char soundfailure[2] = "";
char soundfailurequiet[2] = "";

/* Directories */
//char currdir[250] = "";
char FILENAME_SETTINGS[250] = "tsam_settings";
char FILENAME_LOGS[250] = "logs/log";
char FILENAME_SNAPS[250] = "data/";
char FILENAME_COMP[250] = "tsam_compressed";
char FILENAME_SOUNDSUCC[250] = "sound/success.wav";
char FILENAME_SOUNDFAIL[250] = "sound/fail.wav";
    
struct tm* getCurrentTime(){
    time_t epochtime = time(NULL);
    struct tm* curtime = localtime(&epochtime);
    return curtime;
}

int main(int argc, char** argv) {
    //getcwd(currdir, sizeof(currdir));
    if (argc > 1 && strcmp(argv[1], "-analyse") == 0)
        ;
    else
        startUpdater();
    return (EXIT_SUCCESS);
}

void openLogs(){
    struct tm* t_last = getCurrentTime();
    char logdate[30] = "";
    char usedfilename[250];
    sprintf(logdate, "_%d-%02d-%d", t_last->tm_mday, t_last->tm_mon+1, t_last->tm_year+1900);
    strcpy(usedfilename, FILENAME_LOGS);
    strcat(usedfilename, logdate);
    f_logs = fopen(usedfilename, "a");
    if (f_logs == NULL){
        printf("Couldn't create logfile (%s).\n", usedfilename);
        return(EXIT_FAILURE);
    }
}

void startUpdater(){
    struct tm* t_last = getCurrentTime();
    //prepareFilenames();
    openLogs();
    plog("\n\n=================================================================\n");
    plog("======== Puck's Teamspeak Activity Monitor. Version: "VERSION" ========\n");
    plog("=================================================================\n");
    getSettings();
    plog("[LOG] Current time: %02d:%02d. Waiting for the next snapshot.\n", t_last->tm_hour, t_last->tm_min);
    int sleeptime = atoi(delay);
    while (1){
        sleep(sleeptime);
        updateData();
        fclose(f_logs);
        openLogs();
    }
    cleanExit();
}

/*
 * Saves logs and exits
 */
void cleanExit(){
    struct tm* cur = getCurrentTime();
    plog("Terminating... (%02d:%02d %02d-%02d-%d)\n", cur->tm_hour, cur->tm_min, cur->tm_mday, cur->tm_mon+1, cur->tm_year+1900);
    fclose(f_logs);
    exit(EXIT_SUCCESS);
}




/* Updates activity logs */
int updateData(){
    char buffer[BUFFER_SIZE];
    struct tm* curtime = getCurrentTime();
    plog("\n============================================\n");
    plog("======== ACTIVITY DUMP. TIME: %02d:%02d ========\n", curtime->tm_hour, curtime->tm_min);
    plog("============================================\n");
    
    int error = net_getinfo(buffer);
    if (error == 0)
        error = createSnapshot(buffer);
    if (error < 0){
        plog("======== ERROR: ACTIVITY DUMP FAILED =======\n");
    } else {
        plog("=========== ACTIVITY DUMP SUCCESS ==========\n");
    }
}