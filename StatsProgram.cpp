#include "StatsProgram.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::string;
using std::vector;

/* Print usage message, in the event of bad command-line input (or where requested) */
void StatsProgram::printUsage(char* progname){

	cout << endl << "IRC STATISTICS PROGRAM" << endl << endl;
	cout << "Usage:" << endl << endl;
	cout << "\t" << progname << " [switches] <input filename 1<input filename 2<input filename 3<...>>>> " << endl;
	cout << endl << "Switches:" << endl << endl;
	cout << "\tfile switches:" << endl;
	cout << "\t\t-d\t\tuse default config (skip parsing the config file)" << endl;
	cout << "\t\t-o <arg>\tuse <arg> as output filename; if not specified defaults to what is specified in config file" << endl;
	cout << "\t\t-v\t\tdo not overwrite if file with this name already exists; if not specified, overwrites indiscriminately by default" << endl;
	cout << endl << "\toutput content switches:" << endl;
	cout << "\t\t-b\t\toutput basic statistics (users by linecount, busiest hours, nicks and aliases)" << endl;
	cout << "\t\t-w\t\toutput most-common-word statistics" << endl;
	cout << "\t\t-j\t\toutput join/part/quit/kill/kick information" << endl;
	cout << "\t\t-m\t\toutput mode-change statistics (ops/voices granted, topic changes, ...)" << endl;
	cout << "\t\t-e\t\toutput 'emotion' statistics" << endl;
	cout << "\t\t-f\t\toutput features" << endl;
	cout << "\t\t-A\t\toutput all available statistics (default; overrides any of the above)" << endl;
	cout << endl;

}

int main(int argc, char* argv[]){

	vector<string> filesin;
	string fileout;

	bool fileoutGiven = false; // Whether or not the user has specified an output file by command-line
	bool noOverwrite = false; // Whether or not the output filename may not be overwritten by default
	bool useDefault = false; // Whether we want to skip opening the config file and just use defaults instead
	int content = 0; // Output content vectors

	/* Parse argv, extracting information about:
	 *
	 *	general:
	 *		-?		print usage message
	 *
	 *	file:
	 *		-d		use default config (skip parsing the config file)
	 *		-o <arg> 	use <arg> as output filename (defaults to preprocessor constant if not specified)
	 *		-v 		do-not-overwrite flag in case output file already exists (defaults to false)
	 *
	 *	output content:
	 *		-b		output basic information (linecounts, nicks and aliases, busiest hours)
	 *		-w		output most used words information
	 *		-j		output join/part/quit/kill/kick information
	 *		-m 		output mode-change information (ops/voices granted, topic changes, ...)
	 *		-e		output emotion information (mostly gleaned from smilies)
	 *		-f		output features/fun stuff (more to come ...)
	 *		-A		output all available statistics (default; overrides any of the above)
	 */
	 
	int i = 1;
	bool subset = false; // Whether a subset of stats is being requested
	while(i < argc){

		string current = argv[i];
		if(current[0] == StatsProgram::switch_flag){ // Where switch marker is detected
			int bound = current.length();

			if(bound < 2){ // If no switches are actually specified
				cout << '\t' << argv[0] << ":\tSwitches indicated but not given: \"" << current << "\"." << endl << endl;
				StatsProgram::printUsage(argv[0]);
				exit(1);
			}

			else{ // Otherwise, parse switches
				for(int j = 1; j < bound; j++){
					switch(current[j]){
						case '?': StatsProgram::printUsage(argv[0]); exit(0);
						case 'b': subset = true; content |= CONTENT_FLAGS_B; break;
						case 'w': subset = true; content |= CONTENT_FLAGS_W; break;
						case 'j': subset = true; content |= CONTENT_FLAGS_J; break;
						case 'm': subset = true; content |= CONTENT_FLAGS_M; break;
						case 'e': subset = true; content |= (CONTENT_FLAGS_E | CONTENT_FLAGS_W); break; // To produce emotion stats, we need words
						case 'f': subset = true; content |= (CONTENT_FLAGS_F | CONTENT_FLAGS_W); break; // To produce features, we need words
						case 'A': content = 0xFFFFFFFF; break;
						case 'v': noOverwrite = true; break;
						case 'd': useDefault = true; break;
						case 'o':{
							if(i >= argc){
								cout << '\t' << argv[0] << ":\t'output file' switch indicated, but no output file specified: \"" << current[j] << "\"." << endl << endl;
								StatsProgram::printUsage(argv[0]);
								exit(1);
							}
							else if(fileoutGiven){
								cout << '\t' << argv[0] << ":\tMultiple output target files indicated, please disambiguate." << endl << endl;
								StatsProgram::printUsage(argv[0]);
								exit(1);
							}
							else{
								fileout = argv[i+1]; // Use the next argument as the filename
								fileoutGiven = true;
								i++; // Skip the next argument, as it is the filename
							}
							break;
						}
						default:{ // Unknown switches
							cout << '\t' << argv[0] << ":\tUnknown switch: \"" << current[j] << "\"." << endl << endl;
							StatsProgram::printUsage(argv[0]);
							exit(1);
						}
					}
				}
			}
		}

		else // Not a switch marker, so must be an input file
			filesin.push_back(argv[i]);

		i++;
	}

	if(!subset) // Run all stats by default
		content = 0xFFFFFFFF;

	if(noOverwrite){ // If no-overwrite specified, make sure the output file does not already exist
		ifstream test;
		test.open(fileout.c_str(), std::ios_base::in); // Attempt to open for reading
		if(test){ // If file opened for reading, we do not wish to overwrite
			cerr << "Error opening file: " << fileout << " - file already exists. If you wish to overwrite, re-run without the -v flag." << endl;
			test.close();
			exit(1);
		}
		test.close();
	}

	if(filesin.empty()){ // If no input files were given
		cout << '\t' << argv[0] << ":\tNo input files given." << endl << endl;
		StatsProgram::printUsage(argv[0]);
	}

	else if(find(filesin.begin(), filesin.end(), fileout) != filesin.end()){ // If the output file (specified or otherwise) is one of the input files
		cout << '\t' << argv[0] << ":\tOutput filename matches one of input filenames; please choose another output filename." << endl;
	}

	else if(content == 0){ // FIXME: this should probably go if all stats get run by default
		cout << "\tNo statistics requested; please re-run using the appropriate switches." << endl;
		StatsProgram::printUsage(argv[0]);
	}

	else{ // Otherwise, good to go
		successcode code = SUCCEED; // Return codes
		
		cout << "\033[H\033[2J\n"; // Clear console
		if(!useDefault)
			Config::parseFile(FILENAME_CONFIG);

		if(!fileoutGiven)
			fileout = Config::getString(KEY_CONF_FILENAME_OUT_DEFAULT);

		WordCategories cats;
		if(content & CONTENT_FLAGS_W) // Only build word-lists if doing word statistics
			cats.parseFile(Config::getString(KEY_CONF_FILENAME_WORDS), code);

		if(code != SUCCEED)
			exit(code);
			
		FileParser fp(filesin.front(), &cats);
		fp.buildStats(filesin, content, code);
		
		if(code != SUCCEED)
			exit(code);
			
		fp.adjustStats(Config::getString(KEY_CONF_FILENAME_IGNORE), Config::getString(KEY_CONF_FILENAME_MERGE));
		fp.writeStats(fileout, content);
	}

	return 0;
}
