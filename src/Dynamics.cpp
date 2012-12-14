#include "Dynamics.hpp"
#include "Utils.hpp"

using namespace Eigen;
using namespace space;

void Beam::process( float deltaTime )
{
  m_position += m_velocity * deltaTime;
}

void Beam::draw( spacefx::Manager &fxmanager )
{
  spacefx::Lines* linesEffect = dynamic_cast< spacefx::Lines* >( fxmanager.getEffect( "beam" ).get() );

  Vector3d direction = m_velocity.normalized();
  if( linesEffect )
    linesEffect->addLine( (Vector3d)( direction + m_position ),
                          (Vector3d)( m_position - direction ) );
}

BaseSceneObjectPtr DynamicsFabric::create( QString objectType )
{
  if( objectType == "beam" )
    return BaseSceneObjectPtr( new Beam );

  return BaseSceneObjectPtr( 0 );
}


