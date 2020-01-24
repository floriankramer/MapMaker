
#include "MapWidget.h"

#include <dwarvenutils/Logger.h>

#include <QPainter>
#include <cmath>

namespace mapmaker {
MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent),
      _action(Action::NOTHING),
      _draw_target(DrawTarget::ALTITUDE),
      _brushsize(10) {
  humidity.resize(1280, 720);
  humidity.clear(1);
  altitude.resize(1280, 720);
  altitude.clear(0);
  temperature.resize(1280, 720);
  temperature.clear(1);
  preview = QImage(1280, 720, QImage::Format_RGB32);

  updatePreview(0, 0, preview.width(), preview.height(), true);

  setFocusPolicy(Qt::ClickFocus);
  drawTimer.setInterval(50);
  connect(&drawTimer, &QTimer::timeout, [this]() { drawAt(_last_x, _last_y); });
}

void MapWidget::mouseMoveEvent(QMouseEvent *event) {
  if (_action == Action::RESIZE) {
    _brushsize = std::hypot(event->x() - _start_x, event->y() - _start_y);
  }
  _last_x = event->x();
  _last_y = event->y();
  repaint();
}

void MapWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
    _action = Action::DRAW;
    drawAt(event->x(), event->y());
    drawTimer.start();
    _draw_add = event->button() == Qt::LeftButton;
  } else if (event->button() == Qt::MiddleButton) {
    _action = Action::RESIZE;
  }
  _start_x = event->x();
  _last_x = event->x();
  _start_y = event->y();
  _last_y = event->y();
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (_action == Action::DRAW) {
    updatePreview(0, 0, preview.width(), preview.height(), true);
    repaint();
  }

  drawTimer.stop();
  _action = Action::NOTHING;
}

void MapWidget::drawAt(size_t x, size_t y) {
  ssize_t sx = x;
  ssize_t sy = y;
  for (ssize_t ox = -_brushsize; ox <= _brushsize; ox++) {
    if (sx + ox < 0 || sx + ox > preview.width()) {
      continue;
    }
    for (ssize_t oy = -_brushsize; oy <= _brushsize; oy++) {
      if (sy + oy < 0 || sy + oy > preview.height()) {
        continue;
      }
      float d = std::hypot(ox, oy);
      if (d < _brushsize) {
        float v = 0.07f * std::pow(1 - (d / _brushsize), 0.5);
        switch (_draw_target) {
          case DrawTarget::ALTITUDE:
            if (_draw_add) {
              altitude(sx + ox, sy + oy) =
                  std::min(1.0f, altitude(sx + ox, sy + oy) + v);
            } else {
              altitude(sx + ox, sy + oy) =
                  std::max(0.0f, altitude(sx + ox, sy + oy) - v);
            }
            break;
          case DrawTarget::HUMIDITY:
            if (_draw_add) {
              humidity(sx + ox, sy + oy) =
                  std::min(1.0f, humidity(sx + ox, sy + oy) + v);
            } else {
              humidity(sx + ox, sy + oy) =
                  std::max(0.0f, humidity(sx + ox, sy + oy) - v);
            }
            break;
          case DrawTarget::TEMPERATURE:
            if (_draw_add) {
              temperature(sx + ox, sy + oy) =
                  std::min(1.0f, temperature(sx + ox, sy + oy) + v);
            } else {
              temperature(sx + ox, sy + oy) =
                  std::max(0.0f, temperature(sx + ox, sy + oy) - v);
            }
            break;
        }
      }
    }
  }
  updatePreview(std::max(sx - _brushsize, ssize_t(0)),
                std::max(sy - _brushsize, ssize_t(0)),
                std::min(ssize_t(preview.width()), sx + _brushsize + 1),
                std::min(ssize_t(preview.height()), sy + _brushsize + 1),
                false);
  repaint();
}

void MapWidget::keyReleaseEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_1) {
    _draw_target = DrawTarget::ALTITUDE;
  } else if (event->key() == Qt::Key_2) {
    _draw_target = DrawTarget::HUMIDITY;
  } else if (event->key() == Qt::Key_3) {
    _draw_target = DrawTarget::TEMPERATURE;
  }
  repaint();
}

QColor MapWidget::interpColor(const QColor &q1, const QColor &q2, float f) {
  return QColor(q1.red() * (1 - f) + f * q2.red(),
                q1.green() * (1 - f) + f * q2.green(),
                q1.blue() * (1 - f) + f * q2.blue());
}

void MapWidget::updatePreview(size_t min_x, size_t min_y, size_t max_x,
                              size_t max_y, bool full_update) {
  const QColor SEA = QColor(50, 150, 255);
  const QColor COLD_HUMID = QColor(150, 200, 255);
  const QColor WARM_HUMID = QColor(50, 100, 10);
  const QColor COLD_DRY = QColor(170, 170, 170);
  const QColor WARM_DRY = QColor(255, 255, 0);

  const QColor SEA_BORDER_COLOR = QColor(0, 0, 0);

  const QColor SHADE_POS = QColor(255, 255, 255);
  const QColor SHADE_NEG = QColor(0, 0, 0);
  const float shade_int = 0.2;
  const float sea_border = 0.1;

  for (size_t y = min_y; y < max_y; ++y) {
    for (size_t x = min_x; x < max_x; ++x) {
      // derivate the altitude
      float derivative = 0;
      bool is_sea_border = false;
      if (full_update) {
        if (x > 0 && y > 0 && x + 1 < preview.width() &&
            y + 1 < preview.height()) {
          // check if we are on a sea border
          if (altitude(x, y) > sea_border) {
            if (altitude(x - 1, y + 1) <= sea_border ||
                altitude(x - 1, y + 0) <= sea_border ||
                altitude(x - 1, y - 1) <= sea_border ||
                altitude(x + 0, y + 1) <= sea_border ||
                altitude(x + 0, y - 1) <= sea_border ||
                altitude(x + 1, y + 1) <= sea_border ||
                altitude(x + 1, y + 0) <= sea_border ||
                altitude(x + 1, y - 1) <= sea_border) {
              is_sea_border = true;
            }
          }
        }
      }

      QColor color;
      if (is_sea_border) {
        color = SEA_BORDER_COLOR;
      } else if (altitude(x, y) <= sea_border) {
        color = SEA;
      } else {
        QColor humid = interpColor(COLD_HUMID, WARM_HUMID, temperature(x, y));
        QColor dry = interpColor(COLD_DRY, WARM_DRY, temperature(x, y));
        color = interpColor(dry, humid, humidity(x, y));
      }
      preview.setPixelColor(x, y, color);
    }
  }
}

void MapWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.drawImage(0, 0, preview);

  painter.setPen(QColor(120, 120, 120, 200));
  painter.drawEllipse(_last_x - _brushsize, _last_y - _brushsize,
                      2 * _brushsize, 2 * _brushsize);

  switch (_draw_target) {
    case DrawTarget::ALTITUDE:
      painter.drawText(10, 10, "Altitude");
      break;
    case DrawTarget::HUMIDITY:
      painter.drawText(10, 10, "Humidity");
      break;
    case DrawTarget::TEMPERATURE:
      painter.drawText(10, 10, "Temperature");
      break;
  }
}

void MapWidget::save(const std::string &path) {
  updatePreview(0, 0, preview.width(), preview.height(), true);
  preview.save(QString::fromStdString(path));
}

}  // namespace mapmaker
