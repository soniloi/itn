/* This is to parse the accompanying config file */

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include "Definitions.h"

#define ASCII_OFFSET_NUMERIC 0x30
#define ASCII_OFFSET_ALPHA 0x37

typedef std::map<std::string, int> confmapint;
typedef std::map<std::string, char> confmapchar;
typedef std::map<std::string, std::string> confmapstring;

class Config{
private:
	static confmapint config_ints;
	static confmapchar config_chars;
	static confmapstring config_strings;
	static bool strtoint(int &dest, std::string str, int radix);
	static std::map<std::string, int> setDefaultInts(){
		std::map<std::string, int> m;
		m[KEY_CONF_BOT_CUTOFF] = 24;
		m[KEY_CONF_BOT_LINECOUNT_CUTOFF] = 4;
		m[KEY_CONF_BOT_PROXIMITY_0_SECONDS] = 0;
		m[KEY_CONF_BOT_PROXIMITY_0_VALUE] = 14;
		m[KEY_CONF_BOT_PROXIMITY_1_SECONDS] = 1;
		m[KEY_CONF_BOT_PROXIMITY_1_VALUE] = 12;
		m[KEY_CONF_BOT_PROXIMITY_2_SECONDS] = 2;
		m[KEY_CONF_BOT_PROXIMITY_3_SECONDS] = 3;
		m[KEY_CONF_BOT_PROXIMITY_3_VALUE] = 8;
		m[KEY_CONF_BOT_TRIGGER_VALUE] = 16;
		m[KEY_CONF_DECIMAL_PLACES_TO_PRINT] = 2;
		m[KEY_CONF_DICTATOR_INDICATOR_VALUE] = 43;
		m[KEY_CONF_DICTATOR_SPEECH_MINIMUM] = 11;
		m[KEY_CONF_DICTATOR_SPEECH_VALUE] = 1;
		m[KEY_CONF_DICTATOR_SUGGESTOR_VALUE] = 26;
		m[KEY_CONF_HOUSE_INDICATOR_VALUE] = 13;
		m[KEY_CONF_HOUSE_SUGGESTOR_VALUE] = 2;
		m[KEY_CONF_INFO_TOKENS_BEFORE_DATE] = 4;
		m[KEY_CONF_LANG_MIN_FAVOURITE] = 3;
		m[KEY_CONF_LANG_MODIFIER_TOPUP] = 21;
		m[KEY_CONF_MODE_MAX_CHANGES] = 4;
		m[KEY_CONF_NICK_HASCLOSE] = 1;
		m[KEY_CONF_NICK_HASOPEN] = 1;
		m[KEY_CONF_NICK_LENGTH_MAX] = 16;
		m[KEY_CONF_OUT_WORDS_PER_LINE] = 15;
		m[KEY_CONF_SIZE_LIST_LONG] = 225;
		m[KEY_CONF_SIZE_LIST_MEDIUM] = 24;
		m[KEY_CONF_SIZE_LIST_SHORT] = 12;
		m[KEY_CONF_SIZE_LIST_TOPICS] = 12;
		m[KEY_CONF_TIME_HAS_SECONDS] = 1;
		m[KEY_CONF_TIME_HASCLOSE] = 0;
		m[KEY_CONF_TIME_HASOPEN] = 0;
		m[KEY_CONF_TIME_LENGTH] = 8;
		m[KEY_CONF_WORDCOUNT_CUTOFF] = 2;
		m[KEY_CONF_WORDCOUNT_EXCLUDED] = 550;
		m[KEY_CONF_WORDCOUNT_USERS] = 3;
		return m;
	}

	static std::map<std::string, char> setDefaultChars(){
		std::map<std::string, char> m;
		m[KEY_CONF_FILENAME_PATH_SEPARATOR] = '/';
		m[KEY_CONF_HOSTMASK_CLOSE] = ']';
		m[KEY_CONF_HOSTMASK_OPEN] = '[';
		m[KEY_CONF_MARK_INFO] = '-';
		m[KEY_CONF_MARK_ME] = '*';
		m[KEY_CONF_NICK_CLOSE] = ']';
		m[KEY_CONF_NICK_OPEN] = '[';
		m[KEY_CONF_OUT_CLARIFY_CLOSE] = ')';
		m[KEY_CONF_OUT_CLARIFY_OPEN] = '(';
		m[KEY_CONF_OUT_DATE_SEPARATOR] = '/';
		m[KEY_CONF_OUT_LIST_CLOSE] = '}';
		m[KEY_CONF_OUT_LIST_OPEN] = '{';
		m[KEY_CONF_OUT_TIME_SEPARATOR] = '.';
		m[KEY_CONF_TIME_CLOSE] = '(';
		m[KEY_CONF_TIME_OPEN] = ')';
		m[KEY_CONF_TIME_SEPARATOR_INFO] = ':';
		m[KEY_CONF_TIME_SEPARATOR_OTHER] = '.';
		return m;
	}

	static std::map<std::string, std::string> setDefaultStrings(){
		std::map<std::string, std::string> m;
		m[KEY_CONF_BOT_TRIGGERS] = "!>~^$@£&*#?<";
		m[KEY_CONF_FILENAME_IGNORE] = "ignore.lst";
		m[KEY_CONF_FILENAME_MERGE] = "merge.lst";
		m[KEY_CONF_FILENAME_OUT_DEFAULT] = "results.out";
		m[KEY_CONF_FILENAME_WORDS] = "words.lst";
		m[KEY_CONF_INFO_CHANGEBY] = " by ";
		m[KEY_CONF_INFO_DAYCHANGE] = "Day changed";
		m[KEY_CONF_INFO_JOIN] = " has joined ";
		m[KEY_CONF_INFO_KICK] = " was kicked from ";
		m[KEY_CONF_INFO_KILL] = " [Killed ";
		m[KEY_CONF_INFO_LOG] = "Log ";
		m[KEY_CONF_INFO_MODECHANGE] = " mode/";
		m[KEY_CONF_INFO_NICKCHANGE] = " is now known as ";
		m[KEY_CONF_INFO_PART] = " has left ";
		m[KEY_CONF_INFO_QUIT] = " has quit ";
		m[KEY_CONF_INFO_TOPICCHANGE] = " changed the topic of ";
		m[KEY_CONF_INFO_TOPICCHANGE_TO] = " to: ";
		m[KEY_CONF_NICK_INVALID] = " @%+*";
		m[KEY_CONF_OUT_SECTION_DIVIDER] = "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
		m[KEY_CONF_WORD_VALID_DELIMITERS] = " .,!?;:()[]{}<>~*/";
		m[KEY_CONF_WORD_VALID_PUNCTUATION] = "-'";
		
		// FIXME: Take this out when escape sequences are recognised in Config
		m[KEY_CONF_WORD_VALID_DELIMITERS].append("\t\"\\");

		return m;
	}
	
public:
	static void parseFile(std::string filename);
	static int getInt(std::string key);
	static char getChar(std::string key);
	static std::string getString(std::string key);
};

#endif // CONFIG_H
