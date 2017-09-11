#include "sphere.h"

Sphere::Sphere():
    m_VertexBuffer(QOpenGLBuffer::VertexBuffer),
    m_IndexBuffer(QOpenGLBuffer::IndexBuffer),
    m_ColorBuffer(QOpenGLBuffer::VertexBuffer),
    m_rotAngle(0.0),
    m_rotAxis(1.0, 1.0, 0.0)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)),
            this, SLOT(dealWindowsChanged(QQuickWindow*)), Qt::DirectConnection);
}

void Sphere::dealWindowsChanged(QQuickWindow* pWindow) {
    if ( pWindow == Q_NULLPTR ) return;
    connect( pWindow, SIGNAL( beforeRendering( ) ),
             this, SLOT( renderSphere() ), Qt::DirectConnection );
    pWindow->setClearBeforeRendering( false );
}

void Sphere::renderSphere() {
   static bool runOnce = initializeSphere();
   Q_UNUSED(runOnce)

    // 运动
    m_ModelViewMatrix.setToIdentity( );
    m_ModelViewMatrix.translate( 0.0f, 0.0f, -60.0f );
    m_ModelViewMatrix.rotate( m_rotAngle, m_rotAxis.x( ),
                              m_rotAxis.y( ), m_rotAxis.z( ) );

    // 渲染
    glViewport( 0, 0, window( )->width( ), window( )->height( ) );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glFrontFace( GL_CW );

    m_ShaderProgram.bind( );
    m_VertexBuffer.bind( );
    int posLoc = m_ShaderProgram.attributeLocation( "position" );
    m_ShaderProgram.enableAttributeArray( posLoc );
    m_ShaderProgram.setAttributeBuffer( posLoc,                 // 位置
                                        GL_FLOAT,               // 类型
                                        0,                      // 偏移
                                        3,                      // 元大小
                                        0 );                    // 迈

    m_ColorBuffer.bind( );
    int colorLoc = m_ShaderProgram.attributeLocation( "color" );
    m_ShaderProgram.enableAttributeArray( colorLoc );
    m_ShaderProgram.setAttributeBuffer( colorLoc,               // 位置
                                        GL_FLOAT,               // 类型
                                        0,                      // 偏移
                                        4,                      // 元大小
                                        0 );                    // 迈
    m_IndexBuffer.bind( );
    m_ShaderProgram.setUniformValue( "modelViewMatrix", m_ModelViewMatrix );
    m_ShaderProgram.setUniformValue( "projectionMatrix", m_ProjectionMatrix );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, Q_NULLPTR );

    m_ShaderProgram.disableAttributeArray( posLoc );
    m_ShaderProgram.disableAttributeArray( colorLoc );
    m_IndexBuffer.release( );
    m_VertexBuffer.release( );
    m_ShaderProgram.release( );


}

bool Sphere::initializeSphere() {
    // 初始化着色器
    m_ShaderProgram.addShaderFromSourceFile( QOpenGLShader::Vertex,
                                             ":/Shader/Shader.vsh" );
    m_ShaderProgram.addShaderFromSourceFile( QOpenGLShader::Fragment,
                                             ":/Shader/Shader.fsh" );


    bool flag = m_ShaderProgram.link( );

    qWarning() << "shaderProgram flag is " << flag;
    // 初始化顶点缓存
    const GLfloat length = 10.0f;
    const GLfloat vertices[] =
    {
        length, -length, length,
        length, -length, -length,
        -length, -length, -length,
        -length, -length, length,
        length, length, length,
        length, length, -length,
        -length, length, -length,
        -length, length, length
    };

    m_VertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );

    m_VertexBuffer.create( );
    m_VertexBuffer.bind( );
    m_VertexBuffer.allocate( vertices, sizeof( vertices ) );


    // 初始化颜色的缓存
    const GLfloat colors[] =
    {
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f
    };
    m_ColorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_ColorBuffer.create( );
    m_ColorBuffer.bind( );
    m_ColorBuffer.allocate( colors, sizeof( colors ) );


    // 初始化索引缓存
    GLubyte indices[] =
    {
        0, 1, 2, 0, 2, 3,// 下面
        7, 6, 4, 6, 5, 4,// 上面
        7, 4, 3, 4, 0, 3,// 左面
        5, 6, 1, 6, 2, 1,// 右面
        4, 5, 0, 5, 1, 0,// 前面
        3, 2, 6, 3, 6, 7,// 背面
    };

    m_IndexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_IndexBuffer.create( );
    m_IndexBuffer.bind( );
    m_IndexBuffer.allocate( indices, sizeof( indices ) );

    // 设定模型矩阵和投影矩阵
    float aspectRatio  = float( window( )->width( ) ) / float( window( )->height( ) );
    m_ProjectionMatrix.perspective( 45.0f,
                                    aspectRatio,
                                    0.5f,
                                    500.0f );

    connect( window( )->openglContext( ),
             SIGNAL( aboutToBeDestroyed( ) ),
             this, SLOT( releaseBuffer() ),
             Qt::DirectConnection );
    return true;
}

void Sphere::releaseBuffer() {
    m_VertexBuffer.destroy();
    m_IndexBuffer.destroy();
    m_ColorBuffer.destroy();
}

void Sphere::setRotAngle(qreal value) {
    m_rotAngle = value;
    if(window() != Q_NULLPTR) {
        window() -> update();
    }
}

void Sphere::setRotAxis(QVector3D value) {
    m_rotAxis = value;
    if(window() != Q_NULLPTR) {
        window() -> update();
    }
}
