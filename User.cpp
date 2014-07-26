#include "User.h"
#include "UserCollection.h"

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::map;
using std::pair;
using std::priority_queue;
using std::set;
using std::string;
using std::vector;

/* Constructor */
User::User(string n, string h){
	this->hostmask = h;
	this->in = true; // By default, new user is said to be in the channel; the number of situations where this will not be true is miniscule
	this->nick = n;
	this->aliases.insert(n);
}

/* Destructor */
User::~User(){
	//cout << "\tDestroying user: " << this->getNick() << " address " << std::hex << this << std::dec << endl;
}

/* Get the sum of all values in a modemap */
int User::getModemapValues(modemap mm){
	int result = 0;
	for(modemap::iterator it = mm.begin(); it != mm.end(); it++){
		result += it->second;
	}
	return result;
} 

/* Sum all values of all modemaps for this user */
int User::getAllModemapValues(){
	int result = 0;
	result += User::getModemapValues(this->modePlus);
	result += User::getModemapValues(this->modeMinus);
	result += User::getModemapValues(this->modeRPlus);
	result += User::getModemapValues(this->modeRMinus);
	return result;
}

/* Add or increment the count of this User's word */
void User::addWord(string word){
	this->words[word]++;
}

/* Add or increment the number of words this User has directed at another */
void User::addReferee(User* ref){
	this->referees[ref]++;
}

void User::addLine(stringvectorpair wordsandtokens, string type, int content, WordCategories * cats, UserCollection * uc){

	int lang_modifier_topup = Config::getInt(KEY_CONF_LANG_MODIFIER_TOPUP);
	int dictator_speech_minimum = Config::getInt(KEY_CONF_DICTATOR_SPEECH_MINIMUM);
	int dictator_speech_value = Config::getInt(KEY_CONF_DICTATOR_SPEECH_VALUE);

	this->in = true;
	this->counts[type]++; // Increment either comment or /me count
	this->counts[KEY_ATTR_LINE]++;

	// Handle the real words
	vector<string> focail = wordsandtokens.first;
	this->counts[KEY_ATTR_WORD] += focail.size();
	
	if(focail.size() >= dictator_speech_minimum)
		this->counts[KEY_ATTR_DICTATOR] += (focail.size() - dictator_speech_minimum) * dictator_speech_value;
	
	for(vector<string>::iterator it = focail.begin(); it != focail.end(); it++){
		if(content & CONTENT_FLAGS_W){
			this->addWord(*it);
			// If this word is the current nick of a user, increment that user's KEY_ATTR_REFERENCED count
			User* refd = uc->getUserByKey(*it);
			if(refd){
				refd->incrementCount(KEY_ATTR_REFERENCED); // Increment that other user's KEY_ATTR_REFERENCED count
				this->addReferee(refd); // Increment this user's count of how many times they referenced the other
			}
		}
	}

	// Handle the tokens
	vector<string> tokens = wordsandtokens.second;

	if(content & CONTENT_FLAGS_W){
		for(vector<string>::iterator it=tokens.begin() ; it!=tokens.end() ; it++){
			this->tokens[(*it)]++;
			
			// Single-token features can be finalised at the end
			// Unlike the single-token features, language relies on context, so we have to do this as we are going along
			string lang(*it);
			transform(lang.begin(), lang.end(), lang.begin(), ::tolower);
			
			if(cats->isLanguage(lang)){

				//cout << endl << line << endl;

				this->languagefavourites[lang]++;

				// Check for context in the word immediately before
				vector<string>::reverse_iterator jt(it);
				if(jt != tokens.rend()){
					
					if(cats->isPrecedingLike((*jt))){
						jt++;
						if(jt != tokens.rend()){ // Check in case there is a preceding modifier
							if(cats->isConfirmer((*jt))) // "I like Prolog", "really like c#"
								this->languagefavourites[lang] += lang_modifier_topup;
							else if(cats->isDenier((*jt))) // "don't like haskell", "never enjoy ruby"
								this->languagefavourites[lang] -= lang_modifier_topup;
							else{
								this->languagefavourites[lang] += (lang_modifier_topup>>1);
							}
						}
						else{
							this->languagefavourites[lang] += (lang_modifier_topup>>1);
						}
					}
					else if(cats->isPrecedingHate((*jt))){
						jt++;
						if(jt != tokens.rend()){ // Check for preceding modifier
							if(cats->isConfirmer((*jt))){ // "I hate Java", "really dislike haskell"
								this->languagefavourites[lang] -= lang_modifier_topup;
				        this->languagefavourites[lang]--; // Offset the one that was already added
							}
							else if(cats->isDenier((*jt))){ // "don't dislike c#" (not exactly a ringing endorsement)
								this->languagefavourites[lang] += (lang_modifier_topup>>1);
							}
							else{
								this->languagefavourites[lang] -= (lang_modifier_topup>>1);
		        		this->languagefavourites[lang]--; // Offset the one that was already added
							}
						}
						else{
							this->languagefavourites[lang] -= (lang_modifier_topup>>1);
						}

					}

				}

				// Check for context in the word immediately after
				vector<string>::iterator kt = it;
				kt++;
				if(kt != tokens.end()){
					
					if(cats->isSucceedingLike((*kt)))
						this->languagefavourites[lang] += (lang_modifier_topup>>1);
					else if(cats->isSucceedingHate((*kt))){
						this->languagefavourites[lang] -= (lang_modifier_topup>>1);
						this->languagefavourites[lang]--; // Offset the one that was already added
					}
					else if(cats->isCopula((*kt))){ // "Prolog is ..."
						kt++;
						if(kt != tokens.end()){
							if(cats->isPredicatedLike((*kt)))
								this->languagefavourites[lang] += (lang_modifier_topup);
							else if(cats->isPredicatedHate((*kt))){
								this->languagefavourites[lang] -= (lang_modifier_topup);
								this->languagefavourites[lang]--; // Offset the one that was already added
							}
						}
					}

				}

			}
		}
	}	
}

/*
 *	Return the count of a particular attribute
 *	key parameter is the name of the attribute
 */
int User::getCount(string key){
	return this->counts[key];
}

/* Add an arbitrary amount to a user property */
void User::addToCount(string key, int amount){
	this->counts[key] += amount;
}

/* Increment count of a user property */
void User::incrementCount(string key){
	this->counts[key]++;
}

/* Return whether user is in channel */
bool User::isIn(){
	return this->in;
}

/* Set whether user is in channel */
void User::setIn(bool b){
	this->in = b;
}

/* Return current nick */
string User::getNick(){
	return this->nick;
}

/* Return localpart of hostmask (the bit before the @ sign)
 * If none has been specified, return empty string
 */
string User::getLocalpart(){
	string result = this->hostmask;
	int pos = result.find("@");
	if(pos != result.npos)
		result.erase(pos); // Erase everything after @ sign
	return result;
}

/* Return hostmask */
string User::getHostmask(){
	return this->hostmask;
}

/* Change the current nick of this user
 * Allocates new memory on the heap, but this is taken care of by the STL list class
 */
void User::setNick(string nicknew){
	this->aliases.insert(this->nick);
	this->nick = nicknew;
}

/* Set hostmask for this user; may never be done */
void User::setHostmask(string hostmask){
	this->hostmask = hostmask;
}

/* Return the nick, formatted to line up evenly */
string User::nickToString(int length_max){
	string result(this->nick);
	for(int i = this->nick.length(); i < length_max; i++)
		result.push_back(SPACE);
	return result;
}

/* Return a string representation of all previous aliases used by this user */
string User::aliasesToString(){
	string result(" ");
	for(set<string>::iterator it = this->aliases.begin(); it != this->aliases.end(); it++){
		result.append(*it);
		result.push_back(SPACE);
	}
	return result;
}

/* Return the user's favourite language */
string User::getMostFavouriteLanguage(){
	int lang_min_favourite = Config::getInt(KEY_CONF_LANG_MIN_FAVOURITE);
	string result;
	int resultcount = 0;
	for(countmap::iterator it = languagefavourites.begin(); it != languagefavourites.end(); it++){
		if(it->second > resultcount){
			resultcount = it->second;
			result = it->first;
		}
	}
	if(languagefavourites[result] < lang_min_favourite)
		return ""; // To weed out the statistically-insignificant mentions; only do for most favourite because mention is inferred to imply like, not dislike
	return result;
}

/* Return the user's least favourite language */
string User::getLeastFavouriteLanguage(){
	string result;
	int resultcount = 0;
	for(countmap::iterator it = languagefavourites.begin(); it != languagefavourites.end(); it++){
		if(it->second < resultcount){
			resultcount = it->second;
			result = it->first;
		}
	}
	return result;
}

/* Return the user's house */
string User::getHouse(){
	string result;
	int resultcount = 0;
	cout << "user: " << this->getNick();

	int temp;
	temp = this->counts[KEY_ATTR_GRYFFINDOR];
	cout << "\tgryffindor: " << temp;
	if(temp > resultcount){
		resultcount = temp;
		result = KEY_ATTR_GRYFFINDOR;
	}
	temp = this->counts[KEY_ATTR_HUFFLEPUFF];
	cout << "\thufflepuff: " << temp;
	if(temp > resultcount){
		resultcount = temp;
		result = KEY_ATTR_HUFFLEPUFF;
	}
	temp = this->counts[KEY_ATTR_RAVENCLAW];
	cout << "\travenclaw: " << temp;
	if(temp > resultcount){
		resultcount = temp;
		result = KEY_ATTR_RAVENCLAW;
	}
	temp = this->counts[KEY_ATTR_SLYTHERIN];
	cout << "\tslytherin: " << temp;
	if(temp > resultcount){
		resultcount = temp;
		result = KEY_ATTR_SLYTHERIN;
	}
	
	cout << "\treturning: " << result << endl;
	return result;
}

/* Return a string representation of uncommon words most used by this user
 * Takes the user's wordlist and skips over any found in exclusions */
string User::wordsToString(list<string> exclusions, map<string, int> sayers){

	int size_list_short = Config::getInt(KEY_CONF_SIZE_LIST_SHORT);
	int wordcount_cutoff = Config::getInt(KEY_CONF_WORDCOUNT_CUTOFF);
	int wordcount_users = Config::getInt(KEY_CONF_WORDCOUNT_USERS);
	string word_valid_punctuation = Config::getString(KEY_CONF_WORD_VALID_PUNCTUATION);

	string result;

	result += SPACE;

	int i = 0;
	priority_queue<pair<string, int>, vector<pair<string, int> >, StringIntPairComparator> pairqueue;

	for(countmap::iterator it=this->words.begin() ; it!=this->words.end() ; it++){
		if(WordCategories::isValidWord(it->first, word_valid_punctuation))
			pairqueue.push(*(it));
	}

	while(!pairqueue.empty() && pairqueue.top().second >= wordcount_cutoff && i < size_list_short){
		pair<string, int> word = pairqueue.top();

		if((find(exclusions.begin(), exclusions.end(), word.first) == exclusions.end()) || sayers[word.first] < wordcount_users){
			result += word.first + SPACE; // Only include word if it is not in common wordlist
			i++;
		}

		pairqueue.pop();
	}

	return result;
}

/* Return the number of words in this user's vocabulary */
int User::countVocabulary(){
	return this->words.size();
}

int User::getModePlus(unsigned char index){
	return this->modePlus[index];
}

int User::getModeMinus(unsigned char index){
	return this->modeMinus[index];
}

int User::getModeRPlus(unsigned char index){
	return this->modeRPlus[index];
}

int User::getModeRMinus(unsigned char index){
	return this->modeRMinus[index];
}

void User::incrementModePlus(unsigned char index){
	this->modePlus[index]++;
}

void User::incrementModeMinus(unsigned char index){
	this->modeMinus[index]++;
}

void User::incrementModeRPlus(unsigned char index){
	this->modeRPlus[index]++;
}

void User::incrementModeRMinus(unsigned char index){
	this->modeRMinus[index]++;
}

/* Return percentage of words that indicate a certain statistic */
float User::getPercentageByWordCount(int num){
	int wc = this->counts[KEY_ATTR_WORD];
	if(wc == 0)
		return 0.0;
	return 100.0 * (float) num / wc;
}

/* Return percentage of spoken lines that indicate a certain statistic */
float User::getPercentageByLineCount(int num){
	int lc = this->counts[KEY_ATTR_LINE];
	if(lc == 0)
		return 0.0;
	return 100.0 * (float) num / lc;
}

/* Return percentage of combined spoken and activity lines that indicate a certain statistic */
float User::getPercentageByTotalCount(int num){
	int tc = this->counts[KEY_ATTR_LINE] + this->counts[KEY_ATTR_ACTIVITY];
	if(tc == 0)
		return 0.0;
	return 100.0 * (float) num / tc;
}


float User::getRateByWordCount(string key){
	return this->getPercentageByWordCount(this->counts[key]);
}

float User::getRateByLineCount(string key){
	return this->getPercentageByLineCount(this->counts[key]);
}

float User::getRateByTotalCount(string key){
	return this->getPercentageByTotalCount(this->counts[key]);
}


/* Merge another user into this user
 * Will alter this user, but not that user
 */
void User::merge(User* that){
	cout << "\t\tmerging " << this->nick << '(' << this->counts[KEY_ATTR_LINE] << ") {" << this->aliasesToString() << "} " << std::hex << this << std::dec
		<< " from " << that->nick << '(' << that->counts[KEY_ATTR_LINE] << ") {" << that->aliasesToString() << "} " << std::hex << that << std::dec;

	this->aliases.insert(that->getNick()); // FIXME: This shouldn't be necessary, but for some reason is
	for(set<string>::iterator itthat=that->aliases.begin() ; itthat!=that->aliases.end() ; itthat++){
		this->aliases.insert(*itthat); // Push everything from that list to this one
	}

	/* TODO: The following two statements are of questionable continued existence */
	this->hostmask = that->hostmask; // We take it that the new hostmask is more likely to be the more up-to-date
	this->in = true; // This will only have happened as a result of nick changes, for which user must be in-channel
/*
	for(int i=0 ; i<MODE_ALPHABET_SIZE ; i++){
		this->modePlus[i] += that->modePlus[i];
		this->modeMinus[i] += that->modeMinus[i];
		this->modeRPlus[i] += that->modeRPlus[i];
		this->modeRMinus[i] += that->modeRMinus[i];
	}
*/
	for(modemap::iterator it = this->modePlus.begin(); it != this->modePlus.end(); it++){
		if(that->modePlus.find(it->first) != that->modePlus.end())
			this->modePlus[it->first] += that->modePlus[it->first];
	}
	for(modemap::iterator it = that->modePlus.begin(); it != that->modePlus.end(); it++){
		if(this->modePlus.find(it->first) == this->modePlus.end())
			this->modePlus[it->first] = it->second;
	}

	for(modemap::iterator it = this->modeMinus.begin(); it != this->modeMinus.end(); it++){
		if(that->modeMinus.find(it->first) != that->modeMinus.end())
			this->modeMinus[it->first] += that->modeMinus[it->first];
	}
	for(modemap::iterator it = that->modeMinus.begin(); it != that->modeMinus.end(); it++){
		if(this->modeMinus.find(it->first) == this->modeMinus.end())
			this->modeMinus[it->first] = it->second;
	}

	for(modemap::iterator it = this->modeRPlus.begin(); it != this->modeRPlus.end(); it++){
		if(that->modeRPlus.find(it->first) != that->modeRPlus.end())
			this->modeRPlus[it->first] += that->modeRPlus[it->first];
	}
	for(modemap::iterator it = that->modeRPlus.begin(); it != that->modeRPlus.end(); it++){
		if(this->modeRPlus.find(it->first) == this->modeRPlus.end())
			this->modeRPlus[it->first] = it->second;
	}

	for(modemap::iterator it = this->modeRMinus.begin(); it != this->modeRMinus.end(); it++){
		if(that->modeRMinus.find(it->first) != that->modeRMinus.end())
			this->modeRMinus[it->first] += that->modeRMinus[it->first];
	}
	for(modemap::iterator it = that->modeRMinus.begin(); it != that->modeRMinus.end(); it++){
		if(this->modeRMinus.find(it->first) == this->modeRMinus.end())
			this->modeRMinus[it->first] = it->second;
	}
	/*
	 *	Merge the uncommon words tallies
	 *		Keys found in this set but not that set obviously need no further action
	 *		First we look for all keys that are in both this and that map
	 */
	for(countmap::iterator it = this->words.begin() ; it != this->words.end() ; it++){
		if(that->words.find(it->first) != that->words.end()) // If key is in that's uncommon list, add its value tally
			this->words[it->first] += that->words[it->first];
	}

	/* Now search for any keys present in that set but absent from this set */
	for(countmap::iterator it = that->words.begin() ; it != that->words.end() ; it++){
		if(this->words.find(it->first) == this->words.end())
			this->words[it->first] = it->second;
	}

	/* In the same way, merge the language tallies */
	for(countmap::iterator it = this->languagefavourites.begin() ; it != this->languagefavourites.end() ; it++){
		if(that->languagefavourites.find(it->first) != that->languagefavourites.end()) // If key is in that's uncommon list, add its value tally
			this->languagefavourites[it->first] += that->languagefavourites[it->first];
	}

	for(countmap::iterator it = that->languagefavourites.begin() ; it != that->languagefavourites.end() ; it++){
		if(this->languagefavourites.find(it->first) == this->languagefavourites.end())
			this->languagefavourites[it->first] = it->second;
	}

	/* In the same way, merge the counts tallies */
	for(countmap::iterator it = this->counts.begin() ; it != this->counts.end() ; it++){
		if(that->counts.find(it->first) != that->counts.end()) // Add amount of property only if present in both users (if present only in this, does not matter)
			this->counts[it->first] += that->counts[it->first];
	}

	for(countmap::iterator it = that->counts.begin() ; it != that->counts.end() ; it++){
		if(this->counts.find(it->first) == this->counts.end())
			this->counts[it->first] = it->second;
	}

	cout << " ... merged!" << endl;
}

map<User*, int> User::getReferees(){
	return this->referees;
}

countmap User::getTokens(){
	return this->tokens;
}

countmap User::getWords(){
	return this->words;
}
