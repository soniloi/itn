#include "WordCategories.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::set;
using std::string;
using std::vector;

/* Build word-lists from file */
void WordCategories::parseFile(string filename, successcode &code){
	ifstream filein;
	filein.open(filename.c_str());
	if(!filein){
		cout << "Error opening file: " << (filename) << ", exiting." << endl;
		code = FAIL;
		return;
	}

	string line;
	getline(filein, line);
	while(!filein.eof()){
		vector<string> words = Tokeniser::split(line, '\t');
		if(words.size() >= 2){
			string key = words[0];
			vector<string>::iterator it = words.begin();
			it++; // Skip first element
			while(it != words.end()){
				this->categories[key].insert(*it);
				it++;
			}
		}
		getline(filein, line);
	}

	filein.close();
	code = SUCCEED;
}

/* Check whether two strings match when case is ignored */
bool WordCategories::containsIgnoreCase(set<string> words, string word){
	for(set<string>::iterator it = words.begin(); it != words.end(); it++){
		if(!compareIgnoreCase(word, (*it)))
			return true;
	}
	return false;	
}

/* Return whether a character is a valid letter character to find at the start of a word */
bool WordCategories::isValidInitialLetter(char c){
	return(isalpha(c) || c == '\'');
}

/* Return whether a character is a valid letter character to find within in a word */
bool WordCategories::isValidMiddleLetter(char c, string word_valid_punctuation){
	return(isalpha(c) || word_valid_punctuation.find(c) != string::npos);
}

/* Return whether a character is a valid letter character to find at the end of a word */
bool WordCategories::isValidEndLetter(char c){
	return(isalpha(c) || c == '\'');
}
	
/* Return whether a string (word) consists only of characters that would be valid to find in a word */
bool WordCategories::isValidWord(string str, string word_valid_punctuation){
	int bound = str.length();
	
	if(bound < 1)
		return false;

	if(bound == 1 && !isalpha(str[0]))
		return false;

	if(!isValidInitialLetter(str[0]))
		return false;
	
	for(int i=0 ; i<bound-1 ; i++){
		if(!isValidMiddleLetter(str[i], word_valid_punctuation))
			return false;
	}

	if(!isValidEndLetter(str[bound-1]))
		return false;

	return true;
}

/* Return whether a word is in a particular category */
bool WordCategories::isInCategory(string category, string word){
	set<string> s = this->categories[category];
	return(s.find(word) != s.end());
}

/* Return whether a word is in a particular category */
bool WordCategories::isInCategoryIgnoreCase(string category, string word){
	set<string> s = this->categories[category];
	return containsIgnoreCase(s, word);
}

/* Compare two strings for equality, ignoring case
 * Returns:	-1 if the first string is smaller
 *			 0 if the strings are equal
 *			+1 if the first string is greater
 * Effective only insofar as tolower is effective (i.e. basically only ASCII strings) */
int WordCategories::compareIgnoreCase(string first, string second){
	unsigned int i;
	for(i = 0; first[i] != '\0' && second[i] != '\0'; i++){
		if(tolower(first[i]) < tolower(second[i]))
			return -1;
		if(tolower(first[i]) > tolower(second[i]))
			return 1;
	}

	if(second[i] != '\0')
		return -1;
	if(first[i] != '\0')
		return 1;

	return 0;
}

/* Case-insensitive test of whether a vector of strings contains a particular string */
bool WordCategories::vectorContainsStringIgnoreCase(vector<string> vect, string s){
	for(vector<string>::iterator it = vect.begin(); it != vect.end(); it++){
		if(!compareIgnoreCase((*it), s))
			return true;
	}
	return false;
}

