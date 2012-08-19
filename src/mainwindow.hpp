#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QGLWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setRenderWidget( QGLWidget* renderWidget );

private:
    QGLWidget* m_renderWidget;

};

#endif // MAINWINDOW_H
