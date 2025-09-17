#ifndef UI_IMAGEWIDGET_H
#define UI_IMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QUndoStack>

class ImageDocument;

enum class ToolType {
    None,
    Brush,
    Eraser,
    Text
};

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = nullptr);

    void setDocument(ImageDocument *doc);
    void setUndoStack(QUndoStack *stack);

    void setTool(ToolType tool);
    void setBrushSize(int px);
    void setBrushOpacity(int percent);

    void fitToWidget();
    double scaleFactor() const { return m_scale; }

    void zoomIn();
    void zoomOut();
    void resetView();

signals:
    void requestOpenFile();
    void fileDropped(const QString &path);
    void scaleChanged(double factor);

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QRect imageRectOnWidget() const;
    QPointF widgetToImage(const QPoint &p) const;

    ImageDocument *m_doc;
    QUndoStack *m_undo;

    QImage m_overlay;      // drawing layer
    double m_scale;        // zoom factor
    QPoint m_lastPos;
    bool m_panning;
    QPoint m_panStart;
    QPoint m_offset;       // pan offset in pixels

    ToolType m_tool;
    int m_brushSize;
    int m_brushOpacity;    // 0..100
    bool m_drawing;
};

#endif // UI_IMAGEWIDGET_H
