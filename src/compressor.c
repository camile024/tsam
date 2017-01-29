
#include "libs.h"
#include "defines.h"
#include "main.h"
#include "files.h"
#include "compressor.h"
#include "channel.h"
#include "utilities.h"

struct channel channels[MAX_CHANNELS_STORED];
int channelnum = 0;

/* Starts the program in (c)ompressing/(a)nalysing mode */
void startCompressor(){
    for (int i = 0; i < MAX_CHANNELS_STORED; i++){
        channels[i].cid = -1;
    }
    char choice, choice2, choice3, choice4;
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
                plog("\nChoose sorting criteria: \n"
                        "1 - Channel ID (CID) \n"
                        "2 - Channel Name \n"
                        "3 - Channel Last Active time \n"
                        "4 - Channel Average user count \n"
                        "5 - Channel Rank \n");
                scanf(" %c", &choice2);
                plog("\nChoose sorting order: \n"
                        "1 - increasing, top-bottom\n"
                        "2 - decreasing, top-bottom\n");
                scanf(" %c", &choice3);
                plog("\nSave results to file (y/n)\n");
                scanf(" %c", &choice4);
                showCompressed(choice2, choice3, choice4);
                break;
        }
    }
    exit(EXIT_SUCCESS);
}

/* Shows compressed file's data */
void showCompressed(char criteria, char order, char savetofile){
    compressLoad();
    int icriteria = atoi(&criteria);
    plog("\n===RESULTS:===\n");
    quicksort(channels, 0, channelnum-1, icriteria);
    printChannels(order, NULL);
    if (savetofile == 'y'){
        FILE * f_results;
        char path[FILENAME_SIZE] = "";
        strcpy(path, FILENAME_COMP);
        strcat(path, "_results.txt");
        f_results = fopen(path, "w");
        if (f_results == NULL){
            plog("Couldn't create output file: %s\n", path);
        } else {
            printChannels(order, f_results);
            plog("\nResults saved as: %s\n", path);
            fclose(f_results);
        }
    }
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
            "pid=%d\n"
            "lastName=%s\n"
            "lastUserNum=%d\n"
            "lastActive=%d-%02d-%02d_%02d-%02d\n"
            "average=%2.4f\n"
            "average_w=%4.4f\n"
            "average_wsum=%ld\n"
            "average_wcom=%ld\n", 
                c->cid, c->pid, c->lastName, c->lastUserNum,
                t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min,
                c->average, c->average_w, c->average_wsum, c->average_wcom);
    }
}




/* Updates channels in the memory with info in buffer (a previously
 * read file). If a channel isn't in the memory yet, it gets added.
*/
void updateChannels(char* buffer, struct tm* time){
    int index = -1;
    char* bufptr = buffer;
    char cid[8] = "";
    char pid[8] = "";
    bufptr = getNextField(bufptr, cid, "cid=");
    while (bufptr != NULL){
        char chname[100] = "";
        char clients[5] = "";
        bufptr = getNextField(bufptr, pid, "pid=");
        bufptr = getNextField(bufptr, chname, "channel_name=");
        bufptr = getNextField(bufptr, clients, "total_clients=");
        int icid = atoi(cid);
        int ipid = atoi(pid);
        int iclients = atoi(clients);
        index = findChannel(icid);
        /* Channel doesn't exist in the array */
        if (index < 0){
            channelnum++;
            index = insertChannel(icid);
            channels[index].lastActive = *time;
            channels[index].average = iclients;
            /* Peak hours */
            if (withinPeak(time->tm_hour)){
                channels[index].average_wcom = iclients * 1.34; //every 3 clients get 1 free (update below too if changed)
                channels[index].average_wsum = 1;
            /* non-peak */
            } else {
                channels[index].average_wcom = iclients;
                channels[index].average_wsum = 1;
            }
        /* Channel exists in the array*/
        } else {
            float weight;
            /* Peak hours */
            if (withinPeak(time->tm_hour)){
                weight = 1.34;
            /* Non-peak */
            } else {
                weight = 1;
            }
            channels[index].average_wcom += (iclients * weight);
            channels[index].average_wsum += 1;
        }
        if (iclients > 0){
            channels[index].lastActive = *time;
            channels[index].lastUserNum = iclients;
        }
        strcpy(channels[index].lastName, chname);
        /* Update parents if any */
        while (ipid!=0){
            index = findChannel(ipid);
            if (index != -1){
                channels[index].average_wcom += iclients;
                ipid = channels[index].pid;
            }
        }
        
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
        char pid[8] = "";
        char chname[100] = "";
        char clients[10] = "";
        char lastActive[30] = "";
        char average[10] = "";
        char average_w[10] = "";
        char average_wsum[10] = "";
        char average_wcom[10] = "";
        bufptr = getNextField(bufptr, pid, "pid=");
        bufptr = getNextField(bufptr, chname, "lastName=");
        bufptr = getNextField(bufptr, clients, "lastUserNum=");
        bufptr = getNextField(bufptr, lastActive, "lastActive=");
        bufptr = getNextField(bufptr, average, "average=");
        bufptr = getNextField(bufptr, average_w, "average_w=");
        bufptr = getNextField(bufptr, average_wsum, "average_wsum=");
        bufptr = getNextField(bufptr, average_wcom, "average_wcom=");
        int icid = atoi(cid);
        int ipid = atoi(pid);
        int iclients = atoi(clients);
        float favg = atof(average);
        float favg_w = atof(average_w);
        long lavg_wsum = atoi(average_wsum);
        long lavg_wcom = atoi(average_wcom);
        struct tm* tlastActive = getCurrentTime();
        filenameToTime(lastActive, tlastActive, -5);
        struct channel* c = &channels[channelnum];
        c->cid = icid;
        c->pid = ipid;
        c->lastUserNum = iclients;
        c->lastActive = *tlastActive;
        c->average = favg;
        c->average_w = favg_w;
        c->average_wsum = lavg_wsum;
        c->average_wcom = lavg_wcom;
        /* Count channel rank now */
        c->average_w = c->average_wcom / (float)c->average_wsum;
        /**/
        strcpy(c->lastName, chname);
        channelnum++;
        bufptr = getNextField(bufptr, cid, "cid=");
    }
}

/* Prints all the channels in the memory 
 * if file != NULL also saves the result in a file
 */
void printChannels(char order, FILE* file){
    int i;
    int finish;
    if (order == '1'){
        i = -1;
        finish = channelnum-1;
    } else {
        i = channelnum-1;
        finish = 0;
    }
    while (i != finish){
        if (order == '1'){
            i++;
        } else {
            i--;
        }
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
        plog("=[CID: %d]=\n"
        "Parent CID: %d\n"
        "Last known name: %s\n"
        "Last known activity: %02d:%02d %02d/%02d/%d\n"
        "Last known number of clients: %d\n"
        "Average number of clients: %2.4f\n"
        "Channel's rating: %4.4f\n\n",
                c->cid, c->pid, c->lastName,
                c->lastActive.tm_hour, c->lastActive.tm_min,
                c->lastActive.tm_mday, c->lastActive.tm_mon,
                c->lastActive.tm_year,
                c->lastUserNum, c->average, c->average_w);
        
        /* If saving to file */
        if (file != NULL){
            fprintf(file, "=[CID: %d]=\n"
            "Parent CID: %d\n"
            "Last known name: %s\n"
            "Last known activity: %02d:%02d %02d/%02d/%d\n"
            "Last known number of clients: %d\n"
            "Average number of clients: %2.4f\n"
            "Channel's rating: %4.4f\n\n",
                    c->cid, c->pid, c->lastName,
                    c->lastActive.tm_hour, c->lastActive.tm_min,
                    c->lastActive.tm_mday, c->lastActive.tm_mon,
                    c->lastActive.tm_year,
                    c->lastUserNum, c->average, c->average_w);
        }
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
