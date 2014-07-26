#include "LineParser.h"

using std::cout;
using std::endl;
using std::map;
using std::string;

map<string, int> LineParser::months = LineParser::makeMap();

/* Extract time from an information line and return as a Time object
 * Note: we must already know that this is definitely a valid info-line,
 *  i.e. one that begins with either "Log " or "Day changed"
 */
Time LineParser::getTimeFromInfoLine(string line, successcode &code){
	int info_tokens_before_date = Config::getInt(KEY_CONF_INFO_TOKENS_BEFORE_DATE);
	string info_log = Config::getString(KEY_CONF_INFO_LOG);
	char time_separator_info = Config::getChar(KEY_CONF_TIME_SEPARATOR_INFO);
	
	unsigned int i = 0;
	for(int j = 0; j < info_tokens_before_date; j++){ // Skip the correct number of tokens before starting to parse date
		while(line[i] != SPACE && line[i] != '\0')
			i++;
		if(line[i] == '\0'){
			cout << endl << " *** Expecting whitespace, but found end-of-line" << endl;
			code = FAIL;
			return NULL;
		}

		while(line[i] == SPACE && line[i] != '\0')
			i++;
		if(line[i] == '\0'){
			cout << endl << " *** Expecting token characters, but found end-of-line" << endl;
			code = FAIL;
			return NULL;
		}
	}

	// i is now pointing to the first character in the month part of the date
	string monthname;
	while(line[i] != SPACE && line[i] != '\0'){
		monthname.push_back(line[i]);
		i++;
	}
	
	if(line[i] == '\0'){
		cout << endl << " *** Expecting whitespace after month, but found end-of-line" << endl;
		code = FAIL;
		return NULL;
	}

	map<string, int>::iterator it = LineParser::months.find(monthname);
	if(it == LineParser::months.end()){
		cout << endl << " *** Unknown month name: " << monthname << endl;
		code = FAIL;
		return NULL;
	}
		
	int month = it->second;

	while(line[i] == SPACE && line[i] != '\0')
		i++; // Skip whitespace between month and day

	if(line[i] == '\0'){
		cout << endl << " *** Expecting day, but found end-of-line" << endl;
		code = FAIL;
		return NULL;
	}	

	int day = 0;
	while(line[i] != SPACE && line[i] != '\0'){
		day *= RADIX_DEFAULT;
		day += line[i] - ASCII_OFFSET_NUMERIC;
		i++;
	}
	
	// FIXME: check that number of days appropriate

	while(line[i] == SPACE && line[i] != '\0')
		i++; // Skip whitespace between day and remainder of timestamp

	if(line[i] == '\0'){
		cout << endl << " *** Expecting whitespace after day, but found end-of-line" << endl;
		code = FAIL;
		return NULL;
	}
	
	// FIXME: check that timestamp is correctly formatted
	
	if(line.find(info_log) != string::npos){ // If of the form "--- Log opened/closed ... "

		int hour = 0;
		while(line[i] != time_separator_info){
			hour *= RADIX_DEFAULT;
			hour += line[i] - ASCII_OFFSET_NUMERIC;
			i++;
		}

		i++; // Skip time delimiter

		int minute = 0;
		while(line[i] != time_separator_info){
			minute *= RADIX_DEFAULT;
			minute += line[i] - ASCII_OFFSET_NUMERIC;
			i++;
		}

		i++; // Skip time delimiter

		int second = 0;
		while(line[i] != SPACE){
			second *= RADIX_DEFAULT;
			second += line[i] - ASCII_OFFSET_NUMERIC;
			i++;
		}

		while(line[i] == SPACE)
			i++; // Skip whitespace before year

		int year = 0;
		while(i < line.length() && line[i] != SPACE){
			year *= RADIX_DEFAULT;
			year += line[i] - ASCII_OFFSET_NUMERIC;
			i++;
		}

		return Time(year, month, (day-1), hour, minute, second);
	}

	// Otherwise, it's of the form "--- Day changed ... "
	int year = 0;
	while(i < line.length() && line[i] != SPACE){
		year *= RADIX_DEFAULT;
		year += line[i] - ASCII_OFFSET_NUMERIC;
		i++;
	}

	return Time(year, month, (day-1));
}

/* Extract time from a timestamped line and return as a Time object
 * Since timestamped lines don't typically include year/month/day info,
 *  we need to pass these in as parameters */
Time LineParser::getTimeFromTimestampedLine(string line, int y, int mo, int d, successcode &code){
	bool time_has_seconds = (bool) Config::getInt(KEY_CONF_TIME_HAS_SECONDS);
	bool time_hasopen = (bool) Config::getInt(KEY_CONF_TIME_HASOPEN);
	bool time_hasclose = (bool) Config::getInt(KEY_CONF_TIME_HASCLOSE);
	char time_open = Config::getChar(KEY_CONF_TIME_OPEN);
	char time_close = Config::getChar(KEY_CONF_TIME_CLOSE);
	char time_separator = Config::getChar(KEY_CONF_TIME_SEPARATOR_OTHER);

	unsigned int i = 0;
	while(line[i] == SPACE && line[i] != '\0')
		i++;
		
	if(line[i] == '\0'){
		cout << endl << " *** Expecting timestamp, but found end-of-line" << endl;
		code = FAIL;
		return NULL;
	}
	
	if(time_hasopen){
		if(line[i] != time_open){
			cout << endl << " *** Expecting timestamp opening mark, but found " << line[i] << endl;
			code = FAIL;
			return NULL;
		}
		i++;
		while(line[i] == SPACE && line[i] != '\0')
			i++;
	}
	
	if(!isdigit(line[i])){
		cout << endl << " *** Expecting first timestamp hour digit, but found " << line[i] << endl;
		code = FAIL;
		return NULL;
	}

	int hour = line[i] - ASCII_OFFSET_NUMERIC;
	i++;
	
	if(!isdigit(line[i])){
		cout << endl << " *** Expecting second timestamp hour digit, but found " << line[i] << endl;
		code = FAIL;
		return NULL;
	}
	
	hour *= RADIX_DEFAULT;
	hour += line[i] - ASCII_OFFSET_NUMERIC;
	
	if(hour >= HOURS_PER_DAY){
		cout << endl << " *** Maximum hours in day: " << HOURS_PER_DAY << ", found: " << hour << endl;
		code = FAIL;
		return NULL;
	}
	
	i++;
	
	if(line[i] != time_separator){
		cout << endl << " *** Execting timestamp hour-minute separator, but found " << line[i] << endl;
		code = FAIL;
		return NULL;
	}
	
	i++;
	
	if(!isdigit(line[i])){
		cout << endl << " *** Expecting first timestamp minute digit, but found " << line[i] << endl;
		code = FAIL;
		return NULL;
	}

	int minute = line[i] - ASCII_OFFSET_NUMERIC;
	i++;
	
	if(!isdigit(line[i])){
		cout << endl << " *** Expecting second timestamp minute digit, but found " << line[i] << endl;
		code = FAIL;
		return NULL;
	}
	
	minute *= RADIX_DEFAULT;
	minute += line[i] - ASCII_OFFSET_NUMERIC;

	if(minute >= MINUTES_PER_HOUR){
		cout << endl << " *** Maximum minutes in hour: " << MINUTES_PER_HOUR << ", found: " << minute << endl;
		code = FAIL;
		return NULL;
	}

	i++;
	
	int second = 0;
	if(time_has_seconds){
		if(line[i] != time_separator){
			cout << endl << " *** Execting timestamp minute-second separator, but found " << line[i] << endl;
			code = FAIL;
			return NULL;
		}
		
		i++;
		
		if(!isdigit(line[i])){
			cout << endl << " *** Expecting first timestamp second digit, but found " << line[i] << endl;
			code = FAIL;
			return NULL;
		}

		second = line[i] - ASCII_OFFSET_NUMERIC;
		i++;
		
		if(!isdigit(line[i])){
			cout << endl << " *** Expecting second timestamp second digit, but found " << line[i] << endl;
			code = FAIL;
			return NULL;
		}
		
		second *= RADIX_DEFAULT;
		second += line[i] - ASCII_OFFSET_NUMERIC;

		if(second >= SECONDS_PER_MINUTE){
			cout << endl << " *** Maximum seconds in minute: " << SECONDS_PER_MINUTE << ", found: " << second << endl;
			code = FAIL;
			return NULL;
		}

		i++;
	}

	if(time_hasclose){
		if(line[i] != time_close){
			cout << endl << " *** Expecting timestamp closing mark, but found " << line[i] << endl;
			code = FAIL;
			return NULL;
		}
		i++;
	}
	
	else{ // If there is no closer, we expect space(s) after the timestamp; otherwise there might be more digits or something else erroneous
		if(line[i] != SPACE){
			cout << endl << " *** Expecting whitespace after timestamp, but found " << line[i] << endl;
		}
	}
 
 	return Time(y, mo, d, hour, minute, second);
}

/* Determine what kind of accidental this line is, returning type and index in line */
accidental LineParser::findAccidentalType(string line, int index){
	accidental acc;
	acc.type = UNDEF;
	acc.index = 0;

	int loc;

	 // Topic changes; these must go first, as any text could be in the topic
	loc = line.find(Config::getString(KEY_CONF_INFO_TOPICCHANGE));
	if(loc >= 0){
		acc.type = TOPICCHANGE;
		acc.index = loc;
		return acc;
	}

	// Nick changes
	loc = line.find(Config::getString(KEY_CONF_INFO_NICKCHANGE));
	if(loc >= 0){
		acc.type = NICKCHANGE;
		acc.index = loc;
		return acc;
	}

	// Joins
	loc = line.find(Config::getString(KEY_CONF_INFO_JOIN));
	if(loc >= 0){
		acc.type = JOIN;
		acc.index = loc;
		return acc;
	}

	// Parts
	loc = line.find(Config::getString(KEY_CONF_INFO_PART));
	if(loc >= 0){
		acc.type = PART;
		acc.index = loc;
		return acc;
	}

	// Quits
	loc = line.find(Config::getString(KEY_CONF_INFO_QUIT));
	if(loc >= 0){

		// Quits - kills
		int killoc = line.find(Config::getString(KEY_CONF_INFO_KILL));
  		if(killoc >= 0){
				acc.type = KILL;
				acc.index = loc;
				return acc;
	  	}

		// Quits - other
		else{
			acc.type = QUIT;
			acc.index = loc;
			return acc;
		}
	}

	// Kicks
	loc = line.find(Config::getString(KEY_CONF_INFO_KICK));
	if(loc >= 0){
		acc.type = KICK;
		acc.index = loc;
		return acc;
	}

	// Mode changes
	loc = line.find(Config::getString(KEY_CONF_INFO_MODECHANGE));
	if(loc >= 0){

		// Skip " -!- mode" or equivalent
		loc += (Config::getString(KEY_CONF_INFO_MODECHANGE)).length();

		// Skip the channel name; remember we cannot hard-code this length in as we may not know the channel name by the time we encounter a mode change
		while(line[loc] != SPACE)
			loc++;

		acc.type = MODECHANGE;
		acc.index = loc;
		return acc;
	}

	 // Something I have not yet accounted for, or perhaps couldn't be bothered with
	//cout << "Unrecognised accidental: " << line << endl;
		return acc;
}

/* Split a /me line into the nick and the rest of the content */
nickline LineParser::splitMeLine(string line, int index, successcode &code){
	string invalid = Config::getString(KEY_CONF_NICK_INVALID);
	
	nickline result;
	
	string nick;
	int i = index;
	while(line[i] != '\0' && invalid.find(line[i]) != string::npos){
		i++;
	}
	
	if(line[i] == '\0'){
		cout << endl << endl << " *** Expecting nick, but found end-of-line" << endl;
		code = FAIL;
		return result;
	}
	
	while(invalid.find(line[i]) == string::npos){
		nick.push_back(line[i]);
		i++;
	}
	
	while(line[i] == SPACE)
		i++;
	
	result.nick = nick;
	result.rest = line.substr(i);
	return result;
}

/* Split a comment line into the nick and the rest of the content */
nickline LineParser::splitCommentLine(string line, int index, successcode &code){
	int nick_hasopen = Config::getInt(KEY_CONF_NICK_HASOPEN);
	int nick_hasclose = Config::getInt(KEY_CONF_NICK_HASCLOSE);
	char nick_open = Config::getChar(KEY_CONF_NICK_OPEN);
	char nick_close = Config::getChar(KEY_CONF_NICK_CLOSE);
	unsigned int nick_length_max = (unsigned int) Config::getInt(KEY_CONF_NICK_LENGTH_MAX);
	string invalid = Config::getString(KEY_CONF_NICK_INVALID);

	nickline result;
	
	string nick;
	int i = index;

	if(nick_hasopen){ // If there is a special character denoting the start of a nick
		while(line[i] != nick_open && line[i] != '\0')
			i++; // Skip any preceding invalid characters
		if(line[i] == '\0'){
			cout << endl << endl << " *** Expected nick opening symbol '" << nick_open;
			cout << "\' but found end-of-line" << endl;
			code = FAIL;
			return result;
		}
		i++; // Skip the special character itself
	}

	while(line[i] != '\0' && invalid.find(line[i]) != string::npos) // Skip any invalid characters after the symbol (if it exists)
		i++;
			
	if(line[i] == '\0'){
		cout << endl << endl << " *** Expected nick, but found end-of-line" << endl;
		code = FAIL;
		return result;
	}

	while(invalid.find(line[i]) == string::npos){
		nick.push_back(line[i]);
		i++;
	}
	
	if(nick_hasclose){
		char closer = nick[nick.length()-1];
		if(closer != nick_close){
			cout << endl << endl << " *** Character '" << closer;
			cout << "' found at end of nick did not match expected nick closer '";
			cout << nick_close << '\'' << endl;
			code = FAIL;
			return result;
		}		
		nick.erase(nick.length()-1); // Delete closing symbol, if it exists
	}
	
	if(nick.empty()){
		cout << endl << endl << " *** Empty nick" << endl;
		code = FAIL;
		return result;
	}
	
	if(nick.length() > nick_length_max){
		cout << endl << endl << " *** Nick found greater than maximum length specified" << endl;
		code = FAIL;
		return result;
	}

	while(line[i] != SPACE)
		i++; // Skip any junk after end of nick (sometimes there are op symbols or whatever there)

	while(line[i] == SPACE)
		i++; // Skip any whitespace after end of nick
	
	result.nick = nick;
	result.rest = line.substr(i);
	return result;
}

/* Calculate how botlike a line is based on itself and the line previous to it */
int LineParser::calculateBotcount(activitylineinfo current, activitylineinfo previous){
	bool time_has_seconds = (bool) Config::getInt(KEY_CONF_TIME_HAS_SECONDS);
	string bot_triggers = Config::getString(KEY_CONF_BOT_TRIGGERS);
	int bot_trigger_value = Config::getInt(KEY_CONF_BOT_TRIGGER_VALUE);
	
	int botlike = 0;

	if(time_has_seconds){
		int bot_proximity_0_seconds = Config::getInt(KEY_CONF_BOT_PROXIMITY_0_SECONDS);
		int bot_proximity_1_seconds = Config::getInt(KEY_CONF_BOT_PROXIMITY_1_SECONDS);
		int bot_proximity_2_seconds = Config::getInt(KEY_CONF_BOT_PROXIMITY_2_SECONDS);
		int bot_proximity_3_seconds = Config::getInt(KEY_CONF_BOT_PROXIMITY_3_SECONDS);
		int bot_proximity_0_value = Config::getInt(KEY_CONF_BOT_PROXIMITY_0_VALUE);
		int bot_proximity_1_value = Config::getInt(KEY_CONF_BOT_PROXIMITY_1_VALUE);
		int bot_proximity_2_value = Config::getInt(KEY_CONF_BOT_PROXIMITY_2_VALUE);
		int bot_proximity_3_value = Config::getInt(KEY_CONF_BOT_PROXIMITY_3_VALUE);	
		
		long long bottime = current.time.getDifferenceInSeconds(previous.time);
		
		if(bottime <= bot_proximity_0_seconds)
			botlike += bot_proximity_0_value;
		else if(bottime <= bot_proximity_1_seconds)
			botlike += bot_proximity_1_value;
		else if(bottime <= bot_proximity_2_seconds)
			botlike += bot_proximity_2_value;
		else if(bottime <= bot_proximity_3_seconds)
			botlike += bot_proximity_3_value;
	}

	if(bot_triggers.find(previous.initial) != string::npos)
		botlike += bot_trigger_value; // A line immediately following one that contained a bot-trigger is a dead giveaway

	return botlike;
}
