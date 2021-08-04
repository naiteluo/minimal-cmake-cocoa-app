#include <cstdio>
#include <iostream>
#include "GraphicsManager.h"

GLenum glCheckError_(int line) {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
//        std::string error;
//        switch (errorCode) {
//            case GL_INVALID_ENUM:
//                error = "INVALID_ENUM";
//                break;
//            case GL_INVALID_VALUE:
//                error = "INVALID_VALUE";
//                break;
//            case GL_INVALID_OPERATION:
//                error = "INVALID_OPERATION";
//                break;
//            case GL_STACK_OVERFLOW:
//                error = "STACK_OVERFLOW";
//                break;
//            case GL_STACK_UNDERFLOW:
//                error = "STACK_UNDERFLOW";
//                break;
//            case GL_OUT_OF_MEMORY:
//                error = "OUT_OF_MEMORY";
//                break;
//            case GL_INVALID_FRAMEBUFFER_OPERATION:
//                error = "INVALID_FRAMEBUFFER_OPERATION";
//                break;
//        }
        fprintf(stderr, "line: %i, errorCode: %i\n", line, errorCode);
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__LINE__)

namespace Asset {
    const char *vertexShaderSource = "#version 330 core\n"
                                     "in vec3 vertexPosition;\n"
                                     "in vec3 vertexColor;\n"
                                     "out vec3 fragmentColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0);\n"
                                     "   fragmentColor = vertexColor;\n"
                                     "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "in vec3 fragmentColor;\n"
                                       "out vec3 color;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   color = fragmentColor;\n"
                                       "}\n\0";

    // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
            -1.0f, -1.0f, -1.0f, // triangle 1 : begin
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, // triangle 1 : end
            1.0f, 1.0f, -1.0f, // triangle 2 : begin
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f, // triangle 2 : end
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };

    // One color for each vertex. They were generated randomly.
    static const GLfloat g_color_buffer_data[] = {
            0.583f, 0.771f, 0.014f,
            0.609f, 0.115f, 0.436f,
            0.327f, 0.483f, 0.844f,
            0.822f, 0.569f, 0.201f,
            0.435f, 0.602f, 0.223f,
            0.310f, 0.747f, 0.185f,
            0.597f, 0.770f, 0.761f,
            0.559f, 0.436f, 0.730f,
            0.359f, 0.583f, 0.152f,
            0.483f, 0.596f, 0.789f,
            0.559f, 0.861f, 0.639f,
            0.195f, 0.548f, 0.859f,
            0.014f, 0.184f, 0.576f,
            0.771f, 0.328f, 0.970f,
            0.406f, 0.615f, 0.116f,
            0.676f, 0.977f, 0.133f,
            0.971f, 0.572f, 0.833f,
            0.140f, 0.616f, 0.489f,
            0.997f, 0.513f, 0.064f,
            0.945f, 0.719f, 0.592f,
            0.543f, 0.021f, 0.978f,
            0.279f, 0.317f, 0.505f,
            0.167f, 0.620f, 0.077f,
            0.347f, 0.857f, 0.137f,
            0.055f, 0.953f, 0.042f,
            0.714f, 0.505f, 0.345f,
            0.783f, 0.290f, 0.734f,
            0.722f, 0.645f, 0.174f,
            0.302f, 0.455f, 0.848f,
            0.225f, 0.587f, 0.040f,
            0.517f, 0.713f, 0.338f,
            0.053f, 0.959f, 0.120f,
            0.393f, 0.621f, 0.362f,
            0.673f, 0.211f, 0.457f,
            0.820f, 0.883f, 0.371f,
            0.982f, 0.099f, 0.879f
    };
}

bool Gm::GraphicsManager::InitializeProgram() {
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &Asset::vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &Asset::fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // TODO optional?
    glBindAttribLocation(shaderProgram, 0, "vertexPosition");
    glBindAttribLocation(shaderProgram, 1, "vertexColor");

    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void Gm::GraphicsManager::InitializeBuffers() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // Allocate adn assign a Vertex Array Object to out handle
    glGenVertexArrays(1, &VAO);
    // Bind our VAO as the **current used** object
    glBindVertexArray(VAO);

    // Allocate and assign two Vertex Buffer Object to our handle(handles array)
    glGenBuffers(2, VBOs);

    // Section 1 of processing VBO 1 start

    // Bind out first VBO as being the **active** buffer and storing vertex attributes (coordinates)
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);

    // Copy the vertex data from g_vertex_buffer_data to our active buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Asset::g_vertex_buffer_data), Asset::g_vertex_buffer_data, GL_STATIC_DRAW);

    // Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex
    // see code in vertex shader: `layout (location = 0) in vec3 vertexPosition;`
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    // Enable attribute index 0 ad being used
    glEnableVertexAttribArray(0);

    // Section 1 of processing VBO 1 end

    // Section 2 of processing VBO 2 start

    // Bind our second VBO ad being the **active** buffer and storing vertex attributes (colors)
    // BTW, in opengl programming, splitting attributes of a vertex into arrays instead of combine attributes into a struct,
    // which call SOA modeling, is much more friendly to gpu than the normal AOS modeling way.
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Asset::g_color_buffer_data), Asset::g_color_buffer_data, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    glEnableVertexAttribArray(1);

    // Section 2 of processing VBO 2 end
}

int Gm::GraphicsManager::Initialize() {
    int result;
    result = gladLoadGL();
    if (!result) {
        printf("OpenGL load failed!\n");
        result = -1;
    } else {
        result = 0;
        printf("OpenGL Version %d.%d loaded\n", GLVersion.major, GLVersion.minor);
        if (GLAD_GL_VERSION_3_0) {
            // Set the depth buffer to be entirely cleared to 1.0 values.
            glClearDepth(1.0f);
            // Enable depth testing.
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            // Set the polygon winding to front facing for the right handed system.
            glFrontFace(GL_CW);
            // Enable back face culling.
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
//            glDebugMessageCallbackARB(MessageCallback, 0);
        }
        InitializeBuffers();
        result = InitializeProgram();
    }
    return result;
}

void Gm::GraphicsManager::Finalize() {
    // TODO properly dispose
    glUseProgram(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(2, VBOs);
    glDeleteVertexArrays(1, &VAO);
}

void Gm::GraphicsManager::Clear() {
    // Set the color to clear the screen to.
    glClearColor(0.8f, 0.3f, 0.4f, 1.0f);
    // Clear the screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Gm::GraphicsManager::Draw() {
    glUseProgram(shaderProgram);
    glCheckError();
    // seeing as we only have a single VAO there's no need to bind it every time,
    // but we'll do so to keep things a bit more organized
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    glFlush();
}
