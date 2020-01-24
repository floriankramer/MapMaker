#pragma once

#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QTimer>

#include "floatfield.h"

namespace mapmaker {
class MapWidget : public QWidget {
  enum class Action { NOTHING, DRAW, RESIZE };
  enum class DrawTarget { HUMIDITY, ALTITUDE, TEMPERATURE };

  Q_OBJECT
 public:
  MapWidget(QWidget *parent = nullptr);

  virtual void mouseMoveEvent(QMouseEvent *event) override;
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void keyReleaseEvent(QKeyEvent *event) override;

  virtual void paintEvent(QPaintEvent *event) override;

  void save(const std::string &path);

 private:
  void drawAt(size_t x, size_t y);
  void updatePreview(size_t min_x, size_t min_y, size_t max_x, size_t max_y, bool full_update);
  QColor interpColor(const QColor &q1, const QColor &q2, float f);

  // All values between 0 and 1
  FloatField humidity;
  FloatField altitude;
  FloatField altitude_deriv;
  FloatField temperature;

  QImage preview;

  QTimer drawTimer;

  Action _action;
  bool _draw_add;
  DrawTarget _draw_target;

  int _start_x;
  int _start_y;
  int _last_x;
  int _last_y;

  int _brushsize;
};
}  // namespace mapmaker
