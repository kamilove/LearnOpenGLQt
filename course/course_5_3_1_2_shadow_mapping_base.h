#ifndef COURSE_5_3_1_2_SHADOW_MAPPING_BASE_H
#define COURSE_5_3_1_2_SHADOW_MAPPING_BASE_H

#include "course.h"
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include "core/camera.h"
#include "coursewithcamera.h"

class Course_5_3_1_2_shadow_mapping_base : public CourseWithCamera
{
public:
    Course_5_3_1_2_shadow_mapping_base();
    virtual ~Course_5_3_1_2_shadow_mapping_base();

    void render() override;
    virtual QList<QtProperty*> getPropertyList(QtVariantPropertyManager *manager) override;
private:
    void renderScene(QOpenGLShaderProgram &shader);
    void renderCube();
    void renderQuad();
private:
    unsigned int planeVAO, planeVBO;
    unsigned int woodTexture;
    unsigned int depthMapFBO;
    unsigned int depthMap;
    unsigned int cubeVAO;
    unsigned int cubeVBO;
    unsigned int quadVAO;
    unsigned int quadVBO;
    QVector3D lightPos;
    QOpenGLShaderProgram shader, simpleDepthShader, debugDepthQuad;
};

#endif // COURSE_5_3_1_2_SHADOW_MAPPING_BASE_H