/* 
 * File:   channel.h
 * Author: Kamil Cupial <kac12>
 *
 * Created on 24 January 2017, 17:13
 */
#pragma once


#include "libs.h"
#include "defines.h"

struct channel {
    int cid;
    int lastUserNum;
    struct tm lastActive;
    char lastName[100];
    
    float average;
    float average_w;
    int average_wsum;
    
};