#include "mainwindow.h"
#include "brightnesscontrastdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QPainter>
#include "imagewidget.h"
#include <QToolBar>
#include <QToolButton>
#include <QIcon>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setupMenus();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    // Central view
    m_view = new ImageWidget(this);
    m_view->setDocument(&m_doc);
    setCentralWidget(m_view);

    // Top toolbar with icons
    QToolBar *tb = addToolBar("Toolbar");
    tb->setMovable(false);
    tb->setIconSize(QSize(20, 20));

    m_openAct = new QAction(QIcon(":/icons/open.svg"), tr("Открыть"), this);
    m_openAct->setToolTip(tr("Открыть файл изображения"));
    connect(m_openAct, &QAction::triggered, this, &MainWindow::openImage);
    tb->addAction(m_openAct);

    m_saveAct = new QAction(QIcon(":/icons/save.svg"), tr("Сохранить как"), this);
    m_saveAct->setToolTip(tr("Сохранить изображение как..."));
    connect(m_saveAct, &QAction::triggered, this, &MainWindow::saveImage);
    tb->addAction(m_saveAct);

    tb->addSeparator();

    m_resetViewAct = new QAction(QIcon(":/icons/zoom_out.svg"), tr("Сброс масштаба"), this);
    m_resetViewAct->setToolTip(tr("Сбросить масштаб и центрировать"));
    connect(m_resetViewAct, &QAction::triggered, this, [this]{ m_view->fitToWidget(); });
    tb->addAction(m_resetViewAct);

    tb->addSeparator();

    m_brushAct = new QAction(QIcon(":/icons/brush.svg"), tr("Карандаш"), this);
    m_brushAct->setToolTip(tr("Инструмент: Карандаш"));
    m_brushAct->setCheckable(true);
    connect(m_brushAct, &QAction::triggered, this, [this]{ m_view->setTool(ToolType::Brush); m_eraserAct->setChecked(false); m_textAct->setChecked(false); });
    tb->addAction(m_brushAct);

    m_eraserAct = new QAction(QIcon(":/icons/eraser.svg"), tr("Ластик"), this);
    m_eraserAct->setToolTip(tr("Инструмент: Ластик"));
    m_eraserAct->setCheckable(true);
    connect(m_eraserAct, &QAction::triggered, this, [this]{ m_view->setTool(ToolType::Eraser); m_brushAct->setChecked(false); m_textAct->setChecked(false); });
    tb->addAction(m_eraserAct);

    m_textAct = new QAction(QIcon(":/icons/text.svg"), tr("Текст"), this);
    m_textAct->setToolTip(tr("Инструмент: Текст"));
    m_textAct->setCheckable(true);
    connect(m_textAct, &QAction::triggered, this, [this]{ m_view->setTool(ToolType::Text); m_brushAct->setChecked(false); m_eraserAct->setChecked(false); });
    tb->addAction(m_textAct);

    // Status bar
    statusBar()->showMessage(tr("Готово"));

    // Connections for view state
    connect(m_view, &ImageWidget::fileDropped, this, [this](const QString &p){
        if (m_doc.loadFromFile(p)) { updateView(); }
    });
    connect(m_view, &ImageWidget::scaleChanged, this, [this](double f){ m_cachedScale = f; updateStatus(); });

    setWindowTitle("ImageEditor");
    resize(1000, 700);
}

void MainWindow::setupMenus()
{
    auto *fileMenu = menuBar()->addMenu("Файл");
    fileMenu->addAction(m_openAct);
    fileMenu->addAction(m_saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Выход"), this, &QWidget::close);

    auto *editMenu = menuBar()->addMenu("Правка");
    m_rotateCWAct = editMenu->addAction("Поворот 90° по час.", this, &MainWindow::rotateCW);
    m_rotateCCWAct = editMenu->addAction("Поворот 90° против час.", this, &MainWindow::rotateCCW);
    m_rotate180Act = editMenu->addAction("Поворот 180°", this, &MainWindow::rotate180deg);
    editMenu->addAction("Отразить по горизонтали", this, [this]{ if(!m_doc.isEmpty()){ m_doc.flipHorizontal(); updateView(); } });
    m_resizeAct = editMenu->addAction("Изменить размер...", this, &MainWindow::doResize);
    m_cropAct = editMenu->addAction("Обрезать...", this, &MainWindow::doCrop);
    m_brightnessContrastAct = editMenu->addAction("Яркость/Контраст...", this, &MainWindow::adjustBrightnessContrast);
}

void MainWindow::updateView()
{
    if (m_doc.isEmpty()) {
        statusBar()->showMessage(tr("Нет изображения"));
        return;
    }
    m_view->fitToWidget();
    updateStatus();
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
        statusBar()->showMessage(tr("Сохранено"));
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

void MainWindow::updateStatus()
{
    if (m_doc.isEmpty()) { statusBar()->showMessage(tr("Нет изображения")); return; }
    const QFileInfo fi(m_doc.filePath());
    const QString name = fi.fileName();
    const QSize s = m_doc.image().size();
    const int pct = int(m_view->scaleFactor() * 100.0 + 0.5);
    statusBar()->showMessage(tr("%1    %2x%3    %4%")
                             .arg(name.isEmpty() ? tr("(без имени)") : name)
                             .arg(s.width()).arg(s.height()).arg(pct));
}


