#include "Model3D.hpp"

Model3D::Model3D()
{
    // nil
}

Model3D::Model3D(std::string filename)
{
    this->fileName = filename;
    FILE *file = fopen(filename.c_str(), "r");

    if (!file)
    {
        std::cout << "Could not open model file " << filename << std::endl;
        exit(1);
    }

    // Skip name
    char ch = ' ';
    while (ch != '\n')
        fscanf(file, "%c", &ch);

    fscanf(file, "# triangles = %d\n", &this->triangleCount);
    this->triangles = new Triangle[this->triangleCount];

    fscanf(file, "Material count = %d\n", &this->materialCount);
    this->materials = new Material[this->materialCount];

    for (int i = 0; i < this->materialCount; i++)
        ReadMaterial(file, i);

    // Skip documentation
    fscanf(file, "%c", &ch);
    while (ch != '\n')
        fscanf(file, "%c", &ch);

    for (int i = 0; i < this->triangleCount; i++)
    {
        ReadTriangle(file, i);
    }

    fclose(file);
}

float *Model3D::GetVertexPositionData()
{
    float *positionData = new float[9 * triangleCount];

    for (int i = 0; i < triangleCount; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            positionData[9 * i + (3 * j + 0)] = this->triangles[i].vertices[j].position.x;
            positionData[9 * i + (3 * j + 1)] = this->triangles[i].vertices[j].position.y;
            positionData[9 * i + (3 * j + 2)] = this->triangles[i].vertices[j].position.z;
        }
    }

    return positionData;
}

float *Model3D::GetVertexNormalData()
{
    float *normalData = new float[9 * triangleCount];

    for (int i = 0; i < triangleCount; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            normalData[9 * i + (3 * j + 0)] = this->triangles[i].vertices[j].normal.x;
            normalData[9 * i + (3 * j + 1)] = this->triangles[i].vertices[j].normal.y;
            normalData[9 * i + (3 * j + 2)] = this->triangles[i].vertices[j].normal.z;
        }
    }

    return normalData;
}

float *Model3D::GetVertexColorData()
{
    float *colorData = new float[9 * triangleCount];

    for (int i = 0; i < triangleCount; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            colorData[9 * i + (3 * j + 0)] = this->triangles[i].vertices[j].color.x;
            colorData[9 * i + (3 * j + 1)] = this->triangles[i].vertices[j].color.y;
            colorData[9 * i + (3 * j + 2)] = this->triangles[i].vertices[j].color.z;
        }
    }

    return colorData;
}

void Model3D::PrintInformation()
{
    std::cout << "Model information" << std::endl
              << "\t# of triangles: " << this->triangleCount << std::endl
              << "\t# of materials: " << this->materialCount << std::endl;

    std::cout << "\tMaterials: " << std::endl;
    for (int i = 0; i < this->materialCount; i++)
    {
        std::cout << "\t\tMaterial " << i << std::endl
                  << "\t\t - Ambient color: (" << this->materials[i].ambientColor.x << ", " << this->materials[i].ambientColor.y << ", " << this->materials[i].ambientColor.z << ")" << std::endl
                  << "\t\t - Diffuse color: (" << this->materials[i].diffuseColor.x << ", " << this->materials[i].diffuseColor.y << ", " << this->materials[i].diffuseColor.z << ")" << std::endl
                  << "\t\t - Specular color: (" << this->materials[i].specularColor.x << ", " << this->materials[i].specularColor.y << ", " << this->materials[i].specularColor.z << ")" << std::endl
                  << "\t\t - Shine coefficient: " << this->materials[i].shineCoefficient << std::endl;
    }

    for (int i = 0; i < this->triangleCount; i++)
    {
        std::cout << "\t\tTriangle " << i << std::endl;
        for (int j = 0; j < 3; j++)
            std::cout << "\t\t v" << j
                      << " | (" << this->triangles[i].vertices[j].position.x << ", " << this->triangles[i].vertices[j].position.y << ", " << this->triangles[i].vertices[j].position.z << ")"
                      << " | (" << this->triangles[i].vertices[j].normal.x << ", " << this->triangles[i].vertices[j].normal.y << ", " << this->triangles[i].vertices[j].normal.z << ")"
                      << " | (" << this->triangles[i].vertices[j].color.x << ", " << this->triangles[i].vertices[j].color.y << ", " << this->triangles[i].vertices[j].color.z << ")" << std::endl;
    }

    std::cout << std::endl;
}

void Model3D::SetMaterialDiffuseColor(int material, glm::vec3 color)
{
    this->materials[material].diffuseColor = color;

    for (int i = 0; i < this->triangleCount; i++)
        for (int j = 0; j < 3; j++)
            this->triangles[i].vertices[j].color = this->materials[this->triangles[i].vertices[j].colorIndex].diffuseColor;
}

void Model3D::ReadMaterial(FILE *file, int index)
{
    float x, y, z;

    fscanf(file, "ambient color %f %f %f\n", &x, &y, &z);
    this->materials[index].ambientColor = glm::vec3(x, y, z);

    fscanf(file, "diffuse color %f %f %f\n", &x, &y, &z);
    this->materials[index].diffuseColor = glm::vec3(x, y, z);

    fscanf(file, "specular color %f %f %f\n", &x, &y, &z);
    this->materials[index].specularColor = glm::vec3(x, y, z);

    fscanf(file, "material shine %f\n", &this->materials[index].shineCoefficient);
}

void Model3D::ReadTriangle(FILE *file, int index)
{
    int vertexNumber, colorIndex;
    float vx, vy, vz, nx, ny, nz, cx, cy, cz;

    for (int i = 0; i < 3; i++)
    {
        fscanf(file, "v%d %f %f %f %f %f %f %d\n", &vertexNumber, &vx, &vy, &vz, &nx, &ny, &nz, &colorIndex);
        this->triangles[index].vertices[vertexNumber].position = glm::vec3(vx, vy, vz);
        this->triangles[index].vertices[vertexNumber].normal = glm::vec3(nx, ny, nz);

        cx = this->materials[colorIndex].diffuseColor.x;
        cy = this->materials[colorIndex].diffuseColor.y;
        cz = this->materials[colorIndex].diffuseColor.z;
        this->triangles[index].vertices[vertexNumber].colorIndex = colorIndex;
        this->triangles[index].vertices[vertexNumber].color = glm::vec3(cx, cy, cz);

        boundingBox.update(this->triangles[index].vertices[vertexNumber].position);
    }

    fscanf(file, "face normal %f %f %f\n", &nx, &ny, &nz);
    this->triangles[index].faceNormal = glm::vec3(nx, ny, nz);
}
