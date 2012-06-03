#ifndef BASE_RENDER_WIDGET_HPP
#define BASE_RENDER_WIDGET_HPP

#include <QGLWidget>
#include <QTimer>
#include <QTime>
#include "TextRender.hpp"

//! Abstract widget contains base features for rendering
class BaseRenderWidget : public QGLWidget
{
  Q_OBJECT

public:
  //! Constructs widget for rendering with parent widget
  BaseRenderWidget( QWidget *parent = 0 );
  virtual ~BaseRenderWidget();

  //! Current FPS for this widget
  GLfloat fps() const;

  //! Stops rendering process
  void stopRendering();
  //! Starts rendering process
  void startRendering();

public slots:
  void showFPS( bool show );

protected:
  //! Initializes OpenGL and data linked with it
  void initializeGL();

  //! Resizes widget
  void resizeGL( int width, int height );

  //! Redraws scene linked with widget
  void paintGL();

  virtual void draw( double time );

  TextRender& textRender() { return _text; };

  bool _showFPS;

private slots:
  void updateWindow();



private:
  void createActions();

  static const int defaultXSize = 1024;
  static const int defaultYSize = 768;

  QAction *_showFpsAction;
  QAction *_chooseTextColorAction;

  QTimer *_timer;

  void nextFrame();

  GLfloat _fps;

  GLint _frames;
  GLint _refreshTime;

  QTime* _workTime;
  QTime* _fpsTime;

  TextRender _text;


};

#endif
