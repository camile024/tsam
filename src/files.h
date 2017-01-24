
/* 
 * File:   files.h
 * Author: Kamil Cupial <kac12>
 *
 * Created on 23 January 2017, 21:37
 */
#pragma once

void checkEOF(FILE *);
int getValue(char*, char*);
void getField(FILE*, char*, char*);
int createSnapshot(char*);
void plog(char* format, ...);
void getSettings();
char* getNextField(char*, char*, char*);