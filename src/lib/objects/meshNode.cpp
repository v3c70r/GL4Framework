#include <objects/meshNode.hpp>
#include <core/log.hpp>
//#define STB_IMAGE_IMPLEMENTATION 1
#include <stb/stb_image.h>

void MeshNode::init(GLuint nFaces, GLuint nVertices){
    BUFFER = new GLuint[MESH_ATTR::COUNT];
    numOfFaces = nFaces;
    numOfVertices = nVertices;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(MESH_ATTR::COUNT, BUFFER);
    GLfloat *emptyFloat=new GLfloat[3*numOfVertices];
    GLuint *emptyUint=new GLuint[3*numOfFaces];

    //Vertices
    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[MESH_ATTR::VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*numOfVertices, emptyFloat, GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_ATTR::VERTICES);
    glVertexAttribPointer(MESH_ATTR::VERTICES, 3, GL_FLOAT, 0, 0, 0);

    //Normals
    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[MESH_ATTR::NORMALS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*numOfVertices, emptyFloat, GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_ATTR::NORMALS);
    glVertexAttribPointer(MESH_ATTR::NORMALS, 3, GL_FLOAT, 0, 0, 0);

    //texCoords
    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[MESH_ATTR::TEXCOORDS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*numOfVertices, emptyFloat, GL_STATIC_DRAW);
    glEnableVertexAttribArray(MESH_ATTR::TEXCOORDS);
    glVertexAttribPointer(MESH_ATTR::TEXCOORDS, 2, GL_FLOAT, 0, 0, 0);


    //indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUFFER[MESH_ATTR::INDICES]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*3*numOfFaces, emptyUint, GL_STATIC_DRAW);



    //clean up
    glBindVertexArray(0);
    //Uniform buffer
    glBindBuffer(GL_UNIFORM_BUFFER, BUFFER[MESH_ATTR::MATERIAL]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16, 0, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, BUFFER[MESH_ATTR::OBJ_MATS]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*32, 0,GL_DYNAMIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    delete []emptyFloat;
    delete []emptyUint;
}


void MeshNode::setVertices(const GLfloat *vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[MESH_ATTR::VERTICES]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numOfVertices*3*sizeof(GLfloat), vertices);
    //glBufferData(GL_ARRAY_BUFFER, numOfVertices*3*sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void MeshNode::setNormals(const GLfloat *normals)
{
    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[MESH_ATTR::NORMALS]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numOfVertices*3*sizeof(GLfloat), normals);
    //glBufferData(GL_ARRAY_BUFFER, numOfVertices*3*sizeof(GLfloat), normals, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void MeshNode::setTexCoord(const GLfloat *texCoord)
{
    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[MESH_ATTR::TEXCOORDS]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numOfVertices*2*sizeof(GLfloat), texCoord);
    //glBufferData(GL_ARRAY_BUFFER, numOfVertices*2*sizeof(GLfloat), texCoord, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void MeshNode::setIndices(const GLuint *indices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUFFER[MESH_ATTR::INDICES]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numOfFaces*3*sizeof(GLuint), indices);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfFaces*3*sizeof(GLuint), indices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


/*
 * Load simple OBJ with only 
 * Vertices and faces
 * and assume that index of normals is 
 * as same the index of vertices
 * use at your own risk
 */
void MeshNode::loadSimpleOBJ(std::string objFile)
{
    vector<GLfloat> vertices;
    vector<GLfloat> normals;
    vector<GLuint> indices;
    GLuint nV=0;
    GLuint nF=0;
    std::ifstream inFile(objFile);
    std::string str;
    std::cout<<"Loading "<<objFile<<"..."<<std::endl;
    while (std::getline(inFile, str))   //read  position
    {   
        splitstring split(str);
        vector<string> splited=split.split(' ');
        if (splited.size() > 0)
        {
            if (splited[0] == "v")
            {
                nV++;
                for (const auto& str: splited)
                    vertices.push_back(std::stof(str));
            }
            //else if (splited[0] == "vn")
            //{
            //    for (int i=1; i<splited.size(); i++)
            //        normals.push_back(std::stof(splited[i]));
            //}
            else if (splited[0] == "f")
            {
                if (splited.size() == 4) //triangle
                {
                    nF++;
                    for (const auto& num: splited)
                        indices.push_back((GLuint)std::stoi(num)-1);
                }
                else if (splited.size() == 5) //quad faces
                {
                    nF+=2;
                    indices.push_back((GLuint)std::stoi(splited[1])-1);
                    indices.push_back((GLuint)std::stoi(splited[2])-1);
                    indices.push_back((GLuint)std::stoi(splited[4])-1);
                    indices.push_back((GLuint)std::stoi(splited[4])-1);
                    indices.push_back((GLuint)std::stoi(splited[2])-1);
                    indices.push_back((GLuint)std::stoi(splited[3])-1);
                }
            }
            else
                continue;
        }

    }
    if (normals.size() == 0)        //Calculate normal from mesh
    {
        // Smooth normals
        normals.resize(vertices.size(), 0.0);
        for (auto i=0; i<indices.size(); i+=3)
        {

            glm::vec3 p0 = { vertices[3*indices[i]], vertices[3*indices[i]+1],vertices[3*indices[i]+2]};
            glm::vec3 p1 = { vertices[3*(indices[i+1])], vertices[3*(indices[i+1])+1],vertices[3*(indices[i+1])+2]};
            glm::vec3 p2 = { vertices[3*(indices[i+2])], vertices[3*(indices[i+2])+1],vertices[3*(indices[i+2])+2]};
            glm::vec3 normal = glm::normalize(glm::cross((p1-p0), (p2-p0)));
            normals[3*indices[i]] += normal.x;
            normals[3*indices[i]+1] += normal.y;
            normals[3*indices[i]+2] += normal.z;
            normals[3*(indices[i+1])] += normal.x;
            normals[3*(indices[i+1])+1] += normal.y;
            normals[3*(indices[i+1])+2] += normal.z;
            normals[3*(indices[i+2])] += normal.x;
            normals[3*(indices[i+2])+1] += normal.y;
            normals[3*(indices[i+2])+2] += normal.z;
        }
        //normalize
        for (auto i=0; i<normals.size(); i+=3)
        {
            float distance=glm::sqrt(normals[i]*normals[i]+
                    normals[i+1]*normals[i+1]+
                    normals[i+2]*normals[i+2]
                    );
            normals[i] /= distance;
            normals[i+1] /= distance;
            normals[i+2] /= distance;
        }
    }
    init(nF, nV);

    GLfloat *emptyCoord = new GLfloat[2*nV];
    setVertices(&vertices[0]);
    setNormals(&normals[0]);
    setIndices(&indices[0]);
    setTexCoord(emptyCoord);
    std::cout<<"done!\n";
    delete []emptyCoord;
}

void MeshNode::loadTexture(const std::string &fileName)
{
    //Life is a lil bit easier with stb_image
    ////    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
    ////    // ... process data if not NULL ...
    ////    // ... x = width, y = height, n = # 8-bit components per pixel ...
    ////    // ... replace '0' with '1'..'4' to force that many components per pixel
    ////    // ... but 'n' will always be the number that it would have been if you said 0
    ////    stbi_image_free(data)

    int x, y, n;
    unsigned char * img = nullptr;
    img = stbi_load(fileName.c_str(), &x, &y, &n, 4 /*RGBA*/) ;

    if (img)
    {
        
        LOG::writeLog("Loading Image %d * %d with format %d form %s\n",
                x, y, n, fileName.c_str());
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture interpolation method to use linear interpolation (no MIPMAPS)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glTexImage2D(GL_TEXTURE_2D,
                0,
                GL_RGBA,
                x,
                y,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                img);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
        exit(-1);
        //throw std::runtime_error("Falied to load image");
    //=====Loading image with devil
    //ILuint imageID;
    //ilInit();
    //ILboolean success;
    //ILenum error;
    //ilGenImages(1, &imageID);
    //ilBindImage(imageID);
    //std::cout<<"Loading texture: "<<fileName<<std::endl;
    //success = ilLoadImage(fileName.c_str());
    //if (success)
    //{
    //    ILinfo ImageInfo;
    //    iluGetImageInfo(&ImageInfo);
    //    // Convert the image into a suitable format to work with
    //    // NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
    //    success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

    //    // Quit out if we failed the conversion
    //    if (!success)
    //    {
    //        error = ilGetError();
    //        std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
    //        exit(-1);
    //    }
    //    glGenTextures(1, &texture);
    //    glBindTexture(GL_TEXTURE_2D, texture);
    //    // Set texture clamping method
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //    // Set texture interpolation method to use linear interpolation (no MIPMAPS)
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    //    glTexImage2D(GL_TEXTURE_2D,
    //            0,
    //            ilGetInteger(IL_IMAGE_FORMAT),
    //            ilGetInteger(IL_IMAGE_WIDTH),
    //            ilGetInteger(IL_IMAGE_HEIGHT),
    //            0,
    //            ilGetInteger(IL_IMAGE_FORMAT),
    //            GL_UNSIGNED_BYTE,
    //            ilGetData());
    //}
    //else
    //{
    //    error=ilGetError();
    //    std::cout<<"Unable to load image: IL error: "<<error<<std::endl;
    //    exit(-1);
    //}
    //ilDeleteImages(1, &imageID);
}

void MeshNode::update()
{
    //set model matrix
    glBindBuffer(GL_UNIFORM_BUFFER, BUFFER[MESH_ATTR::OBJ_MATS]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat)*16, &transMat_[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16, sizeof(GLfloat)*16, &(glm::inverse(transMat_))[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void MeshNode::draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3*numOfFaces, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
void MeshNode::setMaterial(const aiMaterial *mat)
{
    const uint MAT_BUF_SIZE = 16;
    float v3[] = {0.0f, 0.0f, 0.0f};
    float f;
    vector<float> matBuf;
    matBuf.reserve(MAT_BUF_SIZE);
    if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_COLOR_DIFFUSE, v3))
    {
        matBuf.push_back(v3[0]);
        matBuf.push_back(v3[1]);
        matBuf.push_back(v3[2]);
        std::cout<<"DIFFUSE: "<<v3[0]<<"\t"<<v3[1]<<"\t"<<v3[2]<<std::endl;
        matBuf.push_back(0.0);
    }
    else
    {
        std::cout<<"\033[1;31mError: \033[0m while loading DIFFUSE, using default"<<std::endl;
        matBuf.push_back(0.0);
        matBuf.push_back(0.0);
        matBuf.push_back(0.0);
        matBuf.push_back(0.0);
    }



    if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_COLOR_AMBIENT, v3))
    {
        matBuf.push_back(v3[0]);
        matBuf.push_back(v3[1]);
        matBuf.push_back(v3[2]);
        matBuf.push_back(0.0);
        std::cout<<"AMBIENT: "<<v3[0]<<"\t"<<v3[1]<<"\t"<<v3[2]<<std::endl;
    }
    else
    {
        std::cout<<"\033[1;31mError: \033[0m while loading AMBIENT, using default"<<std::endl;
        matBuf.push_back(0.3);
        matBuf.push_back(0.3);
        matBuf.push_back(0.3);
        matBuf.push_back(0.0);
    }

    if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_COLOR_SPECULAR, v3))
    {
        matBuf.push_back(v3[0]);
        matBuf.push_back(v3[1]);
        matBuf.push_back(v3[2]);
        matBuf.push_back(0.0);
        std::cout<<"SPECULAR: "<<v3[0]<<"\t"<<v3[1]<<"\t"<<v3[2]<<std::endl;
    }
    else
    {
        std::cout<<"\033[1;31mError: \033[0m while loading SPECULAR, using default"<<std::endl;
        matBuf.push_back(0.7);
        matBuf.push_back(0.7);
        matBuf.push_back(0.7);
        matBuf.push_back(0.0);
    }
    if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &f))
        matBuf.push_back(f);
    else
    {
        std::cout<<"\033[1;31mError: \033[0m while loading SHININESS, using default"<<std::endl;
        matBuf.push_back(0.5);
    }

    if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &f))
        matBuf.push_back(f);
    else
    {
        std::cout<<"\033[1;31mError: \033[0m while loading OPACITY, using default"<<std::endl;
        matBuf.push_back(1.0);
    }
    //padding
    matBuf.push_back(0.0);
    matBuf.push_back(0.0);
    glBindBuffer(GL_UNIFORM_BUFFER, BUFFER[MESH_ATTR::MATERIAL]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*MAT_BUF_SIZE, &matBuf[0]);
}
void MeshNode::bindShader(Shader *s)
{
    auto uniforms = s->getConfig().uniformBlocks;
    //Bind Material
    if (uniforms.find("Material") != uniforms.end())
        glBindBufferBase(GL_UNIFORM_BUFFER, s->getConfig().uniformBlocks.at("Material"), BUFFER[MESH_ATTR::MATERIAL]);
    else
        LOG::writeLogErr("No %s found in uniforms, not binding\n", "Material");
    //Bind Material
    if (uniforms.find("ModelMats") != uniforms.end())
        glBindBufferBase(GL_UNIFORM_BUFFER, s->getConfig().uniformBlocks.at("ModelMats"), BUFFER[MESH_ATTR::OBJ_MATS]);
    else
        LOG::writeLogErr("No %s found in uniforms, not binding\n", "ModelMats");
}





