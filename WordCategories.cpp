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

/* Determine whether a string denotes "happy" */
bool WordCategories::isHappy(string word){
	set<string> s = this->categories["smilies_happy"];
	if(s.find(word) != s.end())
		return true;
	s = this->categories["smilies_vhappy"];
	return(s.find(word) != s.end());
}

/* Determine whether a string denotes "sad" */
bool WordCategories::isSad(string word){
	set<string> s = this->categories["smilies_sad"];
	if(s.find(word) != s.end())
		return true;
	s = this->categories["smilies_vsad"];
	return(s.find(word) != s.end());
}

/* Determine whether a string denotes "grim" */
bool WordCategories::isGrim(string word){
	set<string> s = this->categories["smilies_grim"];
	return(s.find(word) != s.end());
}

/* Determine whether a string denotes "sarcastic" */
bool WordCategories::isSarcastic(string word){
	set<string> s = this->categories["smilies_sarcastic"];
	return(s.find(word) != s.end());
}

/* Determine whether a string denotes "surprised" */
bool WordCategories::isSurprised(string word){
	set<string> s = this->categories["smilies_surprised"];
	return(s.find(word) != s.end());
}

/* Determine whether a string denotes "angry" */
bool WordCategories::isAngry(string word){
	set<string> s = this->categories["smilies_angry"];
	return(s.find(word) != s.end());
}

/* Determine whether a string denotes "evil" */
bool WordCategories::isEvil(string word){
	set<string> s = this->categories["smilies_evil"];
	return(s.find(word) != s.end());
}

/* Determine whether a string denotes "sleazy" */
bool WordCategories::isSleazy(string word){
	set<string> s = this->categories["smilies_sleazy"];
	return(s.find(word) != s.end());
}

/* Determine whether a string is a profanity */
bool WordCategories::isProfanity(string word){
	set<string> s = this->categories["profanities"];
	return(s.find(word) != s.end());
}

/* Return whether a string is the name of a programming language */
bool WordCategories::isLanguage(string word){
	return containsIgnoreCase(this->categories["languages"], word);
}

/* Return whether a string is one that preceding another would indicate liking that other */
bool WordCategories::isPrecedingLike(string word){
	return containsIgnoreCase(this->categories["like_preceding"], word);
}

/* Return whether a string is one that succeeding another would indicate liking that other */
bool WordCategories::isSucceedingLike(string word){
	return containsIgnoreCase(this->categories["like_succeeding"], word);
}

/* Return whether a string is an adjective indicating liking */
bool WordCategories::isPredicatedLike(string word){
	return containsIgnoreCase(this->categories["like_predicated"], word);
}

/* Return whether a string is one that preceding another would indicate disliking that other */
bool WordCategories::isPrecedingHate(string word){
	return containsIgnoreCase(this->categories["hate_preceding"], word);
}

/* Return whether a string is one that succeeding another would indicate disliking that other */
bool WordCategories::isSucceedingHate(string word){
	return containsIgnoreCase(this->categories["hate_succeeding"], word);
}

/* Return whether a string is an adjective indicating hating */
bool WordCategories::isPredicatedHate(string word){
	return containsIgnoreCase(this->categories["hate_predicated"], word);
}

/* Return whether a string is a confirmer of the following statement */
bool WordCategories::isConfirmer(string word){
	return containsIgnoreCase(this->categories["confirmers"], word);
}

/* Return whether a string is a denier of the following statement */
bool WordCategories::isDenier(string word){
	return containsIgnoreCase(this->categories["deniers"], word);
}

/* Return whether a string is a copula */
bool WordCategories::isCopula(string word){
	return containsIgnoreCase(this->categories["copula"], word);
}

/* Return whether a string is indicative of someone who is a dictator */
bool WordCategories::isDictatorIndicator(string word){
	return containsIgnoreCase(this->categories["indicators_dictator"], word);
}

/* Return whether a string is suggestive of someone who is a dictator */
bool WordCategories::isDictatorSuggestor(string word){
	return containsIgnoreCase(this->categories["suggestors_dictator"], word);
}

/* Return whether a string is indicative of someone in Gryffindor */
bool WordCategories::isGryffindorIndicator(string word){
	return containsIgnoreCase(this->categories["indicators_gryffindor"], word);
}

/* Return whether a string is indicative of someone in Hufflepuff */
bool WordCategories::isHufflepuffIndicator(string word){
	return containsIgnoreCase(this->categories["indicators_hufflepuff"], word);
}

/* Return whether a string is indicative of someone in Ravenclaw */
bool WordCategories::isRavenclawIndicator(string word){
	return containsIgnoreCase(this->categories["indicators_ravenclaw"], word);
}

/* Return whether a string is indicative of someone in Slytherin */
bool WordCategories::isSlytherinIndicator(string word){
	return containsIgnoreCase(this->categories["indicators_slytherin"], word);
}

/* Return whether a string is suggestive of someone in Gryffindor */
bool WordCategories::isGryffindorSuggestor(string word){
	return containsIgnoreCase(this->categories["suggestors_gryffindor"], word);
}

/* Return whether a string is suggestive of someone in Hufflepuff */
bool WordCategories::isHufflepuffSuggestor(string word){
	return containsIgnoreCase(this->categories["suggestors_hufflepuff"], word);
}

/* Return whether a string is suggestive of someone in Ravenclaw */
bool WordCategories::isRavenclawSuggestor(string word){
	return containsIgnoreCase(this->categories["suggestors_ravenclaw"], word);
}

/* Return whether a string is suggestive of someone in Slytherin */
bool WordCategories::isSlytherinSuggestor(string word){
	return containsIgnoreCase(this->categories["suggestors_slytherin"], word);
}

/* Compare two strings for equality, ignoring case
 * Returns:	-1 if the first string is smaller
 *			 0 if the strings are equal
 *			+1 if the first string is greater
 * Effective only insofar as tolower is effective (i.e. basically only ASCII strings)
 */
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

