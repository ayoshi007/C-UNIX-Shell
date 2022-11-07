#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>

#include "envvars.h"
#include "parsecommand.h"
#include "execute.h"

int main() {
	initPath();
	initHome();
	std::string input;
	int returnStatus;
	
	std::cout << std::endl;
	std::cout << "Starting myshell (Aki Y Shell)\n";
	std::cout << "PWD: " << getenv("PWD") << std::endl;
	std::cout << std::endl;
	
    while (true) {
        std::cout << get_prompt();
		
        std::getline(std::cin, input);
		
		std::vector<std::string> bySemicolon;
		
		if (!parseBySemicolon(input, bySemicolon)) {
			std::cerr << "Incorrect format\n";
			continue;
		}
		for (int cmdNum = 0; cmdNum < bySemicolon.size(); cmdNum++) {
			std::vector<std::vector<std::string> > byPipeline;
			bool exitShell = false;
			if (!parseByPipeline(bySemicolon[cmdNum], byPipeline)) {
				break;
			} else if (byPipeline.size() > 3) {
				std::cerr << "Can only do 3 command-pipelines\n";
				break;
			} else {
				
				returnStatus = execute(byPipeline, exitShell);
				
				if (exitShell) {
					std::cout << "Exiting shell\n";
					goto endshell;
				}
				
			}
		}
    }
	endshell:
	
    return returnStatus;
}
