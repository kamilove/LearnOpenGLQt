#include "course_1_7_3_camera_mouse_zoom.h"
#include <QDir>
#include <QDebug>
#include "stb_image.h"
#include <QMouseEvent>
#include <qmath.h>

Course_1_7_3_camera_mouse_zoom::Course_1_7_3_camera_mouse_zoom()
    :Course()
    ,mCameraPos(QVector3D(0.0f, 0.0f,  3.0f))
    ,mCameraFront(QVector3D(0.0f, 0.0f, -1.0f))
    ,mCameraUp(QVector3D(0.0f, 1.0f,  0.0f))
    ,mMousePressed(true)
    ,mYaw(-90.0f)
    ,mPitch(0.0f)
    ,mLastX(400.0f)
    ,mLastY(300.0f)
    ,mFov(45.0f)
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    QString shaderPath = QDir::currentPath() + "/shaders/1.getting_started/7.3.camera_mouse_zoom/";
    mProgram = new QOpenGLShaderProgram();
    mProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,shaderPath+"7.3.camera.vs");
    mProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,shaderPath+"7.3.camera.fs");
    mProgram->link();

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    QString texturePath = QDir::currentPath() + "/resources/textures/container.jpg";
    unsigned char *data = stbi_load(texturePath.toStdString().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        qDebug() << "Failed to load texture:" << texturePath;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    texturePath = QDir::currentPath() + "/resources/textures/awesomeface.png";
    data = stbi_load(texturePath.toStdString().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        qDebug() << "Failed to load texture:" << texturePath;
    }
    stbi_image_free(data);

    mProgram->bind();
    glUniform1i(glGetUniformLocation(mProgram->programId(), "texture1"), 0);
    glUniform1i(glGetUniformLocation(mProgram->programId(), "texture2"), 1);
    mProgram->release();

    stbi_set_flip_vertically_on_load(false);
}

Course_1_7_3_camera_mouse_zoom::~Course_1_7_3_camera_mouse_zoom()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    mProgram->release();
    delete mProgram;
}

void Course_1_7_3_camera_mouse_zoom::render()
{
    Course::render();

    processInput();

    // world space positions of our cubes
    QVector3D cubePositions[] = {
        QVector3D( 0.0f,  0.0f,  0.0f),
        QVector3D( 2.0f,  5.0f, -15.0f),
        QVector3D(-1.5f, -2.2f, -2.5f),
        QVector3D(-3.8f, -2.0f, -12.3f),
        QVector3D (2.4f, -0.4f, -3.5f),
        QVector3D(-1.7f,  3.0f, -7.5f),
        QVector3D( 1.3f, -2.0f, -2.5f),
        QVector3D( 1.5f,  2.0f, -2.5f),
        QVector3D( 1.5f,  0.2f, -1.5f),
        QVector3D(-1.3f,  1.0f, -1.5f)
    };

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // activate shader
    mProgram->bind();

    unsigned int modelLoc = glGetUniformLocation(mProgram->programId(), "model");
    unsigned int viewLoc  = glGetUniformLocation(mProgram->programId(), "view");
    unsigned int projectionLoc  = glGetUniformLocation(mProgram->programId(), "projection");
    mProjection.setToIdentity();
    mProjection.perspective((mFov), 800 / 600, 0.1, 100);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, mProjection.data());

    // camera/view transformation
    mView.setToIdentity();
    mView.lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, mView.data());

    // render boxes
    glBindVertexArray(VAO);
    for (unsigned int i = 0; i < 10; i++)
    {
        mModel.setToIdentity();
        mModel.translate(cubePositions[i]);
        float angle = 20.0f * i;
        mModel.rotate(angle, QVector3D(1.0f, 0.3f, 0.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, mModel.data());
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
    mProgram->release();
}

void Course_1_7_3_camera_mouse_zoom::mousePressEvent(QMouseEvent *event)
{
    mMousePressed = true;
    QPoint pos = event->pos();
    mLastX = pos.x();
    mLastY = pos.y();
}

void Course_1_7_3_camera_mouse_zoom::mouseReleaseEvent(QMouseEvent *event)
{
    mMousePressed = false;
}

void Course_1_7_3_camera_mouse_zoom::mouseMoveEvent(QMouseEvent *event)
{
    if (mMousePressed){
        QPoint pos = event->pos();
        int xpos = pos.x();
        int ypos = pos.y();
        float xoffset = xpos - mLastX;
        float yoffset = ypos - mLastY; // reversed since y-coordinates go from bottom to top
        mLastX = xpos;
        mLastY = ypos;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        mYaw += xoffset;
        mPitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (mPitch > 89.0f)
            mPitch = 89.0f;
        if (mPitch < -89.0f)
            mPitch = -89.0f;

        QVector3D front;
        front.setX(cos(qDegreesToRadians(mYaw)) * cos(qDegreesToRadians(mPitch)));
        front.setY(sin(qDegreesToRadians(mPitch)));
        front.setZ(sin(qDegreesToRadians(mYaw)) * cos(qDegreesToRadians(mPitch)));
        mCameraFront = front;
    }
}

void Course_1_7_3_camera_mouse_zoom::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->angleDelta() / 120;
    mFov -= numPixels.y();
    if (mFov < 1.0f)
        mFov = 1.0f;
    if (mFov > 45.0f)
        mFov = 45.0f;
}

void Course_1_7_3_camera_mouse_zoom::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W){
        mPressedKey.insert(Qt::Key_W);
    }else if(event->key() == Qt::Key_S){
        mPressedKey.insert(Qt::Key_S);
    }else if(event->key() == Qt::Key_A){
        mPressedKey.insert(Qt::Key_A);
    }else if(event->key() == Qt::Key_D){
        mPressedKey.insert(Qt::Key_D);
    }

    Course::keyPressEvent(event);
}

void Course_1_7_3_camera_mouse_zoom::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W){
        mPressedKey.remove(Qt::Key_W);
    }else if(event->key() == Qt::Key_S){
        mPressedKey.remove(Qt::Key_S);
    }else if(event->key() == Qt::Key_A){
        mPressedKey.remove(Qt::Key_A);
    }else if(event->key() == Qt::Key_D){
        mPressedKey.remove(Qt::Key_D);
    }

    Course::keyReleaseEvent(event);
}

QList<QtProperty *> Course_1_7_3_camera_mouse_zoom::getPropertyList(QtVariantPropertyManager *manager)
{
    GETPROPERTYLIST(Course_1_7_3_camera_mouse_zoom)
}

void Course_1_7_3_camera_mouse_zoom::processInput()
{
    float cameraSpeed = 2.5 * mDelta;

    if (mPressedKey.contains(Qt::Key_W)){
        mCameraPos += mCameraFront * cameraSpeed;
    }
    if (mPressedKey.contains(Qt::Key_S)){
        mCameraPos -= mCameraFront* cameraSpeed;
    }
    if (mPressedKey.contains(Qt::Key_A)){
        mCameraPos -= QVector3D::crossProduct(mCameraFront, mCameraUp) * cameraSpeed;
    }
    if (mPressedKey.contains(Qt::Key_D)){
        mCameraPos += QVector3D::crossProduct(mCameraFront, mCameraUp) * cameraSpeed;
    }
}
