#ifndef EXECUTE
#define EXECUTE

#include <vector>

int execute(std::vector<std::vector<std::string> > byPipe, bool& exitShell);
int executeOneCommand(std::vector<std::string> line, bool& exitShell);
int executePipedCommands(std::vector<std::vector<std::string> > cmds);
void recur_pipe(std::vector<std::vector<std::string> > cmds, int current, int pipeline[]);
FILE* getFile(std::string operation, int& fd);
bool isDir(const char* path);

#endif