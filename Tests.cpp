
#include "FileSystemEmulator.hpp"

namespace fs = std::filesystem;

int main() {
  FileSystem fse;

  Path path;

  path.set("/");
  path.set("/dir");
  path.set("/dir/");
  path.set("dir/");

  path.set("");
  path.set("asd");
  path.set("asdASD");
  path.set("asdASD123");
  path.set("asdASD123");

  fse.makeDirectory("/dir");
  fse.log();

  return 0;
}