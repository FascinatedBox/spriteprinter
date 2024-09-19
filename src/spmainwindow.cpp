#include <QFileDialog>
#include <QMenuBar>
#include <QPushButton>
#include <QVBoxLayout>

#include "spmainwindow.h"

void SPMainWindow::initLayout(void) {
  _imageView = new SPImageArea;
  setCentralWidget(_imageView);
}

void SPMainWindow::setupMenuBar(void) {
  QMenu *menu = menuBar()->addMenu(tr("&File"));
  QAction *a = menu->addAction(tr("&Open"), this, &SPMainWindow::fileOpen);
  a->setShortcut(QKeySequence::Open);
  _saveAct = menu->addAction(tr("&Save"), this, &SPMainWindow::fileSave);
  _saveAct->setShortcut(QKeySequence::Save);
  _saveAct->setEnabled(false);
  menu->addSeparator();
  a = menu->addAction(tr("&Quit"), this, &QWidget::close);
  a->setShortcut(QKeySequence::Quit);
}

SPMainWindow::SPMainWindow(void) {
  initLayout();
  setupMenuBar();
  resize(300, 300);
}

void SPMainWindow::fileOpen(void) {
  QFileDialog d(this, tr("Select Image"));
  d.setAcceptMode(QFileDialog::AcceptOpen);
  d.setFileMode(QFileDialog::ExistingFile);
  d.setMimeTypeFilters(QStringList() << "image/png");

  if (d.exec() != QDialog::Accepted)
    return;

  _fileName = d.selectedFiles().first();
  QPixmap pix;

  if (pix.load(_fileName, "PNG") == false)
    return;

  _imageView->loadPixmap(pix);
  const char *path = _fileName.toLocal8Bit().data();
  _data = spdata_new_from_png_path(path);

  if (_data == nullptr)
    return;

  _imageView->resetScale();
  _saveAct->setEnabled(true);
}

void SPMainWindow::fileSave(void) {
  QString osName = _fileName.replace(".png", ".scad");
  QString savePath = QFileDialog::getSaveFileName(
      this, tr("Save Design"), osName, tr("OpenSCAD Designs (*.scad)"));

  if (savePath.isEmpty())
    return;

  const char *path = _fileName.toLocal8Bit().data();
  spdata_write_os_to_path(_data, path);
  resetState();
}

void SPMainWindow::resetState(void) {
  _imageView->loadPixmap(QPixmap());
  _imageView->resetScale();
  spdata_delete(_data);
  _saveAct->setEnabled(false);
  _imageView->clearFocus();
  this->setFocus();
}
