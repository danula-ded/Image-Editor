#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QScrollArea>
#include <QToolButton>
#include <QStatusBar>
#include <QUndoStack>
#include <QSlider>
#include <QActionGroup>
#include "imagewidget.h"
#include "../core/ImageDocument.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void saveImage();
    void rotateCW();
    void rotateCCW();
    void rotate180deg();
    void doResize();
    void doCrop();
    void adjustBrightnessContrast();

private:
    void setupUi();
    void setupMenus();
    void applyLightTheme();
    void applyDarkTheme();
    void updateView();
    void updateStatus();

    ImageDocument m_doc;
    ImageWidget *m_view;
    QUndoStack m_undo;

    QAction *m_openAct;
    QAction *m_saveAct;
    QAction *m_resetViewAct;
    QAction *m_brushAct;
    QAction *m_eraserAct;
    QAction *m_textAct;
    QAction *m_rotateCWAct;
    QAction *m_rotateCCWAct;
    QAction *m_rotate180Act;
    QAction *m_resizeAct;
    QAction *m_cropAct;
    QAction *m_brightnessContrastAct;

    double m_cachedScale = 1.0;

    // UI controls on toolbar
    QSlider *m_brushSizeSlider { nullptr };
    QSlider *m_eraserSizeSlider { nullptr };
    QAction *m_colorBlack { nullptr };
    QAction *m_colorRed { nullptr };
    QAction *m_colorBlue { nullptr };
    QAction *m_colorPicker { nullptr };
};

#endif // UI_MAINWINDOW_H


