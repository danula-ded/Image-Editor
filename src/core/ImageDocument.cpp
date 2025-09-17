#include "ImageDocument.h"
#include <QPainter>
#include <QtMath>

ImageDocument::ImageDocument() {}

bool ImageDocument::loadFromFile(const QString &filePath)
{
    QImage img;
    if (!img.load(filePath)) return false;
    m_image = img.convertToFormat(QImage::Format_ARGB32);
    m_path = filePath;
    return true;
}

bool ImageDocument::saveToFile(const QString &filePath, const char *format) const
{
    if (m_image.isNull()) return false;
    return m_image.save(filePath, format);
}

bool ImageDocument::isEmpty() const
{
    return m_image.isNull();
}

void ImageDocument::rotate90Clockwise()
{
    if (m_image.isNull()) return;
    QTransform t; t.rotate(90);
    m_image = m_image.transformed(t, Qt::SmoothTransformation);
}

void ImageDocument::rotate90CounterClockwise()
{
    if (m_image.isNull()) return;
    QTransform t; t.rotate(-90);
    m_image = m_image.transformed(t, Qt::SmoothTransformation);
}

void ImageDocument::rotate180()
{
    if (m_image.isNull()) return;
    QTransform t; t.rotate(180);
    m_image = m_image.transformed(t, Qt::SmoothTransformation);
}

void ImageDocument::flipHorizontal()
{
    if (m_image.isNull()) return;
    m_image = m_image.mirrored(true, false);
}

void ImageDocument::resizeTo(int width, int height, Qt::TransformationMode mode)
{
    if (m_image.isNull() || width <= 0 || height <= 0) return;
    m_image = m_image.scaled(width, height, Qt::IgnoreAspectRatio, mode);
}

void ImageDocument::cropTo(const QRect &rect)
{
    if (m_image.isNull()) return;
    QRect r = rect.intersected(QRect(QPoint(0,0), m_image.size()));
    if (r.isEmpty()) return;
    m_image = m_image.copy(r);
}

void ImageDocument::adjustBrightness(int delta)
{
    if (m_image.isNull() || delta == 0) return;
    QImage img = m_image;
    const int h = img.height();
    const int w = img.width();
    for (int y = 0; y < h; ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < w; ++x) {
            QColor c = QColor::fromRgba(line[x]);
            int r = qBound(0, c.red() + delta, 255);
            int g = qBound(0, c.green() + delta, 255);
            int b = qBound(0, c.blue() + delta, 255);
            line[x] = qRgba(r, g, b, c.alpha());
        }
    }
    m_image = img;
}

void ImageDocument::adjustContrast(int deltaPercent)
{
    if (m_image.isNull() || deltaPercent == 0) return;
    // Contrast factor: 1.0 means no change. Map [-100..100] to [0..2]
    const double factor = qBound(-100, deltaPercent, 100) / 100.0; // [-1..1]
    const double k = 1.0 + factor; // [0..2]
    QImage img = m_image;
    const int h = img.height();
    const int w = img.width();
    for (int y = 0; y < h; ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < w; ++x) {
            QColor c = QColor::fromRgba(line[x]);
            auto adj = [&](int v){
                // center around 128, scale, clamp
                int nv = static_cast<int>((v - 128) * k + 128);
                return qBound(0, nv, 255);
            };
            int r = adj(c.red());
            int g = adj(c.green());
            int b = adj(c.blue());
            line[x] = qRgba(r, g, b, c.alpha());
        }
    }
    m_image = img;
}

const QImage &ImageDocument::image() const
{
    return m_image;
}

void ImageDocument::setImage(const QImage &img)
{
    m_image = img.convertToFormat(QImage::Format_ARGB32);
}


