
#include "FileSystemEmulator.hpp"

namespace fs = std::filesystem;

int main() {
  FileSystem fse;

  fse.makeDirectory("/a");
  fse.makeDirectory("/a/b");
  fse.makeDirectory("/a/b/k");
  fse.makeDirectory("/a/c");
  fse.makeDirectory("/a/c/k");

  fse.makeDirectory("/d");
  fse.makeDirectory("/d/b");
  fse.makeDirectory("/d/b/k");
  fse.makeDirectory("/d/c");
  fse.makeDirectory("/d/c/k");

  fse.makeDirectory("d");

  fse.log();

  return 0;
}