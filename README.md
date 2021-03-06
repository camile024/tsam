# TSAM (Current version: 0.94 [25/08/2017])
Teamspeak Activity Monitor

This software is used to log in to your teamspeak server using TCP/IP protocol and your query data, to gather and store details about the server. I use it on my Raspberry Pi, running 24/7 to determine which channels are abandoned and which channels are particularily active to move them higher up the hierarchy (less scrolling).  
  
**Platform:** Linux (development: Ubuntu, target: Raspberry Pi)  
**IDE:** NetBeans  

## ToDo:
-Anything listed in the [issues](https://github.com/camile024/tsam/issues)
  
## Features:
**Program does the following repeatedly, after time interval and during times specified by user (e.g. every 1 hour, 12:00-00:00):**  
-Stores channel IDs, parent channel's ID, channel name, number of clients on the channel  
-Removes old data (if specified, e.g. data older than 7 days)  
  
**Each time requested the program:**  
-Calculates/shows each channel's data based on the files gathered:  
*channel ID  
*parent channel ID  
*last known name  
*last recorded activity (users > 0)  
*average activity (based solely on the files provided)  
*~~weighted average activity~~ channel rating (based on peak-hours of the server time specified by user and subchannels' activity)  

## Installation/Usage (Ubuntu)
1) Get contents of the [bin/ubuntu](bin/ubuntu) folder  
2) Edit 'tsam_settings' file  
3) Run tsam (./tsam in terminal) or add to autostart  
4) Run tsam with parameter -c or -a (./tsam -c) to use the compressor (converts all the snapshot files into a compressed file with averages/last activity etc. and shows it in a neat output).  
5) For more functionality see [the manual.md file](manual.md)  
  
## Installation/Usage (Raspberry Pi/Debian-based distros)
1) Copy contents of the [bin/rasppi/](bin/rasppi) folder to your raspberry pi  
2) Edit 'tsam_settings' file  
3) './tsam' in terminal or just run 'tsam' file (can set to autostart)  
4) Run tsam with parameter -c or -a (./tsam -c) to use the compressor (converts all the snapshot files into a compressed file with averages/last activity etc. and shows it in a neat output).  
5) For more functionality see [the manual.md file](manual.md)  
  
## Compilation (Linux)
1) Copy contents of the [src/](src/) folder (make sure to copy .h + .c + makefile files and nbproject folder)  
2) 'make' in terminal while being in the folder  
3) Executable will be in 'dist/Debug/GNU-Linux/  
4) Make sure to provide tsam_settings file in the executable folder

## Changelog:
### Version 0.94 (25/08/2017)
-Fixed bugs related to updating channel's last active date  
-Fixed counting channel's rank for channels with only 1 occurence (e.g. temporary)  

### Version 0.93 (02/02/2017):  
**NOT COMPATIBLE WITH EARLIER SETTINGS FILE** (although a missing line can be added to the end of the file - the setting: ignore-temp= with value 0 or 1 (false/true) )  
  
-Added ignoring temporary channels when gathering snapshots (temp channels from earlier snapshots will not be removed and will still be considered)  
-Added automatisation for the compressor for usage straight from the command line (e.g.  
"./tsam -a print -save -inc -avg >> programlog.txt" will create a compressed file, sort it increasingly according to their average client number, save the result to file and exit, at the same time saving all the program output in programlog.txt) - see github page -> manual.md or run ./tsam with parameters: -a help  
-Reworked channel rank to consider the span of all the snapshot files, not just the snapshots that include the channel  
-Fixed average number not updating  
-Fixed not all snapshot files being read by compressor  
  
### Version 0.92 (29/01/2017):  
**NOT COMPATIBLE WITH 0.9/0.91 SNAPSHOTS/SETTINGS FILE(S)!**  
-Added saving compressor's sorted/view results to file  
-Added sorting to the compressor's interface  
-Added parent ID to recorded data  
-Channel rank now based on peak hours and activity on sub-channels (and their sub-channels etc.)  
-Added generating snapshots only during specified hours  
-Addded testing connection and login details on startup  
  
### Version 0.91 (28/01/2017):
-Increased the number of digits after decimal point for 'average' and' channel rank'  
-Fixed compressor failing to load files due to wrong pathnames  
-Fixed incorrect compressor message regarding snapshot files  
-Fixed a bug, where the last few channels get duplicated in the records  
  
### Version 0.9:  
-First release  
