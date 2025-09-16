#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QScrollArea>
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
    void updateView();

    ImageDocument m_doc;
    QLabel *m_imageLabel;
    QScrollArea *m_scroll;

    QAction *m_openAct;
    QAction *m_saveAct;
    QAction *m_rotateCWAct;
    QAction *m_rotateCCWAct;
    QAction *m_rotate180Act;
    QAction *m_resizeAct;
    QAction *m_cropAct;
    QAction *m_brightnessContrastAct;
};

#endif // UI_MAINWINDOW_H


