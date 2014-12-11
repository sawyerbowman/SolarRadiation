 ______     ______     ______     _____        __    __     ______    
/\  == \   /\  ___\   /\  __ \   /\  __-.     /\ "-./  \   /\  ___\   
\ \  __<   \ \  __\   \ \  __ \  \ \ \/\ \    \ \ \-./\ \  \ \  __\   
 \ \_\ \_\  \ \_____\  \ \_\ \_\  \ \____-     \ \_\ \ \_\  \ \_____\ 
  \/_/ /_/   \/_____/   \/_/\/_/   \/____/      \/_/  \/_/   \/_____/ 

Project Name:	Solar Radiation
Created by: 	Sawyer Bowman '15 & Clarence Johnson, III '15
Email:			Sawyer: sawyerbowman@gmail.com
				Clarence: proexodus@gmail.com
Date Created: 	November 16, 2014
Last Modified: 	December 11, 2014 
Version:		1.0
Language: 		C

---------------------------------------------------------------------

DESCRIPTION:
This program takes in a ASCII elevation grid and computes solor radiation on the terrain as well as shadows at specific time of the year, for any amount of time, in any time zone and turbidity with the option for calcualting in daylight savings or not. 

HOW TO RUN:
Enter a filename to read into the program (ex maine.asc), a filename to store the output (ex maine_energy.asc), a start time (ex 12.5 for 12:30 p.m.), an end time (ex 17 for 5:00 p.m.), a step size for time increments (ex 2.5 for 2.5 hours), a day number (ex 1 for Jan 1), a UTC time zone offset (ex -5 for EST), a turbidity value from the SODA database (ex 2.2, assume uniform across grid), a 1 for daylight savings or a 0 for no daylight savings (ex 1) and a number of threads corresponding to number of processors (ex 4).\n")

KNOWN BUGS:
None. :)