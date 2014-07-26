/* Class representing a user of IRC
 * Typically a user is identified by nick/handle, but other features
 * such as hostmask may be taken into account to prevent duplicates
 */

#ifndef USER_H
#define USER_H

#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include "Definitions.h"
#include "WordCategories.h"
#include "Comparators.h"
#include "Tokeniser.h"

typedef std::map<unsigned char, int> modemap;
typedef std::map<std::string, int> countmap;

class UserCollection;
class StringIntPairComparator;

class User{
private:
	std::string nick; // Current nick
	std::set<std::string> aliases; // Any nick user has even been known as
	countmap tokens; // This user's space-separated tokens mapped to their frequencies
	countmap words; // This user's real words mapped their frequencies
	countmap counts; // Assorted countable
	countmap languagefavourites; // Computer languages this user likes
	std::map<User*, int> referees; // The other Users this User has referenced (highlighted), together with the number of words they have directed at that person
	std::string hostmask;
	bool in;
	
	modemap modePlus;
	modemap modeMinus;
	modemap modeRPlus;
	modemap modeRMinus;

	static int getModemapValues(modemap mm);
	float getPercentageByWordCount(int num);
	float getPercentageByLineCount(int num);
	float getPercentageByTotalCount(int num);

public:
	friend class UserCollection; // Taking this out makes UserCollection::amalgamateProperties *really* hard
	User(std::string n, std::string h="");
	~User();
	int getAllModemapValues();
	void addWord(std::string word);
	void addReferee(User* ref);
	std::string nickToString(int length_max);
	std::string getNick();
	std::string getHostmask();
	std::string getLocalpart();
	void setNick(std::string nicknew);
	void setHostmask(std::string hostmask);
	void addAlias(char* s);
	std::string aliasesToString();

	std::string getMostFavouriteLanguage();
	std::string getLeastFavouriteLanguage();
	std::string getHouse();
	std::string wordsToString(std::list<std::string> exclusions, std::map<std::string, int> sayers);
	int countVocabulary();
	void incrementNickChanges();
	void addLine(stringvectorpair wordsandtokens, std::string type, int content, WordCategories * cats, UserCollection * uc);
	
	char* getRandomQuote();
	void addAction(std::string line, int index, int content);
	double getActionProportion();
	double getProportionOfAllLines();

	int getCount(std::string key);
	void addToCount(std::string key, int amount);
	void incrementCount(std::string key);

	int getModePlus(unsigned char index);
	int getModeMinus(unsigned char index);
	int getModeRPlus(unsigned char index);
	int getModeRMinus(unsigned char index);
	void incrementModePlus(unsigned char index);
	void incrementModeMinus(unsigned char index);
	void incrementModeRPlus(unsigned char index);
	void incrementModeRMinus(unsigned char index);

	bool isIn();
	void setIn(bool b);

	float getRateByWordCount(std::string key);
	float getRateByLineCount(std::string key);
	float getRateByTotalCount(std::string key);
	
	std::map<User*, int> getReferees();
	countmap getTokens();
	countmap getWords();

	void merge(User* that);
};

#endif
