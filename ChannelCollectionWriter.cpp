#include "ChannelCollection.h"

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::map;
using std::ofstream;
using std::pair;
using std::priority_queue;
using std::string;
using std::vector;

/* Write channel totals to stream */
void ChannelCollection::Writer::writeTotals(ChannelCollection* cc, ofstream& results){
	results << "[=== OVERVIEW ===]" << endl << endl << endl;
	results << "[--- SUMMARY ---]" << endl << endl;

	results << "Time examined: ";
	if(cc->starttime == NULL)
		results << "???";
	else 
		results << cc->starttime.display();
	results << " - ";
	if(cc->endtime == NULL)
		results << "???";
	else
		results << cc->endtime.display();
	results << endl << endl;
	
	results << "In " << cc->channame << " for this time there have been: " << endl << endl;
	results << "\t- " << cc->lineCount << " total lines of activity" << endl;
	results << "\t- " << cc->users->countUsers() << " users detected" << endl << endl << endl;
}

/* Write per-hour statistics to stream */
void ChannelCollection::Writer::writeHours(ChannelCollection* cc, ofstream& results){
	results << "[--- LINES BY HOUR OF THE DAY ---]" << endl << endl;

	results << "Hour:";
	for(int i=0 ; i<HOURS_PER_DAY ; i++)
		results << TAB << i;
	results << endl;

	results << "Lines:";
	for(int i=0 ; i<HOURS_PER_DAY ; i++)
		results << TAB << cc->hours[i];
	results << endl;

	results << "%";
	for(int i=0 ; i<HOURS_PER_DAY ; i++)
		results << TAB << 100.0 * cc->hours[i] / cc->lineCount;

	results << endl << endl;
}

/* Write topic list and word-total statistics to stream
 * Alters a reference variable, ordered, as this will be needed by UserCollectionWriter */
void ChannelCollection::Writer::writeWords(ChannelCollection* cc, ofstream& results, list<string> &ordered){
	int words_per_line = Config::getInt(KEY_CONF_OUT_WORDS_PER_LINE);
	int size_list_long = Config::getInt(KEY_CONF_SIZE_LIST_LONG);
	char clarify_open = Config::getChar(KEY_CONF_OUT_CLARIFY_OPEN);
	char clarify_close = Config::getChar(KEY_CONF_OUT_CLARIFY_CLOSE);
	string word_valid_punctuation = Config::getString(KEY_CONF_WORD_VALID_PUNCTUATION);

	results << "[=== CONTENT ===]" << endl << endl << endl;
	results << "[--- RECENT TOPICS ---]" << endl << endl;

	int j = Config::getInt(KEY_CONF_SIZE_LIST_TOPICS);
	while(!cc->topics.empty() && j > 0){
		topic top = cc->topics.top();
		results << "\"" << top.text << "\" by: " << top.nick << " at: " << top.time << endl;
		cc->topics.pop();
		j--;
	}

	results << endl << endl << "[--- MOST COMMON WORDS IN " << cc->channame << " ---]" << endl;

	int i = 0;
	priority_queue<pair<string, int>, vector<pair<string, int> >, StringIntPairComparator> pairqueue;

	for(map<string, int>::iterator it=cc->wordcounts.begin() ; it!=cc->wordcounts.end() ; it++){
		if(WordCategories::isValidWord(it->first, word_valid_punctuation))
		pairqueue.push(*(it));
	}

	while(i < size_list_long && !pairqueue.empty()){
		ordered.push_back(pairqueue.top().first); // Push the most common elements, the ones being printed, to the list
		if((i%words_per_line) == 0){
			results << endl;
			results << TAB << clarify_open << (i+1) << '-' << (i+words_per_line) << clarify_close << TAB;
			if((i+words_per_line) < 100)
				results << TAB;
		}
		pair<string, int> word = pairqueue.top();
		results << word.first << clarify_open << word.second << clarify_close << SPACE;
		i++;
		pairqueue.pop();
	}

	while(!pairqueue.empty()){
		ordered.push_back(pairqueue.top().first); // Push the remaining elements, those not significant enough to be printed, to the list
		pairqueue.pop();
	}

	results << endl << endl;
}
