#include "FxManager.hpp"
#include "Utils.hpp"

#include <QGLWidget>

using namespace spacefx;

Manager::Manager()
{
}

void Manager::registerEffect( const QString& id, EffectTypePtr effect )
{
  m_effects.insert( id, effect );
}

EffectTypePtr Manager::getEffect( const QString &id )
{
  EffectMap::iterator i;
  EffectTypePtr result;

  i = m_effects.find( id );
  if( i != m_effects.end() )
  {
    result = i.value();
  }

  return result;
}

void Manager::drawEffects( QMatrix4x4& viewMatrix, QMatrix4x4& projectionMatrix )
{
  for( EffectTypePtr effect: m_effects )
  {
    effect->draw( viewMatrix, projectionMatrix );
  }
}

namespace spacefx
{
  static const uint32_t verticesCount = 8;

  const float linePositions[verticesCount][3] = {
          {-10.0, 10.0, 10.0}, { 10.0, 10.0, 10.0}, { 10.0,-10.0, 10.0}, {-10.0,10.0, -10.0},
          { 10.0, 10.0,-10.0}, {-10.0, 10.0,-10.0}, {-10.0,-10.0,-10.0}, { 10.0,-10.0,-10.0}
  };
}

void Lines::initialize( QGLWidget* widget )
{
  QImage lineImage( QString( SPACECRAFT_PATH ) + "/images/grad3.bmp" );
  m_texture = widget->bindTexture( lineImage );

  if ( !prepareShaderProgram( m_shader,
                              QString( SPACECRAFT_PATH ) + "/shaders/geometryLine.vert",
                              QString( SPACECRAFT_PATH ) + "/shaders/geometryLine.frag",
                              QString( SPACECRAFT_PATH ) + "/shaders/geometryLine.geom" ) )
    return;


  m_mesh.writePositions( linePositions, verticesCount );
  m_mesh.attachShader( m_shader );
  m_mesh.setMode( GL_LINES );
}

void Lines::draw( QMatrix4x4& viewMatrix, QMatrix4x4& projectionMatrix )
{
  glBlendFunc( GL_ONE, GL_ONE ); // additive blending

  glDepthMask( GL_FALSE );

  glBindTexture( GL_TEXTURE_2D, m_texture );

  m_shader.bind();

  m_shader.setUniformValue( "projectionMatrix", projectionMatrix );
  m_shader.setUniformValue( "viewMatrix", viewMatrix );
  m_shader.setUniformValue( "radius", (float) 1.5 );
  m_shader.setUniformValue( "gradientTexture", 0 );

  m_mesh.drawSimple();

  m_shader.release();

  glBindTexture( GL_TEXTURE_2D, 0 );

  glDepthMask( GL_TRUE );
}

void Lines::addLine( Eigen::Vector3f p1, Eigen::Vector3f p2 )
{
}

