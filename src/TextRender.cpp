#include "TextRender.hpp"

TextRender::TextRender() : _font(), _text()
{
}

TextRender::TextRender( const QString& family, int size, QColor color ) : _font( family, size ), _text(), _color( color )
{
}

void TextRender::add( const QString& name, size_t value )
{
  _text.append( name + QString( " %1" ).arg( value ) );
}

void TextRender::add( const QString& name, int value )
{
  _text.append( name + QString( " %1" ).arg( value ) );
}

void TextRender::add( const QString& name, GLfloat value )
{
  _text.append( name + QString( " %1" ).arg( value ) );
}

void TextRender::add( const QString& name, Eigen::Vector2f value )
{
  _text.append( name + QString( " ( %1, %2 )" ).arg( value.x() ).arg( value.y() ) );
}

void TextRender::add( const QString& name, Eigen::Vector3f value )
{
  _text.append( name + QString( " ( %1, %2, %3 )" ).arg( value.x() ).arg( value.y() ).arg( value.z() ) );
}

void TextRender::add( const QString& name, Eigen::Vector4f value )
{
  _text.append( name + QString( " ( %1, %2, %3, %4 )" ).arg( value.x() ).arg( value.y() ).arg( value.z() ).arg( value.w() ) );
}
  
void TextRender::add( const QString& name, Eigen::Quaternionf value )
{
  _text.append( name + QString( " ( %1, %2, %3, %4 )" ).arg( value.x() ).arg( value.y() ).arg( value.z() ).arg( value.w() ) );
}

void TextRender::add( const QString& string )
{
  _text.append( string );
}

void TextRender::clear()
{
  _text.clear();
}

QColor TextRender::color()
{
  return _color;
}

void TextRender::setColor( QColor color )
{
  _color = color;
}

void TextRender::setFont( const QString& family, int size )
{
  _font.setFamily   ( family );
  _font.setPointSize( size   );
}

void TextRender::draw( QGLWidget *widget, int x, int y )
{
  widget->qglColor( _color );

  for( int i = 0; i < _text.size(); i++ )
     widget->renderText( x, y + ( _font.pointSize() + 2 )* i, _text[ i ], _font );
  
  widget->qglColor( Qt::white );
}
