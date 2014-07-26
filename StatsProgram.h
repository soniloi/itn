/* Class handling UI,
 * in this case, parsing command-line arguments 
 */

#include <vector>
#include <algorithm> // For std::find
#include "FileParser.h"
#include "Config.h"

class StatsProgram{
public:
	static const char switch_flag = '-';
	static void printUsage(char* progname);
};
