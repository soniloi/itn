#include "UserCollection.h"

using std::cout;
using std::endl;
using std::list;
using std::map;
using std::ofstream;
using std::pair;
using std::string;
using std::vector;

/* Constructor */
UserCollection::UserCollection(WordCategories * wcats){
	this->cats = wcats;
}

/* Destructor */
UserCollection::~UserCollection(){
	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		delete it->second;
	}
}

/* Amalgamate the properties of two users into one, as far as a third user is concerned
 *	(to be run after merging fromUser into toUser)
 * Does not delete any user
 * 
 * FIXME: should probably re-write in such a way that User does not have to give friendship
 */
void UserCollection::amalgamateProperties(User* toUser, User* fromUser){
	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		User* u = it->second;
		map<User*, int>::iterator jt = u->referees.find(fromUser); // We only need to worry about the fromUser, as that is the one being merged from (and ultimately deleted)
		if(jt != u->referees.end()){
			u->referees[toUser] += jt->second;
			u->referees.erase(jt);
		}
	}
	
	for(map<User*, int>::iterator it = fromUser->referees.begin() ; it != fromUser->referees.end() ; it++){
		toUser->referees[it->first] += it->second;
	}
}

/* Merge two users and update all relevant connecting data */
void UserCollection::mergeUsers(User* toUser, User* fromUser){
	if(toUser->getNick().find(KEY_ATTR_BOT) != string::npos || fromUser->getNick().find(KEY_ATTR_BOT) != string::npos)
		return; // ugh

	toUser->merge(fromUser);
	this->amalgamateProperties(toUser, fromUser);
}

/* Return whether the user with a certain nick is present */
bool UserCollection::containsUser(string nick){
	return this->users.find(nick) != this->users.end();
}

/* Set key for user in usermap */
void UserCollection::setKey(User* user, string key){
	this->users[key] = user;
}

/* Removes key from usermap it it exists, ignores otherwise; does nothing to the value */
void UserCollection::removeKey(string key){
	string k(key);
	transform(k.begin(), k.end(), k.begin(), ::tolower);

	usermap::iterator it = this->users.find(k);
	if(it != this->users.end())
		this->users.erase(it); // Remove the old key
}

/* Remove key from usermap by value; does not delete the actual value */
void UserCollection::removeKeyByValue(User* value){
	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		if(it->second == value){
			usermap::iterator jt = it;
			++it;
			this->users.erase(jt);
		}
	}
}

/* Removes key and calls destructor on value if key exists */
void UserCollection::deleteByKey(string key){
	string k(key);
	transform(k.begin(), k.end(), k.begin(), ::tolower);

	usermap::iterator it = this->users.find(k);
	if(it != this->users.end()){
		delete it->second;
		this->users.erase(it);
	}
}

/* Removes key and calls destructor on value if value exists */
void UserCollection::deleteByValue(User* value){
	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		if(it->second == value){
			delete it->second;
			usermap::iterator jt = it;
			++it; // Get around broken iterator problem
			this->users.erase(jt);
		}
	}
}

/* Add a new user by nick
 * Return a pointer to the new user
 * Note that this accepts a string of any case and converts it to the appropriate case */
User* UserCollection::createUser(string nick, string hostmask){
	User* user = new User(nick, hostmask);
	this->insertUser(nick, user);
	return user;
}

/* Search for a user by current key, which *should* be the same as the nick
 * May return NULL
 * Do not be tempted to use this->users[n], because this will add a key */
User* UserCollection::getUserByKey(string key){
	string n(key);
	transform(n.begin(), n.end(), n.begin(), ::tolower);

	usermap::iterator it = this->users.find(n);
	if(it == this->users.end())
		return NULL;

	return it->second;
}

/* Search for a user by current nick, which FIXME: is not always the same as current key
 * If multiple users in map somehow have the same current nick, will return the first one found
 * May return NULL */
User* UserCollection::getUserByNick(string nick){
	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++)
		if(!WordCategories::compareIgnoreCase(nick, it->second->getNick()))
			return it->second;
	return NULL;
}

/* Wrapper around getUserByKey and createUser */
User* UserCollection::getOrCreateUserByNick(string nick){
	User* u = this->getUserByKey(nick);
	if(!u)
		u = this->createUser(nick);
	u->setNick(nick);		
	return u;
}

/* Insert an existing valid user for a key */
void UserCollection::insertUser(string key, User* user){
	string k(key);
	transform(k.begin(), k.end(), k.begin(), ::tolower);
	this->setKey(user, k);
}

/* Search for a user by hostmask
 * This searches the userlist due to practicality
 * Clearly slower than searching by nick, but this is mitigated by
 *  running string comparisons only on users that are marked as not-in-channel
 * May return NULL */
User* UserCollection::getUserByHostmask(string hostmask){

	// SHOULD PROBABLY BUILD SOME SAFETY INTO THIS, SUCH AS IF USER FOUND HASN'T BEEN SEEN IN OVER A YEAR, IT IS PROBABLY SPURIOUS
	// TO SAY FOR SURE THAT THEY'RE THE ONE WE'RE LOOKING FOR
	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		User* u = it->second;
		if(!hostmask.compare(u->getHostmask()))
			return u;
	}
	return NULL;
}

/* Erase all users in collection with a certain nick */
void UserCollection::eraseByNick(string nick){
	cout << "\tIgnoring user with nick: " << nick << endl;
	string n(nick);
	transform(n.begin(), n.end(), n.begin(), ::tolower);
	this->deleteByKey(n);
	this->ignorednicks.insert(nick);
}

/* Erase all users in the collection with a certain hostmask */
void UserCollection::eraseByHostmask(string hostmask, vector<string> exceptions){
	cout << "\tIgnoring all users with hostmask: " << hostmask << " except: ";
	for(vector<string>::iterator it = exceptions.begin(); it != exceptions.end(); it++)
		cout << (*it) << SPACE;

	cout << endl << "\t{ ";

	int ignoredcount = 0; // The number of users that are going to be ignored due to having this hostmask

	usermap::iterator it = this->users.begin();
	while(it != this->users.end()){
		string userhostmask = it->second->getHostmask();
		string usernick = it->first;
		
		if(!hostmask.compare(userhostmask) && !WordCategories::vectorContainsStringIgnoreCase(exceptions, usernick)){
			cout << usernick << SPACE;
			usermap::iterator jt = it;
			jt++; // We increment this first, to get around the broken iterator problem
			this->users.erase(it);
			it = jt;
			ignoredcount++;			
		}
		else
			it++;
	}

	cout << '}' << endl;
	if(ignoredcount > 0)
		this->ignoredhostmasks.insert(pair<string, int> (hostmask, ignoredcount));
}

/* Parse a nick out of a join/part/quit/kill line
 * Note that this will NOT work on kick lines as these do not have hostmask information
 * The index parameter is the charat index in the line of the first character of
 *  the accidental marker, usually the 'h' in "has" */
string UserCollection::parseNick(string line, int index){
	int i = index - 1;
	while(line[i] != SPACE) // Move backwards through the ident/host information
		i--;
	while(line[i] == SPACE)
		i--;
	while(line[i] != SPACE) // Move backwards through the nick
		i--;
	i++; // Skip initial space

	string nick;
	while(line[i] != SPACE){
		nick.push_back(line[i]);
		i++;
	}

	return nick;
}

/* Parse a hostmask out of a join/part/quit/kill line
 * Note that this will NOT work on kick lines as these do not have hostmask information
 * The index parameter is the charat index in the line of the first character of
 *  the accidental marker, usually the 'h' in "has" */
string UserCollection::parseHostmask(string line, int index){
	int i = index-1;
	while(line[i] != SPACE) // Move backwards through the ident/host information
		i--;
	i++;

	if(line[i] == Config::getChar(KEY_CONF_HOSTMASK_OPEN))
		i++; // Skip any denoter of host

	string mask;
	while(line[i] != SPACE){
		mask.push_back(line[i]);
		i++;
	}

	mask.erase(mask.length()-1); // Remove any denoter of host

	return mask;
}

/* Insert a line, already split into word and token vectors, to the collection */
void UserCollection::insertLine(string nick, stringvectorpair wordsandtokens, string type, int botlike, int content){
	User* user = this->getOrCreateUserByNick(nick);
	user->setIn(true);
	user->addToCount(KEY_ATTR_BOT, botlike);
	user->addLine(wordsandtokens, type, content, this->cats, this);
}

/* Insert nick change */
void UserCollection::insertNickChange(string line, int index){

	int i = index-1;
	while(line[i] != SPACE)
		i--;
	i++;

	string nickold;
	while(line[i] != SPACE){
		nickold.push_back(line[i]);
		i++;
	}

	string nickchange = Config::getString(KEY_CONF_INFO_NICKCHANGE);
	i = index + nickchange.length();

	string nicknew;
	while(line[i] != SPACE && line[i] != '\n' && line[i] != '\0'){
		nicknew.push_back(line[i]);
		i++;
	}

	User* user = this->getOrCreateUserByNick(nickold);
	user->incrementCount("nickchange");

	User* ghost = this->getUserByKey(nicknew); // There may be a user who is changing back to usual nick e.g. -!- mollusc_ is now known as mollusc

	if((ghost==NULL) || (ghost==user)){ // There is no ghost, we proceed as normal
		user->setIn(true);
		user->setNick(nicknew);
		this->removeKey(nickold);
		this->insertUser(nicknew, user); // Map to the new key
	}
	else if(ghost != NULL && !ghost->getLocalpart().compare(user->getLocalpart())){
		mergeUsers(ghost, user);
		ghost->setNick(nicknew); // FIXME: Should this be necessary?
		this->deleteByKey(nickold);
	}
}

/* Get user from an accidental line, which contains both nick and hostmask information */
User* UserCollection::getUserFromAccidental(string line, int index){

	string hostmask = this->parseHostmask(line, index);
	string nick = this->parseNick(line, index);
	User* user = getUserByKey(nick);

	if(!user){
		// Look for user by hostmask, but only if the existing one was marked as not-in-channel, to reduce spurious matches
		User* possible  = getUserByHostmask(hostmask);
		if((possible !=NULL) && !(possible->isIn()))
			user = possible;

		if(!user) // User found neither by nick nor by hostmask, so create new
			user = this->createUser(nick, hostmask);
		else{ // User found by hostmask but not by nick; nick (and therefore key) might be out of date
			this->removeKeyByValue(user);
			this->insertUser(nick, user);
			user->setNick(nick);
		}
	}
	else
		user->setHostmask(hostmask);

	return user;
}

/* Insert a join accidental */
void UserCollection::insertJoin(string line, int index){
	User* user = getUserFromAccidental(line, index);
	user->setIn(true);
	user->incrementCount(KEY_ATTR_JOIN);
}

/* Insert a part/quit/kill accidental */
void UserCollection::insertLeave(string line, int index, string type){
	User* user = getUserFromAccidental(line, index);
	user->setIn(false);
	user->incrementCount(type);
}

/* Insert a kick accidental */
void UserCollection::insertKick(string line, int index){

	// Extract nick of user who was kicked
	int i = index;
	if(line[i] == SPACE)
		i--; // Just a bit of correction in case the substring entered was a little sloppy
	while(line[i] != SPACE)
		i--; // Move back through the actual nick
	i++; // Skip the actual whitespace character

	string kickee;
	while(line[i] != SPACE){
		kickee.push_back(line[i]);
		i++;
	}

	User* kickeeUser = this->getOrCreateUserByNick(kickee);

	kickeeUser->incrementCount(KEY_ATTR_KICKSRECEIVED);
	kickeeUser->setIn(false);

	// Now look for the kicker; go back to the index
	i = index;

	// Skip " was kicked from " or equivalent
	i += (Config::getString(KEY_CONF_INFO_KICK)).length();

	// Skip channel name; we can't hard-code this in because we might not know the channel name before we encounter such a line
	while(line[i] != SPACE)
		i++;

	// Skip " by " or equivalent
	i += (Config::getString(KEY_CONF_INFO_CHANGEBY)).length();

	string kicker;
	while(line[i] != SPACE){
		kicker.push_back(line[i]);
		i++;
	}

	User* kickerUser = this->getOrCreateUserByNick(kicker);

	kickerUser->incrementCount(KEY_ATTR_KICKSPERFORMED);
	kickerUser->setIn(true);
}

/* Insert a mode change accidental
 * index should be the index of the last character in the string mode/#<channel> */
void UserCollection::insertModeChange(string line, int index){
	
	int bound = line.length();
	int i = bound-1;
	while(line[i] != SPACE)
		i--;
	i++; // Skip the space

	string changer;
	while(line[i] != SPACE && i < bound){
		changer.push_back(line[i]);
		i++;
	}

	User* changerUser = this->getOrCreateUserByNick(changer);

	bool polarities[Config::getInt(KEY_CONF_MODE_MAX_CHANGES)];

	bool positive = false;
	int j = 0;
	
	i = index + 2; // Skip the string " [ "// FIXME: BRITTLE!!!
	while(line[i] != SPACE){
		if(line[i] == '+'){
			positive = true;
		}
		else if(line[i] == '-'){
			positive = false;
		}
		else if(line[i] >= 'a' && line[i] <= 'z'){
			polarities[j] = positive;
			if(polarities[j])
				changerUser->incrementModePlus(line[i]);
			else
				changerUser->incrementModeMinus(line[i]);
			j++;
		}

		i++;
	}
}

/* Step performed after all word statistics compiled, but before
 * outputting them, to finalise subjective things like dictatoryness */
void UserCollection::finaliseAttributes(){
	int dictator_indicator_value = Config::getInt(KEY_CONF_DICTATOR_INDICATOR_VALUE);
	int dictator_suggestor_value = Config::getInt(KEY_CONF_DICTATOR_SUGGESTOR_VALUE);
	int house_indicator_value = Config::getInt(KEY_CONF_HOUSE_INDICATOR_VALUE);
	int house_suggestor_value = Config::getInt(KEY_CONF_HOUSE_SUGGESTOR_VALUE);

	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		User* u = it->second;
		
		u->addToCount(KEY_ATTR_ACTIVITY, u->getCount(KEY_ATTR_JOIN));
		u->addToCount(KEY_ATTR_ACTIVITY, u->getCount(KEY_ATTR_PART));
		u->addToCount(KEY_ATTR_ACTIVITY, u->getCount(KEY_ATTR_QUIT));
		u->addToCount(KEY_ATTR_ACTIVITY, u->getCount(KEY_ATTR_KILL));
		u->addToCount(KEY_ATTR_ACTIVITY, u->getCount(KEY_ATTR_KICKSPERFORMED));
		u->addToCount(KEY_ATTR_ACTIVITY, u->getAllModemapValues());
		
		u->addToCount(KEY_ATTR_DICTATOR, (u->getModePlus(KEY_MODE_BAN) + u->getModePlus(KEY_MODE_MODERATE) + u->getModePlus(KEY_MODE_TOPICLOCK))*dictator_indicator_value);
		u->addToCount(KEY_ATTR_DICTATOR, u->getModeMinus(KEY_MODE_VOICE)*dictator_suggestor_value);
		u->addToCount(KEY_ATTR_DICTATOR, u->getModeMinus(KEY_MODE_OP)*dictator_suggestor_value);
		u->addToCount(KEY_ATTR_DICTATOR, u->getModePlus(KEY_MODE_VOICE)*(-dictator_suggestor_value));
		u->addToCount(KEY_ATTR_DICTATOR, u->getModePlus(KEY_MODE_OP)*(-dictator_suggestor_value));
		u->addToCount(KEY_ATTR_DICTATOR, (u->getModeMinus(KEY_MODE_BAN) + u->getModeMinus(KEY_MODE_MODERATE) + u->getModeMinus(KEY_MODE_TOPICLOCK))*dictator_suggestor_value);
		u->addToCount(KEY_ATTR_DICTATOR, u->getCount(KEY_ATTR_KICKSPERFORMED)*dictator_indicator_value);
		
		u->addToCount(KEY_ATTR_GRYFFINDOR, u->getCount(KEY_ATTR_KICKSRECEIVED)*house_suggestor_value);
		u->addToCount(KEY_ATTR_HUFFLEPUFF, u->getCount(KEY_ATTR_KICKSRECEIVED)*house_suggestor_value);
		u->addToCount(KEY_ATTR_RAVENCLAW, u->getCount(KEY_ATTR_KICKSRECEIVED)*house_suggestor_value);
		u->addToCount(KEY_ATTR_SLYTHERIN, u->getCount(KEY_ATTR_KICKSRECEIVED)*(-house_suggestor_value));
		
		u->addToCount(KEY_ATTR_GRYFFINDOR, u->getCount(KEY_ATTR_KICKSPERFORMED)*(-house_suggestor_value));
		u->addToCount(KEY_ATTR_HUFFLEPUFF, u->getCount(KEY_ATTR_KICKSPERFORMED)*(-house_indicator_value));
		u->addToCount(KEY_ATTR_RAVENCLAW, u->getCount(KEY_ATTR_KICKSPERFORMED)*(-house_suggestor_value));
		u->addToCount(KEY_ATTR_SLYTHERIN, u->getCount(KEY_ATTR_KICKSPERFORMED)*house_indicator_value);
		
		countmap utokens = u->getTokens();
		
		for(countmap::iterator jt = utokens.begin(); jt != utokens.end(); jt++){
			if(this->cats->isInCategory(KEY_CAT_SMILIES_HAPPY, jt->first) ||
				this->cats->isInCategory(KEY_CAT_SMILIES_VHAPPY, jt->first))
				u->addToCount(KEY_ATTR_HAPPY, jt->second);
			if(this->cats->isInCategory(KEY_CAT_SMILIES_SAD, jt->first) ||
				this->cats->isInCategory(KEY_CAT_SMILIES_VSAD, jt->first))
				u->addToCount(KEY_ATTR_SAD, jt->second);

			if(this->cats->isInCategory(KEY_CAT_SMILIES_GRIM, jt->first))
				u->addToCount(KEY_ATTR_GRIM, jt->second);
			if(this->cats->isInCategory(KEY_CAT_SMILIES_SARCASTIC, jt->first))
				u->addToCount(KEY_ATTR_SARCASTIC, jt->second);
			if(this->cats->isInCategory(KEY_CAT_SMILIES_SURPRISED, jt->first))
				u->addToCount(KEY_ATTR_SURPRISED, jt->second);
			if(this->cats->isInCategory(KEY_CAT_SMILIES_ANGRY, jt->first))
				u->addToCount(KEY_ATTR_ANGRY, jt->second);
			if(this->cats->isInCategory(KEY_CAT_SMILIES_EVIL, jt->first))
				u->addToCount(KEY_ATTR_EVIL, jt->second);
			if(this->cats->isInCategory(KEY_CAT_SMILIES_SLEAZY, jt->first))
				u->addToCount(KEY_ATTR_SLEAZY, jt->second);
			if(this->cats->isInCategory(KEY_CAT_PROFANITIES, jt->first))
				u->addToCount(KEY_ATTR_PROFANITY, jt->second);

			if(this->cats->isInCategoryIgnoreCase(KEY_CAT_INDICATORS_DICTATOR, jt->first))
				u->addToCount(KEY_ATTR_DICTATOR, (jt->second)*dictator_indicator_value);
			else if(this->cats->isInCategoryIgnoreCase(KEY_CAT_SUGGESTORS_DICTATOR, jt->first))
				u->addToCount(KEY_ATTR_DICTATOR, (jt->second)*dictator_suggestor_value);

			if(this->cats->isInCategoryIgnoreCase(KEY_CAT_INDICATORS_GRYFFINDOR, jt->first))
				u->addToCount(KEY_ATTR_GRYFFINDOR, (jt->second)*house_indicator_value);
			else if(this->cats->isInCategoryIgnoreCase(KEY_CAT_SUGGESTORS_GRYFFINDOR, jt->first))
				u->addToCount(KEY_ATTR_GRYFFINDOR, (jt->second)*house_suggestor_value);
			else if(this->cats->isInCategoryIgnoreCase(KEY_CAT_INDICATORS_HUFFLEPUFF, jt->first))
				u->addToCount(KEY_ATTR_HUFFLEPUFF, (jt->second)*house_indicator_value);
			else if(this->cats->isInCategoryIgnoreCase(KEY_CAT_SUGGESTORS_HUFFLEPUFF, jt->first))
				u->addToCount(KEY_ATTR_HUFFLEPUFF, (jt->second)*house_suggestor_value);

			else if(this->cats->isInCategoryIgnoreCase(KEY_CAT_INDICATORS_RAVENCLAW, jt->first))
				u->addToCount(KEY_ATTR_RAVENCLAW, (jt->second)*house_indicator_value);
			else if(this->cats->isInCategoryIgnoreCase(KEY_CAT_SUGGESTORS_RAVENCLAW, jt->first))
				u->addToCount(KEY_ATTR_RAVENCLAW, (jt->second)*house_suggestor_value);
			else if(this->cats->isInCategoryIgnoreCase(KEY_CAT_INDICATORS_SLYTHERIN, jt->first))
				u->addToCount(KEY_ATTR_SLYTHERIN, (jt->second)*house_indicator_value);
			else if(this->cats->isInCategoryIgnoreCase(KEY_CAT_SUGGESTORS_SLYTHERIN, jt->first))
				u->addToCount(KEY_ATTR_SLYTHERIN, (jt->second)*house_suggestor_value);
		}
	}
}

/* Count the number of users in the collection */
int UserCollection::countUsers(){
	return this->users.size();
}

/* Attempt to merge all users in nick list into user with first nick
 * If user by zeroth doesn't exist nothing happens
 * If user by any subsequent nick doesn't exist it is skipped */
void UserCollection::mergeUsersByNick(vector<string> nicks){

	if(nicks.size() < 2)
		return; // Just in case

	User* primary = getUserByNick(nicks[0]);

	if(!primary)
		return; // No point wasting any more time on this, there is no user by this nick

	for(int i = 1; i < nicks.size(); i++){
		User* secondary = this->getUserByNick(nicks[i]);
		if((secondary) && (secondary != primary)){
			mergeUsers(primary, secondary);
			this->deleteByValue(secondary);
		}
	}
}

/* Return the user with the most absolute value of a certain attribute key */
User* UserCollection::getMostCount(string key){

	User* highest = NULL;

	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		if(highest){
			if(it->second->getCount(key) > highest->getCount(key))
				highest = it->second;
		}
		else{
			if(it->second->getCount(key) > 0)
				highest = it->second;
		}
	}

	return highest;
}

/* Return the user with the most of a certain attribute key scaled by their wordcount */
User* UserCollection::getMostRateByWordCount(string key){

	User* highest = NULL;

	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		if(highest){
			if(it->second->getRateByWordCount(key) > highest->getRateByWordCount(key))
				highest = it->second;
		}
		else{
			if(it->second->getRateByWordCount(key) > 0)
				highest = it->second;
		}
	}

	return highest;
}

void UserCollection::printUsers(ofstream& results){
	for(usermap::iterator it = this->users.begin(); it != this->users.end(); it++){
		cout << "key: " << it->first << "\tvalue: " << std::hex << it->second << std::dec;
		cout << "\tcurrent nick: " << it->second->getNick() << endl;
	}
}

void UserCollection::writeServers(ofstream& results){
	UserCollection::Writer ucw;
	ucw.writeServers(this, results);
}

void UserCollection::writeLineTotals(ofstream& results){
	UserCollection::Writer ucw;
	ucw.writeLineTotals(this, results);
}

void UserCollection::writeWordTotals(ofstream& results, list<string> ordered){
	UserCollection::Writer ucw;
	ucw.writeWordTotals(this, results, ordered);
}

void UserCollection::writeCAG(ofstream& results){
	UserCollection::Writer ucw;
	ucw.writeCAG(this, results);
}

void UserCollection::writeModes(ofstream& results){
	UserCollection::Writer ucw;
	ucw.writeModes(this, results);
}

void UserCollection::writeEmotions(ofstream& results){
	UserCollection::Writer ucw;
	ucw.writeEmotions(this, results);
}

void UserCollection::writeFeatures(ofstream& results){
	UserCollection::Writer ucw;
	ucw.writeFeatures(this, results);
}
