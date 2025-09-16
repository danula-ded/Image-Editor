#include "brightnesscontrastdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

BrightnessContrastDialog::BrightnessContrastDialog(QWidget *parent)
    : QDialog(parent)
    , m_brightnessSlider(new QSlider(Qt::Horizontal, this))
    , m_contrastSlider(new QSlider(Qt::Horizontal, this))
    , m_brightnessSpin(new QSpinBox(this))
    , m_contrastSpin(new QSpinBox(this))
    , m_bLabel(new QLabel("Яркость:", this))
    , m_cLabel(new QLabel("Контраст:", this))
{
    setWindowTitle("Яркость/Контраст");
    m_brightnessSlider->setRange(-100, 100);
    m_contrastSlider->setRange(-100, 100);
    m_brightnessSpin->setRange(-100, 100);
    m_contrastSpin->setRange(-100, 100);

    auto row = [&](QLabel *label, QSlider *slider, QSpinBox *spin){
        auto *h = new QHBoxLayout();
        h->addWidget(label);
        h->addWidget(slider);
        h->addWidget(spin);
        return h;
    };

    auto *ok = new QPushButton("OK", this);
    auto *cancel = new QPushButton("Отмена", this);
    auto *btns = new QHBoxLayout();
    btns->addStretch();
    btns->addWidget(ok);
    btns->addWidget(cancel);

    auto *main = new QVBoxLayout(this);
    main->addLayout(row(m_bLabel, m_brightnessSlider, m_brightnessSpin));
    main->addLayout(row(m_cLabel, m_contrastSlider, m_contrastSpin));
    main->addLayout(btns);

    connect(m_brightnessSlider, &QSlider::valueChanged, this, &BrightnessContrastDialog::onBrightnessChanged);
    connect(m_contrastSlider, &QSlider::valueChanged, this, &BrightnessContrastDialog::onContrastChanged);
    connect(m_brightnessSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &BrightnessContrastDialog::onBrightnessChanged);
    connect(m_contrastSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &BrightnessContrastDialog::onContrastChanged);
    connect(ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
}

int BrightnessContrastDialog::brightness() const { return m_brightnessSlider->value(); }
int BrightnessContrastDialog::contrast() const { return m_contrastSlider->value(); }
void BrightnessContrastDialog::setBrightness(int value) { m_brightnessSlider->setValue(value); m_brightnessSpin->setValue(value); }
void BrightnessContrastDialog::setContrast(int value) { m_contrastSlider->setValue(value); m_contrastSpin->setValue(value); }

void BrightnessContrastDialog::onBrightnessChanged(int value)
{
    if (m_brightnessSpin->value() != value) m_brightnessSpin->setValue(value);
    if (m_brightnessSlider->value() != value) m_brightnessSlider->setValue(value);
}

void BrightnessContrastDialog::onContrastChanged(int value)
{
    if (m_contrastSpin->value() != value) m_contrastSpin->setValue(value);
    if (m_contrastSlider->value() != value) m_contrastSlider->setValue(value);
}


