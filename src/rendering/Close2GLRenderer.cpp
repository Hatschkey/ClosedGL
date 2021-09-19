#include "Close2GLRenderer.hpp"

#include <fstream>

Close2GLRenderer::Close2GLRenderer()
{
    this->engineId = Close2GL;
    this->engineName = "Close2GL";

    buffers = new Buffer(0, 0);

    rasterizationStrategies.insert({Wireframe, new WireframeRasterizationStrategy()});
    rasterizationStrategies.insert({Standard, new FilledRasterizationStrategy()});
    rasterizationStrategies.insert({Points, new PointsRasterizationStrategy()});
}

Close2GLRenderer::~Close2GLRenderer()
{
    delete buffers;
    rasterizationStrategies.clear();
}

bool Close2GLRenderer::ShouldCull(Triangle triangle)
{
    if (triangle.clipped)
        return true;

    if (cullingMode == NoCulling)
        return false;

    float sum = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        float firstTerm = triangle.vertices[i].position.x * triangle.vertices[(i + 1) % 3].position.y;
        float secondTerm = triangle.vertices[(i + 1) % 3].position.x * triangle.vertices[i].position.y;

        sum += firstTerm - secondTerm;
    }

    float area = 0.5f * sum;
    return area * polygonOrientation * cullingMode > 0;
}

void Close2GLRenderer::DrawObject(Model3D object)
{
    glm::mat4 modelViewProjection = projection * view * model;

    for (int triangleIndex = 0; triangleIndex < object.triangleCount; triangleIndex++)
    {
        Triangle triangle = object.triangles[triangleIndex];
        projectTriangleToNDC(triangle, modelViewProjection);

        if (!ShouldCull(triangle))
        {
            projectTriangleToViewport(triangle, viewport);
            rasterizationStrategies[renderMode]->DrawTriangleToBuffer(triangle, buffers);
        }
    }
}

void Close2GLRenderer::CalculateRenderingMatrices(Scene scene, Window *window)
{
    view = scene.camera.GetViewMatrix();
    projection = scene.camera.GetProjectionMatrix();

    float n = scene.camera.nearPlane;
    float f = scene.camera.farPlane;
    float w = window->GetWidth();
    float h = window->GetHeight();

    viewport = glm::mat4(w / 2.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, -h / 2.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f,
                         w / 2.0f, h / 2.0f, 0.0f, 1.0f);
}

void Close2GLRenderer::ClearAndResizeBuffersForWindow(Window *window)
{
    buffers->resize(window->GetWidth(), window->GetHeight());
    buffers->setClearColor(backgroundColor);

    // Delete previous texture
    glDeleteTextures(1, Textures);

    // Generate texture info
    glGenTextures(1, Textures);
    glBindTexture(GL_TEXTURE_2D, Textures[ModelTexture_Close2GL]);

    // Set parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

void Close2GLRenderer::RenderSceneToWindow(Scene scene, Window *window)
{
    CalculateRenderingMatrices(scene, window);
    ClearAndResizeBuffersForWindow(window);

    for (auto object = scene.models.begin(); object != scene.models.end(); object++)
    {
        model = glm::mat4(1.0f);
        DrawObject(*object);
    }

    unsigned char *colorBuffer = buffers->getColorBuffer();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buffers->width, buffers->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindVertexArray(VAOs[ModelObject_Close2GL]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Clear texture info
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(colorBuffer);
}

void Close2GLRenderer::SetupVAOS()
{
    glGenVertexArrays(NumVAOs_Close2GL, VAOs);
    glBindVertexArray(VAOs[ModelObject_Close2GL]);
}

void Close2GLRenderer::SetupVBOS(std::vector<Model3D> objects)
{
    glCreateBuffers(Close2GL_NumBuffers, Buffers);

    float vertexData[] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f};
    float textureCoordinateData[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                                     1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f};

    // Position
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Close2GL_VertexPositionBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, 2 * 6 * sizeof(float), vertexData, GL_DYNAMIC_STORAGE_BIT);
    glVertexAttribPointer(close2GLvertexPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(close2GLvertexPosition);

    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Close2GL_VertexTextureCoordinateBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, 2 * 6 * sizeof(float), textureCoordinateData, GL_DYNAMIC_STORAGE_BIT);
    glVertexAttribPointer(close2GLtextureCoordinates, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(close2GLtextureCoordinates);

    // Textures
    glGenTextures(1, Textures);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// State setters

void Close2GLRenderer::SetCullingMode(CullingModes cullingMode)
{
    glDisable(GL_CULL_FACE);
    this->cullingMode = cullingMode;
}

void Close2GLRenderer::SetPolygonOrientation(PolygonOrientation orientation)
{
    glFrontFace(GL_CCW);
    this->polygonOrientation = orientation;
}

void Close2GLRenderer::SetRenderMode(RenderModes renderMode)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    this->renderMode = renderMode;
}

void Close2GLRenderer::SetBackgroundColor(glm::vec3 color)
{
    this->backgroundColor = color;
}
