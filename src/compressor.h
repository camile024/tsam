
/* 
 * File:   compressor.h
 * Author: Kamil Cupial <kac12>
 *
 * Created on 24 January 2017, 17:11
 */
#pragma once


void startCompressor(void);
void showCompressed(void);
void compressLoad(void);
void compressNew();
void saveCompressedChannels(FILE*);
void filenameToTime(char*,struct tm*,int);
void readFile(FILE*, char*);
int withinPeak(int);
void updateChannels(char*, struct tm*);
void loadChannelsFromCompressed(char*);
void printChannels(void);
int findChannel(int);
int insertChannel(int);
