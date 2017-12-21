Zakee Jabbar
UIC


"Divvy Bike Analysis w/ AVL Trees"


This program takes in data on Divvy bikes, stations, and trips and stores them into three AVL Trees. After the data is stored, the user can enter different commands to get specific data.

To run this program:

gcc avl.c main.c -o myProg
./myProg


The following commands are supported:

1. stats (simple stats on AVL Tree)
2. station id (gives info on stations given an id number)
3. trip id (gives info on a trip given a trip number)
4. bike id (gives info on a bike given a bike number)
5. find latitude longitude distance
	a. Given latitude, longitude and a distance in miles, finds all the divvy stations withing that range.
6. route tripID distance
	a. The user enters a trip id, which defines a starting station S and a destination station D. The user also enters a distance, in miles. Let S’ be all stations that are <= distance away from S, and let D’ be all stations that are <= distance away from D. Searches the trip data and counts all trips that start from a station in S’, and end at a station in D’. Trip Count and percentage out of total trips is shows.


Sample User Input:

stations.csv (Other stations file from Data Files folder)
trips.csv (Other trips file from Data Files Folder)
Any of the commands above after the "Ready" message.




