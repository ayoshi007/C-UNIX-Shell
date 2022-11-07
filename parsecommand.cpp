#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>

#include "parsecommand.h"

bool parseBySemicolon(std::string input, std::vector<std::string>& bySemicolon) {
	std::string command = "";
	for (int i = 0; i < input.length(); i++) {
		char c = input.at(i);
		if (c == '"') {
			int j;
			for (j = i; input.at(j) != '"' && j < input.length(); j++) {
				command += input.at(j);
			}
			if (j < input.length()) {
				command += input.at(j);
			}
			i = j;
		} else if (c == '\'') {
			int j;
			for (j = i; input.at(j) != '\'' && j < input.length(); j++) {
				command += input.at(j);
			}
			if (j < input.length()) {
				command += input.at(j);
			}
			i = j;
		} else if (c == ';') {
			if (command.length() != 0) {
				bySemicolon.push_back(trim(command));
				command = "";
			} else {
				return false;
			}
		} else {
			command += c;
		}
	}
	if (command.length() != 0) {
		bySemicolon.push_back(trim(command));
	}
	return true;
}

bool parseByPipeline(std::string input, std::vector<std::vector<std::string> >& byPipeline) {
	if (input.at(input.length() - 1) == '|' || input.at(0) == '|') {
		std::cerr << "Incorrect pipelining\n";
		return false;
	}
	std::vector<std::string> byPipe = split(input, '|');
	std::vector<std::string> line;
	for (int i = 0; i < byPipe.size(); i++) {
		std::string unsanitized = trim(byPipe[i]);
		std::string command = "";
		for (int j = 0; j < unsanitized.size(); j++) {
			char c = unsanitized.at(j);
			if (isWhitespace(c)) {
				if (command.length() != 0) {
					char lastChar = command.at(command.length() - 1);
					if (!(isWhitespace(lastChar) || lastChar == '<' || lastChar == '>')) {
						line.push_back(command);
						command = "";
					}
				}
			} else if (c == '<') {
				if (command.length() != 0) {
					char lastChar = command.at(command.length() - 1);
					if (!(isWhitespace(lastChar) || lastChar == '<')) {
						line.push_back(command);
						command = "";
					}
				}
				command += "<";
			} else if (c == '>') {
				if (command.length() != 0) {
					char lastChar = command.at(command.length() - 1);
					if (!(isWhitespace(lastChar) || lastChar == '>')) {
						line.push_back(command);
						command = "";
					}
				}
				command += ">";
			} else if (c == '"') {
				if (command.length() == 0) {
					int k;
					for (k = j + 1; unsanitized.at(k) != '"'; k++) {
						command += unsanitized.at(k);
						if (k == unsanitized.length() - 1) {
							std::cerr << "Bad quotations detected\n";
							return false;
						}
					}
					line.push_back(command);
					command = "";
					j = k + 1;
				} else {
					command = "\"";
				}
			} else if (c == '\'') {
				if (command.length() == 0) {
					int k;
					for (k = j + 1; unsanitized.at(k) != '\''; k++) {
						command += unsanitized.at(k);
						if (k == unsanitized.length() - 1) {
							std::cerr << "Bad quotations detected\n";
							return false;
						}
					}
					line.push_back(command);
					command = "";
					j = k + 1;
				} else {
					command = "'";
				}
			} else {
				command += c;
			}
			// implement quotation parsing here
		}
		if (command.length() != 0) {
			line.push_back(command);
		}
		byPipeline.push_back(line);
		line.clear();
	}
	return true;
}
bool parseCmdArgs(std::vector<std::string> input, std::vector<std::string>& args, std::vector<std::string>& redirects) {
	for (int i = 0; i < input.size(); i++) {
		if (input[i].at(0) == '>' || input[i].at(0) == '<') {
			if (!checkRedirect(input[i])) {
				return false;
			}
			redirects.push_back(input[i]);
		} else {
			args.push_back(input[i]);
		}
	}
	return true;
}
bool isWhitespace(char c) {
	return (c == ' ' || c == '\n' || c == '\r' ||
        c == '\t' || c == '\v' || c == '\f');
}
std::vector<std::string> split(std::string input, char delim) {
	std::vector<std::string> delimited;
	std::stringstream ss(input);
	std::string cmd;
	while (std::getline(ss, cmd, delim)) {
		delimited.push_back(cmd);
	}
	return delimited;
}
std::string trim(std::string str) {
	std::size_t found;
	if ((found = str.find_first_not_of(" ")) != std::string::npos) {
		str.erase(0, found);
	}
	if ((found = str.find_last_not_of(" ")) != std::string::npos) {
		str.erase(str.find_last_not_of(" ") + 1, std::string::npos);
	}
	return str;
}
bool checkRedirect(std::string redirect) {
	if (redirect.find("<<") == 0 || redirect.find(">>>") == 0) {
		return false;
	}
	return true;	
}