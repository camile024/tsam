#include "libs.h"
#include "defines.h"
#include "main.h"
#include "channel.h"
#include "utilities.h"



/* Checks if the current hour fits within recording hours */
int withinRecord(){
    int peak1 = atoi(recordstart);
    int peak2 = atoi(recordend);
    int hour = getCurrentTime()->tm_hour;
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






/* Returns struct tm pointer with current date and time */
struct tm* getCurrentTime(){
    time_t epochtime = time(NULL);
    struct tm* curtime = localtime(&epochtime);
    return curtime;
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




/* Compares struct tm*s down to a minute. -1 if time1 is earlier,
 * +1 if time2 is earlier, 0 if equal
 */
int timecmp(struct tm time1, struct tm time2){
    if (time1.tm_year > time2.tm_year)
        return 1;
    else if (time1.tm_year < time2.tm_year)
        return -1;
    
    if (time1.tm_mon > time2.tm_mon)
        return 1;
    else if (time1.tm_mon < time2.tm_mon)
        return -1;
    
    if (time1.tm_mday > time2.tm_mday)
        return 1;
    else if (time1.tm_mday < time2.tm_mday)
        return -1;
    
    if (time1.tm_hour > time2.tm_hour)
        return 1;
    else if (time1.tm_hour < time2.tm_hour)
        return -1;
    
    if (time1.tm_min > time2.tm_min)
        return 1;
    else if (time1.tm_min < time2.tm_min)
        return -1;
    
    return 0;
}





/*========================================================================
 * ============= QUICK SORT ALGORITHM RELATED FUNCTIONS ==================
 * =======================================================================
 */


void quicksort(struct channel* channels, int first, int last, int criteria){
    if (first > last)
        return;
    int pivot = last;
    int part = qs_partition(channels, first, last, pivot, criteria);
    quicksort(channels, first, part-1, criteria);
    quicksort(channels, part+1, last, criteria);
}


/* Quicksort partitioning function (criteria:
 * 1 - sort based on CID
 * 4 - sort based on average clients number
 * 5 - sort based on channel rank
 * 2 - sort based on name
 * 3 - sort based on time
 * Smallest to largest
 */
int qs_partition(struct channel* channels, int first, int last, int pivot, int criteria){
    int start = first-1;
    int end = last;
    struct channel temp;
    switch (criteria){
        /* Cid-based partition */
        case 1:
            while(1){
                //printf("%d\n", channels[start+1].cid);
                while (channels[++start].cid < channels[pivot].cid){
                    ;
                } 
                while (end > 0 && channels[--end].cid > channels[pivot].cid){
                    ;
                } 
                if (start >= end){
                    break;
                } else {
                    temp = channels[start];
                    channels[start] = channels[end];
                    channels[end] = temp;
                }
            }
            temp = channels[start];
            channels[start] = channels[last];
            channels[last] = temp;
            return start;
            break;
        /* Average client num partition*/
        case 4:
            while(1){
                while (channels[++start].average < channels[pivot].average){
                    ;
                } 
                while (end > 0 && channels[--end].average > channels[pivot].average){
                    ;
                } 
                if (start >= end){
                    break;
                } else {
                    temp = channels[start];
                    channels[start] = channels[end];
                    channels[end] = temp;
                }
            }
            temp = channels[start];
            channels[start] = channels[last];
            channels[last] = temp;
            return start;
            break;
        /* Channel rank based partition */
        case 5:
            while(1){
                while (channels[++start].average_w < channels[pivot].average_w){
                    ;
                } 
                while (end > 0 && channels[--end].average_w > channels[pivot].average_w){
                    ;
                } 
                if (start >= end){
                    break;
                } else {
                    temp = channels[start];
                    channels[start] = channels[end];
                    channels[end] = temp;
                }
            }
            temp = channels[start];
            channels[start] = channels[last];
            channels[last] = temp;
            return start;
            break;
        /* Name based partition */
        case 2:
            while(1){
                while (strcmp(channels[++start].lastName, channels[pivot].lastName) < 0){
                    ;
                } 
                while (end > 0 && strcmp(channels[--end].lastName, channels[pivot].lastName) > 0){
                    ;
                } 
                if (start >= end){
                    break;
                } else {
                    temp = channels[start];
                    channels[start] = channels[end];
                    channels[end] = temp;
                }
            }
            temp = channels[start];
            channels[start] = channels[last];
            channels[last] = temp;
            return start;
            break;
        /* Last active time based partition*/
        case 3:
            while(1){
                while (timecmp(channels[++start].lastActive, channels[pivot].lastActive) < 0){
                    ;
                } 
                while (end > 0 && timecmp(channels[--end].lastActive, channels[pivot].lastActive) > 0){
                    ;
                } 
                if (start >= end){
                    break;
                } else {
                    temp = channels[start];
                    channels[start] = channels[end];
                    channels[end] = temp;
                }
            }
            temp = channels[start];
            channels[start] = channels[last];
            channels[last] = temp;
            return start;
            break;
    }
}