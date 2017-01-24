/* 
 * File:   main.h
 * Author: Kamil Cupial <kac12>
 *
 * Created on 23 January 2017, 17:03
 */
#pragma once

extern FILE* f_logs;
extern char ip[];
extern char port[];
extern char sid[];
extern char username[];
extern char password[];
extern char delay[];
extern char oldtime[];
extern char peakstart[];
extern char peakend[];
extern char quietstart[];
extern char quietend[];
extern char soundsuccess[];
extern char soundfailure[];
extern char soundfailurequiet[]; 

/* Directories */
extern char currdir[];
extern char FILENAME_SETTINGS[];
extern char FILENAME_LOGS[];
extern char FILENAME_SNAPS[];
extern char FILENAME_COMP[];
extern char FILENAME_SOUNDSUCC[];
extern char FILENAME_SOUNDFAIL[];

int main(int argc, char** argv);
struct tm* getCurrentTime(void);
void openLogs(void);
void cleanExit(void);
int updateData(void);
