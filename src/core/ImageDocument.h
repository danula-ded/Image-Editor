#ifndef IMAGE_DOCUMENT_H
#define IMAGE_DOCUMENT_H

#include <QImage>
#include <QString>

class ImageDocument
{
public:
    ImageDocument();

    bool loadFromFile(const QString &filePath);
    bool saveToFile(const QString &filePath, const char *format = nullptr) const;

    bool isEmpty() const;

    // Basic operations
    void rotate90Clockwise();
    void rotate90CounterClockwise();
    void rotate180();
    void resizeTo(int width, int height, Qt::TransformationMode mode = Qt::SmoothTransformation);
    void cropTo(const QRect &rect);

    // Simple filters (Qt-only)
    void adjustBrightness(int delta);            // delta in [-100, 100]
    void adjustContrast(int deltaPercent);       // deltaPercent in [-100, 100]

    const QImage &image() const;
    void setImage(const QImage &img);

private:
    QImage m_image;
};

#endif // IMAGE_DOCUMENT_H


