#include "imagewidget.h"
#include "../core/ImageDocument.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
    , m_doc(nullptr)
    , m_undo(nullptr)
    , m_scale(1.0)
    , m_panning(false)
    , m_tool(ToolType::None)
    , m_brushSize(10)
    , m_brushOpacity(100)
    , m_drawing(false)
{
    setAcceptDrops(true);
    setMouseTracking(true);
    setBackgroundRole(QPalette::Base);
}

void ImageWidget::setDocument(ImageDocument *doc)
{
    m_doc = doc;
    m_overlay = QImage();
    fitToWidget();
}

void ImageWidget::setUndoStack(QUndoStack *stack)
{
    m_undo = stack;
}

void ImageWidget::setTool(ToolType tool) { m_tool = tool; }
void ImageWidget::setBrushSize(int px) { m_brushSize = qMax(1, px); }
void ImageWidget::setBrushOpacity(int percent) { m_brushOpacity = qBound(0, percent, 100); }

void ImageWidget::zoomIn() { m_scale *= 1.25; update(); }
void ImageWidget::zoomOut() { m_scale /= 1.25; update(); }
void ImageWidget::resetView() { m_scale = 1.0; m_offset = QPoint(); update(); }

void ImageWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    // Neutral background (centered image with gray margins)
    p.fillRect(rect(), QColor(48, 48, 48));

    if (!m_doc || m_doc->isEmpty()) {
        p.setPen(Qt::gray);
        p.drawText(rect(), Qt::AlignCenter, tr("Нажмите для выбора изображения или перетащите файл сюда"));
        return;
    }

    const QImage &img = m_doc->image();
    if (m_overlay.isNull() || m_overlay.size() != img.size()) {
        m_overlay = QImage(img.size(), QImage::Format_ARGB32_Premultiplied);
        m_overlay.fill(Qt::transparent);
    }

    // compute target rect with zoom and pan
    QSizeF scaledF = img.size() * m_scale;
    QSize scaled((int)scaledF.width(), (int)scaledF.height());
    QRect target(QPoint(0,0), scaled);
    target.moveCenter(rect().center() + m_offset);

    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.drawImage(target, img);
    p.drawImage(target, m_overlay);
}

QRect ImageWidget::imageRectOnWidget() const
{
    if (!m_doc || m_doc->isEmpty()) return QRect();

    QSizeF scaledF = m_doc->image().size() * m_scale;
    QSize scaled((int)scaledF.width(), (int)scaledF.height());
    QRect target(QPoint(0,0), scaled);
    target.moveCenter(rect().center() + m_offset);
    return target;
}

QPointF ImageWidget::widgetToImage(const QPoint &p) const
{
    QRect r = imageRectOnWidget();
    if (r.isEmpty()) return QPointF(-1, -1);
    QPointF rel = QPointF(p - r.topLeft());
    if (m_scale <= 0.0) return QPointF(-1, -1);
    QPointF imgPt = rel / m_scale;
    return imgPt;
}

void ImageWidget::mousePressEvent(QMouseEvent *e)
{
    if (!m_doc || m_doc->isEmpty()) {
        if (e->button() == Qt::LeftButton) emit requestOpenFile();
        return;
    }

    if (e->button() == Qt::MiddleButton) {
        m_panning = true;
        m_panStart = e->pos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    if (e->button() == Qt::LeftButton && (m_tool == ToolType::Brush || m_tool == ToolType::Eraser)) {
        m_drawing = true;
        m_lastPos = e->pos();
    }
    // Text tool: prompt for text and paint on overlay at clicked position
    if (e->button() == Qt::LeftButton && m_tool == ToolType::Text) {
        bool ok = false;
        QString text = QInputDialog::getText(this,
                                             tr("Добавить текст"),
                                             tr("Введите текст:"),
                                             QLineEdit::Normal,
                                             QString(), &ok);
        if (ok && !text.isEmpty()) {
            // Ensure overlay exists
            if (m_overlay.isNull() && m_doc && !m_doc->isEmpty()) {
                m_overlay = QImage(m_doc->image().size(), QImage::Format_ARGB32_Premultiplied);
                m_overlay.fill(Qt::transparent);
            }
            // Map click to image coordinates and draw text on overlay
            QPointF imgPt = widgetToImage(e->pos());
            if (imgPt.x() >= 0 && imgPt.y() >= 0) {
                QPainter painter(&m_overlay);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setPen(QPen(m_brushColor, m_brushSize));
                painter.drawText(imgPt, text);
                update();
            }
        }
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (m_panning) {
        m_offset += e->pos() - m_panStart;
        m_panStart = e->pos();
        update();
        return;
    }

    if (m_drawing && (m_tool == ToolType::Brush || m_tool == ToolType::Eraser)) {
        if (m_overlay.isNull()) return;
        QPainter p(&m_overlay);
        p.setRenderHint(QPainter::Antialiasing, true);
        QColor col = (m_tool == ToolType::Brush) ? QColor(m_brushColor.red(), m_brushColor.green(), m_brushColor.blue(), m_brushOpacity * 255 / 100) : QColor(0,0,0,0);
        if (m_tool == ToolType::Eraser) {
            p.setCompositionMode(QPainter::CompositionMode_Clear);
            p.setPen(QPen(Qt::transparent, m_eraserSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        } else {
            p.setPen(QPen(col, m_brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }
        QPointF a = widgetToImage(m_lastPos);
        QPointF b = widgetToImage(e->pos());
        p.scale(1.0, 1.0);
        p.drawLine(a, b);
        m_lastPos = e->pos();
        update();
    }
}

QImage ImageWidget::compositedImage() const
{
    if (!m_doc || m_doc->isEmpty()) return QImage();
    QImage result = m_doc->image().convertToFormat(QImage::Format_ARGB32);
    if (!m_overlay.isNull()) {
        QPainter p(&result);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        p.drawImage(QPoint(0,0), m_overlay);
    }
    return result;
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_panning && e->button() == Qt::MiddleButton) {
        m_panning = false;
        unsetCursor();
        return;
    }
    if (m_drawing && e->button() == Qt::LeftButton) {
        m_drawing = false;
    }
}

void ImageWidget::wheelEvent(QWheelEvent *e)
{
    const int delta = e->angleDelta().y();
    if (delta > 0) zoomIn(); else zoomOut();
}

void ImageWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void ImageWidget::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls()) return;
    const auto urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;
    const QString file = urls.first().toLocalFile();
    emit fileDropped(file);
}

void ImageWidget::fitToWidget()
{
    if (!m_doc || m_doc->isEmpty() || width() <= 0 || height() <= 0) { update(); return; }
    const QSize imgSize = m_doc->image().size();
    if (imgSize.isEmpty()) { update(); return; }
    const double sx = double(width()) / double(imgSize.width());
    const double sy = double(height()) / double(imgSize.height());
    m_scale = qMin(sx, sy) * 0.98; // small margin
    m_offset = QPoint();
    update();
    emit scaleChanged(m_scale);
}

void ImageWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // keep image centered and scaled to fit when the widget resizes
    fitToWidget();
}
