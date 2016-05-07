#include "pointRenderTem.hpp"
#include <stb/stb_image.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <iostream>

PointRendererTem::PointRendererTem(const int &texWidth, const int &texHeight): shader(nullptr)
{
    glGenTextures(1, &tempTexture);
    //init textures;
    glBindTexture(GL_TEXTURE_2D, tempTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, texWidth, texHeight, 0,GL_RGBA, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);

    LoadTexture("./assets/temperature.png");
}

void PointRendererTem::setShader(Shader* s)
{
    shader = s;
}

void PointRendererTem::LoadTexture(const char* filename)
{
    ILuint imageID;
    ilInit();
    ILboolean success;
    ILenum error;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);
    success = ilLoadImage(filename);
    if (success)
    {
        ILinfo ImageInfo;
        iluGetImageInfo(&ImageInfo);
        if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
        {
            iluFlipImage();
        }

        success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

        if (!success)
        {
            error = ilGetError();
            std::cout << "Image conversion failed - IL reports error: " << error << " - "
                << iluErrorString(error) << std::endl;
            exit(-1);
        }

        glGenTextures(1, &tempTexture);
        glBindTexture(GL_TEXTURE_2D, tempTexture);

        // Set texture clamping method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        // Set texture interpolation method to use linear interpolation (no MIPMAPS)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Specify the texture specification
        glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
                0,				// Pyramid level (for mip-mapping) - 0 is the top level
                ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
                ilGetInteger(IL_IMAGE_WIDTH),	// Image width
                ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
                0,				// Border width in pixels (can either be 1 or 0)
                ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
                GL_UNSIGNED_BYTE,		// Image data type
                ilGetData());			// The actual image data itself
    }
    else // If we failed to open the image file in the first place...
    {
        error = ilGetError();
        std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
        exit(-1);
    }

    ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

    std::cout << "temp Texture creation successful." << std::endl;

}

void PointRendererTem::render() const
{
    for (auto i=0; i<objects.size(); i++)
    {
        Object *obj = objects[i];
        obj->update();
        obj->bindShader(shader);
        
        glUseProgram(shader->getProgramme());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tempTexture);

        obj->draw();
    }

}
