#include "spmainwindow.h"
#include <QApplication>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  SPMainWindow *w = new SPMainWindow;
  w->show();
  app.exec();
}
