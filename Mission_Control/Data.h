/**
 * Data.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef Data_h
#define Data_h

class Data
{
  public:

	//Parses passed in message by using commas as the identifiers.
	float Parse(char[] message, int objective);
	
	
	
	
	//This timer is used for syncing the mesh network. Every craft as 5 seconds to talk. 
	//   When this timer is at 0, that means the craft just broadcasted its data. When this
	//   this timer hits 15, it resets to 0. This insures that the 2 other nodes have their 5
	//   seconds to talk. 3nodes * 5seconds = 15seconds for a total netowrk revolution. 
	unsigned long start = 0;
	
	//Timer object used for tracking overall operation time of the craft.
	time_t Time;

};

#endif