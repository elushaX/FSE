
#include "FileSystem.hpp"

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

  fse.changeCurrent("/d/c/k");

  fse.makeDirectory("/a");
  fse.makeDirectory("/a/b");
  fse.makeDirectory("/a/b/k");
  fse.makeDirectory("/a/c");
  fse.makeDirectory("/a/c/k");

  fse.makeDirectory("ggad");
  fse.makeDirectory("gad");
  fse.makeDirectory("rwed");
  fse.makeDirectory("e");
  fse.makeDirectory("d");
  fse.makeDirectory("d/b");
  fse.makeDirectory("d/b/k");
  fse.makeDirectory("d/c");
  fse.makeDirectory("d/c/k");

  fse.log();

  return 0;
}