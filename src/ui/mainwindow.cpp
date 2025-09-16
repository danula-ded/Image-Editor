#include "mainwindow.h"
#include "brightnesscontrastdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_imageLabel(new QLabel(this))
    , m_scroll(new QScrollArea(this))
{
    setupUi();
    setupMenus();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setBackgroundRole(QPalette::Base);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(false);

    m_scroll->setBackgroundRole(QPalette::Dark);
    m_scroll->setWidget(m_imageLabel);
    setCentralWidget(m_scroll);

    setWindowTitle("ImageEditor");
    resize(1000, 700);
}

void MainWindow::setupMenus()
{
    auto *fileMenu = menuBar()->addMenu("Файл");
    m_openAct = fileMenu->addAction("Открыть", this, &MainWindow::openImage, QKeySequence::Open);
    m_saveAct = fileMenu->addAction("Сохранить", this, &MainWindow::saveImage, QKeySequence::Save);

    auto *editMenu = menuBar()->addMenu("Правка");
    m_rotateCWAct = editMenu->addAction("Поворот 90° по час.", this, &MainWindow::rotateCW);
    m_rotateCCWAct = editMenu->addAction("Поворот 90° против час.", this, &MainWindow::rotateCCW);
    m_rotate180Act = editMenu->addAction("Поворот 180°", this, &MainWindow::rotate180deg);
    m_resizeAct = editMenu->addAction("Изменить размер...", this, &MainWindow::doResize);
    m_cropAct = editMenu->addAction("Обрезать...", this, &MainWindow::doCrop);
    m_brightnessContrastAct = editMenu->addAction("Яркость/Контраст...", this, &MainWindow::adjustBrightnessContrast);
}

void MainWindow::updateView()
{
    if (m_doc.isEmpty()) {
        m_imageLabel->clear();
        statusBar()->showMessage("Нет изображения");
        return;
    }
    m_imageLabel->setPixmap(QPixmap::fromImage(m_doc.image()));
    m_imageLabel->adjustSize();
    statusBar()->showMessage(QString("%1 x %2").arg(m_doc.image().width()).arg(m_doc.image().height()));
}

void MainWindow::openImage()
{
    const QString file = QFileDialog::getOpenFileName(this, "Открыть изображение", QString(),
                                                      "Изображения (*.png *.jpg *.jpeg *.bmp *.tif *.tiff)");
    if (file.isEmpty()) return;
    if (!m_doc.loadFromFile(file)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение");
        return;
    }
    updateView();
}

void MainWindow::saveImage()
{
    if (m_doc.isEmpty()) return;
    const QString file = QFileDialog::getSaveFileName(this, "Сохранить изображение", QString(),
                                                      "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)");
    if (file.isEmpty()) return;
    if (!m_doc.saveToFile(file)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изображение");
    } else {
        statusBar()->showMessage("Сохранено");
    }
}

void MainWindow::rotateCW()
{
    if (m_doc.isEmpty()) return;
    m_doc.rotate90Clockwise();
    updateView();
}

void MainWindow::rotateCCW()
{
    if (m_doc.isEmpty()) return;
    m_doc.rotate90CounterClockwise();
    updateView();
}

void MainWindow::rotate180deg()
{
    if (m_doc.isEmpty()) return;
    m_doc.rotate180();
    updateView();
}

void MainWindow::doResize()
{
    if (m_doc.isEmpty()) return;
    bool okW=false, okH=false;
    int w = QInputDialog::getInt(this, "Ширина", "Новая ширина:", m_doc.image().width(), 1, 10000, 1, &okW);
    int h = QInputDialog::getInt(this, "Высота", "Новая высота:", m_doc.image().height(), 1, 10000, 1, &okH);
    if (!okW || !okH) return;
    m_doc.resizeTo(w, h);
    updateView();
}

void MainWindow::doCrop()
{
    if (m_doc.isEmpty()) return;
    // Простая версия: запросить прямоугольник вручную
    bool ok1=false, ok2=false, ok3=false, ok4=false;
    int x = QInputDialog::getInt(this, "Обрезка", "x:", 0, 0, m_doc.image().width(), 1, &ok1);
    int y = QInputDialog::getInt(this, "Обрезка", "y:", 0, 0, m_doc.image().height(), 1, &ok2);
    int w = QInputDialog::getInt(this, "Обрезка", "width:", m_doc.image().width(), 1, m_doc.image().width(), 1, &ok3);
    int h = QInputDialog::getInt(this, "Обрезка", "height:", m_doc.image().height(), 1, m_doc.image().height(), 1, &ok4);
    if (!ok1 || !ok2 || !ok3 || !ok4) return;
    m_doc.cropTo(QRect(x, y, w, h));
    updateView();
}

void MainWindow::adjustBrightnessContrast()
{
    if (m_doc.isEmpty()) return;
    BrightnessContrastDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.brightness() != 0) m_doc.adjustBrightness(dlg.brightness());
        if (dlg.contrast() != 0) m_doc.adjustContrast(dlg.contrast());
        updateView();
    }
}


