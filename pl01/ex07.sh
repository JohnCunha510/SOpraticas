#!/bin/sh
while true
do
	clear
        # display menu
        echo "Server Name - $(hostname)"
	echo "-------------------------------"
	echo "MENU"
	echo "==============="
	echo "1-List the files of HOME directory"
	echo "2-Show text file"
	echo "3-Edit file"
	echo "4-Quit"
        # get input from the user
	read -p "Enter your option [ 1 - 4 ] " option
        # make decision using case..in..esac
	case $option in
		1)
			ls /home/$USER
			read -p "Press [Enter] key to continue..." readEnterKey
			;;
		2)
			ls -x /home/$USER/*.txt
			read -p "Press [Enter] key to continue..." readEnterKey
			;;
		3)
			kate /home/$USER/*.txt  &
			read -p "Press [Enter] key to continue..." readEnterKey
			;;
		4)
			echo "Close"
			exit 0
			;;
		*)
			echo "Error: Invalid option..."
			read -p "Press [Enter] key to continue..." readEnterKey
			;;
	esac

done
