/*main.cpp*/

//
// Divvy Bike Ride Route Analysis, using AVL trees.
//
// <<Zakee Jabbar (zjabba2)>>
// U. of Illinois, Chicago
// CS251, Spring 2017
// Project #04
//

// ignore stdlib warnings if working in Visual Studio:
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "avl.h"



//
// distBetween2Points:
//
// Returns the distance in miles between 2 points (lat1, long1) and (lat2, long2).
//
double distBetween2Points(double lat1, double long1, double lat2, double long2)
{
  //
  // Reference: http://www8.nau.edu/cvm/latlon_formula.html
  //
  double PI = 3.14159265;
  double earth_rad = 3963.1;  // statue miles:

  double lat1_rad = lat1 * PI / 180.0;
  double long1_rad = long1 * PI / 180.0;
  double lat2_rad = lat2 * PI / 180.0;
  double long2_rad = long2 * PI / 180.0;

  double dist = earth_rad * acos(
    (cos(lat1_rad)*cos(long1_rad)*cos(lat2_rad)*cos(long2_rad))
    +
    (cos(lat1_rad)*sin(long1_rad)*cos(lat2_rad)*sin(long2_rad))
    +
    (sin(lat1_rad)*sin(lat2_rad))
  );

  return dist;
}

//helper function to store all nodes that are <= distance given by user
int _matches(AVLNode *root, Find *values, int *c, double Lat, double Long, double dist)
{

   if(root == NULL)
   {
      return *c;
   }
   else
   {
      double distance = distBetween2Points(root->Value.Station.Lat,
                                           root->Value.Station.Long, Lat, Long);
      if (distance <= dist)
      {
          values[*c].Distance = distance;
          values[*c].Station = root->Value.Station.StationID;
          *c = *c+ 1;
      }

      _matches(root->Left, values, c,Lat, Long, dist);



      _matches(root->Right, values, c,Lat, Long, dist);
      return *c;
   }
}

//returns the array of stations that are within a range of the given coordinates
Find *Matches(AVLNode *root, int size, int* matches, double Lat, double Long, double dist)
{
  Find *values = (Find *)malloc(size * sizeof(Find));
  int c = 0;
  _matches(root, values, &c, Lat, Long, dist);
  *matches = c;

  return values;
}

//helper fuction to determine if a node is in the radius of a station
int _Route2(Find *list, int count, int station)
{
    int i;
    for(i = 0; i < count; i++)
    {
        if(list[i].Station == station)
        {
            return 1;
        }
    }
    return 0;
}

//helper function tot count the number trips that start within s' and end within d'
int _Route(AVLNode *root, Find *sp, Find *dp, int scount, int dcount, int *total)
{
    if(root == NULL)
   {
      return *total;
   }
   else
   {
      if ((_Route2(sp, scount, root->Value.Trip.From) == 1) && (_Route2(dp, dcount, root->Value.Trip.To) == 1))
      {
          *total = *total + 1;
      }

      _Route(root->Left, sp,dp,scount,dcount,total);
      _Route(root->Right, sp,dp,scount,dcount,total);
      return *total;
   }
}

//returns the count computed by the helper function
int Route(AVLNode *root, Find *sp, Find *dp, int scount, int dcount)
{
    int count = 0;
    _Route(root, sp,dp,scount,dcount,&count);
    return count;
}



//
// freeAVLNodeData
//
// Works with AVLFree() to free the data inside (key, value) pairs.
//
void freeAVLNodeData(AVLKey key, AVLValue value)
{
  //
  // what we free depends on what type of value we have:
  //
  if (value.Type == STATIONTYPE)
  {
      free(value.Station.Location);
  }
  else if (value.Type == TRIPTYPE)
  {
      return;
  }
  else if (value.Type == BIKETYPE)
  {
      return;
  }
  else
  {
    printf("**ERROR: unexpected value type in freeAVLNodeData!\n\n");
    exit(-1);
  }
}


//
// getFileName:
//
// Inputs a filename from the keyboard, make sure the file can be
// opened, and returns the filename if so.  If the file cannot be
// opened, an error message is output and the program is exited.
//
char *getFileName()
{
  char filename[512];
  int  fnsize = sizeof(filename) / sizeof(filename[0]);

  // input filename from the keyboard:
  fgets(filename, fnsize, stdin);
  filename[strcspn(filename, "\r\n")] = '\0';  // strip EOL char(s):

  // make sure filename exists and can be opened:
  FILE *infile = fopen(filename, "r");
  if (infile == NULL)
  {
    printf("**Error: unable to open '%s'\n\n", filename);
    exit(-1);
  }

  fclose(infile);

  // duplicate and return filename:
  char *s = (char *)malloc((strlen(filename) + 1) * sizeof(char));
  strcpy(s, filename);

  return s;
}


//
// skipRestOfInput:
//
// Inputs and discards the remainder of the current line for the
// given input stream, including the EOL character(s).
//
void skipRestOfInput(FILE *stream)
{
  char restOfLine[256];
  int rolLength = sizeof(restOfLine) / sizeof(restOfLine[0]);

  fgets(restOfLine, rolLength, stream);
}


///////////////////////////////////////////////////////////////////////
//
// main:
//
int main()
{
  printf("** Welcome to Divvy Route Analysis **\n");

  //
  // get filenames from the user/stdin:
  //
  char  cmd[64];
  char  str[512];
  int   ID;
  int   linesize = sizeof(str) / sizeof(str[0]);
  char *token;
  char *token2;
  char *token3;
  char *token4;
  char *token5;
  char *token6;
  char *token7;
  char *token8;
  char *StationsFileName = getFileName();
  char *TripsFileName = getFileName();

  FILE *Stations = fopen(StationsFileName, "r");
  FILE *Trips = fopen(TripsFileName, "r");

  //
  // As an example, create some trees and insert some
  // dummy (key, value) pairs:
  //
  AVL *stations = AVLCreate();
  AVL *trips = AVLCreate();
  AVL *bikes = AVLCreate();

  fgets(str, linesize, Stations);  //Gets the first useless line
  fgets(str, linesize, Stations);  // input a line:
  int SID;
  double Lat;
  double Long;
  int Cap;

  while (!feof(Stations))  // until the end of file:
  {
    str[strcspn(str, "\r\n")] = '\0';  // strip EOL char(s):

    token = strtok(str, ",");
    SID = atoi(token);
    token2 = strtok(NULL, ",");
    token3 = strtok(NULL, ",");     // gets all the required info
    Lat = atof(token3);
    token4 = strtok(NULL, ",");
    Long = atof(token4);
    token5 = strtok(NULL, ",");
    Cap = atoi(token5);

    AVLValue stationValue;

    stationValue.Type = STATIONTYPE;  // union holds a station:
    stationValue.Station.StationID = SID;
    stationValue.Station.Location = (char *)malloc(strlen(token2)+ 1);
    strcpy(stationValue.Station.Location, token2);
    stationValue.Station.Lat = Lat;
    stationValue.Station.Long = Long;
    stationValue.Station.Cap = Cap;
    stationValue.Station.TripCount = 0;

    AVLInsert(stations, stationValue.Station.StationID, stationValue);

    fgets(str, linesize, Stations);
  }


  fgets(str, linesize, Trips);  //Gets the first useless line
  fgets(str, linesize, Trips);  // input a line:
  int TID;
  int BID;
  int Dur;
  int From;
  int To;

  while (!feof(Trips))  // until the end of file:
  {
    str[strcspn(str, "\r\n")] = '\0';  // strip EOL char(s):

    token = strtok(str, ",");
    TID = atoi(token);
    token2 = strtok(NULL, ",");
    token3 = strtok(NULL, ",");
    token4 = strtok(NULL, ",");     //gets all the required info
    BID = atoi(token4);
    token5 = strtok(NULL, ",");
    Dur = atoi(token5);
    token6 = strtok(NULL, ",");
    From = atoi(token6);
    token7 = strtok(NULL, ",");
    token8 = strtok(NULL, ",");
    To = atoi(token8);

    AVLValue tripValue;

    tripValue.Type = TRIPTYPE;  // union holds a trip:
    tripValue.Trip.TripID = TID;
    tripValue.Trip.BikeID = BID;
    tripValue.Trip.Duration = Dur;
    tripValue.Trip.From = From;
    tripValue.Trip.To = To;

    AVLNode *start = AVLSearch(stations, From);
    AVLNode *end = AVLSearch(stations, To);

    start->Value.Station.TripCount++;   //incrementing station trip count
    end->Value.Station.TripCount++;


    AVLInsert(trips, tripValue.Trip.TripID, tripValue);

    AVLValue bikeValue;

    bikeValue.Type = BIKETYPE;  // union holds a bike:
    bikeValue.Bike.BikeID = BID;
    bikeValue.Bike.TripCount = 1;

    AVLNode *bike;
    bike = AVLSearch(bikes, BID);
    if(bike != NULL)
    {
        bike->Value.Bike.TripCount++;   //incrementing bike trip count
    }
    else
    {
        AVLInsert(bikes, bikeValue.Bike.BikeID, bikeValue); //building bike tree
    }

    fgets(str, linesize, Trips);
  }
  //
  // now interact with user:
  //
  printf("** Ready **\n");

  scanf("%s", cmd);

  while (strcmp(cmd, "exit") != 0)
  {
    cmd[strcspn(cmd, "\r\n")] = '\0';
    if (strcmp(cmd, "stats") == 0)
    {
      //
      // Output some stats about our data structures:
      //
      printf("** Trees:\n");

      printf("   Stations: count = %d, height = %d\n",
        AVLCount(stations), AVLHeight(stations));
      printf("   Trips:    count = %d, height = %d\n",
        AVLCount(trips), AVLHeight(trips));
      printf("   Bikes:    count = %d, height = %d\n",
        AVLCount(bikes), AVLHeight(bikes));
    }
    else if(strcmp(cmd, "station") == 0)
    {
        scanf("%d", &ID);
        AVLNode *station = AVLSearch(stations, ID);
        if(station == NULL)
        {
            printf("**not found\n");
        }
        else
        {
            printf("**Station %d:\n", ID);
            printf("  Name: '%s'\n", station->Value.Station.Location);
            printf("  Location:   (%lf,%lf)\n", station->Value.Station.Lat, station->Value.Station.Long);
            printf("  Capacity:   %d\n", station->Value.Station.Cap);
            printf("  Trip count: %d\n", station->Value.Station.TripCount);
        }
    }
    else if(strcmp(cmd, "trip") == 0)
    {
        scanf("%d", &ID);
        AVLNode *trip = AVLSearch(trips, ID);
        if(trip == NULL)
        {
            printf("**not found\n");
        }
        else
        {
            printf("**Trip %d:\n", ID);
            printf("  Bike: %d\n", trip->Value.Trip.BikeID);
            printf("  From: %d\n", trip->Value.Trip.From);
            printf("  To:   %d\n", trip->Value.Trip.To);
            printf("  Duration: %d min, %d secs\n", trip->Value.Trip.Duration/60, trip->Value.Trip.Duration % 60);;
        }
    }
    else if(strcmp(cmd, "bike") == 0)
    {
        scanf("%d", &ID);
        AVLNode *bike = AVLSearch(bikes, ID);
        if(bike == NULL)
        {
            printf("**not found\n");
        }
        else
        {
            printf("**Bike %d:\n", ID);
            printf("  Trip count: %d\n", bike->Value.Bike.TripCount);
        }
    }
    else if(strcmp(cmd, "find") == 0)
    {
        double lat, longi, dist;
        scanf("%lf", &lat);
        scanf("%lf", &longi);
        scanf("%lf", &dist);
        Find *match;
        int numcount;

        match = Matches(stations->Root, trips->Count, &numcount, lat, longi, dist);

      //sorts the array in ascending order
      int i, j;
      for(i = 0; i < numcount - 1; i++)
      {
          int smallest = i;

        for (j = i+1; j < numcount; j++)
        {
            if(match[j].Distance < match[smallest].Distance)
            {

                smallest = j;
            }
            else if(match[j].Distance == match[smallest].Distance)
            {
                if(match[j].Station < match[smallest].Station)
                {
                    smallest = j;
                }
            }
        }

        Find T = match[i];  //swaps the values
        match[i] = match[smallest];
        match[smallest] = T;

      }

      for (i = 0; i < numcount; i++)
      {
          printf("Station %d: distance %lf miles\n", match[i].Station, match[i].Distance);
      }
      free(match);


    }
    else if(strcmp(cmd, "route") == 0)
    {
        int tripn;
        double dist2;
        scanf("%d", &tripn);
        scanf("%lf", &dist2);
        AVLNode *route = AVLSearch(trips, tripn);
        if(route == NULL)
        {
            printf("**not found\n");
        }
        else
        {
            AVLNode *s = AVLSearch(stations, route->Value.Trip.From);
            AVLNode *d = AVLSearch(stations, route->Value.Trip.To);
            int numcount1, numcount2;
            Find *sp = Matches(stations->Root, stations->Count, &numcount1, s->Value.Station.Lat, s->Value.Station.Long, dist2);
            Find *dp = Matches(stations->Root, stations->Count, &numcount2, d->Value.Station.Lat, d->Value.Station.Long, dist2);
            sp[numcount1].Station = s->Value.Station.StationID;
            dp[numcount2].Station = d->Value.Station.StationID;
            numcount1++;
            numcount2++;

            int total = Route(trips->Root, sp, dp, numcount1, numcount2);

            printf("** Route: from station #%d to station #%d\n", s->Value.Station.StationID, d->Value.Station.StationID);
            printf("** Trip count: %d\n", total);
            int tripc = trips->Count;
            double percentage = (double)total/tripc;
            percentage = percentage * 100;
            printf("** Percentage: %lf%%\n", percentage);
            free(sp);
            free(dp);
        }

    }
    else
    {
      printf("**unknown cmd, try again...\n");
    }

    scanf("%s", cmd);
  }

  //
  // done, free memory and return:
  //
  printf("** Freeing memory **\n");

  AVLFree(stations, freeAVLNodeData);
  AVLFree(trips, freeAVLNodeData);
  AVLFree(bikes, freeAVLNodeData);
  fclose(Trips);
  fclose(Stations);
  free(StationsFileName);
  free(TripsFileName);

  printf("** Done **\n");

  return 0;
}


