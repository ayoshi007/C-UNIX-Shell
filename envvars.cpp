#include <vector>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <sstream>

#include <limits.h>

#include "envvars.h"

std::vector<std::string> path;
std::string home;

void initPath() {
	std::string path_env(getenv("PATH"));

    std::replace(path_env.begin(), path_env.end(), ':', ' ');

    std::stringstream ss(path_env);
    std::string dir;
    while(ss >> dir) {
		path.push_back(dir);
    }
}
void initHome() {
	home = getenv("HOME");	
}
std::string get_prompt() {
	char cwd[PATH_MAX];
	std::string pwd(getcwd(cwd, sizeof(cwd)));
	size_t homePos = pwd.find(home);
	if (homePos != std::string::npos) {
		pwd.replace(pwd.find(home), home.length(), "~");
	}
	return "{myshell-aki-y:" + pwd + "} > ";
}
bool searchPath(std::string command) {
	for (int i = 0; i < path.size(); i++) {
		const char* pathname = (path[i] + "/" + command).c_str();
		if (access(pathname, F_OK) == 0) {
			return true;
		}
	}
	return false;
}