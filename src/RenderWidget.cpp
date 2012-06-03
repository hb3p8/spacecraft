#include <glog/logging.h>
#include <QKeyEvent>

#include "RenderWidget.hpp"

RenderWidget::RenderWidget( QWidget* parent ): BaseRenderWidget( parent )
{

}

void RenderWidget::draw( double time )
{
  textRender().add( "Hello World!");

}

void RenderWidget::keyPressEvent( QKeyEvent *event )
{

}

void RenderWidget::wheelEvent( QWheelEvent *event )
{

}

void RenderWidget::mousePressEvent( QMouseEvent *event )
{

}

void RenderWidget::mouseMoveEvent( QMouseEvent *event )
{

}
