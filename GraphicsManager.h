#pragma once
#include "glad/glad.h"

namespace Gm {
    class GraphicsManager {
    public:
        virtual int Initialize();

        virtual void Finalize();

        virtual void Clear();

        virtual void Draw();

    private:
        void InitializeBuffers();

        GLuint shaderProgram;
        GLuint VAO;
        GLuint VBO;
    };
}