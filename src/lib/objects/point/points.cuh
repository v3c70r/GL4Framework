/*A class to draw points*/
#pragma once
#include <GL/glew.h>
#include <cstdlib>
#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>
#define ILUT_USE_OPENGL
#include <IL/ilut.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <core/object.h>
#include <core/splitstring.h>
#include <cstdlib>
#include <fstream>

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
    if (code != cudaSuccess) 
    {
        fprintf(stderr,"GPUassert: %s %s: %d\n", cudaGetErrorString(code), file, line);
        fprintf(stderr,"Error Code: %d\n", code);
        if (abort) exit(code);
    }
}
//ENUM for different vertex buffers;
enum VertexBuffers
{
    BUFFER_INDICES=0,       //Uints, first Uints
    BUFFER_STATES_PING,          //Uints
    BUFFER_STATES_PONG,          //Uints
    BUFFER_OFFSET,
    BUFFER_CELLID_PING,          //Uints, last of Uint
    BUFFER_CELLID_PONG,          //Uints, last of Uint

    BUFFER_POSITIONS_PING,       //float3s, First float3
    BUFFER_POSITIONS_PONG,       //float3s, First float3
    BUFFER_VELOCITIES_PING,      //float3s
    BUFFER_VELOCITIES_PONG,      //float3s
    BUFFER_PRESSURES,       //float3s
    BUFFER_COLORGRAD_PING,       //float3s
    BUFFER_COLORGRAD_PONG,      //floa3s
    BUFFER_NORMALS,         //float3s, smoothed normals, last of float3

    BUFFER_DENSITIES_PING,       //floats, First float
    BUFFER_DENSITIES_PONG,
    //BUFFER_MASSES,          //floats
    //BUFFER_SPACERADIUS,     //floats
    //BUFFER_DELTATEMP,       //floats
    BUFFER_TEMP_PING,            //floats, temperature
    BUFFER_TEMP_PONG,            //floats
    //BUFFER_COLORLAPS,       //floats
    //BUFFER_MELTPOINT,       //floats
    BUFFER_FREEZEPOINT,     //floats, last of float
    BUFFER_COUNT
};


struct Grid
{
    //grid parameters
    uint3 gridRes;     
    float3 startPoint;
    float3 cellSize;
    //Arrys
    uint *count;
    uint *firstIdx;
};



class Points: public Object
{
    GLuint numPoints;
    GLuint *dNumPoints;
    //Pingpong buffer VAOs, I will not use the VAO variable in base class
    GLuint pongVAO;
    GLuint pingVAO;
    bool isPing;

    //device pointers;
    void *dPointers[BUFFER_COUNT];      //device pointer for arreis
    void *dSumArray;
    void *dTmpPos;
    void *dTmpSpeed;
    void *dTmpCellIDs;
    void *dTmpStates;
    void *dTmpTemp;

    void *dDensSum;
    void *dDensCount;
    void *tempPos;

    //Grid pointers
    Grid grid;

    cudaGraphicsResource *resources[BUFFER_COUNT];      //resources
    size_t numBytes[BUFFER_COUNT];
    const GLuint MAX_NUM_POINTS;


    public:
    float surfaceDens;
    GLuint tex1;
    Points(GLuint maxNumP=65536):   //Should be equal to MAX_NUM_PAR 32*32*32
        MAX_NUM_POINTS(maxNumP)
    {
        BUFFER = new GLuint[BUFFER_COUNT];
        gpuErrchk(cudaMalloc((void**)&dNumPoints, sizeof(GLuint)));
        gpuErrchk(cudaMemset(dNumPoints, 0, sizeof(GLuint)));
        numPoints = 0;
    }

    float getElement()
    {
        return surfaceDens;
    }

    ~Points ()
    {
        //Unregister cuda
        for (uint i=0; i<BUFFER_COUNT; i++)
            gpuErrchk(cudaGraphicsUnregisterResource(resources[i]));
        //Delete opengl buffers
        glDeleteVertexArrays(1, &pingVAO);
        glDeleteVertexArrays(1, &pongVAO);

        glDeleteBuffers(BUFFER_COUNT, BUFFER);

        //free cuda memos
        gpuErrchk(cudaFree(grid.count));
        gpuErrchk(cudaFree(grid.firstIdx));
        gpuErrchk(cudaFree(dSumArray));
        gpuErrchk(cudaFree(dDensSum));
        gpuErrchk(cudaFree(dDensCount));

        gpuErrchk(cudaFree(tempPos));
    }
    void init();

    void setValues ( const GLfloat *val, const GLuint &num, const GLuint &size, const int &bufferIdx);  //set values
    void addPars( const GLfloat *pos, const GLfloat *temp, const GLfloat *velos, const GLuint &size, const GLuint *s);

    void draw();
    void update();      //overwrite of update function
    void loadTempText();
    void runKernels();
    void runKernelsPCISPH();
    GLuint getNumPoints() const{return numPoints;}
    void reset() {numPoints = 0;}
    void bindShader(Shader *s) 
    {
    }
    static inline void swapPtr(void *&a, void *&b)
    {
        void *temp = a;
        a = b;
        b = temp;
    }

    //stupid functions
    void insertParsFromOBJ(std::string fileName, float temperature = 30.0, GLuint state=0);
    void insertCUBE(const glm::vec3&, const glm::vec3&);
    void insertCUBEE();
    void insertBar();
    void insertBowl(const glm::vec3&, const glm::vec3&);


};


