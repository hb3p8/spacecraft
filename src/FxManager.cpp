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

  float linePositions[verticesCount][3] = {
          {-10.0, 10.0, 10.0}, { 10.0, 10.0, 10.0}, { 10.0,-10.0, 10.0}, {-10.0,10.0, -10.0},
          { 10.0, 10.0,-10.0}, {-10.0, 10.0,-10.0}, {-10.0,-10.0,-10.0}, { 10.0,-10.0,-10.0}
  };
}

Lines::Lines(  QString texName, float radius ):
  m_vertexBuffer( QGLBuffer::VertexBuffer ),
  m_verticesCount( 0 ),
  m_data( 64 ),
  m_texName( texName ),
  m_radius( radius )
{
}

void Lines::initialize( QGLWidget* widget )
{
  QImage lineImage( QString( SPACECRAFT_PATH ) + m_texName );
  m_texture = widget->bindTexture( lineImage );

  if ( !prepareShaderProgram( m_shader,
                              QString( SPACECRAFT_PATH ) + "/shaders/geometryLine.vert",
                              QString( SPACECRAFT_PATH ) + "/shaders/geometryLine.frag",
                              QString( SPACECRAFT_PATH ) + "/shaders/geometryLine.geom" ) )
    return;


  m_vertexBuffer.create();
  m_vertexBuffer.setUsagePattern( QGLBuffer::StreamDraw );
  if ( !m_vertexBuffer.bind() )
  {
      qWarning() << "Could not bind vertex buffer to the context";
      return;
  }
  m_vertexBuffer.allocate( m_data.data(), m_data.size() * 3 * sizeof( float ) );

  m_vertexBuffer.release();

  m_verticesCount = verticesCount;
}

void Lines::draw( QMatrix4x4& viewMatrix, QMatrix4x4& projectionMatrix )
{
  if( m_verticesCount == 0 ) return;

  m_vertexBuffer.bind();
  m_vertexBuffer.write( 0, m_data.data(), m_verticesCount * 3 * sizeof( float ) );
  m_vertexBuffer.release();

  glBlendFunc( GL_ONE, GL_ONE ); // additive blending

  glDepthMask( GL_FALSE );

  glBindTexture( GL_TEXTURE_2D, m_texture );

  m_shader.bind();

  m_shader.setUniformValue( "projectionMatrix", projectionMatrix );
  m_shader.setUniformValue( "viewMatrix", viewMatrix );
  m_shader.setUniformValue( "radius", m_radius );
  m_shader.setUniformValue( "gradientTexture", 0 );

  m_vertexBuffer.bind();
  m_shader.setAttributeBuffer( "position", GL_FLOAT, 0, 3 );
  m_vertexBuffer.release();

  m_shader.enableAttributeArray( "position" );
  glDrawArrays( GL_LINES, 0, m_verticesCount );
  m_shader.disableAttributeArray( "position" );

//  m_mesh.drawSimple();

  m_shader.release();

  glBindTexture( GL_TEXTURE_2D, 0 );

  glDepthMask( GL_TRUE );

  m_verticesCount = 0;
}

void Lines::addLine( Eigen::Vector3d p1, Eigen::Vector3d p2 )
{
  addLine( doubleVectorToFloat( p1 ), doubleVectorToFloat( p2 ) );
}

void Lines::addLine( Eigen::Vector3f p1, Eigen::Vector3f p2 )
{
  if( m_verticesCount + 2 >= m_data.size() )
  {
    m_data.resize( m_data.size() * 2 );

    if ( !m_vertexBuffer.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_vertexBuffer.allocate( m_data.data(), m_data.size() * 3 * sizeof( float ) );

    m_vertexBuffer.release();
  }

  m_data[ m_verticesCount ] = p1; m_verticesCount++;
  m_data[ m_verticesCount ] = p2; m_verticesCount++;
}

