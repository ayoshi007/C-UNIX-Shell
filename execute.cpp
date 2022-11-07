#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "execute.h"
#include "parsecommand.h"
#include "envvars.h"

int execute(std::vector<std::vector<std::string> > byPipe, bool& exitShell) {
	if (byPipe.size() == 1) {
		return executeOneCommand(byPipe[0], exitShell);
	} else {
		exitShell = false;
		return executePipedCommands(byPipe);
	}
}
int executeOneCommand(std::vector<std::string> line, bool& exitShell) {
	int returnStatus;
	std::vector<std::string> args;
	std::vector<std::string> redirects;
	
	// parse commands and redirects from line
	if (!parseCmdArgs(line, args, redirects)) {
		std::cerr << "Bad command detected\n";
		return -1;
	}
	std::string file;
	std::vector<char *> argv(args.size() + 1);
	file = args[0];
	// vector to store the rest of the arguments
	// vector has pointers to each string
	for (int i = 0; i < args.size(); i++) {
		argv[i] = &args[i][0];
	}
	argv[args.size()] = NULL;
	
	// if the command was exit, exit exit the shell
	if (file == "exit") {
		exitShell = true;
		execvp(file.c_str(), argv.data());
	}
	// if command was cd, either go to home or cd to argument directory
	else if (file == "cd") {
		// if a directory was given, cd to that directory
		if (argv.size() > 2) {
			if (!isDir(argv[1])) {
				std::cerr << argv[1] << " is not a directory.\n";
				return -1;
			}
			return chdir(argv[1]);
		}
		// else, cd to home directory
		else {
			return chdir(home.c_str());
		}
	}
	// if another command, fork a child process and run the command
	else {
		pid_t pid;
		// if fork failed
		if ((pid = fork()) < (pid_t) 0) {
			std::cerr << "Fork error occurred\n";
			return -1;
		}
		// child process
		else if (pid == (pid_t) 0) {
			for (int i = 0; i < redirects.size(); i++) {
				int fd;
				FILE* redirectFile = getFile(redirects[i], fd);
			
				dup2(fileno(redirectFile), fd);
				fclose(redirectFile);
			}
			if (!searchPath(file)) {
				std::cerr << "Command not found\n";
				exit(1);
			}
			execvp(file.c_str(), argv.data());
		}
		// parent process waits
		else {
			waitpid(pid, &returnStatus, 0);
		}
	}
	return returnStatus;
}
int executePipedCommands(std::vector<std::vector<std::string> > cmds) {
	int status;
	pid_t pid = fork();
	
	if (pid < (pid_t) 0) {
		std::cerr << "Fork error\n";
		return -1;
	}
	// process that runs head in the recursive call
	if (pid == (pid_t) 0) {
		int pipeline[2];
		pipe(pipeline);
		
		recur_pipe(cmds, cmds.size() - 1, pipeline);
	}
	else {
		waitpid(pid, &status, 0);
	}
	return status;
}
void recur_pipe(std::vector<std::vector<std::string> > cmds, int current, int pipeline[]) {
	pid_t pid = fork();
	if (pid < (pid_t) 0) {
		std::cerr << "Fork error\n";
	}
	// process that runs head in the recursive call
	if (pid == (pid_t) 0) {
		close(pipeline[0]);
		dup2(pipeline[1], STDOUT_FILENO);
		close(pipeline[1]);
		
		pipe(pipeline);
		// base case
		if (current == 1) {
			std::vector<std::string> args;
			std::vector<std::string> redirects;
			if (parseCmdArgs(cmds[0], args, redirects)) {
				std::string file;
				std::vector<char *> argv(args.size() + 1);
				file = args[0];
				// vector to store the rest of the arguments
				// vector has pointers to each string
				for (int i = 0; i < args.size(); i++) {
					argv[i] = &args[i][0];
				}
				argv[args.size()] = NULL;
				
				if (file != "exit") {
					if (!searchPath(file)) {
						std::cerr << "Command not found\n";
						exit(1);
					}
					for (int i = 0; i < redirects.size(); i++) {
						int fd;
						FILE* redirectFile = getFile(redirects[i], fd);
					
						dup2(fileno(redirectFile), fd);
						fclose(redirectFile);
					}		
					execvp(file.c_str(), argv.data());
				} else {
					exit(0);
				}
			}
		}
		// recursive case
		else {
			recur_pipe(cmds, current - 1, pipeline);
		}
	}
	else {
		close(pipeline[1]);
		dup2(pipeline[0], STDIN_FILENO);
		close(pipeline[0]);
		
		std::vector<std::string> args;
		std::vector<std::string> redirects;
		if (parseCmdArgs(cmds[current], args, redirects)) {
			std::string file;
			std::vector<char *> argv(args.size() + 1);
			file = args[0];
			// vector to store the rest of the arguments
			// vector has pointers to each string
			for (int i = 0; i < args.size(); i++) {
				argv[i] = &args[i][0];
			}
			argv[args.size()] = NULL;
			if (file != "exit") {
				if (!searchPath(file)) {
					std::cerr << "Command not found\n";
					exit(1);
				}
				for (int i = 0; i < redirects.size(); i++) {
					int fd;
					FILE* redirectFile = getFile(redirects[i], fd);
				
					dup2(fileno(redirectFile), fd);
					fclose(redirectFile);
				}		
				execvp(file.c_str(), argv.data());
			} else {
				exit(0);
			}
		} else {
			std::cerr << "Command parsing error\n";
			return;
		}
	}
}
FILE* getFile(std::string operation, int& fd) {
	if (operation.at(0) == '>') {
		fd = STDOUT_FILENO;
		if (operation.at(1) == '>') {
			return fopen(operation.substr(2).c_str(), "a");
		} else {
			return fopen(operation.substr(1).c_str(), "w");
		}
	} else {
		fd = STDIN_FILENO;
		return fopen(operation.substr(1).c_str(), "r");
	}
}
bool isDir(const char* path) {
	struct stat statbuf;
	if (stat(path, &statbuf) < 0) {
		std::cerr << "Stat error\n";
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}