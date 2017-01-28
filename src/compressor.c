
#include "libs.h"
#include "defines.h"
#include "main.h"
#include "files.h"
#include "compressor.h"
#include "channel.h"

struct channel channels[MAX_CHANNELS_STORED];
int channelnum = 0;

/* Starts the program in (c)ompressing/(a)nalysing mode */
void startCompressor(){
    for (int i = 0; i < MAX_CHANNELS_STORED; i++){
        channels[i].cid = -1;
    }
    char choice;
    while(choice != '3'){
        plog("\nCompressor interface enabled.\n");
        plog("Select compressing mode: \n");
        plog("1. Create new compressed file based on snapshots\n");
        plog("2. Display channel data from compressed file\n");
        plog("3. Exit\n");
        scanf(" %c", &choice);
        switch(choice){
            case '1':
                plog("Warning! If there's already a compressed file in the current"
                        "directory it WILL be overwritten.\n"
                        "Snapshot files will not be removed after the operation.\n"
                        "Are you sure you want to continue (y/n)?\n");
                scanf(" %c", &choice);
                if (choice == 'y')
                    compressNew();
                else
                    continue;
                break;
            case '2':
                showCompressed();
                break;
        }
    }
    exit(EXIT_SUCCESS);
}

/* Shows compressed file's data */
void showCompressed(){
    compressLoad();
    printChannels();;
}



/* Loads the compressed file's data into memory */
void compressLoad(){
    FILE * f_compressed;
    char buffer[BUFFER_SIZE];
    f_compressed = fopen(FILENAME_COMP, "r");
    if (f_compressed == NULL){
        plog("Couldn't read %s.\n", FILENAME_COMP);
        return;
    }
    readFile(f_compressed, buffer);
    loadChannelsFromCompressed(buffer);
    fclose(f_compressed);
}



/* Saves channel info from all the snapshot files
 * into one, compressed file
 */
void compressNew(){
    channelnum = 0;
    for (int i = 0; i < MAX_CHANNELS_STORED; i++){
        channels[i].cid = -1;
    }
    FILE * f_compressed, * f_temp;
    DIR *directory = opendir(FILENAME_SNAPS);
    struct tm* time = getCurrentTime();
    char buffer[BUFFER_SIZE];
    struct dirent * entry;
    entry = readdir(directory);
    if (directory == NULL || entry == NULL){
        plog("Couldn't open snapshot directory or no snapshots available.\n");
        return;
    }
    f_compressed = fopen(FILENAME_COMP, "w");
    if (f_compressed == NULL){
        plog("Couldn't create %s.\n", FILENAME_COMP);
        return;
    }
    int count = 0;
    while (entry != NULL && entry->d_name[0] != '.'){
        char filename[150] = "";
        strcpy(filename, FILENAME_SNAPS);
        strcat(filename, "/");
        strcat(filename, entry->d_name);
        f_temp = fopen(filename, "r");
        filenameToTime(entry->d_name, time, 0);
        readFile(f_temp, buffer);
        updateChannels(buffer, time);
        fclose(f_temp);
        entry = readdir(directory);
        plog("Read %d files...\n", ++count);
    }
    saveCompressedChannels(f_compressed);
    fclose(f_compressed);
    closedir(directory);
    plog("Operation Complete.\n");
}


/* Saves channel info into compressed file */
void saveCompressedChannels(FILE* f_temp){
    plog("Channelnum: %d\n", channelnum);
    for (int i=0; i < channelnum; i++){
        struct channel* c = &channels[i];
        struct tm t = c->lastActive;
        fprintf(f_temp, 
            "cid=%d\n"
            "lastName=%s\n"
            "lastUserNum=%d\n"
            "lastActive=%d-%02d-%02d_%02d-%02d\n"
            "average=%2.4f\n"
            "average_w=%2.4f\n"
            "average_wsum=%ld\n"
            "average_wcom=%ld\n", 
                c->cid, c->lastName, c->lastUserNum,
                t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min,
                c->average, c->average_w, c->average_wsum, c->average_wcom);
    }
}

/* Reads a given filename and transfers the date and time from it
 * to structure time. Filename format: snap_yyyy:mm_dd-hh-mm. 
 * Can be used for other strings, but start has to be specified according to:
 * start = yy_begin - 5, where yyyy_begin is the index of the first 'y'.
 * Spacer types (':', '-', '_') don't matter as long as particular
 * datetime characters are in the correct indexes from each other and start
 * value passed is correct
 */
void filenameToTime(char* filename, struct tm* time, int start){
    char buf[6] = "";
    buf[5] = '\0';
    memcpy(buf, &filename[start+5], 4);
    time->tm_year = atoi(buf);
    buf[2] = '\0';
    memcpy(buf, &filename[start+10], 2);
    time->tm_mon = atoi(buf);
    memcpy(buf, &filename[start+13], 2);
    time->tm_mday = atoi(buf);
    memcpy(buf, &filename[start+16], 2);
    time->tm_hour = atoi(buf);
    memcpy(buf, &filename[start+19], 2);
    time->tm_min = atoi(buf);
}


/* Reads a whole file into buffer */
void readFile(FILE* f_temp, char* buffer){
    char line[BUFFER_FILESIZE];
    long offset = 0;
    while (!feof(f_temp)){
        fscanf(f_temp, "%s", line);
        sprintf(&buffer[offset], "%s\n", line);
        offset += strlen(line)+1;
    }
}


/* Checks if the hour given is within current peak hours */
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




/* Updates channels in the memory with info in buffer (a previously
 * read file). If a channel isn't in the memory yet, it gets added.
*/
void updateChannels(char* buffer, struct tm* time){
    int index = -1;
    char* bufptr = buffer;
    char cid[8] = "";
    bufptr = getNextField(bufptr, cid, "cid=");
    while (bufptr != NULL){
        char chname[100] = "";
        char clients[5] = "";
        bufptr = getNextField(bufptr, chname, "channel_name=");
        bufptr = getNextField(bufptr, clients, "total_clients=");
        int icid = atoi(cid);
        int iclients = atoi(clients);
        index = findChannel(icid);
        if (index < 0){
            channelnum++;
            index = insertChannel(icid);
            channels[index].average = iclients;
            if (withinPeak(time->tm_hour)){
                channels[index].average_wcom = iclients * 2;
                channels[index].average_wsum = 2;
            } else {
                channels[index].average_wcom = iclients * 1;
                channels[index].average_wsum = 1;
            }
        } else {
            channels[index].average = (channels[index].average + iclients) / (float) 2.00;
            int weight;
            if (withinPeak(time->tm_hour)){
                weight = 2;
            } else {
                weight = 1;
            }
            channels[index].average_wcom += (iclients * weight);
            channels[index].average_wsum += 1;
            long avgws = channels[index].average_wsum;
            long avgwcom = channels[index].average_wcom;
        }
        if (iclients > 0){
            channels[index].lastActive = *time;
            channels[index].lastUserNum = iclients;
        }
        strcpy(channels[index].lastName, chname);
        bufptr = getNextField(bufptr, cid, "cid=");
    }
}





/* Loads channels into memory from the compressed file */
void loadChannelsFromCompressed(char* buffer){
    channelnum = 0;
    bzero(channels, sizeof(channels[0]) * MAX_CHANNELS_STORED);
    int index = -1;
    char* bufptr = buffer;
    char cid[8] = "";
    bufptr = getNextField(bufptr, cid, "cid=");
    while (bufptr != NULL){
        char chname[100] = "";
        char clients[10] = "";
        char lastActive[30] = "";
        char average[10] = "";
        char average_w[10] = "";
        char average_wsum[10] = "";
        char average_wcom[10] = "";
        bufptr = getNextField(bufptr, chname, "lastName=");
        bufptr = getNextField(bufptr, clients, "lastUserNum=");
        bufptr = getNextField(bufptr, lastActive, "lastActive=");
        bufptr = getNextField(bufptr, average, "average=");
        bufptr = getNextField(bufptr, average_w, "average_w=");
        bufptr = getNextField(bufptr, average_wsum, "average_wsum=");
        bufptr = getNextField(bufptr, average_wcom, "average_wcom=");
        int icid = atoi(cid);
        int iclients = atoi(clients);
        float favg = atof(average);
        float favg_w = atof(average_w);
        long lavg_wsum = atoi(average_wsum);
        long lavg_wcom = atoi(average_wcom);
        struct tm* tlastActive = getCurrentTime();
        filenameToTime(lastActive, tlastActive, -5);
        struct channel* c = &channels[channelnum];
        c->cid = icid;
        c->lastUserNum = iclients;
        c->lastActive = *tlastActive;
        c->average = favg;
        c->average_w = favg_w;
        c->average_wsum = lavg_wsum;
        c->average_wcom = lavg_wcom;
        strcpy(c->lastName, chname);
        channelnum++;
        bufptr = getNextField(bufptr, cid, "cid=");
    }
}

/* Prints all the channels in the memory*/
void printChannels(){
    for (int i=0; i < channelnum; i++){
        struct channel* c = &channels[i];
        /* Replace \s with spaces for clarity */
        char *ptr;
        ptr = strstr(c->lastName, "\\s");
        while (ptr != NULL){
            ptr[0] = ' ';
            ptr[1] = ' ';
            ptr = strstr(c->lastName, "\\s");
        } 
        /* End of replacing code */
        
        /* Count weighted average now */
        channels[i].average_w = channels[i].average_wcom / (float)channels[i].average_wsum;
        /**/
        plog("=[CID: %d]=\n"
        "Last known name: %s\n"
        "Last known activity: %02d:%02d %02d/%02d/%d\n"
        "Last known number of clients: %d\n"
        "Average number of clients: %2.4f\n"
        "Channel's rating [incomplete]: %2.4f\n\n",
                c->cid, c->lastName,
                c->lastActive.tm_hour, c->lastActive.tm_min,
                c->lastActive.tm_mday, c->lastActive.tm_mon,
                c->lastActive.tm_year,
                c->lastUserNum, c->average, c->average_w);
    }
}


/* Checks if the channel with channelID cid exists in the memory 
 * returns index of the channel in the channels[] array if found, negative
 * number if not found
 */
int findChannel(int cid){
    int left = 0;
    int right = channelnum-1;
    int m;
    while (left <= right){
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


/* Creates space in a correct position for a channel with
 * given cid and returns its index in the array
*/
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
