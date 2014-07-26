#include "Config.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::string;

confmapint Config::config_ints = Config::setDefaultInts();
confmapchar Config::config_chars = Config::setDefaultChars();
confmapstring Config::config_strings = Config::setDefaultStrings();

/* String to int */
bool Config::strtoint(int &dest, string str, int radix){

  dest = 0;

  if(str[0] == '\0')
    return false; // Empty string

  if(str[0] != '-'){
    int offset = 0;
    if(str[0] >= '0' && str[0] <= '9')
      offset = ASCII_OFFSET_NUMERIC;
    else
      offset = ASCII_OFFSET_ALPHA;
    dest = str[0] - offset;
  }
  else if(str[1] == '\0')
    return false; // String consisted of "-" only

  if(dest > radix)
    return false; // First character was something other than '-' or a digit

  int i;
  for(i=1 ; str[i]!='\0' ; i++){

    int offset = 0;

    if(str[i] >= '0' && str[i] <= '9')
      offset = ASCII_OFFSET_NUMERIC;

    else
      offset = ASCII_OFFSET_ALPHA;

    int digit = str[i] - offset;

    if((unsigned) digit > (unsigned) radix)
      return false;

    dest *= radix;
    dest += digit;

  }
  if(str[0] == '-')
    dest *= -1; // Negate if necessary

  return true;
}

int Config::getInt(string key){
	return Config::config_ints[key];
}

char Config::getChar(string key){
	return Config::config_chars[key];
}
	
string Config::getString(string key){
	return Config::config_strings[key];
}

void Config::parseFile(string filename){
	
	ifstream file;
	file.open(filename.c_str());
	if(!file.good()){
		cout << "Config file absent or inaccesssible; falling back to defaults." << endl;
		return;
	}

	string line;
	getline(file, line);
	while(!file.eof()){

		int i = 0;
		while(line[i] == ' ') // Skip leading whitespace
			i++;
		if(line[i] != '\0' && line[i] != '#'){ // Prevents us dealing with lines that are only spaces or constants
			string key;
			while(line[i] != '\0' && line[i] != ' ' && line[i] != '='){
				key.push_back(line[i]);
				i++;
			}
			while(line[i] == ' ' || line[i] == '=')
				i++;
			if(line[i] != '\0'){
				string value;
				if(line[i] == '"'){ // Strings
					i++;
					while(line[i] != '"' && line[i] != '\0'){
						value.push_back(line[i]);
						i++;
					}
					if(line[i] == '"') // Just to be sure the string ended the right way
						Config::config_strings[key] = value;
					else
						cout << "Malformed line, skipping: \"" << line << "\"" << endl;
				}
				else if(line[i] == '\''){ // Chars
					i++;
					char value;
					if(line[i] != '\'' && line[i] != '\0'){
						value = line[i];
						i++;
						if(line[i] == '\'')
							Config::config_chars[key] = value;
						else
							cout << "Malformed line, skipping: \"" << line << "\"" << endl;
					}
					else
						cout << "Malformed line, skipping: \"" << line << "\"" << endl;
				}
				else{ // Ints/bools, or errors
					string str;
					while((line[i] != ' ') && (line [i] != '#') && (line[i] != '\0')){
						str.push_back(line[i]);
						i++;
					}
					int value;
					bool success = strtoint(value, str, 10);
					if(success) // Just to be sure this is a valid
						Config::config_ints[key] = value;
					else
						cout << "Malformed line, skipping: \"" << line << "\"" << endl;
				}
			}

		}

		getline(file, line);
	}
	
	// FIXME: Take this out when escape sequences are recognised in Config
	Config::config_strings["word_valid_delimiters"].append("\t\"\\");

	file.close();
}
