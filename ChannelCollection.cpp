#include "ChannelCollection.h"

using std::list;
using std::map;
using std::ofstream;
using std::pair;
using std::stack;
using std::string;
using std::vector;

/* Constructor */
ChannelCollection::ChannelCollection(string cn, UserCollection* uc){
	this->lineCount = 0;
	this->channame = cn;
	this->users = uc;
	for(int i=0 ; i<HOURS_PER_DAY ; i++)
		hours[i] = 0;
	this->starttime = NULL;
	this->endtime = NULL;
}

/* Destructor */
ChannelCollection::~ChannelCollection(){
}

/* Return the channel name */
string ChannelCollection::getName(){
	return this->channame;
}

/* Increment the number of lines found for this channel */
void ChannelCollection::incrementLineCount(){
	this->lineCount++;
}

/* Increment the number of hours found at a particular hour */
void ChannelCollection::incrementHour(int hour){
	this->hours[hour]++;
}

void ChannelCollection::setStarttime(Time time){
	this->starttime = time;
}

void ChannelCollection::setEndtime(Time time){
	this->endtime = time;
}

/* Add a line of words to channel */
void ChannelCollection::insertLine(string nick, string line, string type, int botlike, int content){
	this->incrementLineCount();
	
	string delimiters = Config::getString(KEY_CONF_WORD_VALID_DELIMITERS);
	stringvectorpair wordsandtokens = Tokeniser::doubletokenise(line, delimiters);
	this->addWords(wordsandtokens.first);
	this->users->insertLine(nick, wordsandtokens, type, botlike, content);
}

/* Add words to channel */
void ChannelCollection::addWords(vector<string> words){
	for(vector<string>::iterator it=words.begin() ; it!=words.end() ; it++){
		this->wordcounts[(*it)]++;
	}
}

/* Add a topic change by a particular nick */
void ChannelCollection::insertTopicChange(string line, int loc, Time time){
	string info_topicchange = Config::getString(KEY_CONF_INFO_TOPICCHANGE);
	string info_topicchange_to = Config::getString(KEY_CONF_INFO_TOPICCHANGE_TO);

	string nick;

	int i = loc - 1;
	while(line[i] != SPACE)
		i--; // Go back to before start of nick
	i++; // Skip preceding space

	while(line[i] != SPACE){
		nick.push_back(line[i]);
		i++;
	}

	int topicstart = loc + ((string) (info_topicchange)).length() + 
		this->channame.length() + ((string) (info_topicchange_to)).length();
	string topictext(line);
	topictext.erase(0, topicstart);
	
	topic top;
	top.text = topictext;
	top.nick = nick;
	top.time = time.display();
	
	this->topics.push(top);
	
	User* user = this->users->getUserByKey(nick);
	if(!user)
		this->users->createUser(nick);
}

/* Write word totals to stream */
void ChannelCollection::writeWords(ofstream& results, list<string> &ordered){
	ChannelCollection::Writer ccw;
	ccw.writeWords(this, results, ordered);
}

/* Write overview to stream */
void ChannelCollection::writeOverview(ofstream& results){
	ChannelCollection::Writer ccw;
	ccw.writeTotals(this, results);
	ccw.writeHours(this, results);
}
