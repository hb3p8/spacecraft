#ifndef SCENE_HPP
#define SCENE_HPP

#include <QObject>
#include <QMap>
#include <QKeyEvent>

#include <memory>

typedef QMap< Qt::Key, bool > InputMap;

class Scene : public QObject
{
    Q_OBJECT
public:
    explicit Scene( QObject *parent = 0 );

    virtual void initialize() = 0;
    virtual void draw() = 0;

    virtual void process( int newTime ) = 0;

    virtual void viewportResize( int width, int height ) = 0;

    InputMap& getInputMap() { return m_inputMap; }

    virtual void keyPressEvent( QKeyEvent* e ) = 0;
    virtual void mouseMoveEvent( QMouseEvent* e ) = 0;
    virtual void mousePressEvent( QMouseEvent* e ) = 0;
    virtual void wheelEvent( QWheelEvent* e ) = 0;

protected:

    InputMap m_inputMap;

};

typedef std::shared_ptr< Scene > ScenePtr;

#endif // SCENE_HPP
