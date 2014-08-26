#include "FileParser.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::list;
using std::map;
using std::ofstream;
using std::string;
using std::vector;

/* Constructor */
FileParser::FileParser(string filepath, WordCategories * wcats){

	// Guess the channel name based on the input filename
	char separator = Config::getChar(KEY_CONF_FILENAME_PATH_SEPARATOR);
	int i = filepath.length();
	while(i > 0 && filepath[i] != separator)
		i--; // Strip out any filepath, if present

	if(i > 0)
		i++; // Minor error-correction

	int j = i;
	while(filepath[j] != '.' && filepath[j] != '\0') // Strip out any file extension, if present
		j++;

	string channame = filepath.substr(i, (j-i));

	this->users = new UserCollection(wcats);
	this->channel = new ChannelCollection(channame, this->users);
}

/* Destructor */
FileParser::~FileParser(){
	delete this->users;
	delete this->channel;
}

/* Compile statistics from input files */
void FileParser::buildStats(vector<string> filesin, int contentVectors, successcode &code){

	// Load some config stuff
	char mark_info = Config::getChar(KEY_CONF_MARK_INFO);
	char mark_me = Config::getChar(KEY_CONF_MARK_ME);
	string invalid = Config::getString(KEY_CONF_NICK_INVALID); // Characters that may not appear in a nick
	string info_log = Config::getString(KEY_CONF_INFO_LOG);
	string info_daychange = Config::getString(KEY_CONF_INFO_DAYCHANGE);
	string info_topicchange = Config::getString(KEY_CONF_INFO_TOPICCHANGE);
	string info_topicchange_to = Config::getString(KEY_CONF_INFO_TOPICCHANGE_TO);
	string info_nickchange = Config::getString(KEY_CONF_INFO_NICKCHANGE);
	string info_join = Config::getString(KEY_CONF_INFO_JOIN);
	string info_part = Config::getString(KEY_CONF_INFO_PART);
	string info_quit = Config::getString(KEY_CONF_INFO_QUIT);
	string info_kill = Config::getString(KEY_CONF_INFO_KILL);
	string info_kick = Config::getString(KEY_CONF_INFO_KICK);
	string info_modechange = Config::getString(KEY_CONF_INFO_MODECHANGE);
	string info_changeby = Config::getString(KEY_CONF_INFO_CHANGEBY);

	activitylineinfo current;
	current.time = NULL;
	current.nick = "";
	current.initial = '\0';
	
	activitylineinfo previous;
	previous.time = NULL;
	previous.nick = "";
	previous.initial = '\0';
	
	cout << endl << "---> Processing files ..." << endl;

	int filecount = 0;
	int filelines;
	for(vector<string>::iterator it = filesin.begin(); ((code == SUCCEED) && it != filesin.end()); it++){ // Read each of the input files in turn

		ifstream filein;
		filein.open((*it).c_str());
		if(!filein){
			cout << "Error opening file: " << (*it) << ", exiting program without completing." << endl;
			code = FAIL;
			return;
		}

		cout << TAB << (*it) << endl;

		filelines = 1;
		string line;
		getline(filein, line); // Read first line from file
		
		// See if initial timestamp can be gleaned from first line of the first file
		if(filecount == 0 && !filein.eof() && line[0] == mark_info){
			if(line.find(info_log) != string::npos || line.find(info_daychange) != string::npos){ // If line is of the form "--- Log opened/closed ... " or "--- Day changed ... "
				current.time = LineParser::getTimeFromInfoLine(line, code);
				if(code == SUCCEED){
					this->channel->setStarttime(current.time);
					getline(filein, line);
				}
			}
		}
		
		while((code == SUCCEED) && !filein.eof()){
//			cout << line << endl;
//			cout << "\t\tRaw lines in file: " << filelines++ << "  \r"; // Uncommenting this line is NOT a good idea if re-directing stdout to a file

			previous.time = current.time;

			// Date changes, log opens/closes
			if(line[0] == mark_info){
				if(line.find(info_log) != string::npos || line.find(info_daychange) != string::npos) // If line is of the form "--- Log opened/closed ... " or "--- Day changed ... "
					current.time = LineParser::getTimeFromInfoLine(line, code);
				else // Some sort of other line not accounted for
					current.time = Time();
			}

			// Timestamped lines: comments, actions, joins, quits, parts, kicks, ...
			else{
				current.time = LineParser::getTimeFromTimestampedLine(line, current.time.getYear(), current.time.getMonth(), current.time.getDay(), code); // Update the current time

				if(current.time != NULL){
					int i = 0;
					while(line[i] > '9' || line[i] < '0')
						i++;

					int hour = line[i] - ASCII_OFFSET_NUMERIC;
					hour *= RADIX_DEFAULT;
					hour += line[i+1] - ASCII_OFFSET_NUMERIC;

					i = 0;
					while(line[i] != SPACE)
						i++; // Skip the timestamp
					while(line[i] == SPACE)
						i += 1; // Skip all whitespace after timestamp

					int botlike = LineParser::calculateBotcount(current, previous);

					//	Determine what kind of line this is and handle appropriately
					if(line[i] == mark_info){ // Accidental line
						accidental acc = LineParser::findAccidentalType(line, i);
						switch(acc.type){
							case TOPICCHANGE: this->channel->insertTopicChange(line, acc.index, current.time); break;
							case NICKCHANGE: this->users->insertNickChange(line, acc.index); break;
							case JOIN: this->users->insertJoin(line, acc.index); break;
							case PART: this->users->insertLeave(line, acc.index, KEY_ATTR_PART); break;
							case QUIT: this->users->insertLeave(line, acc.index, KEY_ATTR_QUIT); break;
							case KILL: this->users->insertLeave(line, acc.index, KEY_ATTR_KILL); break;
							case KICK: this->users->insertKick(line, acc.index); break;
							case MODECHANGE: this->users->insertModeChange(line, acc.index); break;
							default: break;
						}
					}

					else if(line[i] == '!'){
						// Do nothing, it's some weird server-sync message
					}

					else if(line[i] == mark_me){ // /me line
						this->channel->incrementHour(hour);
						nickline nl = LineParser::splitMeLine(line, i, code);
						if(code == SUCCEED)	
							this->channel->insertLine(nl.nick, nl.rest, KEY_ATTR_ME, botlike, contentVectors);
					}

					else{ // Comment line
						this->channel->incrementHour(hour);
						nickline nl = LineParser::splitCommentLine(line, i, code);					
						if(code == SUCCEED){
							this->channel->insertLine(nl.nick, nl.rest, KEY_ATTR_COMMENT, botlike, contentVectors);

							if(!(nl.rest.empty()))
								previous.initial = nl.rest[0];
							else
								previous.initial = '\0';
						}
					}
				}

			}

			if(code != SUCCEED){
				cout << " * Error: bad config or input file at line ";
				cout << (filelines-1) << " of file " << (*it) << endl;
			}
				
			getline(filein, line);
		}

		filein.close();
		filecount++;

		cout << endl << " ... done!" << endl;
	}
	
	this->channel->setEndtime(current.time);

	cout << "---> " << filecount << " file(s) processed." << endl;
}

/* Perform adjustments on compiled statistics */
void FileParser::adjustStats(string fileignore, string filemerge){

	cout << endl << "---> Performing adjustments ..." << endl;

	//this->users->printUsers(); // Uncomment this to print the user container before adjustments completed
	//this->users->printMothballs(); // Uncomment this to print mothballed users before adjustments completed

	this->users->unmothballAll(); // Merge mothballed users back into master collection

	// Search for users to ignore
	ifstream ignore;
	ignore.open(fileignore.c_str());
	if(!ignore)
		cerr << "Error opening ignore list file, omitting this step." << endl;
	else{

		string line;
		getline(ignore, line);
		while(!ignore.eof()){
			vector<string> ignores = Tokeniser::split(line, TAB);
			if(ignores.size() >= 2){ // Will need to be at least a length of two before there is any point processing it
				if(!ignores[0].compare("hostmask")){ // Search for hostmasks to ignore
					string hostmask = ignores[1];
					ignores.erase(ignores.begin(), ignores.begin()+2); // Remove the first two elements; the remaining elements are exceptions to the ignore
					this->users->eraseByHostmask(hostmask, ignores);
				}
				else if(!ignores[0].compare("nick")){ // Search for nicks to ignore
					string nick = ignores[1];
					this->users->eraseByNick(nick);
				}
			}

			getline(ignore, line);
		}

		ignore.close();
	}

	cout << endl << endl;;

	// Manually merge users, from file
	ifstream merge;
	merge.open(filemerge.c_str());
	if(!merge)
		cerr << "Error opening merge list file, omitting this step." << endl;
	else{
		string line;
		getline(merge, line);
		while(!merge.eof()){
			if(!line.empty() && line[0] != '#'){ // Avoid lines that are 'commented out'
				vector<string> merges = Tokeniser::split(line, TAB);
				if(merges.size() >= 2){ // Will need to be at least a length of two before there is any point processing it
					this->users->mergeUsersByNick(merges);
				}
			}
			getline(merge, line);
		}

		merge.close();
	}
	
	cout << endl << "---> Finalising ..." << endl;

	this->users->finaliseAttributes();

	cout << "---> Adjustments completed." << endl;
}

/* Write requested statistics to file */
void FileParser::writeStats(string fileout, int contentVectors){
	string section_divider = Config::getString(KEY_CONF_OUT_SECTION_DIVIDER);
	int decimal_places_to_print = Config::getInt(KEY_CONF_DECIMAL_PLACES_TO_PRINT);

	ofstream results;

	results.open(fileout.c_str()); // Attempt to open for writing
	if(!results){
		cerr << "Something went wrong opening the output file - exiting program." << endl;
		exit(1);
	}

	cout << endl << "---> Writing requested statistics to file ..." << endl;

	results << std::fixed << std::setprecision(decimal_places_to_print);

	//this->users->printUsers(); // Uncomment this to print the user container after adjustments completed

	if(contentVectors & CONTENT_FLAGS_B){ // (Conditionally) write out basic channel statistics
		results << endl << section_divider << endl << endl;

		this->channel->writeOverview(results); // Write out channel overview
		this->users->writeServers(results); // Write out server statistics

		results << endl << section_divider << endl << endl;

		this->users->writeLineTotals(results); // Write out line total statistics
		results << endl << section_divider << endl << endl;
	}

	if(contentVectors & CONTENT_FLAGS_W){ // (Conditionally) write out most-used-words statistics
		list<string> ordered;
		this->channel->writeWords(results, ordered);
		this->users->writeWordTotals(results, ordered);
		results << endl << section_divider << endl << endl;
	}

	if(contentVectors & CONTENT_FLAGS_J){ // (Conditionally) write out join/part/quit/kill/kick statistics
		this->users->writeCAG(results);
		results << endl << section_divider << endl << endl;
	}

	if(contentVectors & CONTENT_FLAGS_M){ // (Conditionally) write out mode-change statistics
		this->users->writeModes(results);
		results << endl << section_divider << endl << endl;
	}

	if(contentVectors & CONTENT_FLAGS_E){
		this->users->writeEmotions(results); // (Conditionally) write out emotion statistics
		results << endl << section_divider << endl << endl;
	}

	if(contentVectors & CONTENT_FLAGS_F){ // (Conditionally) write out feature statistics
		this->users->writeFeatures(results);
		results << endl << section_divider << endl << endl;
	}

	results.close();

	cout << "---> Results written successfully to " << fileout << endl << endl;
}
