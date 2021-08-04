#pragma once

#include "glad/glad.h"
#include "Eigen/Core"

namespace Gm {
    class GraphicsManager {
    public:
        virtual int Initialize();

        virtual void Finalize();

        virtual void Clear();

        virtual void Draw();


    private:
        void InitializeBuffers();

        bool InitializeProgram();

        void CalculateCameraPosition();


    private:

        // handle to the shader program
        GLuint shaderProgram;
        // handle for Vertex Array Object
        GLuint VAO;
        // handles for Vertex Buffer Object
        GLuint VBOs[2];

        Eigen::Matrix4Xf m_worldMatrix;
        Eigen::Matrix4Xf m_viewMatrix;
        Eigen::Matrix4Xf m_projectionMatrix;

        float m_positionX = 0, m_positionY = 0, m_positionZ = -10;
        float m_rotationX = 0, m_rotationY = 0, m_rotationZ = 0;

        const float screenDepth = 1000.0f;
        const float screenNear = 0.1f;
    };
}