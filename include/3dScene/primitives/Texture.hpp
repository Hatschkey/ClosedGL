#pragma once

#include <iostream>
#include <glm/vec3.hpp>

#define NUM_GENERATED_MIPMAPS 8

typedef struct t_mipmap 
{
    unsigned char *image;
    int width, height;

    glm::vec<3, unsigned char> sample(int s, int t);
} Mipmap;

typedef struct t_texture
{
    Mipmap *mipmaps;
    int mipmapCount;
    unsigned char *image;
    std::string name;
    int width, height, numChannels;
    bool needsReload;

    t_texture();
    t_texture(const char *filename);
    ~t_texture();
    std::string getName();
    bool exists();
    void generateMipmap();
    glm::vec3 sample(int s, int t);

} Texture;
