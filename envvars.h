#ifndef ENVVARS
#define ENVVARS

#include <vector>

extern std::vector<std::string> path;
extern std::string home;
void initPath();
void initHome();
std::string get_prompt();
bool searchPath(std::string command);

#endif
