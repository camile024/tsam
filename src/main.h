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

/* Directories */
extern char currdir[];
extern char FILENAME_SETTINGS[];
extern char FILENAME_LOGS[];

int main(int argc, char** argv);
void cleanExit();
int updateData();
