#ifndef SPMAINWINDOW_H
#define SPMAINWINDOW_H
#include <QAction>
#include <QMainWindow>

#include "spdata.h"
#include "spimagearea.h"

class SPMainWindow : public QMainWindow {
  Q_OBJECT

public:
  SPMainWindow(void);

private slots:
  void fileOpen(void);
  void fileSave(void);

private:
  void resetState(void);
  void initLayout(void);
  void setupMenuBar(void);

  QAction *_saveAct;
  QString _fileName;
  SPData *_data;
  SPImageArea *_imageView;
};

#endif
