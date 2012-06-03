#ifndef RENDER_WIDGET_HPP
#define RENDER_WIDGET_HPP

#include "BaseRenderWidget.hpp"

class RenderWidget : public BaseRenderWidget
{
  Q_OBJECT
public:
  RenderWidget( QWidget* parent = NULL );

protected:
  void keyPressEvent ( QKeyEvent *e );

  void mousePressEvent   ( QMouseEvent *e );
  void mouseMoveEvent    ( QMouseEvent *e );
  void wheelEvent        ( QWheelEvent *e );

  void draw( double time );

private:
//

};

#endif
