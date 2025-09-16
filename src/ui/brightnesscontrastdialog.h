#ifndef BRIGHTNESSCONTRASTDIALOG_H
#define BRIGHTNESSCONTRASTDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>

class BrightnessContrastDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BrightnessContrastDialog(QWidget *parent = nullptr);
    int brightness() const;
    int contrast() const;
    void setBrightness(int value);
    void setContrast(int value);

private slots:
    void onBrightnessChanged(int value);
    void onContrastChanged(int value);

private:
    QSlider *m_brightnessSlider;
    QSlider *m_contrastSlider;
    QSpinBox *m_brightnessSpin;
    QSpinBox *m_contrastSpin;
    QLabel *m_bLabel;
    QLabel *m_cLabel;
};

#endif // BRIGHTNESSCONTRASTDIALOG_H


