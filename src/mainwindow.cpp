#include "mainwindow.h"
#include <QApplication>
#include <QPixmap>
#include <QImage>
#include <QSizePolicy>
#include <QFileInfo>

// BrightnessContrastDialog implementation
BrightnessContrastDialog::BrightnessContrastDialog(QWidget *parent)
    : QDialog(parent)
    , brightnessSlider(nullptr)
    , contrastSlider(nullptr)
    , brightnessSpinBox(nullptr)
    , contrastSpinBox(nullptr)
    , brightnessLabel(nullptr)
    , contrastLabel(nullptr)
{
    setWindowTitle("Настройка яркости и контраста");
    setModal(true);
    resize(400, 200);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Brightness controls
    QHBoxLayout *brightnessLayout = new QHBoxLayout();
    brightnessLabel = new QLabel("Яркость:", this);
    brightnessSlider = new QSlider(Qt::Horizontal, this);
    brightnessSlider->setRange(-100, 100);
    brightnessSlider->setValue(0);
    brightnessSpinBox = new QSpinBox(this);
    brightnessSpinBox->setRange(-100, 100);
    brightnessSpinBox->setValue(0);
    
    brightnessLayout->addWidget(brightnessLabel);
    brightnessLayout->addWidget(brightnessSlider);
    brightnessLayout->addWidget(brightnessSpinBox);
    
    // Contrast controls
    QHBoxLayout *contrastLayout = new QHBoxLayout();
    contrastLabel = new QLabel("Контраст:", this);
    contrastSlider = new QSlider(Qt::Horizontal, this);
    contrastSlider->setRange(-100, 100);
    contrastSlider->setValue(0);
    contrastSpinBox = new QSpinBox(this);
    contrastSpinBox->setRange(-100, 100);
    contrastSpinBox->setValue(0);
    
    contrastLayout->addWidget(contrastLabel);
    contrastLayout->addWidget(contrastSlider);
    contrastLayout->addWidget(contrastSpinBox);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Отмена", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(brightnessLayout);
    mainLayout->addLayout(contrastLayout);
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(brightnessSlider, &QSlider::valueChanged, this, &BrightnessContrastDialog::onBrightnessChanged);
    connect(contrastSlider, &QSlider::valueChanged, this, &BrightnessContrastDialog::onContrastChanged);
    connect(brightnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &BrightnessContrastDialog::onBrightnessChanged);
    connect(contrastSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &BrightnessContrastDialog::onContrastChanged);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

int BrightnessContrastDialog::getBrightness() const
{
    return brightnessSlider->value();
}

int BrightnessContrastDialog::getContrast() const
{
    return contrastSlider->value();
}

void BrightnessContrastDialog::setBrightness(int value)
{
    brightnessSlider->setValue(value);
    brightnessSpinBox->setValue(value);
}

void BrightnessContrastDialog::setContrast(int value)
{
    contrastSlider->setValue(value);
    contrastSpinBox->setValue(value);
}

void BrightnessContrastDialog::onBrightnessChanged(int value)
{
    brightnessSlider->blockSignals(true);
    brightnessSpinBox->blockSignals(true);
    brightnessSlider->setValue(value);
    brightnessSpinBox->setValue(value);
    brightnessSlider->blockSignals(false);
    brightnessSpinBox->blockSignals(false);
}

void BrightnessContrastDialog::onContrastChanged(int value)
{
    contrastSlider->blockSignals(true);
    contrastSpinBox->blockSignals(true);
    contrastSlider->setValue(value);
    contrastSpinBox->setValue(value);
    contrastSlider->blockSignals(false);
    contrastSpinBox->blockSignals(false);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , buttonLayout(nullptr)
    , openButton(nullptr)
    , imageLabel(nullptr)
    , statusLabel(nullptr)
    , openAction(nullptr)
    , saveAction(nullptr)
    , grayscaleAction(nullptr)
    , blurAction(nullptr)
    , brightnessContrastAction(nullptr)
    , brightnessValue(0)
    , contrastValue(0)
{
    setupUI();
    setupMenuBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Создаем центральный виджет
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Создаем основной layout
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Создаем layout для кнопок
    buttonLayout = new QHBoxLayout();
    
    // Создаем кнопку открытия изображения
    openButton = new QPushButton("Открыть изображение", this);
    openButton->setMinimumHeight(40);
    openButton->setStyleSheet("QPushButton { font-size: 14px; font-weight: bold; }");
    
    // Создаем label для отображения изображения
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(400, 300);
    imageLabel->setStyleSheet("QLabel { border: 2px dashed #ccc; background-color: #f9f9f9; }");
    imageLabel->setText("Изображение не выбрано");
    imageLabel->setScaledContents(true);
    
    // Создаем label для статуса
    statusLabel = new QLabel("Готов к работе", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    
    // Добавляем элементы в layout
    buttonLayout->addWidget(openButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(imageLabel, 1);
    mainLayout->addWidget(statusLabel);
    
    // Подключаем сигнал к слоту
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openImage);
    
    // Настраиваем окно
    setWindowTitle("Редактор изображений");
    setMinimumSize(500, 400);
    resize(800, 600);
}

void MainWindow::setupMenuBar()
{
    // Создаем меню "Файл"
    QMenu *fileMenu = menuBar()->addMenu("Файл");
    
    openAction = new QAction("Открыть", this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);
    
    saveAction = new QAction("Сохранить", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setEnabled(false);
    fileMenu->addAction(saveAction);
    
    // Создаем меню "Фильтры"
    QMenu *filtersMenu = menuBar()->addMenu("Фильтры");
    
    grayscaleAction = new QAction("Черно-белый", this);
    grayscaleAction->setEnabled(false);
    filtersMenu->addAction(grayscaleAction);
    
    blurAction = new QAction("Размытие", this);
    blurAction->setEnabled(false);
    filtersMenu->addAction(blurAction);
    
    brightnessContrastAction = new QAction("Яркость/Контраст", this);
    brightnessContrastAction->setEnabled(false);
    filtersMenu->addAction(brightnessContrastAction);
    
    // Подключаем сигналы
    connect(openAction, &QAction::triggered, this, &MainWindow::openImage);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveImage);
    connect(grayscaleAction, &QAction::triggered, this, &MainWindow::applyGrayscale);
    connect(blurAction, &QAction::triggered, this, &MainWindow::applyBlur);
    connect(brightnessContrastAction, &QAction::triggered, this, &MainWindow::adjustBrightnessContrast);
}

void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Выберите изображение",
        "",
        "Изображения (*.png *.jpg *.jpeg *.bmp *.tiff *.gif);;Все файлы (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        // Загружаем изображение с помощью OpenCV
        originalImage = cv::imread(fileName.toStdString());
        
        if (originalImage.empty()) {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение!");
            statusLabel->setText("Ошибка загрузки изображения");
            return;
        }
        
        // Сбрасываем фильтры
        currentImage = originalImage.clone();
        currentFilter = "";
        brightnessValue = 0;
        contrastValue = 0;
        
        // Отображаем изображение
        displayImage(currentImage);
        statusLabel->setText(QString("Загружено: %1").arg(QFileInfo(fileName).fileName()));
        
        // Включаем меню фильтров и сохранения
        saveAction->setEnabled(true);
        grayscaleAction->setEnabled(true);
        blurAction->setEnabled(true);
        brightnessContrastAction->setEnabled(true);
    }
}

void MainWindow::displayImage(const cv::Mat& image)
{
    // Конвертируем BGR в RGB для Qt
    cv::Mat rgbImage;
    cv::cvtColor(image, rgbImage, cv::COLOR_BGR2RGB);
    
    // Создаем QImage из OpenCV Mat
    QImage qImage(rgbImage.data, rgbImage.cols, rgbImage.rows, 
                  rgbImage.step, QImage::Format_RGB888);
    
    // Создаем QPixmap и масштабируем для отображения
    QPixmap pixmap = QPixmap::fromImage(qImage);
    
    // Масштабируем изображение, сохраняя пропорции
    QSize labelSize = imageLabel->size();
    QPixmap scaledPixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    imageLabel->setPixmap(scaledPixmap);
    imageLabel->setText(""); // Убираем текст
}

void MainWindow::saveImage()
{
    if (currentImage.empty()) {
        QMessageBox::warning(this, "Ошибка", "Нет изображения для сохранения!");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Сохранить изображение",
        "",
        "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp);;TIFF (*.tiff);;Все файлы (*.*)"
    );
    
    if (!fileName.isEmpty()) {
        if (cv::imwrite(fileName.toStdString(), currentImage)) {
            statusLabel->setText(QString("Сохранено: %1").arg(QFileInfo(fileName).fileName()));
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изображение!");
        }
    }
}

void MainWindow::applyGrayscale()
{
    if (originalImage.empty()) return;
    
    currentFilter = "grayscale";
    applyFilter();
    statusLabel->setText("Применен фильтр: Черно-белый");
}

void MainWindow::applyBlur()
{
    if (originalImage.empty()) return;
    
    currentFilter = "blur";
    applyFilter();
    statusLabel->setText("Применен фильтр: Размытие");
}

void MainWindow::adjustBrightnessContrast()
{
    if (originalImage.empty()) return;
    
    BrightnessContrastDialog dialog(this);
    dialog.setBrightness(brightnessValue);
    dialog.setContrast(contrastValue);
    
    if (dialog.exec() == QDialog::Accepted) {
        brightnessValue = dialog.getBrightness();
        contrastValue = dialog.getContrast();
        currentFilter = "brightness_contrast";
        applyFilter();
        statusLabel->setText(QString("Применен фильтр: Яркость=%1, Контраст=%2").arg(brightnessValue).arg(contrastValue));
    }
}

void MainWindow::applyFilter()
{
    if (originalImage.empty()) return;
    
    currentImage = originalImage.clone();
    
    if (currentFilter == "grayscale") {
        cv::cvtColor(currentImage, currentImage, cv::COLOR_BGR2GRAY);
        cv::cvtColor(currentImage, currentImage, cv::COLOR_GRAY2BGR);
    }
    else if (currentFilter == "blur") {
        cv::GaussianBlur(currentImage, currentImage, cv::Size(15, 15), 0);
    }
    else if (currentFilter == "brightness_contrast") {
        currentImage.convertTo(currentImage, -1, (contrastValue + 100) / 100.0, brightnessValue);
    }
    
    displayImage(currentImage);
}
