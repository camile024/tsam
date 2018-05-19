# The Manual For TSAM - Teamspeak Activity Monitor
Last update - 02/02/2017 - release of 0.93
# Index
[1. Settings file](#settings-file-explanation)  
[2. Script files](#script-file-explanation)  
[3. Running/usage](#runningusage)
# Settings file explanation
A settings file must have the following (in a particular order) lines:  
**ip=** followed by the ip number of the server. Used to connect to a particular TeamSpeak server via telnet.  
**port=** followed by the **query** port number of the server, same as above.  
**sid=** followed by the server ID - used to choose the server to store the data about. E.g. one ip can lead to a server
hosting more than one virtual teamspeak servers.  
**login=** followed by the query (or any other privileged account) login/username.  
**password=** followed by the query (or any other privileged account) password.  
**delay-seconds=** followed by a number of seconds - delay between each snapshot  
**old-time=** followed by a number of days that specify the age of files to be removed at each snapshot  
**peak-start=** followed by a *HH* format of hour, after which the channel rank activity gets increased more  
**peak-end=** followed by a *HH* format of hour, after which the channel rank activity gets increased at a normal rate  
**record-start=** followed by a *HH* format of hour, after which the snapshots will be taken with the appropriate delays between them  
**record-end=** followed by a *HH* format of hour, after which the snapshots will not be taken  
**ignore-temp=** followed by a *0*/*1* (true/false) value specifying whether to ignore temporary and semi-permament channels during
snapshots  
  
The number of characters/whitespaces between each *option=value* is trivial and does not affect the file reading.
Settings file has to be in the executable's folder.  
  
 
# Script file explanation
Script files have been provided that can be modified to meet user's needs in terms of action during certain events (e.g. to do something in case of error):  
**tsam_onStartup.sh** is ran when the software starts up.  
**tsam_onError.sh** is ran when either server connection can't be established, software exits, or snapshot was not taken successfully.  
**tsam_onSnapshot.sh** is ran every time a snapshot is taken and actual data is stored (every time SNAPSHOT SUCCESSFULL message pops up).  
 
# Running/usage
The software can be run in 3 modes:
## Regular
Use ./tsam to run (no parameters). Main mode - creates snapshots and saves them in the data/ folder every X seconds specified in the settings
file. Cannot be interacted with and runs until either closed by user or an error occurs.
## With parameter -c
Example: ./tsam -c  
Used to compile all the snapshot files into a compressed file, which analyses all the snapshot files and provides statistics for
each channel. Includes a text-based UI to display the results, sort them, or save to file
## With parameter -a
Example: **./tsam -a [main_parameter] [extra_options]**
Can be used as a Linux command-line tool. Behaves like ./tsam -c, but accepts a set of parameters and exits automatically after finishing.
Useful if, for example, a scipt is to be run to generate a new compressed file/results every X time.  
Can be pipelined to other Linux tools if needed (e.g. grep). Use **./tsam -a help** for available commands
