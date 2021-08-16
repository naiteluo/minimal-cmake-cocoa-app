#include <cstdio>
#include <iostream>
#include "GraphicsManager.h"

GLenum glCheckError_(int line) {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        fprintf(stderr, "line: %i, errorCode: %i\n", line, errorCode);
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__LINE__)

using namespace Eigen;

namespace Asset {

    static const float Width = 960;
    static const float Height = 540;

    static const float MinPositionZ = -50;
    static const float MaxPositionZ = -4;
    static const float DefaultPositionZ = -10;
    static const float DefaultRotationAngle = 45;

    const char *vertexShaderSource = "#version 330 core\n"
                                     "in vec3 vertexPosition;\n"
                                     "in vec3 vertexColor;\n"
                                     "out vec3 fragmentColor;\n"
                                     "uniform mat4 worldMatrix;\n"
                                     "uniform mat4 viewMatrix;\n"
                                     "uniform mat4 projectionMatrix;\n"
                                     "void main()\n"
                                     "{\n"
                                     //                                     "   gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(vertexPosition, 1.0f);\n"
                                     "   gl_Position = worldMatrix * vec4(vertexPosition, 1.0f);\n"
                                     "   gl_Position = viewMatrix * gl_Position;\n"
                                     "   gl_Position = projectionMatrix * gl_Position;\n"
                                     "   fragmentColor = vertexColor;\n"
                                     "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "in vec3 fragmentColor;\n"
                                       "out vec4 color;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   color = vec4(fragmentColor, 1.0f);\n"
                                       "}\n\0";

    struct VertexType {
        Vector3f position;
        Vector3f color;
    };

    static const VertexType g_vertex_buffer_data[] = {
            {{1.0f,  1.0f,  1.0f},  {1.0f, 0.0f, 0.0f}},
            {{1.0f,  1.0f,  -1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, 1.0f,  -1.0f}, {0.0f, 0.0f, 1.0f}},
            {{-1.0f, 1.0f,  1.0f},  {1.0f, 1.0f, 0.0f}},
            {{1.0f,  -1.0f, 1.0f},  {1.0f, 0.0f, 1.0f}},
            {{1.0f,  -1.0f, -1.0f}, {0.0f, 1.0f, 1.0f}},
            {{-1.0f, -1.0f, -1.0f}, {0.5f, 1.0f, 0.5f}},
            {{-1.0f, -1.0f, 1.0f},  {1.0f, 0.5f, 1.0f}},
    };

    static const uint16_t g_indices_buffer_data[] = {1, 2, 3, 3, 2, 6, 6, 7, 3, 3, 0, 1, 0, 3, 7, 7, 6, 4, 4, 6, 5, 0,
                                                     7, 4, 1, 0, 4, 1, 4, 5, 2,
                                                     1, 5, 2, 5, 6};

    static const int m_vertex_count = sizeof(g_vertex_buffer_data) / sizeof(VertexType);
    static const int m_index_count = sizeof(g_indices_buffer_data) / sizeof(uint16_t);
}

void BuildPerspectiveFovLHMatrix(Matrix4f &matrix, const float fieldOfView, const float screenAspect,
                                 const float screenNear, const float screenDepth) {
    matrix << 1.0f / (screenAspect * tanf(fieldOfView * 0.5f)), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / tanf(fieldOfView * 0.5f), 0.0f, 0.0f,
            0.0f, 0.0f, screenDepth / (screenDepth - screenNear), 1.0f,
            0.0f, 0.0f, (-screenNear * screenDepth) / (screenDepth - screenNear), 0.0f;
    // eigen matrix are row-based by default
    // opengl accepts col-based matrix data by default
    // eigen's matrix had a data method to return col-based data.
    // the matrix inputs before are col-based (copy from MakiEngine)
    // so we transpose the data from "col-based" form to "row-based"
    // so that we can use matrix's data method to pass data to opengl api
    matrix.transposeInPlace();
    return;
}

void BuildViewMatrix(Matrix4f &result, const Vector3f position, const Vector3f lookAt, const Vector3f up) {
    Vector3f zAxis, xAxis, yAxis;
    float result1, result2, result3;

    zAxis = lookAt - position;
    zAxis.normalize();

    xAxis = up.cross(zAxis);
    xAxis.normalize();

    yAxis = zAxis.cross(xAxis);

    result1 = -(xAxis.dot(position));
    result2 = -(yAxis.dot(position));
    result3 = -(zAxis.dot(position));

    result << xAxis[0], yAxis[0], zAxis[0], 0.0f,
            xAxis[1], yAxis[1], zAxis[1], 0.0f,
            xAxis[2], yAxis[2], zAxis[2], 0.0f,
            result1, result2, result3, 1.0f;
    // see `BuildPerspectiveFovLHMatrix`
    result.transposeInPlace();
    return;
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

/**
 * create&bind VAP and VBO
 */
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Asset::VertexType), 0);
    // offset to color portion
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, sizeof(Asset::VertexType), (float *) NULL + 3);

    // Enable attribute index 0 ad being used
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Section 1 of processing VBO 1 end

    // Section 2 of processing VBO 2 start

    // Bind our second VBO ad being the **active** buffer and storing index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Asset::g_indices_buffer_data), Asset::g_indices_buffer_data,
                 GL_STATIC_DRAW);
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

            // Set the polygon winding to front facing for the right-handed system.
            glFrontFace(GL_CW);

            // Enable back face culling.
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            // Initialize the model matrix to the identity matrix.
            m_worldMatrix = Matrix4f::Identity();
            InitializePerspectiveMatrix();
        }
        result = InitializeProgram();
        InitializeBuffers();
    }
    return result;
}

void Gm::GraphicsManager::Finalize() {
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
    UpdateModelMatrix();
    UpdateCameraViewMatrix();

    glUseProgram(shaderProgram);
    glCheckError();
    SetShaderParameters(m_worldMatrix.data(), m_viewMatrix.data(), m_projectionMatrix.data());
    // seeing as we only have a single VAO there's no need to bind it every time,
    // but we'll do so to keep things a bit more organized
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, Asset::m_index_count, GL_UNSIGNED_SHORT, 0);
    glFlush();
}

void Gm::GraphicsManager::InitializePerspectiveMatrix() {
    // Set the field of view and screen aspect ratio.
    float fieldOfView = M_PI / 4.0f;
    float screenAspect = Asset::Width / Asset::Height;

    BuildPerspectiveFovLHMatrix(m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
}

void Gm::GraphicsManager::UpdateCameraViewMatrix() {
    Eigen::Vector3f up, position, lookAt;
    float yaw, pitch, roll;
    Affine3f rotationMatrix;

    // Setup the vector that points upwards.
    up[0] = 0.0f;
    up[1] = 1.0f;
    up[2] = 0.0f;

    // Setup the position of the camera in the world.
    position[0] = m_positionX;
    position[1] = m_positionY;
    position[2] = m_positionZ;

    // Setup where the camera is looking by default.
    lookAt[0] = 0.0f;
    lookAt[1] = 0.0f;
    lookAt[2] = 1.0f;

    // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
    pitch = m_rotationX * 0.0174532925f;
    yaw = m_rotationY * 0.0174532925f;
    roll = m_rotationZ * 0.0174532925f;

    rotationMatrix = AngleAxisf(pitch, Vector3f::UnitX()) * AngleAxisf(yaw, Vector3f::UnitY()) *
                     AngleAxisf(roll, Vector3f::UnitZ());

    // Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
    lookAt = rotationMatrix * lookAt;
    up = rotationMatrix * up;

    // Translate the rotated camera position to the location of the viewer.
    lookAt = position + lookAt;

    // Finally, create the view matrix from the three updated vectors.
    BuildViewMatrix(m_viewMatrix, position, lookAt, up);
}

void Gm::GraphicsManager::UpdateModelMatrix() {
    // Update world matrix to rotate the model
//    m_modelRotationX += DEG_RAD_3;
//    m_modelRotationY += DEG_RAD_3 / 2;
//    m_modelRotationZ += DEG_RAD_3 / 2;
    Eigen::Affine3f transform = Eigen::Affine3f::Identity();
    transform.rotate(Eigen::AngleAxisf(m_modelRotationX * DEG_TO_RAD, Eigen::Vector3f::UnitX()));
    transform.rotate(Eigen::AngleAxisf(m_modelRotationY * DEG_TO_RAD, Eigen::Vector3f::UnitY()));
    transform.rotate(Eigen::AngleAxisf(m_modelRotationZ * DEG_TO_RAD, Eigen::Vector3f::UnitZ()));


    m_worldMatrix = transform * Matrix4f::Identity();
}

bool Gm::GraphicsManager::SetShaderParameters(float *worldMatrix, float *viewMatrix, float *projectionMatrix) {
    unsigned int location;

    // Set the world matrix in the vertex shader.
    location = glGetUniformLocation(shaderProgram, "worldMatrix");
    if (location == -1) {
        return false;
    }
    glUniformMatrix4fv(location, 1, false, worldMatrix);

    // Set the view matrix in the vertex shader.
    location = glGetUniformLocation(shaderProgram, "viewMatrix");
    if (location == -1) {
        return false;
    }
    glUniformMatrix4fv(location, 1, false, viewMatrix);

    // Set the projection matrix in the vertex shader.
    location = glGetUniformLocation(shaderProgram, "projectionMatrix");
    if (location == -1) {
        return false;
    }
    glUniformMatrix4fv(location, 1, false, projectionMatrix);

    return true;
}

void Gm::GraphicsManager::Reset() {
    m_positionZ = Asset::DefaultPositionZ;
    m_modelRotationX = Asset::DefaultRotationAngle;
    m_modelRotationY = Asset::DefaultRotationAngle;
    m_modelRotationZ = 0.0f;
}

void Gm::GraphicsManager::UpdateCameraPositionZ(float dz) {
    m_positionZ += dz;
    if (m_positionZ >= Asset::MaxPositionZ) {
        m_positionZ = Asset::MaxPositionZ;
    }
    if (m_positionZ <= Asset::MinPositionZ) {
        m_positionZ = Asset::MinPositionZ;
    }
}

void Gm::GraphicsManager::UpdateCameraRotationXY(float drx, float dry) {
    m_modelRotationX += drx;
    m_modelRotationY += dry;
}



