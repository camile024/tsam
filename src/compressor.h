
/* 
 * File:   compressor.h
 * Author: Kamil Cupial <kac12>
 *
 * Created on 24 January 2017, 17:11
 */
#pragma once

void compressorAuto(int, char**);
void startCompressor(int, char**);
void showCompressed(char, char, char);
void compressLoad(void);
void compressNew();
void saveCompressedChannels(FILE*);
void updateChannels(char*, struct tm*);
void loadChannelsFromCompressed(char*);
void printChannels(char, FILE*);
int findChannel(int);
int insertChannel(int);
