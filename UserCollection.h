/* Class containing data structures for maintaining collection of users */

#ifndef USERCOLLECTION_H
#define USERCOLLECTION_H

#include <algorithm>
#include <fstream>
#include <iomanip> // Required for cout output formatting stuff#include <string>
#include <list>
#include <queue>
#include <vector>

#include "Config.h"
#include "Tokeniser.h" // For stringvectorpair
#include "UserComparators.h"
#include "UserPairCharacteristic.h"
#include "UserPairComparators.h"

typedef std::map<std::string, User*> usermap;
typedef std::multimap<std::string, User*> usermultimap;
typedef std::pair<std::string, User*> nickuserpair;

class UserCollection{

private:
	WordCategories * cats; // Pointer to word category structure
	usermap users; // Map from current nick to pointer to User object
	usermultimap mothballs; // A temporary place to push users out to if there is a nick clash

	void amalgamateProperties(User* toUser, User* fromUser);
	void mergeUsers(User* toUser, User* fromUser);
	std::string parseNick(std::string line, int index);
	std::string parseHostmask(std::string line, int index);
	User* getUserFromAccidental(std::string line, int index);
	User* getMostCount(std::string key);
	User* getMostRateByWordCount(std::string key);

	User* getUserByNick(std::string nick);
	User* getUserByHostmask(std::string nick);

	void setKey(User* user, std::string key);
	void removeKey(std::string key);
	void removeKeyByValue(User* value);
	void deleteByKey(std::string key);
	void deleteByValue(User* value);
	User* getOrCreateUserByNick(std::string nick);
	void insertUser(std::string key, User* user);
	void mothball(std::string key, User* user);

	std::set<std::string> ignorednicks;
	std::set<std::pair<std::string, int> > ignoredhostmasks;

	class Writer{
	public:
		void writeServers(UserCollection* uc, std::ofstream& results);
		void writeLineTotals(UserCollection* uc, std::ofstream& results);
		void writeWordTotals(UserCollection* uc, std::ofstream& results, std::list<std::string> ordered);
		void writeCAG(UserCollection* uc, std::ofstream& results);
		void writeModes(UserCollection* uc, std::ofstream& results);
		void writeEmotions(UserCollection* uc, std::ofstream& results);
		void writeFeatures(UserCollection* uc, std::ofstream& results);
	};

public:
	UserCollection(WordCategories * wcats);
	~UserCollection();
	bool containsUser(std::string nick);
	User* createUser(std::string nick, std::string hostmask="");
	User* getUserByKey(std::string key);

	void eraseByNick(std::string nick);
	void eraseByHostmask(std::string hostmask, std::vector<std::string> exceptions);

	void insertLine(std::string nick, stringvectorpair wordsandtokens, std::string type, int botlike, int content);	
	void insertNickChange(std::string line, int index);
	void insertJoin(std::string line, int index);
	void insertLeave(std::string line, int index, std::string type);
	void insertKick(std::string line, int index);
	void insertModeChange(std::string line, int index);

	void unmothballAll();
	void finaliseAttributes();

	int countUsers();
	void printList();
	void mergeUsersByNick(std::vector<std::string> nicks);

	void printMothballs(); // Basically just a debugging tool
	void printUsers(); // Basically just a debugging tool
	void writeServers(std::ofstream& results);
	void writeLineTotals(std::ofstream& results);
	void writeWordTotals(std::ofstream& results, std::list<std::string> ordered);
	void writeCAG(std::ofstream& results);
	void writeModes(std::ofstream& results);
	void writeEmotions(std::ofstream& results);
	void writeFeatures(std::ofstream& results);
};

#endif
