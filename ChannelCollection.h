/* Class containing data and methods pertaining to global channel information */

#ifndef CHANNELCOLLECTION_H
#define CHANNELCOLLECTION_H

#include <fstream>
#include <list>
#include <vector>
#include <queue>
#include <map>
#include <stack>
#include "Config.h"
#include "Time.h"
#include "UserCollection.h"

typedef struct topic{
	std::string text; // Content of topic
	std::string nick; // Nick of setter at time of setting
	std::string time; // Time of setting in text format
} topic;

class ChannelCollection{

private:
	int lineCount;
	std::string channame;
	Time starttime; // First timestamp found in log
	Time endtime; // Last timestamp found in log
	UserCollection* users; // Pointer to the user collection with which this channel is associated
	int hours[HOURS_PER_DAY];
	std::map<std::string, int> wordcounts;
	std::stack<topic> topics; // If one were to do this by User, one would risk dangling pointers when it came to merging them
	class Writer{
	public:
		void writeHours(ChannelCollection* cc, std::ofstream& results);
		void writeTotals(ChannelCollection* cc, std::ofstream& results);
		void writeWords(ChannelCollection* cc, std::ofstream& results, std::list<std::string> &ordered);
	};

public:
	ChannelCollection(std::string cn, UserCollection* uc);
	~ChannelCollection();
	std::string getName();
	void incrementLineCount();
	void incrementHour(int hour);
	void setStarttime(Time time);
	void setEndtime(Time time);

	void insertLine(std::string nick, std::string line, std::string type, int botlike, int content);
	void addWords(std::vector<std::string> words);
	void insertTopicChange(std::string line, int loc, Time time); // Add a topic change
	void writeOverview(std::ofstream& results);
	void writeWords(std::ofstream& results, std::list<std::string> &ordered);
};

#endif
