#include "spimagearea.h"

#define SCALE_FACTOR 0.905

SPImageArea::SPImageArea(void) {
  _scale = 1.0;
  _viewLabel = new QLabel;
  _viewLabel->setScaledContents(true);
  _viewLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  setFrameStyle(QFrame::NoFrame);
  setVisible(true);
  setWidget(_viewLabel);
  setWidgetResizable(false);
}

void SPImageArea::loadPixmap(QPixmap pix) {
  _viewLabel->setPixmap(pix);
  _pixmapSize = pix.size();
}

void SPImageArea::wheelEvent(QWheelEvent *e) {
  if (e->modifiers() == Qt::ControlModifier) {
    int forward = e->angleDelta().y() > 0;

    if (forward)
      zoomIn();
    else
      zoomOut();
  } else
    QScrollArea::wheelEvent(e);
}

void SPImageArea::resizeImage(double s) {
  _scale = s;
  _viewLabel->resize(_scale * _pixmapSize);
}

void SPImageArea::resetScale(void) { resizeImage(1.0); }

void SPImageArea::zoomIn(void) { resizeImage(_scale / SCALE_FACTOR); }

void SPImageArea::zoomOut(void) { resizeImage(_scale * SCALE_FACTOR); }
