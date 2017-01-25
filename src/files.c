#include "libs.h"
#include "defines.h"
#include "main.h"
#include "files.h"

/*
 * Checks for EOF and terminates program if found
 */
void checkEOF(FILE *file){
    if (feof(file)){
        plog("[ERR] Unexpected end of file error. Terminating.\n");
        cleanExit();
    }
}

/* 
 * Gets the first string (until newline character, space or EOF) after '=' 
 * from 'buffer' and puts it inside 'value'. 
 * Returns the position in buffer after reading the value.
 */
int getValue(char* buffer, char* value){
    char c = '\0';
    int position = 0;
    int position2 = 0;;
    for (position = 0; buffer[position] != '='; position++);
    while(buffer[++position] != '\n'
            && buffer[position] != EOF
            && buffer[position] != ' '
            && buffer[position] != '\0'){
        value[position2++] = buffer[position];
        /* Replaces spaces \s in channel names
         * MOVED TO COMPRESSOR PRINTING FUNCTION
         * Space is used as a separator for values up above and can't be removed
         * since error MSGs from TS servers have spaces in them as separators */
        /* if (position2 > 1 && value[position2-1] == 's' && value[position2-2] == '\\'){
            value[position2-2] = ' ';
            position--;
        } */
    }
    value[position2] = '\0';
    return position;
}

/* Looks for the specified fieldname (e.g. 'ip=') in the file, using passed
 * buffer to store the whole line in afterwards.
 */
void getField(FILE* file, char* buffer, char* fieldname){
    char field[25];
    while(strcmp(field, fieldname) != 0 && fgets(buffer, BUFFER_FILESIZE-1, file) != 0){
        memcpy(field, buffer, strlen(fieldname));
        field[strlen(fieldname)] = '\0';
    }
    checkEOF(file);
}


/* Gets value of the next field with a name 'fieldname' (e.g. "cid=") 
 * from 'data' and stores it in 'value'
 * Returns a pointer to data right after the value ends
 */
char* getNextField(char* data, char* value, char* fieldname){
    char* result;
    result = strstr(data, fieldname);
    if (result == NULL)
        return result;
    return result + getValue(result + strlen(fieldname) - 1, value);
}

/* Creates a snapshot file with current time and data in its name */
int createSnapshot(char* buffer){
    FILE* f_snapshot;
    char filename[250] = "";
    char* bufptr = buffer;
    struct tm* currtime = getCurrentTime();
    sprintf(filename, "%ssnap_%d-%02d-%02d_%02d-%02d", FILENAME_SNAPS,
            currtime->tm_year + 1900, currtime->tm_mon+1, currtime->tm_mday, currtime->tm_hour, currtime->tm_min);
    f_snapshot = fopen(filename, "w");
    if (f_snapshot == NULL){
        plog("[ERR] Snapshot file could not be created.\n");
        return -1;
    }
    printf("Data size: %lu\n", strlen(bufptr));
    char cid[8] = "";
    bufptr = getNextField(bufptr, cid, "cid=");
    while (bufptr != NULL){
        char chname[100] = "";
        char clients[5] = "";
        bufptr = getNextField(bufptr, chname, "channel_name=");
        bufptr = getNextField(bufptr, clients, "total_clients=");
        fprintf(f_snapshot, "cid=%s\nchannel_name=%s\ntotal_clients=%s\n\n", cid, chname, clients);
        bufptr = getNextField(bufptr, cid, "cid=");
    }
    fclose(f_snapshot);
    plog("Snapshot %s saved.\n", filename);
    return 1;
}


/* Loads up settings in global variables */
void getSettings(){
    FILE *f_settings;
    plog("Loading server/login info from %s.\n", FILENAME_SETTINGS);
    f_settings = fopen(FILENAME_SETTINGS, "r");
    if (f_settings == NULL){
        plog("[ERR] Settings file not found. Dumping and terminating...\n");
    }
    char buffer[BUFFER_FILESIZE] = "";
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
    getField(f_settings, buffer, "delay-seconds=");
    getValue(buffer, delay);
    getField(f_settings, buffer, "old-time=");
    getValue(buffer, oldtime);
    getField(f_settings, buffer, "peak-start=");
    getValue(buffer, peakstart);
    getField(f_settings, buffer, "peak-end=");
    getValue(buffer, peakend);
    plog("Host: %s:%s\nServerID: %s\nUsername: %s\n", ip, port, sid, username);
    fclose(f_settings);
}




/*
 * Prints to both stdout ant file, use instead of printf
 * if the message should be logged
 */
void plog(char* format, ...){
    va_list va_list;
    va_start(va_list, format);
    vfprintf(f_logs, format, va_list);
    va_end(va_list);
    va_start(va_list, format);
    vfprintf(stdout, format, va_list);
    va_end(va_list);
}

/* In case it's needed some day
 */ /*
void prepareFilenames(){
    char names[250] = "";
    strcpy(names, currdir);
    strcat(names, "/");
    strcat(names, FILENAME_LOGS);
    FILENAME_LOGS[0] = '\0';
    strcpy(FILENAME_LOGS, names);
    names[0] = '\0';
    
    strcpy(names, currdir);
    strcat(names, "/");
    strcat(names, FILENAME_SETTINGS);
    FILENAME_SETTINGS[0] = '\0';
    strcpy(FILENAME_SETTINGS, names);
    names[0] = '\0';
} */