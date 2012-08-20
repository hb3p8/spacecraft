#ifndef GLRENDERWIDGET_H
#define GLRENDERWIDGET_H

#include <QGLWidget>
#include <QTime>
#include <QTimer>
#include <QPoint>
#include <QMap>

#include "EditorScene.hpp"

class GLRenderWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLRenderWidget( const QGLFormat& format, QWidget* parent = 0, QString modelFileName = "default.txt" );
    ~GLRenderWidget();

    float getFPS(){ return m_fps; }

protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    virtual void paintGL();

    virtual void keyPressEvent( QKeyEvent* e );
    virtual void keyReleaseEvent( QKeyEvent* e );
    virtual void mouseMoveEvent( QMouseEvent* e );
    virtual void mousePressEvent( QMouseEvent* e );
    virtual void wheelEvent( QWheelEvent* e );

private:
    void nextFrame();

    static const int defaultXSize = 1024;
    static const int defaultYSize = 768;

    QTimer* m_timer;

    float m_fps;

    int m_frames;
    int m_refreshTime;

    QTime* m_workTime;
    QTime* m_fpsTime;

    ScenePtr m_scene;

};

#endif // GLRENDERWIDGET_H
