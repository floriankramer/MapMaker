#include "MainWindow.h"

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

namespace mapmaker {

MainWindow::MainWindow() { initUi(); }

MainWindow::~MainWindow() { delete _widget; }

void MainWindow::initUi() {
  _widget = new MapWidget(this);
  setCentralWidget(_widget);

  QMenu *file = menuBar()->addMenu("File");

  QAction *save = new QAction("save");
  save->setShortcut(QKeySequence::Save);
  connect(save, &QAction::triggered, [this]() {
    QString path = QFileDialog::getSaveFileName(this, "save");
    if (path.length() > 0) {
      _widget->save(path.toUtf8().constData());
    }
  });
  file->addAction(save);
}

}  // namespace mapmaker
