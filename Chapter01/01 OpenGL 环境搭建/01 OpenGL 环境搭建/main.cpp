#include "GLShaderManager.h"

#include "GLTools.h"

#include <glut/glut.h>

GLBatch triangleBatch;

GLShaderManager shaderManager;

GLfloat blockSize = 0.1;
GLfloat offsetX = 0.0;
GLfloat offsetY = 0.0;
GLfloat step = 0.1;

//设置四边形顶点
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0,
    blockSize, -blockSize, 0,
    blockSize, blockSize, 0,
    -blockSize, blockSize, 0
};


//窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素

void ChangeSize(int w,int h)

{
    
    glViewport(0,0, w, h);
    
}

//为程序作一次性的设置

void SetupRC()
{
    
    //设置背影颜色
    glClearColor(0.0f,0.0f,1.0f,1.0f);
    
    //初始化着色管理器
    shaderManager.InitializeStockShaders();
    
//    //设置四边形顶点
//    GLfloat vVerts[] = {
//        -0.5, -0.5, 0,
//        0.5, -0.5, 0,
//        0.5, 0.5, 0,
//        -0.5, 0.5, 0
//    };
    
//    //设置三角形，其中数组vVert包含所有3个顶点的x,y,笛卡尔坐标对。
//    GLfloat vVerts[] = {
//        -0.5f,0.0f,0.0f,
//        0.5f,0.0f,0.0f,
//        0.0f,0.5f,0.0f,
//    };
    
    //批次处理
    triangleBatch.Begin(GL_POLYGON,4);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
    
}

//开始渲染

void RenderScene(void)
{
    
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};

    
    M3DMatrix44f mTransformMatrix, mFinalTransform, mRotationMatix;
    
    //平移
    m3dTranslationMatrix44(mTransformMatrix, offsetX, offsetY, 0.0);
    
    static float zRot = 0.0;
    
    //旋转
    m3dRotationMatrix44(mRotationMatix, m3dDegToRad(zRot), 0.0, 0.0, 1.0);
    zRot += 5.0;
    
    //矩阵相乘
    m3dMatrixMultiply44(mFinalTransform, mTransformMatrix, mRotationMatix);
    
    //平面着色器
    shaderManager.UseStockShader(GLT_SHADER_FLAT, mFinalTransform,vRed);
    
//    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
//    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    
}

//键盘处理回调方法
void SpecialKeys(int key, int x, int y) {
    
    switch (key) {
        case GLUT_KEY_UP:
            //向上
            offsetY += step;
            break;
        case GLUT_KEY_DOWN:
            //向下
            offsetY -= step;
            break;
        case GLUT_KEY_LEFT:
            //向左
            offsetX -= step;
            break;
        case GLUT_KEY_RIGHT:
            //向右
            offsetX += step;
            break;
            
        default:
            break;
    }
    
    //边界检测
    if (blockSize+offsetX >= 1.0) {
        offsetX = 1.0 - blockSize;
    }
    
    if (-blockSize+offsetX <= -1.0) {
        offsetX = -1.0+blockSize;
    }
    
    if (blockSize+offsetY >= 1.0) {
        offsetY = 1.0-blockSize;
    }
    
    if (-blockSize+offsetY <= -1.0) {
        offsetY = -1.0+blockSize;
    }
        
    
#if 0
    //通过计算来确定顶点
    vVerts[0] = -blockSize+offsetX;
    vVerts[1] = -blockSize+offsetY;
    
    vVerts[3] = blockSize+offsetX;
    vVerts[4] = -blockSize+offsetY;
    
    vVerts[6] = blockSize+offsetX;
    vVerts[7] = blockSize+offsetY;
    
    vVerts[9] = -blockSize+offsetX;
    vVerts[10] = blockSize+offsetY;
    
    triangleBatch.CopyVertexData3f(vVerts);
#endif
    
    //通过矩阵运算的方式来实现平移，缩放
    
    //会调用刷屏函数RenderScene
    glutPostRedisplay();
    

    
}

int main(int argc,char* argv[])

{
    
    //设置当前工作目录，针对MAC OS X
    
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    
    glutInit(&argc, argv);
    
    /*初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小，标题窗口
    
    glutInitWindowSize(800,600);
    
    glutCreateWindow("Triangle");
    
    //注册回调函数
    
    glutReshapeFunc(ChangeSize);
    
    glutDisplayFunc(RenderScene);
    
    //注册键盘事件
    glutSpecialFunc(SpecialKeys);
    
    //驱动程序的初始化中没有出现任何问题。
    
    GLenum err = glewInit();
    
    if(GLEW_OK != err) {
        
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        
        return 1;
        
    }
    
    //调用SetupRC
    
    SetupRC();
    
    glutMainLoop();
    
    return 0;
    
}

