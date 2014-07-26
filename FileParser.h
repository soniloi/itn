/* Logfile handling:
 *  reading in from input file(s) by line
 *  determining the nature of a line and passing this to the relevant Collection for parsing
 */
 
#include <fstream>
#include <map>
#include <stdlib.h>
#include "ChannelCollection.h"
#include "Time.h"
#include "LineParser.h"	

class FileParser{
private:
	ChannelCollection* channel;
	UserCollection* users;

public:
	FileParser(std::string filepath, WordCategories * wcats=NULL);
	~FileParser();
	void buildStats(std::vector<std::string> filesin, int contentVectors, successcode &code);
	void adjustStats(std::string fileignore, std::string filemerge);
	void writeStats(std::string fileout, int contentVectors);
};

