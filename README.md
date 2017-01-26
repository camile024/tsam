# TSAM
Teamspeak Activity Monitor

This software is used to log in to your teamspeak server using TCP/IP protocol and your query data, to gather and store details about the server. I use it on my Raspberry Pi, running 24/7 to determine which channels are abandoned and which channels are particularily active to move them higher up the hierarchy (less scrolling).  
  
Platform: Linux (development: Ubuntu, target: Raspberry Pi)  
IDE: NetBeans  

##ToDo:
-Anything listed in the issues
  
##Features:
Every set period of time (default: 1 hour) the program:  
-Stores channel IDs, channel name, number of clients on the channel  
-Removes old logs (if specified)  
  
Each time requested the program:  
-Calculates/shows each channel's data based on the files provided:  
*channel ID  
*last known name  
*last recorded activity (users > 0)  
*average activity (based solely on the files provided)  
*~~weighted average activity~~ channel rating (based on peak-hours of the server time)  

##Installation (Ubuntu)
1) Get contents of the bin/ubuntu folder  
2) Edit 'tsam_settings' file  
3) Run tsam (./tsam in terminal) or add to autostart  
  
##Installation (Raspberry Pi/Debian-based distros)
1) Copy contents of the bin/rasppi/ folder to your raspberry pi  
2) Edit 'tsam_settings' file  
3) './tsam' in terminal or just run 'tsam' file (can set to autostart)  
  
##Compilation (Linux)
1) Copy contents of the src/ folder (make sure to copy .h + .c + makefile files and nbproject folder)  
2) 'make' in terminal while being in the folder  
3) Executable will be in 'dist/Debug/GNU-Linux/
4) Make sure to provide tsam_settings file in the executable folder
