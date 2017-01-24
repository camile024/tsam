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
    }
    value[position2] = '\0';
    return position;
}

/* Looks for the specified fieldname (e.g. 'ip=') in the file, using passed
 * buffer to store the whole line in afterwards.
 */
void getField(FILE* file, char* buffer, char* fieldname){
    char field[15];
    while(strcmp(field, fieldname) != 0 && fgets(buffer, BUFFER_SIZE-1, file) != 0){
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


int createSnapshot(char* buffer){
    FILE* f_snapshot;
}


/* Loads up settings in global variables */
void getSettings(){
    FILE *f_settings;
    plog("Loading server/login info from %s.\n", FILENAME_SETTINGS);
    f_settings = fopen(FILENAME_SETTINGS, "r");
    if (f_settings == NULL){
        plog("[ERR] Settings file not found. Dumping and terminating...\n");
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