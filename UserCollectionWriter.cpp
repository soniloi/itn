#include "UserCollection.h"

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::map;
using std::ofstream;
using std::pair;
using std::priority_queue;
using std::set;
using std::string;
using std::stringstream;
using std::vector;

/* Write server statistics to stream */
void UserCollection::Writer::writeServers(UserCollection* uc, ofstream& results){
	char clarify_open = Config::getChar(KEY_CONF_OUT_CLARIFY_OPEN);
	char clarify_close = Config::getChar(KEY_CONF_OUT_CLARIFY_CLOSE);
	int size_list_short = Config::getInt(KEY_CONF_SIZE_LIST_SHORT);

	map<string, int> servers;
	map<string, int> hostmasks;

	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		string hm = it->second->getHostmask();
		if(!hm.empty()){
			int hostpoint = hm.find("@");
			string serv = hm.substr(hostpoint+1);
			servers[serv]++;
			hostmasks[hm]++;
		}
	}

	if(!servers.empty()){

		results << endl << "[--- TOP SERVERS ---]" << endl << endl;
		results << "(this information is limited as it only takes into account users seen to join or quit during the time examined)" << endl << endl;

		priority_queue<pair<string, int>, vector<pair<string, int> >, StringIntPairComparator> serverqueue;
		for(map<string, int>::iterator it = servers.begin(); it != servers.end(); it++)
			serverqueue.push(*(it));

		int i = 0;
		while(i < size_list_short && !serverqueue.empty()){
			results << (i+1) << ".\t" << serverqueue.top().first << SPACE << clarify_open << serverqueue.top().second << clarify_close << endl;
			serverqueue.pop();
			i++;
		}

	}

	results << endl;
/*
	if(!hostmasks.empty()){
		results << endl << "[--- TOP CONNECTORS ---]" << endl << endl;
		results << "(this information is only available for users who are seen to join or quit during the time examined)" << endl << endl;

		priority_queue<pair<string, int>, vector<pair<string, int> >, StringIntPairComparator> hmqueue;
		for(map<string, int>::iterator it = hostmasks.begin(); it != hostmasks.end(); it++)
			hmqueue.push(*(it));

		int i = 0;
		while(i < size_list_short && !hmqueue.empty()){
			results << (i+1) << ".\t" << hmqueue.top().first << SPACE << clarify_open << hmqueue.top().second << clarify_close << endl;
			hmqueue.pop();
			i++;
		}
	}
*/
}

/* Write line total statistics to stream */
void UserCollection::Writer::writeLineTotals(UserCollection* uc, ofstream& results){
	char list_open = Config::getChar(KEY_CONF_OUT_LIST_OPEN);
	char list_close = Config::getChar(KEY_CONF_OUT_LIST_CLOSE);
	int nick_length_max = Config::getInt(KEY_CONF_NICK_LENGTH_MAX);

	results << "[=== PEOPLE ===]" << endl << endl << endl;

	results << std::setprecision(2);
	results << "[--- USERS BY LINECOUNT ---]" << endl << endl;
	results << "\tCurrent nick\tLines\t/me%\tAlias" << endl << endl;

	priority_queue<User*, vector<User*>, UserLineComparator> userqueue;

	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		userqueue.push(it->second);
	}

	int i = 1;
	while(!userqueue.empty()){
		User* u = userqueue.top();
		string ni =  u->nickToString(nick_length_max);
		int linecount = u->getCount(KEY_ATTR_LINE);
		float mepc = 0.0;
		if(linecount > 0)
			mepc = (float) u->getCount(KEY_ATTR_ME)/linecount * 100.0;
		string alias = u->aliasesToString();
		results << i << ".\t" << ni << linecount << TAB << mepc;
		results << "\t" << list_open << alias << list_close << endl;
		userqueue.pop();
		i++;
	}

	results << endl << endl << "[--- IGNORES ---]" << endl << endl;
	results << "(Anyone detected, but then ignored)" << endl << endl;

	results << "Individual nicks:" << endl << endl;
	if(uc->ignorednicks.empty())
		results << "\tNone" << endl;
	else{
		for(set<string>::iterator it = uc->ignorednicks.begin(); it != uc->ignorednicks.end(); it++)
			results << TAB << (*it) << endl;
	}

	results << endl << "Hostmasks:" << endl << endl;
	if(uc->ignoredhostmasks.empty())
		results << "\tNone" << endl;
	else{
		for(set<pair<string, int> >::iterator it = uc->ignoredhostmasks.begin(); it != uc->ignoredhostmasks.end(); it++)
			results << TAB << (*it).first << " (ignored " << (*it).second << " in total)" << endl;
	}
}

/* Write Users' word statistics to stream */
void UserCollection::Writer::writeWordTotals(UserCollection* uc, ofstream& results, list<string> ordered){

	int wordcount_excluded = Config::getInt(KEY_CONF_WORDCOUNT_EXCLUDED);
	char list_open = Config::getChar(KEY_CONF_OUT_LIST_OPEN);
	char list_close = Config::getChar(KEY_CONF_OUT_LIST_CLOSE);
	int size_list_short = Config::getInt(KEY_CONF_SIZE_LIST_SHORT);
	int nick_length_max = Config::getInt(KEY_CONF_NICK_LENGTH_MAX);
	string word_valid_punctuation = Config::getString(KEY_CONF_WORD_VALID_PUNCTUATION);

	// Create a map of all words in the channel to the number of users who use them
	map<string, int> wordSayerCounts;
	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		User* sayer = it->second;
		countmap sayerwords = sayer->getWords();
		for(map<string, int>::iterator jt = sayerwords.begin(); jt != sayerwords.end(); jt++){
			wordSayerCounts[jt->first]++;
		}
	}

	// Determine the cutoff point in the ordered wordlist to be used for exclusions
	int cutoff = 0;
	list<string>::iterator lit;
	for(lit = ordered.begin() ; lit != ordered.end() && cutoff < wordcount_excluded ; lit++){
		cutoff++;
	}
	ordered.erase(lit, ordered.end()); // Erase the appropriate number of elements from the exclusion list

	results << endl << "[--- USERS BY WORDCOUNT ---]" << endl << endl;
	results << "\tCurrent nick\tTotal\t/Line\tVocab\tVar%\tMost used uncommon words" << endl << endl;

	priority_queue<User*, vector<User*>, UserWordComparator> userqueue;

	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		userqueue.push(it->second);
	}

	int i = 1;
	while(!userqueue.empty() && userqueue.top()->getCount(KEY_ATTR_WORD) > 0){ // In this case, don't print the lurkers
		User* u = userqueue.top();
		string ni =  u->nickToString(nick_length_max);
		int linecount = u->getCount(KEY_ATTR_LINE);
		int wordcount = u->getCount(KEY_ATTR_WORD);
		float wpl = 0.0;
		int vocab = u->countVocabulary();
		float variation = ((float) vocab / (float) wordcount) * 100.0;
		if(linecount > 0) // Need to catch divide-by-zeros
			wpl = (float) wordcount/linecount;
		string uncommon = u->wordsToString(ordered, wordSayerCounts);
		results << i << ".\t" << ni << wordcount << TAB << wpl << TAB << vocab
			<< TAB << variation << TAB << list_open << uncommon << list_close << endl;

		userqueue.pop();
		i++;
	}

	/* Write list of most-directly-referenced-nicks to stream */
	results << endl << endl << "[--- MOST REFERENCED USERS ---]" << endl << endl;

	priority_queue<User*, vector<User*>, UserReferencedComparator> refqueue;
	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		refqueue.push(it->second);
	}

	if(refqueue.empty() || refqueue.top()->getCount(KEY_ATTR_REFERENCED) < 1)
		results << "Looks like no-one's talking directly to anyone else in here >.>" << endl;

	else{
		results << "\tCurrent Nick\tReferences" << endl << endl;

		i = size_list_short;
		while(i > 0 && !refqueue.empty() && refqueue.top()->getCount(KEY_ATTR_REFERENCED) > 0){
			User* u = refqueue.top();
			results << (size_list_short-i+1) << ".\t" << u->nickToString(nick_length_max) << u->getCount(KEY_ATTR_REFERENCED) << endl;
			refqueue.pop();
			i--;
		}

	}
}

/* Write Comings-and-Goings statistics to stream */
void UserCollection::Writer::writeCAG(UserCollection* uc, ofstream& results){
	int size_list_short = Config::getInt(KEY_CONF_SIZE_LIST_SHORT);
	int nick_length_max = Config::getInt(KEY_CONF_NICK_LENGTH_MAX);

	results << "[=== COMINGS AND GOINGS ===]" << endl << endl << endl;

	User* u = NULL;
	results << TAB;

	u = uc->getMostCount(KEY_ATTR_JOIN);
	if(u == NULL)
		results << "No-one joined the channel during the time examined";
	else
		results << u->getNick() << " joined the most times, " << u->getCount(KEY_ATTR_JOIN);
	results << endl << endl << TAB;

	u = uc->getMostCount(KEY_ATTR_PART);
	if(u == NULL)
		results << "No-one left the channel during the time examined";
	else
		results << u->getNick() << " left the channel the most times, " << u->getCount(KEY_ATTR_PART);
	results << endl << endl << TAB;

	u = uc->getMostCount(KEY_ATTR_QUIT);
	if(u == NULL)
		results << "No-one in the channel quit IRC during the time examined";
	 else
		results << u->getNick() << " quit the most times, " << u->getCount(KEY_ATTR_QUIT);
	results << endl << endl << TAB;

	u = uc->getMostCount(KEY_ATTR_KILL);
	if(u == NULL)
		results << "No-one in the channel was killed during the time examined";
	else
		results << u->getNick() << " was killed the most times, " << u->getCount(KEY_ATTR_KILL);

	results << endl << endl << endl;

	// Write most kicks information to stream 
	results << "[--- KICKS ---]" << endl << endl;

	priority_queue<User*, vector<User*>, UserKRComparator> userKRqueue;
	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		if(it->second->getCount(KEY_ATTR_KICKSRECEIVED) > 0)
			userKRqueue.push(it->second);
	}

	if(userKRqueue.empty())
		results << "None of that carry-on here, everyone was civilised \\o/" << endl;

	else{
		//	Write most kicks received statistics
		results << "Received: " << endl << endl;
		results << "\tNick\t\tQuantity" << endl << endl;

		User* u = NULL;
		int i = size_list_short;
		while(i>0 && !userKRqueue.empty() && userKRqueue.top()->getCount(KEY_ATTR_KICKSRECEIVED) > 0){
			u = userKRqueue.top();
			results << (size_list_short-i+1) << ".\t" << u->nickToString(nick_length_max) << u->getCount(KEY_ATTR_KICKSRECEIVED) << endl;
			userKRqueue.pop();
			i--;
		}

		results << endl << endl;

		//	Write most kicks performed statistics; if none were received, then none were performed, so this is conditional
		results << "Performed: " << endl << endl;
		results << "\tNick\t\tQuantity" << endl << endl;

		priority_queue<User*, vector<User*>, UserKPComparator> userKPqueue;
		for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
			if(it->second->getCount(KEY_ATTR_KICKSPERFORMED) > 0)
				userKPqueue.push(it->second);
		}

		i = size_list_short;
		while(i>0 && !userKPqueue.empty()){
			u = userKPqueue.top();
			results << (size_list_short-i+1) << ".\t" << u->nickToString(nick_length_max) << u->getCount(KEY_ATTR_KICKSPERFORMED) << endl;
			userKPqueue.pop();
			i--;
		}

	}
}

/* Write mode-changes to stream */
void UserCollection::Writer::writeModes(UserCollection* uc, ofstream& results){
	int size_list_short = Config::getInt(KEY_CONF_SIZE_LIST_SHORT);
	int nick_length_max = Config::getInt(KEY_CONF_NICK_LENGTH_MAX);
	int dictator_scale = Config::getInt(KEY_CONF_DICTATOR_INDICATOR_VALUE); // Scale by the maximum number of points a line can get for dictatorness

	// Write most voices granted statistics
	results << "[=== GOVERNANCE ===]" << endl << endl << endl;
	results << "[--- CHAMPIONS OF FREE SPEECH ---]" << endl << endl;
	results << "Most (voices granted - voices removed)" << endl << endl;

	priority_queue<User*, vector<User*>, UserVBComparator> userVBqueue; // B for balanced as we balance voices granted against voices removed to reduce messing
	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		if((it->second->getModePlus(KEY_MODE_VOICE) > it->second->getModeMinus(KEY_MODE_VOICE)) > 0)
			userVBqueue.push(it->second);
	}

	if(userVBqueue.empty())
		results << "No champions in this channel ;_;" << endl;

	else{
		results << "\tNick\t\tQuantity" << endl << endl;
		User* u;
		int i = size_list_short;
		while(i>0 && !userVBqueue.empty()){
			u = userVBqueue.top();
			results << (size_list_short-i+1) << ".\t" << u->nickToString(nick_length_max) << (u->getModePlus(KEY_MODE_VOICE)-u->getModeMinus(KEY_MODE_VOICE)) << endl;
			userVBqueue.pop();
			i--;
		}
	}

	results << endl << endl << "[--- MOST LIKELY TO BE DICTATORS ---]" << endl << endl;
	results << "(Bans, kicks, topic-locks, and more)" << endl << endl;

	priority_queue<User*, vector<User*>, UserDictatorComparator> userDqueue;
	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		if(it->second->getRateByTotalCount(KEY_ATTR_DICTATOR) > 0.0)
			userDqueue.push(it->second);
	}

	if(userDqueue.empty())
		results << "Complete freedom from dictatorship in this channel \\o\\ \\o/ /o/" << endl;
	else{
		results << "\tNick\t\tDictator%" << endl << endl;
		int i = size_list_short;
		User* u = NULL;
		while(i>0 && !userDqueue.empty()){
			u = userDqueue.top();
			results << (size_list_short+1-i) << ".\t" << u->nickToString(nick_length_max) << (u->getRateByTotalCount(KEY_ATTR_DICTATOR)/(dictator_scale)) << endl;
			userDqueue.pop();
			i--;
		}
	}
}

/* Write emotion statistics to file */
void UserCollection::Writer::writeEmotions(UserCollection* uc, ofstream& results){

	results << "[=== GENERAL CARRY-ON ===]" << endl << endl << endl;
	results << "(based on an assortment of smilies and other trigger words said by a user)" << endl << endl;

	User* u = NULL;
	results << TAB;

	// Happy
	u = uc->getMostRateByWordCount(KEY_ATTR_HAPPY);
	if(u==NULL)
		results << "No-one in this channel is happy ;_;";
	else
		results << "The happiest person in the channel is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_HAPPY) << "% of words indicate happiness :)";
	results << endl << endl << TAB;

	// Sad
	u = uc->getMostRateByWordCount(KEY_ATTR_SAD);
	if(u==NULL)
		results << "No-one in this channel is sad \\o/";
	else
		results << "The saddest person in the channel is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_SAD) << "% of words indicate sadness :(";
	results << endl << endl << TAB;

	// Grim
	u = uc->getMostRateByWordCount(KEY_ATTR_GRIM);
	if(u==NULL)
		results << "No-one in this channel is grim";
	else
		results << "The grimmest person in the channel is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_GRIM) << "% of words indicate grimness :|";
	results << endl << endl << TAB;

	// Sarcastic
	u = uc->getMostRateByWordCount(KEY_ATTR_SARCASTIC);
	if(u==NULL)
		results << "No-one in this channel is sarcastic. We all say what we mean, and mean what we say, all of the time";
	else
		results << "The most sarcastic person in the channel is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_SARCASTIC) << "% of words indicate sarcasm :P";
	results << endl << endl << TAB;

	// Surprised
	u = uc->getMostRateByWordCount(KEY_ATTR_SURPRISED);
	if(u==NULL)
		results << "No-one in this channel is ever surprised";
	else
		results << "The person most likely to be caught by surprise is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_SURPRISED) << "% of words indicate surprise :O";
	results << endl << endl << TAB;

	// Angry
	u = uc->getMostRateByWordCount(KEY_ATTR_ANGRY);
	if(u==NULL)
		results << "No-one in this channel is angry \\o/";
	else
		results << "The angriest person in the channel is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_ANGRY) << "% of words indicate anger >:(";
	results << endl << endl << TAB;

	// Evil
	u = uc->getMostRateByWordCount(KEY_ATTR_EVIL);
	if(u==NULL)
		results << "No-one in this channel is evil, which is good";
	else
		results << "The most evil person in the channel is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_EVIL) << "% of words indicate evil >:D";
	results << endl << endl << TAB;

	// Sleaze
	u = uc->getMostRateByWordCount(KEY_ATTR_SLEAZY);
	if(u==NULL)
		results << "No sleaze in this channel";
	else
		results << "The sleaziest person in the channel is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_SLEAZY) << "% of words imply sleaze ;) ;) ;)";
	results << endl << endl << TAB;

	u = uc->getMostRateByWordCount(KEY_ATTR_PROFANITY);
	if(u==NULL)
		results << "No profanity in this channel";
	else
		results << "The person most likely to utter a profanity is: " << u->getNick() << "; " << u->getRateByWordCount(KEY_ATTR_PROFANITY) << "% of words were something profane ****";

	results << endl;
}

/* Write features to file */
void UserCollection::Writer::writeFeatures(UserCollection* uc, ofstream& results){

	float bot_cutoff = (float) Config::getInt(KEY_CONF_BOT_CUTOFF);
	int bot_proximity_0_value = Config::getInt(KEY_CONF_BOT_PROXIMITY_0_VALUE);
	int bot_trigger_value = Config::getInt(KEY_CONF_BOT_TRIGGER_VALUE);
	int bot_scale = bot_proximity_0_value + bot_trigger_value; // We scale by this amount because it is the maximum botishness value an individual line can receive
	int bot_linecount_cutoff = Config::getInt(KEY_CONF_BOT_LINECOUNT_CUTOFF);
	int decimal_places_to_print = Config::getInt(KEY_CONF_DECIMAL_PLACES_TO_PRINT);
	int size_list_long = Config::getInt(KEY_CONF_SIZE_LIST_LONG);
	int size_list_medium = Config::getInt(KEY_CONF_SIZE_LIST_MEDIUM);
	int nick_length_max = Config::getInt(KEY_CONF_NICK_LENGTH_MAX);

	results << "[=== FEATURES ===]" << endl << endl << endl;
	results << "[--- MOST LIKELY TO BE BOTS ---]" << endl << endl;
	results << "(lists all users with " << bot_linecount_cutoff << " or more spoken lines and a bot% of " << bot_cutoff << " or more)" << endl << endl;

	priority_queue<User*, vector<User*>, UserBotComparator> userqueue;
	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		if((it->second->getCount(KEY_ATTR_LINE) >= bot_linecount_cutoff) && (it->second->getRateByLineCount(KEY_ATTR_BOT)/(bot_scale) > bot_cutoff))
			userqueue.push(it->second);
	}

	if(userqueue.empty())
		results << "This channel is a bot-free zone B)." << endl;
	else{
		results << "\tNick\t\t\tBot%" << endl << endl;
		int i = size_list_long;
		User* u = NULL;
		while(i>0 && !userqueue.empty()){
			u = userqueue.top();
			results << (size_list_long+1-i) << ".\t" << u->nickToString(nick_length_max) << "\t" << (u->getRateByLineCount(KEY_ATTR_BOT)/(bot_scale)) << endl;
			userqueue.pop();
			i--;
		}
	}

	results << endl << endl << "[--- BFFs ---]" << endl << endl;
	results << "(based on the number of times users reference one another directly ... of course, sometimes the other person might not talk back as often ... )" << endl << endl;

	priority_queue<UserPairCharacteristic, vector<UserPairCharacteristic>, UserPairCharacteristicComparator> upairqueue;

	for(usermap::iterator it = uc->users.begin() ; it != uc->users.end() ; it++){
		map<User*, int> itreferees = it->second->getReferees();
		
		usermap::iterator jt = it;
		for(++jt ; jt != uc->users.end() ; jt++){
			map<User*, int> jtreferees = jt->second->getReferees();
			
			int firsttosecond = 0;
			map<User*, int>::iterator kt = itreferees.find(jt->second); // Search for second user in first user's map
			if(kt != itreferees.end())
				firsttosecond = kt->second;

			int secondtofirst = 0;
			map<User*, int>::iterator lt = jtreferees.find(it->second); // Search for first user in second user's map
			if(lt != jtreferees.end())
				secondtofirst = lt->second;

			int combinedrefs = firsttosecond + secondtofirst;

			if(combinedrefs > 0){

				// We want the user who made more references to appear first
				User* first = it->second;
				User* second = jt->second;
				if(secondtofirst > firsttosecond){
					first = jt->second;
					second = it->second;
					secondtofirst = firsttosecond;
					firsttosecond = lt->second;
				}

				UserPairCharacteristic upc(first, second, (float) combinedrefs);
				upairqueue.push(upc);
			}			
		}
	}

	if(upairqueue.empty())
		results << "No friends in this channel :(" << endl;

	else{
		results << "\tRefs\tNick 1\t\t\tNick 2\t\t\tStatus" << endl << endl;

		int maxwidth = 20;

		int i = size_list_medium;
		while(i>0 && !upairqueue.empty()){

			UserPairCharacteristic upc = upairqueue.top();
			int firsttosecond = upc.getFirst()->getReferees()[upc.getSecond()];
			int secondtofirst = upc.getSecond()->getReferees()[upc.getFirst()];
			float fsf = (100.0*(((float) firsttosecond)/upc.getCharacteristic()));
			float sff = (100.0*(((float) secondtofirst)/upc.getCharacteristic()));

			results << (size_list_medium-i+1) << ".\t" << (int) upc.getCharacteristic() << "\t";
			
			stringstream temp1;
			temp1 << std::fixed << std::setprecision(decimal_places_to_print) << upc.getFirst()->getNick () << " (" << fsf << "%)";
			int len = temp1.str().length();
			while(len < maxwidth){
				temp1 << SPACE;
				len++;
			}
			
			stringstream temp2;
			temp2 << std::fixed << std::setprecision(decimal_places_to_print) << upc.getSecond()->getNick() << " (" << sff << "%)";
			len = temp2.str().length();
			while(len < maxwidth){
				temp2 << SPACE;
				len++;
			}
						
			results << temp1.rdbuf() << "\t" << temp2.rdbuf() << "\t";

			if(fsf > 80)
				results << ">.>";
			else if(fsf > 60)
				results << ">=((*>";
			else if(fsf > 50 || fsf != sff)
				results << "-<--<@";
			else
				results << "<3 <3 <3";

			results << endl;
			upairqueue.pop();
			i--;
		}

	}

	results << endl << endl << "[--- PROGRAMMING LANGUAGES ---]" << endl << endl;

	map<string, set<string> > mostfavourites; // Map of language names to the users who like them most
	map<string, set<string> > leastfavourites; // Map of language names to the users who like them least

	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){
		string mostfavourite = it->second->getMostFavouriteLanguage();
		if(!mostfavourite.empty()){ // Maybe this person has never expressed a language preference
			map<string, set<string> >::iterator jt = mostfavourites.find(mostfavourite);
			if(jt == mostfavourites.end()){ // Language has not been seen yet, must add it to map
				set<string> likers;
				likers.insert(it->second->getNick());
				mostfavourites[mostfavourite] = likers;
			}
			else{ // Language has already been seen, so we just add this user's nick to the list of likers
				jt->second.insert(it->second->getNick());
			}
		}

		string leastfavourite = it->second->getLeastFavouriteLanguage();
		if(!leastfavourite.empty()){
			map<string, set<string> >::iterator jt = leastfavourites.find(leastfavourite);

			if(jt == leastfavourites.end()){ // Language has not been seen yet, must add it to map
				set<string> haters;
				haters.insert(it->second->getNick());
				leastfavourites[leastfavourite] = haters;
			}
			else{ // Language has already been seen, so we just add this user's nick to the list of likers
				jt->second.insert(it->second->getNick());
			}
		}
	}

	results << "Favourite languages" << endl << endl;
	results << "Language\tAdherents" << endl;

	for(map<string, set<string> >::iterator kt = mostfavourites.begin(); kt != mostfavourites.end(); kt++){
		results << kt->first << "\t\t{ ";
		for(set<string>::iterator lt = kt->second.begin(); lt != kt->second.end(); lt++){
			results << (*lt) << SPACE;
		}
		results << '}' << endl;
	}

	results << endl << endl;

	results << "Least favourite languages" << endl << endl;
	results << "Language\tH8rs" << endl;

	for(map<string, set<string> >::iterator kt = leastfavourites.begin(); kt != leastfavourites.end(); kt++){
		results << kt->first << "\t\t{ ";
		for(set<string>::iterator lt = kt->second.begin(); lt != kt->second.end(); lt++){
			results << (*lt) << SPACE;
		}
		results << '}' << endl;
	}

	results << endl << endl << "[--- HOUSES OF HOGWARTS ---]" << endl << endl;
	results << "(imagine the Sorting Hat made its decisions based on your IRC history)" << endl << endl;

	map<string, set<string> > houses; // Map of house names to member names

	for(usermap::iterator it = uc->users.begin(); it != uc->users.end(); it++){

		string house = it->second->getHouse();
		if(!house.empty()){ // Maybe user has no house

			map<string, set<string> >::iterator jt = houses.find(house);
			if(jt == houses.end()){ // House has not been seen yet, must add it to map
				set<string> members;
				members.insert(it->second->getNick());
				houses[house] = members;
			}
			else{ // House has already been seen, so we just add this user's nick to the list of members
				jt->second.insert(it->second->getNick());
			}
		}
	}

	results << "House\t\t\tMembers" << endl;

	for(map<string, set<string> >::iterator kt = houses.begin(); kt != houses.end(); kt++){
		results << kt->first << " (" << kt->second.size() << ")\t\t{ ";
		for(set<string>::iterator lt = kt->second.begin(); lt != kt->second.end(); lt++){
			results << (*lt) << SPACE;
		}
		results << '}' << endl;
	}
}
