
/* 
 * File:   sockets.h
 * Author: Kamil Cupial <kac12>
 *
 * Created on 23 January 2017, 21:41
 */
#pragma once


int net_getinfo(char*);
int testConnection();
int net_connect();
int login(int);
int selectServer(int);
int getCommand(int, char*, char*);