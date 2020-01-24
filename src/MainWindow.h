#pragma once

#include <QLabel>
#include <QMainWindow>
#include <QPixmap>

#include "MapWidget.h"

namespace mapmaker {
class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow();
  virtual ~MainWindow();

 private:
  void initUi();

  MapWidget *_widget;
};
}  // namespace mapmaker
