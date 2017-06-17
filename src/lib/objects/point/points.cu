#include <objects/point/points.cuh>
#include <objects/point/kernels.cu>
#include <core/ImguiConfig.hpp>

void Points::init()
{
    isPing = true;
    //Generating buffers
    glGenBuffers (BUFFER_COUNT, BUFFER);     

    std::cout<<glGetError()<<std::endl;
    //init buffers
    uint *tempUint = new uint[MAX_NUM_POINTS];
    for (uint i = BUFFER_INDICES; i<= BUFFER_CELLID_PONG; i++)
    {
        glBindBuffer (GL_ARRAY_BUFFER, BUFFER[i]);
        glBufferData (GL_ARRAY_BUFFER, MAX_NUM_POINTS * sizeof(uint), tempUint, GL_DYNAMIC_DRAW);
    }
    delete []tempUint;

    float *tempFloat = new float[MAX_NUM_POINTS*3];
    for (uint i=BUFFER_POSITIONS_PING; i<=BUFFER_NORMALS; i++)
    {
        glBindBuffer( GL_ARRAY_BUFFER, BUFFER[i]);
        glBufferData (GL_ARRAY_BUFFER, MAX_NUM_POINTS * sizeof(float) * 3, tempFloat, GL_DYNAMIC_DRAW);
    }
    delete []tempFloat;
    for (uint i=BUFFER_DENSITIES_PING; i<=BUFFER_FREEZEPOINT; i++)
    {
        glBindBuffer( GL_ARRAY_BUFFER, BUFFER[i]);
        glBufferData (GL_ARRAY_BUFFER, MAX_NUM_POINTS * sizeof(float) , 0, GL_DYNAMIC_DRAW);
    }
    std::cout<<glGetError()<<std::endl;

    //Setting Ping VAO
    glGenVertexArrays (1, &pingVAO);
    glBindVertexArray (pingVAO);

    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[BUFFER_POSITIONS_PING]);
    glEnableVertexAttribArray (0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[BUFFER_TEMP_PING]);
    glEnableVertexAttribArray (1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[BUFFER_COLORGRAD_PING]);
    glEnableVertexAttribArray (2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[BUFFER_DENSITIES_PING]);
    glEnableVertexAttribArray (3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, NULL);


    //Setting Pong VAO;
    glGenVertexArrays (1, &pongVAO);
    glBindVertexArray (pongVAO);

    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[BUFFER_POSITIONS_PONG]);
    glEnableVertexAttribArray (0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[BUFFER_TEMP_PONG]);
    glEnableVertexAttribArray (1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[BUFFER_COLORGRAD_PONG]);
    glEnableVertexAttribArray (2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer( GL_ARRAY_BUFFER, BUFFER[BUFFER_DENSITIES_PONG]);
    glEnableVertexAttribArray (3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, NULL);


    //load temperature texture
    loadTempText();

    //Register CUDA resources
    for (int i=0; i<BUFFER_COUNT; i++)
        gpuErrchk( cudaGraphicsGLRegisterBuffer(&(resources[i]), BUFFER[i], cudaGraphicsRegisterFlagsWriteDiscard));
    //allocate space for constants

    //Allocate and init grid
    gpuErrchk(cudaMalloc((void**)&(grid.gridRes), sizeof(uint3)));
    gpuErrchk(cudaMalloc((void**)&(grid.startPoint), sizeof(float3)));
    gpuErrchk(cudaMalloc((void**)&(grid.cellSize), sizeof(float3)));
    
    grid.gridRes = make_uint3(16, 16, 16);
    grid.startPoint = make_float3(-15, -15, -15);
    grid.cellSize = make_float3( -grid.startPoint.x*2/grid.gridRes.x, -grid.startPoint.y*2/grid.gridRes.y, -grid.startPoint.z*2/grid.gridRes.z);


    gpuErrchk(cudaMalloc((void**)&(grid.count), sizeof(uint)*grid.gridRes.x*grid.gridRes.y*grid.gridRes.z));
    gpuErrchk(cudaMalloc((void**)&(grid.firstIdx), sizeof(uint)*grid.gridRes.x*grid.gridRes.y*grid.gridRes.z));
    
    gpuErrchk(cudaMalloc((void**)&dSumArray, sizeof(uint)
                *(grid.gridRes.x * grid.gridRes.y * grid.gridRes.z -1)/2048+1));  //dead param

    gpuErrchk(cudaMalloc((void**)&dDensSum, sizeof(float)));
    gpuErrchk(cudaMalloc((void**)&dDensCount, sizeof(int)));

    gpuErrchk(cudaMalloc((void**)&tempPos, sizeof(float)*3*MAX_NUM_POINTS));



}

void Points::setValues ( const GLfloat *val, const GLuint &num, const GLuint &size, const int &bufferIdx)
{
    numPoints = num;
    gpuErrchk(cudaMemcpy(dNumPoints, &numPoints, sizeof(uint), cudaMemcpyHostToDevice));
    if ( numPoints > MAX_NUM_POINTS)
        exit(-1);
    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[bufferIdx]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, num*size*sizeof(GLfloat), val);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Points::addPars( const GLfloat *pos, const GLfloat *temp, const GLfloat *velos, const GLuint &num, const GLuint *s)
{
    if ( numPoints+num >= MAX_NUM_POINTS)
    {
        std::cout<<"To many particles\n";
        exit(-1);
    }

    int BUFFER_POSITIONS = 0;
    int BUFFER_TEMP = 0;
    int BUFFER_STATES = 0;
    int BUFFER_VELOCITIES = 0;
    if (isPing)
    {
        BUFFER_POSITIONS = BUFFER_POSITIONS_PING;
        BUFFER_TEMP = BUFFER_TEMP_PING;
        BUFFER_STATES = BUFFER_STATES_PING;
        BUFFER_VELOCITIES = BUFFER_VELOCITIES_PING;
    }
    else
    {
        BUFFER_POSITIONS = BUFFER_POSITIONS_PONG;
        BUFFER_TEMP = BUFFER_TEMP_PONG;
        BUFFER_STATES = BUFFER_STATES_PONG;
        BUFFER_VELOCITIES = BUFFER_VELOCITIES_PONG;
    }

    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[BUFFER_POSITIONS]);
    glBufferSubData(GL_ARRAY_BUFFER, 3*numPoints*sizeof(GLfloat), 3*num*sizeof(GLfloat), pos);

    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[BUFFER_TEMP]);
    glBufferSubData(GL_ARRAY_BUFFER, numPoints*sizeof(GLfloat), num*sizeof(GLfloat), temp);

    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[BUFFER_STATES]);
    glBufferSubData(GL_ARRAY_BUFFER, numPoints*sizeof(GLuint), num*sizeof(GLuint), s);

    //set velo to zero
    glBindBuffer(GL_ARRAY_BUFFER, BUFFER[BUFFER_VELOCITIES]);
    glBufferSubData(GL_ARRAY_BUFFER, 3*numPoints*sizeof(GLfloat), 3*num*sizeof(GLfloat), velos);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    numPoints += num;


}

void Points::update()
{
    runKernels();
    float GPUsum;
    int GPUcount;
    gpuErrchk(cudaMemcpy(&GPUsum, dDensSum, sizeof(float), cudaMemcpyDeviceToHost));
    gpuErrchk(cudaMemcpy(&GPUcount, dDensCount, sizeof(int), cudaMemcpyDeviceToHost));
    surfaceDens = GPUsum / GPUcount;
}

void Points::draw()
{

    if (isPing)
        glBindVertexArray(pingVAO);
    else
        glBindVertexArray(pongVAO);
    glDrawArrays( GL_POINTS, 0, numPoints);
    glBindVertexArray(0);
    isPing = !isPing; //swap buffer
}

void Points::loadTempText()     //load texture with il
{

    //TODO: load image with other tools
    //
    ILuint imageID;				// Create an image ID as a ULuint
    ilInit();
    GLuint textureID;			// Create a texture ID as a GLuint
    ILboolean success;			// Create a flag to keep track of success/failure
    ILenum error;				// Create a flag to keep track of the IL error state
    ilGenImages(1, &imageID); 		// Generate the image ID
    ilBindImage(imageID); 			// Bind the image
    success = ilLoadImage("./assets/temperature.png"); 	// Load the image file
    // If we managed to load the image, then we can start to do things with it...
    if (success)
    {
        // If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
        ILinfo ImageInfo;
        iluGetImageInfo(&ImageInfo);
        if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
        {
            iluFlipImage();
        }

        // Convert the image into a suitable format to work with
        // NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
        success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

        // Quit out if we failed the conversion
        if (!success)
        {
            error = ilGetError();
            std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
            exit(-1);
        }

        // Generate a new texture
        glGenTextures(1, &textureID);

        // Bind the texture to a name
        glBindTexture(GL_TEXTURE_2D, textureID);

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

    std::cout << "Texture creation successful." << std::endl;

    tex1 =  textureID; // Return the GLuint to the texture so you can use it!
}

void Points::runKernelsPCISPH()
{
    cudaMemset(dDensSum, 0, sizeof(float));
    cudaMemset(dDensCount, 0, sizeof(int));

    if (numPoints == 0)
        return;
    //get pointers
    for (int i=0; i<BUFFER_COUNT; i++)
    {
        gpuErrchk(cudaGraphicsMapResources(1, &(resources[i])));
        gpuErrchk(cudaGraphicsResourceGetMappedPointer( &(dPointers[i]), &(numBytes[i]), resources[i]));
    }
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );
    if (!isPing)
    {
        //swap ping pong pointers
        Points::swapPtr(dPointers[BUFFER_STATES_PING], dPointers[BUFFER_STATES_PONG]);
        Points::swapPtr(dPointers[BUFFER_CELLID_PING], dPointers[BUFFER_CELLID_PONG]);
        Points::swapPtr(dPointers[BUFFER_POSITIONS_PING], dPointers[BUFFER_POSITIONS_PONG]);
        Points::swapPtr(dPointers[BUFFER_VELOCITIES_PING], dPointers[BUFFER_VELOCITIES_PONG]);
        Points::swapPtr(dPointers[BUFFER_TEMP_PING], dPointers[BUFFER_TEMP_PONG]);
        Points::swapPtr(dPointers[BUFFER_COLORGRAD_PING], dPointers[BUFFER_COLORGRAD_PONG]);
        Points::swapPtr(dPointers[BUFFER_DENSITIES_PING], dPointers[BUFFER_DENSITIES_PONG]);
    }
    resetGrid<<<16*16, 16>>>
        (grid.count,
         grid.gridRes);
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );

    gpuErrchk(cudaMemcpy(dNumPoints, &numPoints, sizeof(uint), cudaMemcpyHostToDevice));

    insertPars<<<max((numPoints-1)/1024+1, 1), 1024>>>
        ((float3*)dPointers[BUFFER_POSITIONS_PING],
         (uint*)dPointers[BUFFER_CELLID_PING],
         dNumPoints,
         (uint*)dPointers[BUFFER_OFFSET],
         grid.startPoint,
         grid.cellSize,
         grid.gridRes,
         grid.count,
         numPoints);
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );
    prefixSumKernel<<<2,1024>>>(grid.count,grid.firstIdx,(uint*)dSumArray, grid.gridRes.x*grid.gridRes.y*grid.gridRes.z);
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );
    prefexSumPostShort<<<2, 1024>>>(grid.firstIdx, (uint*)dSumArray, 2);
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );
    countingSort<<<max((numPoints-1)/1024+1, 1), 1024>>>(
            grid.firstIdx,
            (uint*)dPointers[BUFFER_OFFSET],
            (uint*)dPointers[BUFFER_CELLID_PING],
            (uint*)dPointers[BUFFER_CELLID_PONG],
            grid.count,

            (float3*)dPointers[BUFFER_VELOCITIES_PING],
            (float3*)dPointers[BUFFER_VELOCITIES_PONG],
            (float3*)dPointers[BUFFER_POSITIONS_PING],
            (float3*)dPointers[BUFFER_POSITIONS_PONG],
            (States*)dPointers[BUFFER_STATES_PING],
            (States*)dPointers[BUFFER_STATES_PONG],
            (float*)dPointers[BUFFER_TEMP_PING],
            (float*)dPointers[BUFFER_TEMP_PONG],
            numPoints);

    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );

    gpuErrchk(cudaMemcpy(&numPoints, dNumPoints, sizeof(uint), cudaMemcpyDeviceToHost));
    if (numPoints != 0)
    {
        calculateDensitiesGlobal<<<max((numPoints-1)/1024+1, 1), 1024>>>(
                (float3*)dPointers[BUFFER_POSITIONS_PONG], 
                (float*)dPointers[BUFFER_DENSITIES_PONG], 
                (uint*)dPointers[BUFFER_CELLID_PONG],
                grid.firstIdx,
                grid.count,
                numPoints);
        gpuErrchk( cudaPeekAtLastError() );
        gpuErrchk( cudaDeviceSynchronize() );
        //compute external forces
    }
    for (int i=0; i<BUFFER_COUNT; i++)
        gpuErrchk(cudaGraphicsUnmapResources(1, &(resources[i])));
}
void Points::runKernels()
{
    if (numPoints == 0)
        return;

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    
    //get pointers
    for (int i=0; i<BUFFER_COUNT; i++)
    {
        gpuErrchk(cudaGraphicsMapResources(1, &(resources[i])));
        gpuErrchk(cudaGraphicsResourceGetMappedPointer( &(dPointers[i]), &(numBytes[i]), resources[i]));
    }
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    ImGui::Text("Num of Points\t %d", numPoints);
    ImGui::Text("Mapping\t %f",elapsedTime);

    /*ping:
      ping -sort-> pong -> render
      pong
      pong -sort-> ping -> render
      */

    if (!isPing)
    {
        //swap ping pong pointers
        Points::swapPtr(dPointers[BUFFER_STATES_PING], dPointers[BUFFER_STATES_PONG]);
        Points::swapPtr(dPointers[BUFFER_CELLID_PING], dPointers[BUFFER_CELLID_PONG]);
        Points::swapPtr(dPointers[BUFFER_POSITIONS_PING], dPointers[BUFFER_POSITIONS_PONG]);
        Points::swapPtr(dPointers[BUFFER_VELOCITIES_PING], dPointers[BUFFER_VELOCITIES_PONG]);
        Points::swapPtr(dPointers[BUFFER_TEMP_PING], dPointers[BUFFER_TEMP_PONG]);
        Points::swapPtr(dPointers[BUFFER_COLORGRAD_PING], dPointers[BUFFER_COLORGRAD_PONG]);
        Points::swapPtr(dPointers[BUFFER_DENSITIES_PING], dPointers[BUFFER_DENSITIES_PONG]);
    }
   
    cudaEventRecord(start, 0);
    resetGrid<<<16*16, 16>>>
        (grid.count,
         grid.gridRes);
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);
    ImGui::Text("Reset Grid\t %f",elapsedTime);

    //insert particles and count
    //std::cout<<"Inserting particles...\n";
    gpuErrchk(cudaMemcpy(dNumPoints, &numPoints, sizeof(uint), cudaMemcpyHostToDevice));


    cudaEventRecord(start, 0);
    insertPars<<<max((numPoints-1)/1024+1, 1), 1024>>>
        ((float3*)dPointers[BUFFER_POSITIONS_PING],
         (uint*)dPointers[BUFFER_CELLID_PING],
         dNumPoints,
         (uint*)dPointers[BUFFER_OFFSET],
         grid.startPoint,
         grid.cellSize,
         grid.gridRes,
         grid.count,
         numPoints);
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);
    ImGui::Text("Insert Pars\t %f",elapsedTime);

    //do prefix sum on particles
    cudaEventRecord(start, 0);
    prefixSumKernel<<<2,1024>>>(grid.count,grid.firstIdx,(uint*)dSumArray, grid.gridRes.x*grid.gridRes.y*grid.gridRes.z);
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );

    prefexSumPostShort<<<2, 1024>>>(grid.firstIdx, (uint*)dSumArray, 2);
    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );


    //std::cout<<"counting sort...\n";
    countingSort<<<max((numPoints-1)/1024+1, 1), 1024>>>(
            grid.firstIdx,
            (uint*)dPointers[BUFFER_OFFSET],
            (uint*)dPointers[BUFFER_CELLID_PING],
            (uint*)dPointers[BUFFER_CELLID_PONG],
            grid.count,

            (float3*)dPointers[BUFFER_VELOCITIES_PING],
            (float3*)dPointers[BUFFER_VELOCITIES_PONG],
            (float3*)dPointers[BUFFER_POSITIONS_PING],
            (float3*)dPointers[BUFFER_POSITIONS_PONG],
            (States*)dPointers[BUFFER_STATES_PING],
            (States*)dPointers[BUFFER_STATES_PONG],
            (float*)dPointers[BUFFER_TEMP_PING],
            (float*)dPointers[BUFFER_TEMP_PONG],
            numPoints);

    gpuErrchk( cudaPeekAtLastError() );
    gpuErrchk( cudaDeviceSynchronize() );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);
    ImGui::Text("Counting sort\t %f",elapsedTime);

    //std::cout<<"Copying data back...\n";
    gpuErrchk(cudaMemcpy(&numPoints, dNumPoints, sizeof(uint), cudaMemcpyDeviceToHost));


    if (numPoints != 0)
    {
        //===============++Densities++=====================
        //std::cout<<"Calculating densities... numOfPar: "<<max((numPoints-1)/1024+1, 1)<<std::endl;;
        cudaEventRecord(start, 0);
        calculateDensitiesGlobal<<<max((numPoints-1)/1024+1, 1), 1024>>>(
                (float3*)dPointers[BUFFER_POSITIONS_PONG], 
                (float*)dPointers[BUFFER_DENSITIES_PONG], 
                (uint*)dPointers[BUFFER_CELLID_PONG],
                grid.firstIdx,
                grid.count,
                numPoints);
        //calculateDensitiesShared<<<16*16*16, 100>>>(
        //        (float3*)dPointers[BUFFER_POSITIONS_PONG], 
        //        (float*)dPointers[BUFFER_DENSITIES], 
        //        (uint*)dPointers[BUFFER_CELLID_PONG],
        //        grid.firstIdx,
        //        grid.count,
        //        numPoints);

        gpuErrchk( cudaPeekAtLastError() );
        gpuErrchk( cudaDeviceSynchronize() );
        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&elapsedTime, start, stop);
        ImGui::Text("Densities\t %f",elapsedTime);

        //Color grad
        cudaEventRecord(start, 0);
            computeColorGradsGlobal<<<max((numPoints-1)/1024+1, 1), 1024>>>(
                    (float3*)dPointers[BUFFER_POSITIONS_PONG],
                    (float*)dPointers[BUFFER_DENSITIES_PONG],
                    (float3*)dPointers[BUFFER_COLORGRAD_PONG],
                    (float*)dPointers[BUFFER_TEMP_PONG],
                    (float*)dDensSum,
                    (uint*)dDensCount,
                    (uint*)dPointers[BUFFER_STATES_PONG],
                    (uint*)dPointers[BUFFER_CELLID_PONG],
                    grid.firstIdx,
                    grid.count,
                    numPoints
                    );
            gpuErrchk( cudaPeekAtLastError() );
            gpuErrchk( cudaDeviceSynchronize() );
        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&elapsedTime, start, stop);
        ImGui::Text("Color Grad\t %f",elapsedTime);



        //=============++Forces++=======================
        //std::cout<<"Computing forces...\n";
#define FORCE_BLOCK_DIM 512
        cudaEventRecord(start, 0);
        calculateForcesGlobal<<<max((numPoints-1)/FORCE_BLOCK_DIM+1,1), FORCE_BLOCK_DIM>>>(
                (float3*)dPointers[BUFFER_POSITIONS_PONG],
                (float*)dPointers[BUFFER_DENSITIES_PONG],
                (float3*)dPointers[BUFFER_VELOCITIES_PONG],
                (float3*)dPointers[BUFFER_PRESSURES],
                (float3*)dPointers[BUFFER_COLORGRAD_PONG],
                (uint*)dPointers[BUFFER_CELLID_PONG],
                (uint*)dPointers[BUFFER_STATES_PONG],
                (uint*)grid.firstIdx, 
                (uint*)grid.count,
                (float3*)dPointers[BUFFER_NORMALS],
                numPoints);

        gpuErrchk( cudaPeekAtLastError() );
        gpuErrchk( cudaDeviceSynchronize() );
        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&elapsedTime, start, stop);
        ImGui::Text("Forces\t %f",elapsedTime);
    }
    //Unmap pointers;
    for (int i=0; i<BUFFER_COUNT; i++)
        gpuErrchk(cudaGraphicsUnmapResources(1, &(resources[i])));
}
void Points::insertParsFromOBJ(std::string fileName, float temperature, GLuint state)
{
    std::cout<<"Inserting OBJ "<<fileName<<std::endl;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> vals;
    std::vector<GLuint> states;
    std::vector<GLfloat> velos;
    std::string str;
    std::ifstream inFile(fileName);

    while(std::getline(inFile, str))
    {
        splitstring split(str);
        vector<std::string> splited = split.split(' ');
        if (splited.size() > 0)
            if (splited[0] == "v")
                for (int i=1; i<splited.size(); i++)
                    vertices.push_back(std::stof(splited[i]));
    }
    for (auto i=0; i<vertices.size()/3; i++)
    {
        vals.push_back(temperature);
        states.push_back(state);
        velos.push_back(0.0);
        velos.push_back(0.0);
        velos.push_back(0.0);
    }
    std::cout<<states.size()<<std::endl;;
    std::cout<<"Num vertices: "<<vertices.size()<<std::endl;
    addPars(&vertices[0], &vals[0], &velos[0], states.size(), &states[0]);
}

void Points::insertBowl(const glm::vec3 &center, const glm::vec3 &dir)
{
    const float radius = 1.0;
    const uint X=16;
    const uint Y=16;
    const uint Z=16;
    const float step = 10.0/32.0;

    std::vector<GLfloat> pos;
    std::vector<GLfloat> vals;
    std::vector<GLuint> states;
    std::vector<GLfloat> velos;
    pos.reserve(X*Y*Z*3);
    vals.reserve(X*Y*Z);
    states.reserve(X*Y*Z);
    velos.reserve(X*Y*Z*3);

    GLuint numPar = 0;
    for (uint i=0; i<X; i++)
        for (uint j=0; j<Y; j++)
            for (uint k=0; k<Z; k++)
            {
                float r = (step*i-2.5)*(step*i-2.5)+
                    (step*j-2.5)*(step*j-2.5)+
                    (step*k-2.5)*(step*k-2.5);
                if ( 
                        r < 
                        radius * radius
                        && 
                        r > radius * radius -0.5 
                         &&
                         step*j-2.5 < 0.0
                   ){
                    pos.push_back(center.x-2.5 + step * i);
                    pos.push_back(center.y-2.5 + step * j);
                    pos.push_back(center.z-2.5 + step * k);
                    vals.push_back(80.0);
                    states.push_back(0);
                    velos.push_back(dir.x*200);
                    velos.push_back(dir.y*200);
                    velos.push_back(dir.z*200);
                    numPar++;
                }
            }
    addPars(&pos[0], &vals[0], &velos[0], numPar, &states[0]);
}

void Points::insertCUBE(const glm::vec3 &center, const glm::vec3 &dir)
{
    //init a grid or pars
    const uint X=20;
    const uint Y=20;
    const uint Z=20;
    float step = 10.0/16.0;
    GLfloat pos[X*Y*Z*3];
    GLfloat vals[X*Y*Z];
    GLuint states[X*Y*Z];
    GLfloat velos[X*Y*Z*3];
    uint parIdx = 0;
    for (uint i=0; i<X; i++)
        for (uint j=0; j<Y; j++)
            for (uint k=0; k<Z; k++)
            {
                pos[parIdx++] = center.x-7 + step * i;
                pos[parIdx++] = center.y-14 + step * j;
                pos[parIdx++] = center.z-14 + step * k;
            }
    for (uint i=0; i<X*Y*Z; i++)
    {
        vals[i] = 80.0;
        states[i] = 0;
        velos[3*i+0] = dir.x*100;
        velos[3*i+1] = dir.y*100;
        velos[3*i+2] = dir.z*100;
    }
    addPars(pos, vals,velos, X*Y*Z, states);
}

void Points::insertCUBEE()
{
    //init a grid or pars
    const uint X=20;
    const uint Y=4;
    const uint Z=20;
    float step = 10.0/26.0;
    GLfloat pos[X*Y*Z*3];
    GLfloat vals[X*Y*Z];
    GLuint states[X*Y*Z];
    GLfloat velos[X*Y*Z*3];
    uint parIdx = 0;
    for (uint i=0; i<X; i++)
        for (uint j=0; j<Y; j++)
            for (uint k=0; k<Z; k++)
            {
                pos[parIdx++] = -20 + step * i;
                pos[parIdx++] = -20 + step * k;
                pos[parIdx++] = -10 + step * j;
            }
    for (uint i=0; i<X*Y*Z; i++)
    {
        vals[i] = 10.0;
        states[i] = 0;
        velos[3*i+0] = 0.0;
        velos[3*i+1] = 0.0;
        velos[3*i+2] = 0.0;
    }
    addPars(pos, vals,velos, X*Y*Z, states);
}

void Points::insertOne(const glm::vec3& origin, const glm::vec3 &velocity, float temperature)
{
    addPars(&origin[0], &temperature, &velocity[0], 1, 0);
}

void Points::insertBar()
{
    //init a grid or pars
    const uint X=30;
    const uint Y=4;
    const uint Z=64;
    float step = 10.0/32.0;
    GLfloat pos[2*X*Y*Z*3];
    GLfloat vals[2*X*Y*Z];
    GLuint states[2*X*Y*Z];
    GLfloat velos[2*X*Y*Z*3];
    uint parIdx = 0;
    for (uint i=0; i<X; i++)
        for (uint j=0; j<Y; j++)
            for (uint k=0; k<Z; k++)
            {
                pos[parIdx++] = -5.2+ step * i;
                pos[parIdx++] = -10 + step * j;
                pos[parIdx++] = -15 + step * k;

                pos[parIdx++] = 5.2- step * i;
                pos[parIdx++] = -10 + step * j;
                pos[parIdx++] = -15 + step * k;
            }
    for (uint i=0; i<X*Y*Z*2; i++)
    {
        vals[i] = 30.0;
        states[i] = 1;
        velos[3*i+0] = 0.0;
        velos[3*i+1] = 0.0;
        velos[3*i+2] = 0.0;
    }
    addPars(pos, vals,velos, X*Y*Z, states);
}
