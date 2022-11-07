#ifndef PARSECMD
#define PARSECMD

#include <vector>

bool parseBySemicolon(std::string input, std::vector<std::string>& bySemicolon);
bool parseByPipeline(std::string input, std::vector<std::vector<std::string> >& byPipeline);
bool parseCmdArgs(std::vector<std::string> input, std::vector<std::string>& args, std::vector<std::string>& redirects);
bool isWhitespace(char c);	
std::vector<std::string> split(std::string input, char delim);
std::string trim(std::string str);
bool checkRedirect(std::string redirect);

#endif