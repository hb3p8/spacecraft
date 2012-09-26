#include <QtGui/QApplication>
#include "mainwindow.hpp"
#include "GLRenderWidget.hpp"
#include "EditorScene.hpp"
#include "SimulatedScene.hpp"
#include <iostream>
#include <QGLFormat>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool skipNext = false;

    QString fileToOpen( "default.txt" );
    ScenePtr startScenePtr;
    bool useEditorScene = true;
<<<<<<< HEAD
=======
    bool openScene = false;
>>>>>>> octree

    for( int i = 0; i < QApplication::arguments().size(); i++ )
    {
      if( skipNext ) { skipNext = false; continue; }
      QString argument = QApplication::arguments().at( i );

      if( argument == "--help" )
      {
        cout << "Spacecraft can handle these arguments:" << endl;
        cout << "\t--help\n\t\tto print this message" << endl;
        cout << "\t--model <file_name>\n\t\tto load model from file" << endl;
        cout << "\t--export <file_name>\n\t\tto optimize model and save to file" << endl;
        cout << "\t--sim \n\t\tto open model in simulation scene" << endl;
        cout << flush;
        return 0;
      }

      if( argument == "--model" )
      {
        skipNext = true;
        assert( i + 1 < QApplication::arguments().size() );
        fileToOpen = QApplication::arguments().at( i + 1 );
<<<<<<< HEAD
=======
        openScene = false;
      }

      if( argument == "--scene" )
      {
        skipNext = true;
        assert( i + 1 < QApplication::arguments().size() );
        fileToOpen = QApplication::arguments().at( i + 1 );
        openScene = true;
>>>>>>> octree
      }

      if( argument == "--export" )
      {
        skipNext = true;
        assert( i + 1 < QApplication::arguments().size() );

        QString saveFile = QApplication::arguments().at( i + 1 );
        ShipModel model( fileToOpen.toStdString() );
        model.optimize();
        model.saveToFile( saveFile.toStdString() );
        return 0;
      }

      if( argument == "--sim" )
      {
<<<<<<< HEAD
        startScenePtr.reset( new SimulatedScene( fileToOpen ) );
=======
        SimulatedScene* scene = new SimulatedScene();
        startScenePtr.reset( scene );
        if( !openScene )
          scene->addModelFromFile( fileToOpen );
        else
          scene->loadSceneFromFile( fileToOpen );
>>>>>>> octree
        useEditorScene = false;
      }

    }

    if( useEditorScene )
    {
      startScenePtr.reset( new EditorScene( fileToOpen ) );
    }

    MainWindow w;

    QGLFormat glFormat;
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setSampleBuffers( true );

<<<<<<< HEAD
    //startScenePtr.reset( new SimulatedScene( fileToOpen ) );

=======
>>>>>>> octree
    GLRenderWidget* renderWidget = new GLRenderWidget( glFormat, startScenePtr, &w );
    startScenePtr->setWidget( renderWidget );
    w.setRenderWidget( renderWidget );

    w.show();

    
    return a.exec();
}
