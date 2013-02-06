#ifndef GUIMANAGER_HPP
#define GUIMANAGER_HPP

#include <MyGUI_OpenGLPlatform.h>
#include <MyGUI.h>
#include <QObject>
#include "Utils.hpp"

namespace space
{
  namespace gui
  {

    class GUIManager:/* public QObject,*/ public MyGUI::OpenGLImageLoader//, public space::Singleton<GUIManager>
    {
     // Q_OBJECT

    public:
      GUIManager( /*QObject *parent*/ );
      ~GUIManager();

      void onReshape(int nw, int nh);
      void setupResources();

      void createGui();
      void destroyGui();

      void onRender();

      void addResourceLocation(const std::string& _name, bool _recursive = true );
      void setResourceFilename(const std::string& _flename);

      const std::string& getRootMedia();

      void* loadImage(int& _width, int& _height, MyGUI::PixelFormat& _format, const std::string& _filename);
      void saveImage(int _width, int _height, MyGUI::PixelFormat _format, void* _texture, const std::string& _filename);

      void onMousePress( int _absx, int _absy, MyGUI::MouseButton _id );
      void onMouseRelease( int _absx, int _absy, MyGUI::MouseButton _id );
      void onMouseMove( int _absx, int _absy );

      void onKeyPress( MyGUI::KeyCode _key, MyGUI::Char _text );
      void onKeyRelease( MyGUI::KeyCode _key );

      void onWheel( int _value );


    /*internal:*/

      //protected:

//      void injectMouseMove(int _absx, int _absy, int _absz);
//      void injectMousePress(int _absx, int _absy, MyGUI::MouseButton _id);
//      void injectMouseRelease(int _absx, int _absy, MyGUI::MouseButton _id);
//      void injectKeyPress(MyGUI::KeyCode _key, MyGUI::Char _text);
//      void injectKeyRelease(MyGUI::KeyCode _key);

    //private:


    //private:
      MyGUI::Gui* mGUI;
      MyGUI::OpenGLPlatform* mPlatform;

      bool mExit;

      std::string mRootMedia;
      std::string mResourceFileName;
      int m_mx;
      int m_my;
    };


  }

}




#endif
