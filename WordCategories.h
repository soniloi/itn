/* Class for categorising different types of words,
 *  and for performing assorted string-related operations
 */
#ifndef WORDCATEGORIES_H
#define WORDCATEGORIES_H

#include <map>
#include <string>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "Definitions.h"
#include "Tokeniser.h"

class WordCategories{

private:
	static bool containsIgnoreCase(std::set<std::string> words, std::string word);
	std::map<std::string, std::set<std::string> > categories;
	
	static bool isValidInitialLetter(char c);
	static bool isValidMiddleLetter(char c, std::string word_valid_punctuation);
	static bool isValidEndLetter(char c);

public:
	void parseFile(std::string filename, successcode &code);
	bool isHappy(std::string word);
	bool isSad(std::string word);
	bool isGrim(std::string word);
	bool isSarcastic(std::string word);
	bool isSurprised(std::string word);
	bool isAngry(std::string word);
	bool isEvil(std::string word);
	bool isSleazy(std::string word);
	bool isProfanity(std::string word);
	bool isLanguage(std::string word);
	bool isPrecedingLike(std::string word);
	bool isSucceedingLike(std::string word);
	bool isPrecedingHate(std::string word);
	bool isSucceedingHate(std::string word);
	bool isPredicatedLike(std::string word);
	bool isPredicatedHate(std::string word);
	bool isConfirmer(std::string word);
	bool isDenier(std::string word);
	bool isCopula(std::string word);
	bool isDictatorIndicator(std::string word);
	bool isDictatorSuggestor(std::string word);
	bool isGryffindorIndicator(std::string word);
	bool isHufflepuffIndicator(std::string word);
	bool isRavenclawIndicator(std::string word);
	bool isSlytherinIndicator(std::string word);
	bool isGryffindorSuggestor(std::string word);
	bool isHufflepuffSuggestor(std::string word);
	bool isRavenclawSuggestor(std::string word);
	bool isSlytherinSuggestor(std::string word);
	
	static bool isValidWord(std::string str, std::string word_valid_punctuation);	

	static int compareIgnoreCase(std::string first, std::string second);
	static bool vectorContainsStringIgnoreCase(std::vector<std::string> vect, std::string s);
};

#endif
