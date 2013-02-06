#include "GUIManager.hpp"
#include <QImage>
#include <MyGUI_RenderFormat.h>

using namespace space::gui;

GUIManager::GUIManager( /*QObject *parent */) : //QObject( parent ), //Singleton<GUIManager>( *this ),
  mGUI(nullptr),
  mPlatform(nullptr),
  mExit(false),
  mResourceFileName("MyGUI_Core.xml")
{

}

GUIManager::~GUIManager()
{
}

void GUIManager::onReshape(int nw, int nh)
{
  if (mPlatform)
    mPlatform->getRenderManagerPtr()->setViewSize(nw, nh);
}

void GUIManager::setupResources()
{
  MyGUI::xml::Document doc;

  if (!doc.open(std::string("resources.xml")))
    doc.getLastError();

  MyGUI::xml::ElementPtr root = doc.getRoot();
  if (root == nullptr || root->getName() != "Paths")
    return;

  MyGUI::xml::ElementEnumerator node = root->getElementEnumerator();
  while (node.next())
  {
    if (node->getName() == "Path")
    {
      bool root = false;
      if (node->findAttribute("root") != "")
      {
        root = MyGUI::utility::parseBool(node->findAttribute("root"));
        if (root) mRootMedia = node->getContent();
      }
      addResourceLocation(node->getContent(), false);
    }
  }

  addResourceLocation(mRootMedia + "/Common/Base");
}

void GUIManager::createGui()
{
  mPlatform = new MyGUI::OpenGLPlatform();
  mPlatform->initialise(this);

  setupResources();

  mGUI = new MyGUI::Gui();
  mGUI->initialise(mResourceFileName);
}

void GUIManager::destroyGui()
{
  if (mGUI)
  {

    mGUI->shutdown();
    delete mGUI;
    mGUI = nullptr;
  }

  if (mPlatform)
  {
    mPlatform->shutdown();
    delete mPlatform;
    mPlatform = nullptr;
  }
}

void GUIManager::addResourceLocation(const std::string& _name, bool _recursive)
{
  mPlatform->getDataManagerPtr()->addResourceLocation(_name, _recursive);
}

const std::string& GUIManager::getRootMedia()
{
  return mRootMedia;
}

void GUIManager::setResourceFilename(const std::string& _flename)
{
  mResourceFileName = _flename;
}

void GUIManager::onRender()
{
  if (mPlatform)
    mPlatform->getRenderManagerPtr()->drawOneFrame();
}

void* GUIManager::loadImage(int &_width, int &_height, MyGUI::PixelFormat &_format, const std::string &_filename)
{
  std::string filename = mPlatform->getDataManagerPtr()->getDataPath( _filename );
  QImage image( QString( filename.c_str() ) );
  _width = image.width();
  _height = image.height();
  _format = MyGUI::PixelFormat(MyGUI::PixelFormat::R8G8B8A8);

  size_t size = image.byteCount();
  const void* imageData = image.constBits();
  unsigned char* data = new unsigned char[size];
  memcpy(data, imageData, size);

  return data;
}

void GUIManager::saveImage(int _width, int _height, MyGUI::PixelFormat _format, void *_texture, const std::string &_filename)
{
}

void GUIManager::onMousePress(int _absx, int _absy, MyGUI::MouseButton _id)
{
  if (!mGUI)
    return;

  MyGUI::InputManager::getInstance().injectMousePress(_absx, _absy, _id);
}

void GUIManager::onMouseRelease(int _absx, int _absy, MyGUI::MouseButton _id)
{
  if (!mGUI)
    return;

  MyGUI::InputManager::getInstance().injectMouseRelease(_absx, _absy, _id);
}

void GUIManager::onMouseMove(int _absx, int _absy)
{
  if (!mGUI)
    return;

  MyGUI::InputManager::getInstance().injectMouseMove(_absx, _absy, 0);
  m_mx = _absx;
  m_my = _absy;
}

void GUIManager::onKeyPress(MyGUI::KeyCode _key, MyGUI::Char _text)
{
}

void GUIManager::onKeyRelease(MyGUI::KeyCode _key)
{
}

void GUIManager::onWheel(int _value)
{
  if (!mGUI)
    return;

  MyGUI::InputManager::getInstance().injectMouseMove(m_mx, m_my, _value);
}












