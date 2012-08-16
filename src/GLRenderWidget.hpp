#ifndef GLRENDERWIDGET_H
#define GLRENDERWIDGET_H

#include <QGLWidget>
#include <QGLBuffer>
#include <QGLShaderProgram>
#include <QTime>
#include <QTimer>
#include <QPoint>
#include <QMap>

#include "TextRender.hpp"
#include "Camera.hpp"
#include "ShipModel.hpp"

typedef QMap< Qt::Key, bool > InputMap;

class GLRenderWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLRenderWidget( const QGLFormat& format, QWidget* parent = 0 );
    ~GLRenderWidget();

protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    virtual void paintGL();

    void process();

    virtual void keyPressEvent( QKeyEvent* e );
    virtual void keyReleaseEvent( QKeyEvent* e );
    virtual void mouseMoveEvent( QMouseEvent* e );
    virtual void mousePressEvent( QMouseEvent* e );
    virtual void wheelEvent( QWheelEvent* e );

private slots:
    void applyInput();


private:
    void nextFrame();

    static const int defaultXSize = 800;
    static const int defaultYSize = 600;

    QGLShaderProgram m_shader;

    QTimer* m_timer;

    float m_fps;

    int m_frames;
    int m_refreshTime;

    QTime* m_workTime;
    QTime* m_fpsTime;

    TextRender m_text;

    CameraPtr m_camera;
    QPoint m_lastMousePos;

    InputMap m_inputMap;

    ShipModel m_shipModel;

};

#endif // GLRENDERWIDGET_H
