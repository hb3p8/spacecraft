#ifndef TEXTRENDER_HPP
#define TEXTRENDER_HPP

#include <Eigen/Geometry>

#include <QStringList>
#include <QFont>
#include <QGLWidget>

//! Class for rendering text in OpenGL widgets
class TextRender : QObject
{
Q_OBJECT

public:
  TextRender();
  //! Constructor
  TextRender( const QString& fontFamily, int fontSize, QColor textColor );

  //! Sets using font
  void setFont( const QString& textFamily, int textSize );

  //! Adds line of text
  void add( const QString& text );

  //! Adds line of text
  /*! Will be represented as "name value"
   */

  void add( const QString& name, int value );

  void add( const QString& name, size_t value );

  void add( const QString& name, GLfloat value );
  //! Adds line of text
  /*! Will be represented as "name ( value.x(), value.y() )"
   */
  void add( const QString& name, Eigen::Vector2f value );

  //! Adds line of text
  /*! Will be represented as "name ( value.x(), value.y(), value.z() )"
   */
  void add( const QString& name, Eigen::Vector3f value );

  //! Adds line of text
  /*! Will be represented as "name ( value.x(), value.y(), value.z(), value.w() )"
   */
  void add( const QString& name, Eigen::Vector4f value );

  //! Adds line of text
  void add( const QString& name, Eigen::Quaternionf value );

  //! Clears text data
  void clear();

  //! Draws text data in QGLWidget object
  void draw( QGLWidget *widget, int x, int y );

  //! Color used for rendering text
  QColor color();

  //! Sets color for rendering text
  void setColor( QColor color );

private:
  QFont       _font;
  QStringList _text;

  QColor _color;

};

#endif
