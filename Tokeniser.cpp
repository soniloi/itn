#include "Tokeniser.h"

using std::pair;
using std::string;
using std::vector;

/* Split a string according to some delimiting character
 * Note that this will NOT return empty tokens, but will count ANY 
 * NUMBER of the specified delimiter to be a single delimiter */
vector<string> Tokeniser::split(string str, char delim){
	vector<string> result;

	int bound = str.length();
	int i = 0;
	string current;
	while(str[i] == delim)
		i++; // Ignore any leading delimiters
	while(i < bound){
		while(str[i] != delim && i != bound){
			current += str[i];
			i++;
		}
		result.push_back(current);
		current.erase(current.begin(), current.end());
		while(str[i] == delim)
			i++; // Treat any number of delimiter as a single delimiter
	}

	return result;
}

/* Tokenise a line into a pair of vectors, one split on
 * punctuation+whitespace, the other on whitespace only */
stringvectorpair Tokeniser::doubletokenise(string input, string punc){

	vector<string> punctuated;
	vector<string> spaced;
	
	string currentpunctuated;
	string currentspaced;

	int bound = input.length();

	int i = 0;
	while(input[i] == ' ')
		i++; // ignore leading whitespace
	while(i < bound){

		if(input[i] == ' '){
			if(!currentpunctuated.empty()){
				punctuated.push_back(currentpunctuated);
				currentpunctuated.clear();
			}
			if(!currentspaced.empty()){
				spaced.push_back(currentspaced);
				currentspaced.clear();
			}
		}
		else if(punc.find(input[i]) != string::npos){
			if(!currentpunctuated.empty()){
				punctuated.push_back(currentpunctuated);
				currentpunctuated.clear();
			}
			currentspaced.push_back(input[i]);
		}
		else{
			currentpunctuated.push_back(tolower(input[i]));
			currentspaced.push_back(input[i]);
		}
		i++;
	}

	if(!currentpunctuated.empty()){
		punctuated.push_back(currentpunctuated);
		currentpunctuated.clear();
	}
	if(!currentspaced.empty()){
		spaced.push_back(currentspaced);
		currentspaced.clear();
	}
	
	return stringvectorpair(punctuated, spaced);
}
