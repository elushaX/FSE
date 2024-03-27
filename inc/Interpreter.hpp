#pragma once

#include "FileSystem.hpp"
#include <map>

class Interpreter {
  struct Command {
    const char* description = nullptr;
    ui32 numArguments = 0;
    bool(*callback)(FileSystem&, const std::vector<std::string>&) = nullptr;
  };

public:
  Interpreter();

  void interpret(const std::string& command);
  void printHelp();

private:
  static void reportError(const std::string& string);

private:
  std::map<std::string, Command> mCommands;
  FileSystem mFileSystem;
  std::string mError;
};