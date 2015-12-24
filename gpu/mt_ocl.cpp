#include "mt_ocl.h"

const char *cluErrorString(cl_int err) {

   switch(err) {

      case CL_SUCCESS: return "CL_SUCCESS";
      case CL_DEVICE_NOT_FOUND: return "CL_DEVICE_NOT_FOUND";
      case CL_DEVICE_NOT_AVAILABLE: return "CL_DEVICE_NOT_AVAILABLE";
      case CL_COMPILER_NOT_AVAILABLE: return
                                       "CL_COMPILER_NOT_AVAILABLE";
      case CL_MEM_OBJECT_ALLOCATION_FAILURE: return
                                       "CL_MEM_OBJECT_ALLOCATION_FAILURE";
      case CL_OUT_OF_RESOURCES: return "CL_OUT_OF_RESOURCES";
      case CL_OUT_OF_HOST_MEMORY: return "CL_OUT_OF_HOST_MEMORY";
      case CL_PROFILING_INFO_NOT_AVAILABLE: return
                                       "CL_PROFILING_INFO_NOT_AVAILABLE";
      case CL_MEM_COPY_OVERLAP: return "CL_MEM_COPY_OVERLAP";
      case CL_IMAGE_FORMAT_MISMATCH: return "CL_IMAGE_FORMAT_MISMATCH";
      case CL_IMAGE_FORMAT_NOT_SUPPORTED: return
                                       "CL_IMAGE_FORMAT_NOT_SUPPORTED";
      case CL_BUILD_PROGRAM_FAILURE: return "CL_BUILD_PROGRAM_FAILURE";
      case CL_MAP_FAILURE: return "CL_MAP_FAILURE";
      case CL_INVALID_VALUE: return "CL_INVALID_VALUE";
      case CL_INVALID_DEVICE_TYPE: return "CL_INVALID_DEVICE_TYPE";
      case CL_INVALID_PLATFORM: return "CL_INVALID_PLATFORM";
      case CL_INVALID_DEVICE: return "CL_INVALID_DEVICE";
      case CL_INVALID_CONTEXT: return "CL_INVALID_CONTEXT";
      case CL_INVALID_QUEUE_PROPERTIES: return "CL_INVALID_QUEUE_PROPERTIES";
      case CL_INVALID_COMMAND_QUEUE: return "CL_INVALID_COMMAND_QUEUE";
      case CL_INVALID_HOST_PTR: return "CL_INVALID_HOST_PTR";
      case CL_INVALID_MEM_OBJECT: return "CL_INVALID_MEM_OBJECT";
      case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return
                                       "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
      case CL_INVALID_IMAGE_SIZE: return "CL_INVALID_IMAGE_SIZE";
      case CL_INVALID_SAMPLER: return "CL_INVALID_SAMPLER";
      case CL_INVALID_BINARY: return "CL_INVALID_BINARY";
      case CL_INVALID_BUILD_OPTIONS: return "CL_INVALID_BUILD_OPTIONS";
      case CL_INVALID_PROGRAM: return "CL_INVALID_PROGRAM";
      case CL_INVALID_PROGRAM_EXECUTABLE: return
                                       "CL_INVALID_PROGRAM_EXECUTABLE";
      case CL_INVALID_KERNEL_NAME: return "CL_INVALID_KERNEL_NAME";
      case CL_INVALID_KERNEL_DEFINITION: return "CL_INVALID_KERNEL_DEFINITION";
      case CL_INVALID_KERNEL: return "CL_INVALID_KERNEL";
      case CL_INVALID_ARG_INDEX: return "CL_INVALID_ARG_INDEX";
      case CL_INVALID_ARG_VALUE: return "CL_INVALID_ARG_VALUE";
      case CL_INVALID_ARG_SIZE: return "CL_INVALID_ARG_SIZE";
      case CL_INVALID_KERNEL_ARGS: return "CL_INVALID_KERNEL_ARGS";
      case CL_INVALID_WORK_DIMENSION: return "CL_INVALID_WORK_DIMENSION";
      case CL_INVALID_WORK_GROUP_SIZE: return "CL_INVALID_WORK_GROUP_SIZE";
      case CL_INVALID_WORK_ITEM_SIZE: return "CL_INVALID_WORK_ITEM_SIZE";
      case CL_INVALID_GLOBAL_OFFSET: return "CL_INVALID_GLOBAL_OFFSET";
      case CL_INVALID_EVENT_WAIT_LIST: return "CL_INVALID_EVENT_WAIT_LIST";
      case CL_INVALID_EVENT: return "CL_INVALID_EVENT";
      case CL_INVALID_OPERATION: return "CL_INVALID_OPERATION";
      case CL_INVALID_GL_OBJECT: return "CL_INVALID_GL_OBJECT";
      case CL_INVALID_BUFFER_SIZE: return "CL_INVALID_BUFFER_SIZE";
      case CL_INVALID_MIP_LEVEL: return "CL_INVALID_MIP_LEVEL";
      case CL_INVALID_GLOBAL_WORK_SIZE: return "CL_INVALID_GLOBAL_WORK_SIZE";

      default: return "UNKNOWN CL ERROR CODE";
   }
}

int
mtOcl::initializeHost(void)
{
    return SDK_SUCCESS;
}

std::string
convertToString(const char *filename)
{
    size_t size;
    char*  str;
    std::string s;

    std::fstream f(filename, (std::fstream::in | std::fstream::binary));

    if(f.is_open())
    {
        size_t fileSize;
        f.seekg(0, std::fstream::end);
        size = fileSize = (size_t)f.tellg();
        f.seekg(0, std::fstream::beg);

        str = new char[size+1];
        if(!str)
        {
            f.close();
            std::cout << "Memory allocation failed";
            return NULL;
        }

        f.read(str, fileSize);
        f.close();
        str[size] = '\0';
    
        s = str;
        delete[] str;
        return s;
    }
    else
    {
        std::cout << "\nFile containg the kernel code(\".cl\") not found. Please copy the required file in the folder containg the executable.\n";
        exit(1);
    }
    return NULL;
}

int printBuildLog(cl_program program,cl_device_id *devices,size_t deviceListSize)
{
cl_build_status build_status;
cl_int errcode;

for(int i = 0; i < deviceListSize; ++i)
{
    do
    {
        // Query if compilation was successful
        errcode = clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_STATUS,
                                            sizeof(build_status), &build_status, NULL);
        if(errcode) {
	      std::cout << "Error: clGetProgramBuildInfo\n";
      		return SDK_FAILURE;
	}
 
        if(build_status == CL_BUILD_ERROR)
        {
                // Compilation failed. Print the build log
                cl_char *build_log     = NULL;
                size_t   build_log_len = 0;
 
                errcode = clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
        if(errcode) {
	      std::cout << "Error: clGetProgramBuildInfo\n";
      		return SDK_FAILURE;
	}
 
                build_log = (cl_char*)malloc(build_log_len + 1);
                if(!build_log){
	        std::cout << "Error: clGetProgramBuildInfo\n";
      		return SDK_FAILURE;
		}
 
                errcode = clGetProgramBuildInfo(program, devices[i], CL_PROGRAM_BUILD_LOG, build_log_len, build_log, NULL);
                if(errcode)
                {
                    free(build_log);
	        std::cout << "Error: clGetProgramBuildInfo\n";
      		return SDK_FAILURE;
                }
 
                printf("Program build failed. Build log follows:\n%s", build_log);
 
                free(build_log);
        }
    }
    while(build_status == CL_BUILD_IN_PROGRESS);
}

}

int
mtOcl::initializeCL()
{
    cl_int status = 0;
    cl_uint deviceListSize=0;


    cl_uint numPlatforms;
    cl_platform_id* platforms;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    ASSERT_CL(status);

    
    
    if(numPlatforms > 0)
    {

	std::cout <<"Platforms "<<numPlatforms<<" found\n";
        platforms = new cl_platform_id[numPlatforms];
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
	ASSERT_CL(status);
        for(unsigned int i=0; i < numPlatforms; ++i)
        {
            char pbuff[100];
            status = clGetPlatformInfo(
                        platforms[i],
                        CL_PLATFORM_VENDOR,
                        sizeof(pbuff),
                        pbuff,
                        NULL);
	    ASSERT_CL(status);
	    std::cout<<pbuff<<std::endl;
        }

    }

    if(!(pl_idx>=0 && pl_idx<numPlatforms)){
        std::cout << "Incorrect platform index " <<pl_idx<< std::endl;
        return SDK_FAILURE;
    }

    if(NULL == platforms[pl_idx])
    {
        std::cout << "NULL platform found so Exiting Application." << std::endl;
        return SDK_FAILURE;
    }


    status=clGetDeviceIDs(platforms[pl_idx],CL_DEVICE_TYPE_ALL,0,NULL,&deviceListSize);
    ASSERT_CL(status);

    if(!(dev_idx>=0 && dev_idx<deviceListSize)){
        std::cout << "Incorrect device index " <<dev_idx<< std::endl;
        return SDK_FAILURE;
    }


    devices=(cl_device_id*)malloc(deviceListSize*sizeof(cl_device_id));
    if (devices == 0)
    {
        std::cout << "No device memory available\n";
        return SDK_FAILURE;
    }

    status=clGetDeviceIDs(platforms[pl_idx],CL_DEVICE_TYPE_ALL,deviceListSize,devices,NULL);
    ASSERT_CL(status);

    std::cout <<"Devices "<<deviceListSize<<" found\n";


    context=clCreateContext(NULL,deviceListSize,devices,NULL,NULL,&status);

    ASSERT_CL(status);
/*
    cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };

    context = clCreateContextFromType(cps, 
                                      CL_DEVICE_TYPE_ALL, 
                                      NULL, 
                                      NULL, 
                                      &status);
    ASSERT_CL(status);

    status = clGetContextInfo(context, 
                              CL_CONTEXT_DEVICES, 
                              0, 
                              NULL, 
                              &deviceListSize);
    ASSERT_CL(status);

    devices = (cl_device_id *)malloc(deviceListSize);
    if(devices == 0)
    {
        std::cout << "Error: No devices found.\n";
        return SDK_FAILURE;
    }

    // Now, get the device list data
    status = clGetContextInfo(
                 context, 
                 CL_CONTEXT_DEVICES, 
                 deviceListSize, 
                 devices, 
                 NULL);
    ASSERT_CL(status);
*/


    size_t tempSize = 0;
    char* openclCVersion;

    // Device OpenCL C version
    status = clGetDeviceInfo(
                             devices[dev_idx],
                             CL_DEVICE_OPENCL_C_VERSION,
                             0,
                             NULL,
                             &tempSize);
    ASSERT_CL(status);

    openclCVersion = new char[tempSize];
                
    status = clGetDeviceInfo(
                             devices[dev_idx],
                             CL_DEVICE_OPENCL_C_VERSION,
                             sizeof(char) * tempSize,
                             openclCVersion,
                             NULL);
    ASSERT_CL(status);

    std::cout << "Version " << openclCVersion << "\n";


    cl_uint maxClockFq=0; 
    status = clGetDeviceInfo(
                             devices[dev_idx],
                             CL_DEVICE_MAX_CLOCK_FREQUENCY,
                             sizeof(cl_uint),
                             &maxClockFq,
                             NULL);
  
    ASSERT_CL(status);
    std::cout << "Max clock frequency " << maxClockFq << "\n";

    commandQueue = clCreateCommandQueue(
                       context, 
                       devices[dev_idx],
#ifdef MT_ENABLE_KERNEL_PROFILING 
                       CL_QUEUE_PROFILING_ENABLE,
#else
			0,
#endif 
                       &status);
    ASSERT_CL(status);

// https://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
    //const char* buildFlags="-I ./ -cl-std=CL2.0";
    //const char* buildFlags="-cl-fast-relaxed-math";
    //const char* buildFlags=NULL;
    char buildFlags[256];
    std::ifstream fin("build.flags");
    fin.getline(buildFlags, 256);
    fin.close();

    std::cout<<"buildFlags:"<<buildFlags<<"\n";

    

    if(1){
    const char * filename  = MT_KERNELS_FILE;
    std::string  sourceStr = convertToString(filename);
    const char * source    = sourceStr.c_str();
    size_t sourceSize[]    = { strlen(source) };

    program1 = clCreateProgramWithSource(
                  context, 
                  1, 
                  &source,
                  sourceSize,
                  &status);
    ASSERT_CL(status);

    // create a cl program executable for all the devices specified
    
    status = clBuildProgram(program1, 1, devices, buildFlags, NULL, NULL);
    if(status != CL_SUCCESS) 
    { 
        std::cout << "Error: Building Program (clBuildProgram) "<<status<<"\n";

        printBuildLog(program1,devices,deviceListSize);

        return SDK_FAILURE; 
    }
    

#ifdef MT_KERNEL2D_1G_NAME
    kernel2d_1g = clCreateKernel(program1, MT_KERNEL2D_1G_NAME, &status);
    ASSERT_CL(status);
#endif

#ifdef MT_KERNEL2D_MG_NAME
    kernel2d_mg = clCreateKernel(program1, MT_KERNEL2D_MG_NAME, &status);
    ASSERT_CL(status);
#endif
    }



    if(1){
    const char * filename2  = MT_KERNELS2_FILE;
    std::string  sourceStr2 = convertToString(filename2);
    const char * source2    = sourceStr2.c_str();
    size_t sourceSize2[]    = { strlen(source2) };

    program2 = clCreateProgramWithSource(
                  context, 
                  1, 
                  &source2,
                  sourceSize2,
                  &status);
    ASSERT_CL(status);

    // create a cl program executable for all the devices specified
    status = clBuildProgram(program2, 1, devices, buildFlags, NULL, NULL);
    if(status != CL_SUCCESS) 
    { 
        std::cout << "Error: Building Program (clBuildProgram) "<<status<<"\n";

        printBuildLog(program2,devices,deviceListSize);

        return SDK_FAILURE; 
    }
    

#ifdef MT_KERNEL1_NAME
    kernel1 = clCreateKernel(program2, MT_KERNEL1_NAME, &status);
    ASSERT_CL(status);
#endif

#ifdef MT_KERNEL2_NAME
    kernel2 = clCreateKernel(program2, MT_KERNEL2_NAME, &status);
    ASSERT_CL(status);
#endif
#ifdef MT_KERNEL3_NAME
    kernel3 = clCreateKernel(program2, MT_KERNEL3_NAME, &status);
    ASSERT_CL(status);
#endif
    }


    return SDK_SUCCESS;
}

/*

int
mtOcl::initializeBuffers(
                float*  x_in,
                float*  y_in,
                float*  t_in
)
{


    cl_int   status;
    
    xBuf = clCreateBuffer(
                      context, 
                      CL_MEM_READ_WRITE,
                      sizeof(float) * bufSz,
                      NULL, 
                      &status);
    ASSERT_CL(status);
    yBuf = clCreateBuffer(
                      context, 
                      CL_MEM_READ_WRITE,
                      sizeof(float) * bufSz,
                      NULL, 
                      &status);
    ASSERT_CL(status);

    tBuf = clCreateBuffer(
                      context, 
                      CL_MEM_READ_WRITE,
                      sizeof(float) * bufSz,
                      NULL, 
                      &status);
    ASSERT_CL(status);



    lat_l_Buf = clCreateBuffer(
                      context, 
                      CL_MEM_READ_WRITE,
                      sizeof(cl_float4) * 13*(N_d+1),
                      NULL, 
                      &status);
    ASSERT_CL(status);


    lat_r_Buf = clCreateBuffer(
                      context, 
                      CL_MEM_READ_WRITE,
                      sizeof(cl_float4) * 13*(N_d+3),
                      NULL, 
                      &status);
    ASSERT_CL(status);

    long_u_Buf = clCreateBuffer(
                      context, 
                      CL_MEM_READ_WRITE,
                      sizeof(cl_float4) * 13*(N_d+1),
                      NULL, 
                      &status);
    ASSERT_CL(status);


    long_d_Buf = clCreateBuffer(
                      context, 
                      CL_MEM_READ_WRITE,
                      sizeof(cl_float4) * 13*(N_d+1),
                      NULL, 
                      &status);
    ASSERT_CL(status);

    fBuf = clCreateBuffer(
                      context, 
                      CL_MEM_READ_WRITE,
                      sizeof(cl_int) * 13*N_d,
                      NULL, 
                      &status);
    ASSERT_CL(status);


    return SDK_SUCCESS;
}

int 
mtOcl::prepareCLKernels()
{
    cl_int   status;
    cl_uint maxDims;
    cl_event events[2];

    size_t maxWorkGroupSize;
    size_t maxWorkItemSizes[3];

    
    status = clGetDeviceInfo(
        devices[dev_idx], 
        CL_DEVICE_MAX_WORK_GROUP_SIZE, 
        sizeof(size_t), 
        (void*)&maxWorkGroupSize, 
        NULL);
    ASSERT_CL(status);

    std::cout<<"maxWorkGroupSize"<<maxWorkGroupSize<<std::endl;

    if(lszX > maxWorkGroupSize )
    {
        std::cout << "Unsupported: Device does not support requested number of work items.";
        return SDK_FAILURE;
    }
    
    status = clGetDeviceInfo(
        devices[dev_idx], 
        CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 
        sizeof(cl_uint), 
        (void*)&maxDims, 
        NULL);
    ASSERT_CL(status);


    std::cout<<"maxDims"<<maxDims<<std::endl;

    status = clGetDeviceInfo(
        devices[dev_idx], 
        CL_DEVICE_MAX_WORK_ITEM_SIZES, 
        sizeof(size_t)*maxDims,
        (void*)maxWorkItemSizes,
        NULL);
    ASSERT_CL(status);

    std::cout<<"maxWorkItemSizes:";
    for (int i=0;i<maxDims;i++)
    {
        std::cout<<maxWorkItemSizes[i]<<";";
    }
	std::cout<<"\n";

    size_t kernel2DWorkGroupSize; 

    status = clGetKernelWorkGroupInfo(kernel1,
                                      devices[dev_idx],
                                      CL_KERNEL_WORK_GROUP_SIZE,
                                      sizeof(size_t),
                                      &kernel2DWorkGroupSize,
                                      0);

    ASSERT_CL(status);

    std::cout<<"WorkGroupSize"<<kernel2DWorkGroupSize<<std::endl;

    size_t kernel2DWorkGroupSizeMult; 

    status = clGetKernelWorkGroupInfo(kernel1,
                                      devices[dev_idx],
                                      CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                                      sizeof(size_t),
                                      &kernel2DWorkGroupSizeMult,
                                      0);
    ASSERT_CL(status);

    std::cout<<"WorkGroupSizeMult"<<kernel2DWorkGroupSizeMult<<std::endl;

    size_t tempSize = 0;
    char* extensions;
            // Device extensions
            status = clGetDeviceInfo(
                         devices[dev_idx],
                         CL_DEVICE_EXTENSIONS,
                         0,
                         NULL,
                         &tempSize);
    ASSERT_CL(status);

  //          if(extensions != NULL) delete[] extensions;
            extensions = new char[tempSize];
//            CHECK_ALLOCATION(extensions, "Failed to allocate memory(extensions)");
            status = clGetDeviceInfo(
                         devices[dev_idx],
                         CL_DEVICE_EXTENSIONS,
                         sizeof(char) * tempSize,
                         extensions,
                         NULL);
//            CHECK_OPENCL_ERROR(status, "clGetDeviceInfo(CL_DEVICE_EXTENSIONS) failed");
    std::cout<<"extensions "<<extensions<<std::endl;
    delete[] extensions; 

  return SDK_SUCCESS;
}
*/

cl_int spinForEventsComplete( cl_uint num_events, cl_event *event_list )
{
    cl_int ret = 0;
#if 0
    ret = clWaitForEvents( num_events, event_list );
#else
    cl_int param_value;
    size_t param_value_size_ret;

    for( cl_uint e=0; e < num_events; e++ )
    {
        while(1)
        {
            ret |= clGetEventInfo( event_list[ e ], 
                                   CL_EVENT_COMMAND_EXECUTION_STATUS,
                                   sizeof( cl_int ),
                                   &param_value,
                                   &param_value_size_ret );

            if( param_value == CL_COMPLETE )
                break;
        }
    }
#endif

//    for( cl_uint e=0; e < num_events; e++ )
//        clReleaseEvent( event_list[e] );

    return ret;
}


int mtOcl::preRunCLKernels0(float*  x,float*  y, float*  t)
{
    cl_int   status;

    xBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSz,NULL, &status);
    ASSERT_CL(status);
    yBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSz,NULL, &status);
    ASSERT_CL(status);
    tBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSz,NULL, &status);
    ASSERT_CL(status);


    if(use_rw_buffer == 0){

    cl_map_flags mapFlag = CL_MAP_READ | CL_MAP_WRITE;
    _x = (float*) clEnqueueMapBuffer(commandQueue, xBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _y = (float*) clEnqueueMapBuffer(commandQueue, yBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _t = (float*) clEnqueueMapBuffer(commandQueue, tBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);


    }

    status = clSetKernelArg(kernel2d_1g,0,sizeof(cl_int),(void *)&numIter);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2d_1g,1,sizeof(cl_mem),(void *)&xBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2d_1g,2,sizeof(cl_mem),(void *)&yBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2d_1g,3,sizeof(cl_mem),(void *)&tBuf);
    ASSERT_CL(status);    

    return SDK_SUCCESS;
}

int mtOcl::runCLKernels0(float*  x,float*  y, float*  t)
{
    cl_int   status;
 
    cl_event writeEvt[3];

    if(use_rw_buffer){

    status = clEnqueueWriteBuffer(commandQueue,xBuf,CL_TRUE,0,sizeof(float) * bufSz,x,0,NULL,NULL);
    ASSERT_CL(status);    
    status = clEnqueueWriteBuffer(commandQueue,yBuf,CL_TRUE,0,sizeof(float) * bufSz,y,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,tBuf,CL_TRUE,0,sizeof(float) * bufSz,t,0,NULL,NULL);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);

    } // if use_rw_ buffer
    else{
    memcpy(_x, x, sizeof(float) * bufSz);
    memcpy(_y, y, sizeof(float) * bufSz);
    memcpy(_t, t, sizeof(float) * bufSz);
    }


    size_t globalThreads[] = {gszX, gszY};
    size_t localThreads[] = {lszX, lszY};
    cl_event ndrEvt=0;
    status = clEnqueueNDRangeKernel(commandQueue,kernel2d_1g,2,NULL,globalThreads,localThreads,0,NULL,&ndrEvt);
    ASSERT_CL(status);


    status = clFinish(commandQueue);	
    ASSERT_CL(status);

#ifdef MT_ENABLE_KERNEL_PROFILING
        cl_ulong start, end;
        status = clGetEventProfilingInfo(ndrEvt, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
        ASSERT_CL(status);
        status = clGetEventProfilingInfo(ndrEvt, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
        ASSERT_CL(status);
        prof_time = 1.0e-9 * (double)(end - start);

#endif



    cl_event readEvt[3];


    if(use_rw_buffer){

    status = clEnqueueReadBuffer(commandQueue,xBuf,CL_TRUE,0,sizeof(float) * bufSz,x,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueReadBuffer(commandQueue,yBuf,CL_TRUE,0,sizeof(float) * bufSz,y,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueReadBuffer(commandQueue,tBuf,CL_TRUE,0,sizeof(float) * bufSz,t,0,NULL,NULL);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);

    }
    else{
    memcpy(x, _x, sizeof(float) * bufSz);
    memcpy(y, _y, sizeof(float) * bufSz);
    memcpy(t, _t, sizeof(float) * bufSz);
    }

    return SDK_SUCCESS;
}


int mtOcl::postRunCleanup0(void)
{
    cl_int status;
    if(use_rw_buffer ==0){

    status = clEnqueueUnmapMemObject(commandQueue, xBuf, _x, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, yBuf, _y, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, tBuf, _t, 0, NULL,NULL);
    ASSERT_CL(status);

    status = clFlush(commandQueue);
    ASSERT_CL(status);

    }


    status = clReleaseMemObject(xBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(yBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(tBuf);
    ASSERT_CL(status);

    return SDK_SUCCESS;
}

int mtOcl::preRunCLKernels1(float*  x,float*  y, float*  t)
{
    cl_int   status;


    xBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSzNg,NULL, &status);
    ASSERT_CL(status);
    yBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSzNg,NULL, &status);
    ASSERT_CL(status);
    tBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSzNg,NULL, &status);
    ASSERT_CL(status);


    if(use_rw_buffer == 0){

    cl_map_flags mapFlag = CL_MAP_READ | CL_MAP_WRITE;

    _x = (float*) clEnqueueMapBuffer(commandQueue, xBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSzNg, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _y = (float*) clEnqueueMapBuffer(commandQueue, yBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSzNg, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _t = (float*) clEnqueueMapBuffer(commandQueue, tBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSzNg, 0, NULL, NULL, &status);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);

    
    }

    status = clSetKernelArg(kernel2d_mg,0,sizeof(cl_int),(void *)&numIter);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2d_mg,1,sizeof(cl_mem),(void *)&xBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2d_mg,2,sizeof(cl_mem),(void *)&yBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2d_mg,3,sizeof(cl_mem),(void *)&tBuf);
    ASSERT_CL(status);

    return SDK_SUCCESS;
}
int mtOcl::runCLKernels1(float*  x,float*  y, float*  t)
{
    cl_int   status;

    cl_event writeEvt[3];


    if(use_rw_buffer){

    status = clEnqueueWriteBuffer(commandQueue,xBuf,CL_TRUE,0,sizeof(float) * bufSzNg,x,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,yBuf,CL_TRUE,0,sizeof(float) * bufSzNg,y,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,tBuf,CL_TRUE,0,sizeof(float) * bufSzNg,t,0,NULL,NULL);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);


    } // use_rw_buffer
    else{
    memcpy(_x, x, sizeof(float) * bufSzNg);
    memcpy(_y, y, sizeof(float) * bufSzNg);
    memcpy(_t, t, sizeof(float) * bufSzNg);

    }



    size_t globalThreads[] = {gszX, gszY};
    size_t localThreads[] = {lszX, lszY};
    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(commandQueue,kernel2d_mg,2,NULL,globalThreads,localThreads,0,NULL,&ndrEvt);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);

#ifdef MT_ENABLE_KERNEL_PROFILING
        cl_ulong start, end;
        status = clGetEventProfilingInfo(ndrEvt, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
        ASSERT_CL(status);
        status = clGetEventProfilingInfo(ndrEvt, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
        ASSERT_CL(status);
        prof_time = 1.0e-9 * (double)(end - start);

#endif


    if(use_rw_buffer){

    status = clEnqueueReadBuffer(commandQueue,xBuf,CL_TRUE,0,sizeof(float) * bufSzNg,x,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueReadBuffer(commandQueue,yBuf,CL_TRUE,0,sizeof(float) * bufSzNg,y,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueReadBuffer(commandQueue,tBuf,CL_TRUE,0,sizeof(float) * bufSzNg,t,0,NULL,NULL);
    ASSERT_CL(status);
    status = clFinish(commandQueue);	
    ASSERT_CL(status);

    } else {

    memcpy(x, _x, sizeof(float) * bufSzNg);
    memcpy(y, _y, sizeof(float) * bufSzNg);
    memcpy(t, _t, sizeof(float) * bufSzNg);

    }
    return SDK_SUCCESS;
}

int mtOcl::postRunCleanup1(void)
{
    cl_int status;
    
  
    if(use_rw_buffer == 0){

    status = clEnqueueUnmapMemObject(commandQueue, xBuf, _x, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, yBuf, _y, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, tBuf, _t, 0, NULL,NULL);
    ASSERT_CL(status);

    status = clFlush(commandQueue);
    ASSERT_CL(status);

    }

    status = clReleaseMemObject(xBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(yBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(tBuf);
    ASSERT_CL(status);

    return SDK_SUCCESS;

}




// ----------------------------------------------------------------

int mtOcl::preRunCLKernels2(float*  x,float*  y, float*  t)
{
    cl_int   status;


    xBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSz,NULL, &status);
    ASSERT_CL(status);
    yBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSz,NULL, &status);
    ASSERT_CL(status);
    tBuf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(float) * bufSz,NULL, &status);
    ASSERT_CL(status);
    lat_l_Buf = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(cl_float4) * 13*(N_d+1), NULL, &status);
    ASSERT_CL(status);
    lat_r_Buf = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(cl_float4) * 13*(N_d+3), NULL, &status);
    ASSERT_CL(status);
    long_u_Buf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float4) * 13*(N_d+1),NULL, &status);
    ASSERT_CL(status);
    long_d_Buf = clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float4) * 13*(N_d+1),NULL, &status);
    ASSERT_CL(status);
    fBuf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_int) * 13*N_d,NULL,  &status);
    ASSERT_CL(status);


    if(use_rw_buffer == 0){

    cl_map_flags mapFlag = CL_MAP_READ | CL_MAP_WRITE;

    _x = (float*) clEnqueueMapBuffer(commandQueue, xBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _y = (float*) clEnqueueMapBuffer(commandQueue, yBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _t = (float*) clEnqueueMapBuffer(commandQueue, tBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _ll = (float*) clEnqueueMapBuffer(commandQueue, lat_l_Buf, CL_TRUE, mapFlag, 0, sizeof(cl_float4) * 13*(N_d+1), 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _lr = (float*) clEnqueueMapBuffer(commandQueue, lat_r_Buf, CL_TRUE, mapFlag, 0, sizeof(cl_float4) * 13*(N_d+3), 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _lu = (float*)clEnqueueMapBuffer(commandQueue, long_u_Buf, CL_TRUE, mapFlag, 0, sizeof(cl_float4) * 13*(N_d+1), 0, NULL, NULL, &status);
    ASSERT_CL(status);
    _ld = (float*)clEnqueueMapBuffer(commandQueue, long_d_Buf, CL_TRUE, mapFlag, 0, sizeof(cl_float4) * 13*(N_d+1), 0, NULL, NULL, &status);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);  
    
    }
    else{
     _ll = (float *) malloc(sizeof(cl_float4) * 13*(N_d+1));
     memset(_ll,0,sizeof(cl_float4) * 13*(N_d+1));
     _lr = (float *) malloc(sizeof(cl_float4) * 13*(N_d+3));
     memset(_lr,0,sizeof(cl_float4) * 13*(N_d+3));
     _lu = (float *) malloc(sizeof(cl_float4) * 13*(N_d+1));
     memset(_lu,0,sizeof(cl_float4) * 13*(N_d+1));
     _ld = (float *) malloc(sizeof(cl_float4) * 13*(N_d+1));
     memset(_ld,0,sizeof(cl_float4) * 13*(N_d+1));
    }


    status = clSetKernelArg(kernel1,0,sizeof(cl_int),(void *)&numIter);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel1,1,sizeof(cl_mem),(void *)&xBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel1,2,sizeof(cl_mem),(void *)&yBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel1,3,sizeof(cl_mem),(void *)&tBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel1,4,sizeof(cl_mem),(void *)&lat_l_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel1,5,sizeof(cl_mem),(void *)&lat_r_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel1,6,sizeof(cl_mem),(void *)&long_u_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel1,7,sizeof(cl_mem),(void *)&long_d_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel1,8,sizeof(cl_mem),(void *)&fBuf);
    ASSERT_CL(status);

    status = clSetKernelArg(kernel2,0,sizeof(cl_int),(void *)&numIter);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2,1,sizeof(cl_mem),(void *)&xBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2,2,sizeof(cl_mem),(void *)&yBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2,3,sizeof(cl_mem),(void *)&tBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2,4,sizeof(cl_mem),(void *)&lat_l_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2,5,sizeof(cl_mem),(void *)&lat_r_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2,6,sizeof(cl_mem),(void *)&long_u_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2,7,sizeof(cl_mem),(void *)&long_d_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel2,8,sizeof(cl_mem),(void *)&fBuf);
    ASSERT_CL(status);


    return SDK_SUCCESS;
}

int mtOcl::runCLKernels2(float*  x,float*  y, float*  t)
{
    cl_int   status;
    cl_event writeEvt[3];

    if(use_rw_buffer){

    status = clEnqueueWriteBuffer(commandQueue,xBuf,CL_TRUE,0,sizeof(float) * bufSz,x,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,yBuf,CL_TRUE,0,sizeof(float) * bufSz,y,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,tBuf,CL_TRUE,0,sizeof(float) * bufSz,t,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,lat_l_Buf,CL_TRUE,0,sizeof(cl_float4) * 13*(N_d+1),_ll,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,lat_r_Buf,CL_TRUE,0,sizeof(cl_float4) * 13*(N_d+3),_lr,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,long_u_Buf,CL_TRUE,0,sizeof(cl_float4) * 13*(N_d+1),_lu,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueWriteBuffer(commandQueue,long_d_Buf,CL_TRUE,0,sizeof(cl_float4) * 13*(N_d+1),_ld,0,NULL,NULL);
    ASSERT_CL(status);
    status = clFinish(commandQueue);	
    ASSERT_CL(status);

    } // use_rw_buffer
    else{
    memcpy(_x, x, sizeof(float) * bufSz);
    memcpy(_y, y, sizeof(float) * bufSz);
    memcpy(_t, t, sizeof(float) * bufSz);
    memset(_ll, 0, sizeof(cl_float4) * 13*(N_d+1));
    memset(_lr, 0, sizeof(cl_float4) * 13*(N_d+3));
    memset(_lu, 0, sizeof(cl_float4) * 13*(N_d+1));
    memset(_ld, 0, sizeof(cl_float4) * 13*(N_d+1));
    }


    size_t globalThreads[] = {gszX, gszY};
    size_t localThreads[] = {lszX, lszY};
    cl_event ndrEvt[2] = {0};

    for(int i=0;i<numIter-1;i++){ // numIter = 1000

    status = clEnqueueNDRangeKernel(commandQueue,kernel1,2,NULL,globalThreads,localThreads,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueNDRangeKernel(commandQueue,kernel2,2,NULL,globalThreads,localThreads,0,NULL,NULL);
    ASSERT_CL(status);
    if(i>0 && i%16==0){ // just
    status = clFlush(commandQueue);
    ASSERT_CL(status);
    } // if
    } // for i

    status = clEnqueueNDRangeKernel(commandQueue,kernel1,2,NULL,globalThreads,localThreads,0,NULL,&ndrEvt[0]);
    ASSERT_CL(status);
    status = clEnqueueNDRangeKernel(commandQueue,kernel2,2,NULL,globalThreads,localThreads,0,NULL,&ndrEvt[1]);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);


#ifdef MT_ENABLE_KERNEL_PROFILING
        cl_ulong start, end;
        start=end=0;
        status = clGetEventProfilingInfo(ndrEvt[0], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
        ASSERT_CL(status);
        status = clGetEventProfilingInfo(ndrEvt[0], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
        ASSERT_CL(status);
        double t0 = 1.0e-9 * (double)(end - start);
        start=end=0;
        status = clGetEventProfilingInfo(ndrEvt[1], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
        ASSERT_CL(status);
        status = clGetEventProfilingInfo(ndrEvt[1], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
        ASSERT_CL(status);
        double t1 = 1.0e-9 * (double)(end - start);

        prof_time = (t0 + t1)*numIter;

#endif




    if(use_rw_buffer){

    cl_event readEvt[3];
    status = clEnqueueReadBuffer(commandQueue,xBuf,CL_TRUE,0,sizeof(float) * bufSz,x,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueReadBuffer(commandQueue,yBuf,CL_TRUE,0,sizeof(float) * bufSz,y,0,NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueReadBuffer(commandQueue,tBuf,CL_TRUE,0,sizeof(float) * bufSz,t,0,NULL,NULL);
    ASSERT_CL(status);

    status = clFinish(commandQueue);	
    ASSERT_CL(status);

    } else {

    memcpy(x, _x, sizeof(float) * bufSz);
    memcpy(y, _y, sizeof(float) * bufSz);
    memcpy(t, _t, sizeof(float) * bufSz);

    }
    return SDK_SUCCESS;
}


int mtOcl::postRunCleanup2(void)
{
    cl_int status;

    if(use_rw_buffer == 0){

    status = clEnqueueUnmapMemObject(commandQueue, xBuf, _x, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, yBuf, _y, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, tBuf, _t, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, lat_l_Buf, _ll, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, lat_r_Buf, _lr, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, long_d_Buf, _ld, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, long_u_Buf, _lu, 0, NULL,NULL);
    ASSERT_CL(status);

    }
    else{
     free(_ll);
     free(_lr);
     free(_lu);
     free(_ld);
    }


    status = clReleaseMemObject(xBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(yBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(tBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(lat_l_Buf);
    ASSERT_CL(status);
    status = clReleaseMemObject(lat_r_Buf);
    ASSERT_CL(status);
    status = clReleaseMemObject(long_u_Buf);
    ASSERT_CL(status);
    status = clReleaseMemObject(long_d_Buf);
    ASSERT_CL(status);
    status = clReleaseMemObject(fBuf);
    ASSERT_CL(status);

    return SDK_SUCCESS;
}


// -------------------------------------------------------------------------
/*

int mtOcl::preRunCLKernels3(float*  x,float*  y, float*  t)
{
    cl_int   status;

    cl_map_flags mapFlag = CL_MAP_READ | CL_MAP_WRITE;

    _x = (float*) clEnqueueMapBuffer(commandQueue, xBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    memcpy(_x, x, sizeof(float) * bufSz);
    _y = (float*) clEnqueueMapBuffer(commandQueue, yBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    memcpy(_y, y, sizeof(float) * bufSz);
    _t = (float*) clEnqueueMapBuffer(commandQueue, tBuf, CL_TRUE, mapFlag, 0, sizeof(float) * bufSz, 0, NULL, NULL, &status);
    ASSERT_CL(status);
    memcpy(_t, t, sizeof(float) * bufSz);
    _ll = (float*) clEnqueueMapBuffer(commandQueue, lat_l_Buf, CL_TRUE, mapFlag, 0, sizeof(cl_float4) * 13*(N_d+1), 0, NULL, NULL, &status);
    ASSERT_CL(status);
    memset(_ll, 0, sizeof(cl_float4) * 13*(N_d+1));
    _lr = (float*) clEnqueueMapBuffer(commandQueue, lat_r_Buf, CL_TRUE, mapFlag, 0, sizeof(cl_float4) * 13*(N_d+3), 0, NULL, NULL, &status);
    ASSERT_CL(status);
    memset(_lr, 0, sizeof(cl_float4) * 13*(N_d+3));
    _lu = (float*)clEnqueueMapBuffer(commandQueue, long_u_Buf, CL_TRUE, mapFlag, 0, sizeof(cl_float4) * 13*(N_d+1), 0, NULL, NULL, &status);
    ASSERT_CL(status);
    memset(_lu, 0, sizeof(cl_float4) * 13*(N_d+1));
    _ld = (float*)clEnqueueMapBuffer(commandQueue, long_d_Buf, CL_TRUE, mapFlag, 0, sizeof(cl_float4) * 13*(N_d+1), 0, NULL, NULL, &status);
    ASSERT_CL(status);
    memset(_ld, 0, sizeof(cl_float4) * 13*(N_d+1));


   
    status = clSetKernelArg(kernel3,0,sizeof(cl_int),(void *)&numIter);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel3,1,sizeof(cl_mem),(void *)&xBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel3,2,sizeof(cl_mem),(void *)&yBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel3,3,sizeof(cl_mem),(void *)&tBuf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel3,4,sizeof(cl_mem),(void *)&lat_l_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel3,5,sizeof(cl_mem),(void *)&lat_r_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel3,6,sizeof(cl_mem),(void *)&long_u_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel3,7,sizeof(cl_mem),(void *)&long_d_Buf);
    ASSERT_CL(status);
    status = clSetKernelArg(kernel3,8,sizeof(cl_mem),(void *)&fBuf);
    ASSERT_CL(status);

    return SDK_SUCCESS;
}

int mtOcl::runCLKernels3(float*  x,float*  y, float*  t)
{
    cl_int   status;

    size_t globalThreads[] = {gszX, gszY};
    size_t localThreads[] = {lszX, lszY};
    cl_event ndrEvt = 0;


    status = clEnqueueNDRangeKernel(commandQueue,kernel1,2,NULL,globalThreads,localThreads,0,NULL,&ndrEvt);
    ASSERT_CL(status);
    status = clFlush(commandQueue);
    ASSERT_CL(status);
    spinForEventsComplete( 1, &ndrEvt );

    memcpy(x, _x, sizeof(float) * bufSz);
    memcpy(y, _y, sizeof(float) * bufSz);
    memcpy(t, _t, sizeof(float) * bufSz);

    return SDK_SUCCESS;
}


int mtOcl::postRunCleanup3(void)
{
    cl_int status;
    status = clEnqueueUnmapMemObject(commandQueue, xBuf, _x, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, yBuf, _y, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, tBuf, _t, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, lat_l_Buf, _ll, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, lat_r_Buf, _lr, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, long_d_Buf, _ld, 0, NULL,NULL);
    ASSERT_CL(status);
    status = clEnqueueUnmapMemObject(commandQueue, long_u_Buf, _lu, 0, NULL,NULL);
    ASSERT_CL(status);
    return SDK_SUCCESS;
}
*/

int  
mtOcl::cleanupCL(void)
{
    cl_int status;


    status = clReleaseKernel(kernel2d_1g);
    ASSERT_CL(status);
    status = clReleaseKernel(kernel2d_mg);
    ASSERT_CL(status);
    
    status = clReleaseKernel(kernel1);
    ASSERT_CL(status);
    status = clReleaseKernel(kernel2);
    ASSERT_CL(status);
    status = clReleaseProgram(program1);
    ASSERT_CL(status);
    status = clReleaseProgram(program2);
    ASSERT_CL(status);
/*
    status = clReleaseMemObject(xBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(yBuf);
    ASSERT_CL(status);
    status = clReleaseMemObject(tBuf);
    ASSERT_CL(status);
*/
    status = clReleaseCommandQueue(commandQueue);
    ASSERT_CL(status);
    status = clReleaseContext(context);
    ASSERT_CL(status);
    return SDK_SUCCESS;
}


void
mtOcl::cleanupHost(void)
{
    if(devices != NULL)
    {
        free(devices);
        devices = NULL;
    }
}





int mtOcl::run(	int algo, int		n_step,
				int 	load_coords,
				float* 	x_inout,
				float* 	y_inout,
				float* 	t_inout

			)

{
 int r=0;
	switch(algo){
	case 1: 
           if(load_coords){ 
		r=preRunCLKernels0((float*)x_inout,(float*)y_inout,(float*)t_inout);
		if(r!=SDK_SUCCESS)return r;
	   }
           return runCLKernels0((float*)x_inout,(float*)y_inout,(float*)t_inout);
	case 2: 
           if(load_coords){ 
		r=preRunCLKernels1((float*)x_inout,(float*)y_inout,(float*)t_inout);
		if(r!=SDK_SUCCESS)return r;
	   }
           return runCLKernels1((float*)x_inout,(float*)y_inout,(float*)t_inout);
	case 3:
           if(load_coords){ 
		r=preRunCLKernels2((float*)x_inout,(float*)y_inout,(float*)t_inout);
		if(r!=SDK_SUCCESS)return r;
	   }
           r=runCLKernels2((float*)x_inout,(float*)y_inout,(float*)t_inout);
           return r;
/*
	case 4:
           if(load_coords){ 
		r=preRunCLKernels3((float*)x_inout,(float*)y_inout,(float*)t_inout);
		if(r!=SDK_SUCCESS)return r;
	   }
           r=runCLKernels3((float*)x_inout,(float*)y_inout,(float*)t_inout);
           return r;
*/
	}
return 0;
}

int mtOcl::run2(	int		n_step,
				int 	load_coords,
				float* 	x_inout,
				float* 	y_inout,
				float* 	t_inout

			)

{
 int r=0;
           if(load_coords){ 
		r=preRunCLKernels1((float*)x_inout,(float*)y_inout,(float*)t_inout);
		if(r!=SDK_SUCCESS)return r;
	   }
           return runCLKernels1((float*)x_inout,(float*)y_inout,(float*)t_inout);
}


int mtOcl::postRun(int algo){
	switch(algo){
	case 1: return postRunCleanup0();
        case 2: return postRunCleanup1();
        case 3: return postRunCleanup2();
 //       case 4: return postRunCleanup3();
        }
return 0;
}

