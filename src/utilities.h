
/* 
 * File:   utilities.h
 * Author: Kamil Cupial <kac12>
 *
 * Created on 28 January 2017, 14:07
 */
#pragma once
#include "channel.h"

struct tm* getCurrentTime(void);
int withinRecord(void);
void plog(char* format, ...);
int withinPeak(int);
void readFile(FILE*, char*);
void filenameToTime(char*,struct tm*,int);
void quicksort(struct channel*, int, int, int);
int qs_partition(struct channel*, int, int, int, int);
int timecmp(struct tm, struct tm);