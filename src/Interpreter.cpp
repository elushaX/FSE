#include "Interpreter.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

void getWords(const std::string& in, std::vector<std::string>& out) {
  std::stringstream ss(in);
  std::string word;
  while (ss >> word) out.push_back(word);
}

Interpreter::Interpreter() {
  mCommands["cd"] = { 1, [](FileSystem& filesystem, const std::vector<std::string>& args){
    return filesystem.changeCurrent(args[1]);
  }};

  mCommands["md"] = { 1, [](FileSystem& filesystem, const std::vector<std::string>& args){
    return filesystem.makeDirectory(args[1]);
  }};
}

void Interpreter::reportError(const std::string& description) {
  std::cout << "ERROR : " << description << std::endl;
}

void Interpreter::interpret(const std::string& command) {
  std::cout << "\"" << command << "\"\n";

  std::vector<std::string> words;
  getWords(command, words);

  if (words.empty()) {
    reportError("Empty command");
    return;
  }

  std::string& commandName = words.front();
  std::transform(commandName.begin(), commandName.end(), commandName.begin(), [](unsigned char c) { return std::tolower(c); });

  auto iter = mCommands.find(commandName);

  if (iter == mCommands.end()) {
    reportError("Command not found");
    return;
  }

  if (iter->second.numArguments != words.size() - 1) {
    reportError("invalid number of arguments given");
    return;
  }

  bool success = iter->second.callback(mFileSystem, words);

  if (!success) {
    reportError(mFileSystem.getLastError());
    return;
  }

  mFileSystem.log();
}