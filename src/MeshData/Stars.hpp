#ifndef STARS_HPP
#define STARS_HPP

#include <Eigen/Eigen>

class StarBuilder
{

public:
  StarBuilder(): m_vertices( 0 ), m_texcoords( 0 ) {}

  ~StarBuilder()
  {
    if( m_vertices != 0 )
      delete[] m_vertices;
    if( m_texcoords != 0 )
      delete[] m_texcoords;
  }

  void buildStarMesh( int starCount = 800, float patchSize = 0.8, float patchDist = 10. )
  {
    size_t verticesSize = starCount * 6 * 3;
    size_t texcoordsSize = starCount * 6 * 2;

    m_vertices = new float[ verticesSize ];
    m_texcoords = new float[ texcoordsSize ];

    std::vector< Eigen::Vector2f > quadTex;
    quadTex.push_back( Eigen::Vector2f( 0.0, 1.0 ) );
    quadTex.push_back( Eigen::Vector2f( 1.0, 1.0 ) );
    quadTex.push_back( Eigen::Vector2f( 1.0, 0.0 ) );
    quadTex.push_back( Eigen::Vector2f( 0.0, 0.0 ) );


    int indices[] = { 0, 3, 1, 1, 3, 2 };
    int vertCounter = 0;

    int ranseq = 123;
    for ( int s = 0; s < starCount; s++ )
    {
      // get a random point, normalize for center of patch
      Eigen::Vector3f eye;

      int p = (ranseq++) * 17;
      p = (p<<13) ^ p;
      eye.x() = 1.0 - ((p * (p * p * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
      eye.y() = 1.0 - ((p * (p * p * 14983 + 825193) + 1376312589) & 0x7fffffff) / 1073741824.0;
      eye.z() = 1.0 - ((p * (p * p * 23431 + 830237) + 1376312589) & 0x7fffffff) / 1073741824.0;
      eye.normalize();

      // get a second random point
      Eigen::Vector3f xaxis;

      p = (ranseq++) * 17;
      p = (p<<13) ^ p;
      xaxis.x() = 1.0 - ((p * (p * p * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
      xaxis.y() = 1.0 - ((p * (p * p * 14983 + 825193) + 1376312589) & 0x7fffffff) / 1073741824.0;
      xaxis.z() = 1.0 - ((p * (p * p * 23431 + 830237) + 1376312589) & 0x7fffffff) / 1073741824.0;

      // cross with eye to get two axis
      xaxis = xaxis.cross(eye);
      xaxis.normalize();
      Eigen::Vector3f yaxis(xaxis);
      yaxis = yaxis.cross(eye);

      // randomly flip the axis to cut down on visible repetition
      p = (ranseq++) * 17;
      p = (p<<13) ^ p;
      bool xflip = 0 < (1.0 - ((p * (p * p * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
      bool yflip = 0 < (1.0 - ((p * (p * p * 14983 + 825193) + 1376312589) & 0x7fffffff) / 1073741824.0);

      // build corner points
      xaxis *= patchSize;
      if (xflip)
        xaxis *= -1;
      yaxis *= patchSize;
      if (yflip)
        yaxis *= -1;
      eye *= 0.49;

      std::vector< Eigen::Vector3f > quadVerts;
      quadVerts.push_back( eye );
      quadVerts[ quadVerts.size() - 1 ] -= xaxis;
      quadVerts[ quadVerts.size() - 1 ] += yaxis;

      quadVerts.push_back( eye );
      quadVerts[ quadVerts.size() - 1 ] += xaxis;
      quadVerts[ quadVerts.size() - 1 ] += yaxis;

      quadVerts.push_back( eye );
      quadVerts[ quadVerts.size() - 1 ] += xaxis;
      quadVerts[ quadVerts.size() - 1 ] -= yaxis;

      quadVerts.push_back( eye );
      quadVerts[ quadVerts.size() - 1 ] -= xaxis;
      quadVerts[ quadVerts.size() - 1 ] -= yaxis;


      for( int i = 0; i < 6; i++ )
      {
        int idx = indices[ i ];
        float* vertex = m_vertices + vertCounter * 3;
        Eigen::Vector3f current = quadVerts[ idx ] + eye * patchDist;
        vertex[ 0 ] = current.x();
        vertex[ 1 ] = current.y();
        vertex[ 2 ] = current.z();

        float* tex = m_texcoords + vertCounter * 2;
        tex[ 0 ] = quadTex[ idx ].x();
        tex[ 1 ] = quadTex[ idx ].y();

        vertCounter++;
      }

    }

    m_verticesCount = vertCounter;

  }

  float* getVertices() { return m_vertices; }
  float* getTexcoords() { return m_texcoords; }
  int getVerticeCount() { return m_verticesCount; }

private:

  float* m_vertices;
  float* m_texcoords;

  int m_verticesCount;

};

#endif // STARS_HPP
