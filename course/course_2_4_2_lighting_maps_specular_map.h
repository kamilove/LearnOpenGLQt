#ifndef COURSE_2_4_2_LIGHTING_MAPS_SPECULAR_MAP_H
#define COURSE_2_4_2_LIGHTING_MAPS_SPECULAR_MAP_H

#include "course.h"
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include "core/camera.h"
#include "coursewithcamera.h"

class Course_2_4_2_lighting_maps_specular_map : public CourseWithCamera
{
public:
    Course_2_4_2_lighting_maps_specular_map();
    virtual ~Course_2_4_2_lighting_maps_specular_map();

    void render() override;
    virtual QList<QtProperty*> getPropertyList(QtVariantPropertyManager *manager) override;

private:
    unsigned int VBO, cubeVAO;
    unsigned int lightCubeVAO;
    unsigned int diffuseMap, specularMap;
    QVector3D lightPos;

    QOpenGLShaderProgram mLightingProgram, mLightCubeProgram;
};

#endif // COURSE_2_4_2_LIGHTING_MAPS_SPECULAR_MAP_H
