#ifndef SPIMAGEAREA_H
#define SPIMAGEAREA_H
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include <QWheelEvent>

class SPImageArea : public QScrollArea {
  Q_OBJECT

public:
  SPImageArea(void);
  void wheelEvent(QWheelEvent *e);
  void loadPixmap(QPixmap pix);
  void resetScale(void);
  void resizeImage(double s);

signals:
  void zoomedIn(void);
  void zoomedOut(void);

private:
  void zoomOut(void);
  void zoomIn(void);

  double _scale;
  QLabel *_viewLabel;
  QSize _pixmapSize;
};

#endif
