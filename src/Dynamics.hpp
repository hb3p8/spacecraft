#ifndef DYNAMICS_HPP
#define DYNAMICS_HPP

#include "BaseSceneObject.hpp"

namespace space
{

  class Beam : public BaseSceneObject
  {
  public:
    void refreshModel() {}
    void process( float deltaTime );
    void draw( spacefx::Manager& fxmanager );
    void attachShader( QGLShaderProgram& shader ){}


  };

  class DynamicsFabric
  {
  public:
    static BaseSceneObjectPtr create( QString objectType );
  };

}




#endif
