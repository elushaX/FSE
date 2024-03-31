#include "Interpreter.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

void getWords(const std::string& in, std::vector<std::string>& out) {
  std::stringstream ss(in);
  std::string word;
  while (ss >> word) out.push_back(word);
}

Interpreter::Interpreter() {
  mCommands["cd"] = {
      "change working directory",
      1,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.changeCurrent(Path(args[1]));
      }
  };

  mCommands["md"] = {
      "create directory",
      1,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.makeDirectory(Path(args[1]));
      }
  };

  mCommands["rd"] = {
      "remove directory",
      1,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.removeDirectory(Path(args[1]), false);
      }
  };

  mCommands["deltree"] = {
      "remove directory recursively",
      1,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.removeDirectory(Path(args[1]), true);
      }
  };

  mCommands["mf"] = {
      "make file",
      1,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.makeFile(Path(args[1]));
      }
  };

  mCommands["del"] = {
      "remove file or link",
      1,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.removeFileOrLink(Path(args[1]));
      }
  };

  mCommands["copy"] = {
      "recursive copy of a node",
      2,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.copyNode(Path(args[1]), Path(args[2]));
      }
  };

  mCommands["move"] = {
      "move node",
      2,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.moveNode(Path(args[1]), Path(args[2]));
      }
  };

  mCommands["mhl"] = {
      "make hard link",
      2,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.makeLink(Path(args[1]), Path(args[2]), false);
      }
  };

  mCommands["mdl"] = {
      "make dynamic link",
      2,
      [](FileSystem& filesystem, const std::vector<std::string>& args){
        return filesystem.makeLink(Path(args[1]), Path(args[2]), true);
      }
  };
}

void Interpreter::reportError(const std::string& description) const {
  if (logType == DEFAULT)
    std::cout << "ERROR: " << description << "\n\n";
}

void Interpreter::printHelp() {
  if (logType != DEFAULT) return;

  std::cout << "Commands: \n";
  for (auto& command : mCommands) {
    std::cout << command.first << " - ";
    std::cout << command.second.description;
    std::cout << "\n";
  }
  std::cout << "\n\n";
}

bool Interpreter::interpret(const std::string& command) {
  std::vector<std::string> words;
  getWords(command, words);

  if (words.empty()) {
    reportError("Empty command");
    return false;
  }

  std::string& commandName = words.front();
  std::transform(commandName.begin(), commandName.end(), commandName.begin(), [](unsigned char c) { return std::tolower(c); });

  auto iter = mCommands.find(commandName);

  if (iter == mCommands.end()) {
    reportError("Command not found");
    printHelp();
    return false;
  }

  if (iter->second.numArguments != words.size() - 1) {
    reportError("Invalid number of arguments given for: " + commandName);
    return false;
  }

  bool success = iter->second.callback(mFileSystem, words);

  if (logType == DEFAULT || (logType == DEBUG && success)) {
    std::cout << command << "\n";
    mFileSystem.log();
  }

  if (!success) {
    reportError(FileSystem::getLastError());
    return false;
  }

  return true;
}

void Interpreter::dumpToFile(const std::string& name) const {
  std::ofstream stream(name);
  mFileSystem.dump(stream);
}
