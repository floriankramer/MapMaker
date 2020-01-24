#include <QApplication>
#include "MainWindow.h"

#include <gperftools/profiler.h>

int main(int argc, char **argv) {
  ProfilerStart("./profile");
  QApplication qapp(argc, argv);
  mapmaker::MainWindow window;
  window.show();
  qapp.exec();
  ProfilerStop();
}
