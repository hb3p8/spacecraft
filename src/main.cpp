#include <QtGui/QApplication>
#include "mainwindow.hpp"
#include "GLRenderWidget.hpp"
#include <iostream>
#include <QGLFormat>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool skipNext = false;

    QString fileToOpen( "default.txt" );

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
        cout << flush;
      }

      if( argument == "--model" )
      {
        skipNext = true;
        assert( i + 1 < QApplication::arguments().size() );
        fileToOpen = QApplication::arguments().at( i + 1 );
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

    }

    MainWindow w;

    QGLFormat glFormat;
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setSampleBuffers( true );

    GLRenderWidget* renderWidget = new GLRenderWidget( glFormat, &w, fileToOpen );
    w.setRenderWidget( renderWidget );

    w.show();

    
    return a.exec();
}
