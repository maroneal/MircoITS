#!/bin/bash

# Log wireless status every 1s

cd /home/pi/Desktop/MicroITS

filename="log.txt" # Filename of the log file

if [ "$1" = -h ] # Help section
then
	echo -e "This script launch a wireless monitor and log data like signal strength, signal quality, sent and received packets...\nYou can use the argument \"-del\" to delete the log file before starting"
else
	if [ "$1" = -del ] && [ -f ${filename} ] # Option to delete the previous log file if existing
	then
		rm ${filename}
	fi
	
	lxterminal -e wavemon # Launch the graphical Wi-Fi monitoring named Wavemon

	while true
	do
		date +"%D - %T" >> ${filename}	# Write date in the log file
		sudo iwlist wlan0 scan essid MicroITS | egrep "Cell|ESSID|Signal|Rates" >> ${filename} # This command takes around 1s to execute and write Wi-Fi quality in the log file
		echo -e "\n" >> ${filename}
		ifconfig wlan0 | egrep "RX packets|TX packets" >> ${filename} # Write the number of TX and RX packets sent and lost
		echo -e "\n" >> ${filename}
	done
fi
