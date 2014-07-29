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
	bool isInCategory(std::string category, std::string word);
	bool isInCategoryIgnoreCase(std::string category, std::string word);

	static bool isValidWord(std::string str, std::string word_valid_punctuation);	
	static int compareIgnoreCase(std::string first, std::string second);
	static bool vectorContainsStringIgnoreCase(std::vector<std::string> vect, std::string s);
};

#endif
