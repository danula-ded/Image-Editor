#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QAction>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

class BrightnessContrastDialog : public QDialog
{
    Q_OBJECT

public:
    BrightnessContrastDialog(QWidget *parent = nullptr);
    int getBrightness() const;
    int getContrast() const;
    void setBrightness(int value);
    void setContrast(int value);

private slots:
    void onBrightnessChanged(int value);
    void onContrastChanged(int value);

private:
    QSlider *brightnessSlider;
    QSlider *contrastSlider;
    QSpinBox *brightnessSpinBox;
    QSpinBox *contrastSpinBox;
    QLabel *brightnessLabel;
    QLabel *contrastLabel;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void saveImage();
    void applyGrayscale();
    void applyBlur();
    void adjustBrightnessContrast();

private:
    void setupUI();
    void setupMenuBar();
    void displayImage(const cv::Mat& image);
    void applyFilter();
    
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QPushButton *openButton;
    QLabel *imageLabel;
    QLabel *statusLabel;
    
    // Menu actions
    QAction *openAction;
    QAction *saveAction;
    QAction *grayscaleAction;
    QAction *blurAction;
    QAction *brightnessContrastAction;
    
    cv::Mat originalImage;
    cv::Mat currentImage;
    QString currentFilter;
    int brightnessValue;
    int contrastValue;
};

#endif // MAINWINDOW_H
