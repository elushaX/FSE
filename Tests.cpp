#include "FileSystemEmulator.hpp"

int main() {
  FileSystem fse;

  fse.makeDirectory("/newDir");
  fse.log();

  return 0;
}