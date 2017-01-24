
#include "libs.h"
#include "defines.h"
#include "main.h"
#include "files.h"
#include "compressor.h"
#include "channel.h"

struct channel channels[1000];
int channelnum = 0;

void startCompressor(){
    for (int i = 0; i < 1000; i++){
        channels[i].cid = -1;
    }
    printf("\n\n=================================================================\n");
    printf("======== Puck's Teamspeak Activity Monitor. Version: "VERSION" ========\n");
    printf("=================================================================\n");
    char choice;
    while(choice != '3'){
        printf("Compressor interface enabled.\n");
        printf("Select compressing mode: \n");
        printf("1. Create new compressed file based on snapshots\n");
        printf("2. Display channel data from compressed file\n");
        printf("3. Exit\n");
        scanf("%c", &choice);
        switch(choice){
            case '1':
                printf("Warning! If there's already a compressed file in the current"
                        "directory it WILL be overwritten.\n"
                        "All the snapshot files WILL be removed after this operation.\n"
                        "Are you sure you want to continue (y/n)?\n");
                scanf(" %c", &choice);
                if (choice == 'y')
                    compressNew();
                else
                    continue;
                break;
            case '2':
                ;
                break;
        }
    }
    exit(EXIT_SUCCESS);
}

void compressNew(){
    FILE * f_compressed, * f_temp;
    DIR *directory = opendir(FILENAME_SNAPS);
    struct tm* time = getCurrentTime();
    char buffer[BUFFER_SIZE];
    struct dirent * entry;
    entry = readdir(directory);
    if (directory == NULL || entry == NULL){
        printf("Couldn't open snapshot directory or no snapshots available.\n");
        return;
    }
    f_compressed = fopen(FILENAME_COMP, "w");
    if (f_compressed == NULL){
        printf("Couldn't create %s.\n", FILENAME_COMP);
        return;
    }
    while (entry != NULL){
        char filename[100] = "";
        strcpy(filename, FILENAME_SNAPS);
        strcpy(filename, entry->d_name);
        f_temp = fopen(filename, "r");
        filenameToTime(entry->d_name, time);
        readSnapshot(f_temp, buffer);
        updateChannels(buffer, time);
        //saveCompressedChannels();
        fclose(f_temp);
        entry = readdir(directory);
    }
}



void filenameToTime(char* filename, struct tm* time){
    char buf[6] = "";
    memcpy(buf, filename[5], 2);
    buf[2] = '\0';
    buf[5] = '\0';
    time->tm_hour = atoi(buf);
    memcpy(buf, filename[8], 2);
    time->tm_min = atoi(buf);
    memcpy(buf, filename[11], 2);
    time->tm_mday = atoi(buf);
    memcpy(buf, filename[14], 2);
    time->tm_mon = atoi(buf);
    memcpy(buf, filename[17], 4);
    time->tm_year = atoi(buf);
}



void readSnapshot(FILE* f_temp, char* buffer){
    char line[BUFFER_FILESIZE];
    while (!feof(f_temp)){
        fscanf(f_temp, line, BUFFER_FILESIZE);
        strcat(buffer, line);
    }
}


int withinPeak(int hour){
    int peak1 = atoi(peakstart);
    int peak2 = atoi(peakend);
    if (peak1 > peak2){
        if (hour > peak1 || hour < peak2)
            return 1;
        else
            return 0;
    } else {
        if (hour > peak1 && hour < peak2)
            return 1;
        else
            return 0;
    }
}



void updateChannels(char* buffer, struct tm* time){
    int index = -1;
    char* bufptr = buffer;
    char cid[8] = "";
    bufptr = getNextField(bufptr, cid, "cid=");
    while (bufptr != NULL){
        char chname[100] = "";
        char clients[5] = "";
        bufptr = getNextField(bufptr, chname, "name=");
        bufptr = getNextField(bufptr, clients, "clients=");
        int icid = atoi(cid);
        int iclients = atoi(clients);
        index = findChannel(icid);
        if (index < 0){
            channelnum++;
            index = insertChannel(icid);
            channels[index].average = iclients;
            if (withinPeak(time->tm_hour)){
                channels[index].average_w = iclients * 2;
                channels[index].average_wsum = 2;
            } else {
                channels[index].average_w = iclients * 1;
                channels[index].average_wsum = 1;
            }
        } else {
            channels[index].average = (channels[index].average + iclients) / (float) 2.00;
            float avgw = channels[index].average_w;
            int weight;
            if (withinPeak(time->tm_hour)){
                weight = 2;
            } else {
                weight = 1;
            }
            channels[index].average_wsum += weight;
            float avgws = channels[index].average_wsum;
            channels[index].average_w = (avgw + (iclients * weight))/avgws;
        }
        if (clients > 0){
            channels[index].lastActive = *time;
            channels[index].lastUserNum = clients;
        }
        strcpy(channels[index].lastName, chname);
        bufptr = getNextField(bufptr, cid, "cid=");
    }
}

int findChannel(int cid){
    int left = 0;
    int right = channelnum-1;
    int m;
    while (left < right){
        m = (left+right)/2;
        if (channels[m].cid == cid)
            return m;
        if (channels[m].cid < cid)
            left = m+1;
        else
            right = m-1;
    }
    return -1;
}



int insertChannel(int cid){
    int i = channelnum-1;
    channels[i].cid = cid;
    while (i > 0 && channels[i].cid < channels[i-1].cid){
        memcpy(&channels[i], &channels[i-1], sizeof(channels[i-1]));
        channels[i-1].cid = cid;
        i--;
    }
    return i;
}