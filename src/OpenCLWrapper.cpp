
#ifndef OPENCL_WRAPPER_CPP
#define OPENCL_WRAPPER_CPP

#include "OpenCLWrapper.h"

namespace CL
{
	cl_uint numberOfPlatforms = 0;
	cl_platform_id platformID = nullptr;
	
	cl_uint numberOfDevices = 0;
	cl_device_id deviceID = nullptr;
	
	cl_context context;
	
	cl_command_queue commandQueue;
	
	std::map < void*, MemoryObjectData* > memoryObjects;
	std::map < std::string, ProgramData* > programs;
	
	
	
	
	
	unsigned Init()		// TODO: error checking
	{
		cl_int ret = 0;
		ret = clGetPlatformIDs( 1, &platformID, &numberOfPlatforms );
		PrintError( ret );
		ret = clGetDeviceIDs( platformID, CL_DEVICE_TYPE_GPU, 1, &deviceID, &numberOfDevices );
		PrintError( ret );
		context = clCreateContext( nullptr, 1, &deviceID, nullptr, nullptr, &ret );
		PrintError( ret );
		commandQueue = clCreateCommandQueue( context, deviceID, 0, &ret );
		PrintError( ret );
		printf( "\n OpenCL inited" );
	}
	
	void DeInit()
	{
		cl_int ret = 0;
		ret = clFlush( commandQueue );
		PrintError( ret );
		ret = clFinish( commandQueue );
		PrintError( ret );
		
		for( auto it1 = programs.begin(); it1 != programs.end(); ++it1 )
		{
			for( auto it2 = it1->second->kernels.begin(); it2 != it1->second->kernels.end(); ++it2 )
			{
				ret = clReleaseKernel( *(it2->second) );
				PrintError( ret );
				delete (it2->second);
			}
			ret = clReleaseProgram( it1->second->program );
			PrintError( ret );
			it1->second->kernels.clear();
			delete it1->second;
		}
		programs.clear();
		
		for( auto it = memoryObjects.begin(); it!= memoryObjects.end(); ++it )
		{
			ret = clReleaseMemObject( it->second->mem );
			PrintError( ret );
			free( it->second );
			free( it->first );
		}
		memoryObjects.clear();
		
		ret = clReleaseCommandQueue( commandQueue );
		PrintError( ret );
		ret = clReleaseContext( context );
		PrintError( ret );
	}
	
	
	
	cl_mem * GetMemoryObject( void* ptr )
	{
		if( ptr )
		{
			auto it = memoryObjects.find( ptr );
			if( it != memoryObjects.end() )
			{
				return &(it->second->mem);
			}
		}
		return nullptr;
	}
	
	
	
	unsigned Free( void* ptr )
	{
		cl_int ret = 0;
		if( ptr )
		{
			auto it = memoryObjects.find( ptr );
			if( it != memoryObjects.end() )
			{
				ret = clReleaseMemObject( it->second->mem );
				if( ret )
					PrintError( ret );
				free( it->second );
				memoryObjects.erase( it );
				free( ptr );
				return ret;
			}
			else
			{
				printf( "\n Trying to free unexisting memory object at memory point: %llu ", (unsigned long long)(ptr) );
				return 100;
			}
		}
		else
		{
			printf( "\n Trying to CL::Free( nullptr ) " );
		}
		return 0;
	}
	
	unsigned CopyToVRAM( void* ptr )
	{
		cl_int ret = 0;
		if( ptr )
		{
			auto it = memoryObjects.find( ptr );
			if( it != memoryObjects.end() )
			{
				ret = clEnqueueWriteBuffer( commandQueue, it->second->mem, CL_TRUE, 0, it->second->elements * it->second->bytesPerElement, ptr, 0, nullptr, nullptr );
				ret = clFlush( commandQueue );
				ret = clFinish( commandQueue );
				PrintError( ret );
				return ret;
			}
		}
		return 0;
	}
	
	unsigned CopyFromVRAM( void* ptr )
	{
		cl_int ret = 0;
		if( ptr )
		{
			auto it = memoryObjects.find( ptr );
			if( it != memoryObjects.end() )
			{
				ret = clEnqueueReadBuffer( commandQueue, it->second->mem, CL_TRUE, 0, it->second->elements * it->second->bytesPerElement, ptr, 0, nullptr, nullptr );
				ret = clFlush( commandQueue );
				ret = clFinish( commandQueue );
				PrintError( ret );
				return ret;
			}
		}
		return 0;
	}
	
	
	
	cl_kernel* GetFunction( const char* fileName, const char* function, unsigned * error )
	{
		if( fileName && function )
		{
			cl_int ret = 0;
			auto it = programs.find( fileName );
			if( it != programs.end() )
			{
				auto it2 = it->second->kernels.find( function );
				if( it2 != it->second->kernels.end() )
				{
					return it2->second;
				}
				else
				{
					cl_kernel* kernel = (cl_kernel*)malloc( sizeof(cl_kernel) );
					if( kernel )
					{
						*kernel = clCreateKernel( it->second->program, function, &ret );
						if( ret != 0 )
						{
							if( error )
								*error = ret;
							delete kernel;
							PrintError( ret );
							return nullptr;
						}
						
						it->second->kernels[ function ] = kernel;
						
						return kernel;
					}
					else
					{
						printf( "\n Can not allocate memory for kernel" );
						if( error )
							*error = 203;
						return nullptr;
					}
				}
			}
			else
			{
				FILE * file = fopen( fileName, "rb" );
				if( file )
				{
					fseek( file, 0, SEEK_END );
					size_t size = ftell( file );
					fseek( file, 0, SEEK_SET );
					char* source = (char*)malloc( size + 1 );
					
					if( source )
					{
						fread( source, size, 1, file );
						fclose( file );
						source[size] = 0;
						
						cl_int ret = 0;
						
						ProgramData* data = new ProgramData;
						data->program = clCreateProgramWithSource( context, 1, (const char **)&(source), (const size_t*)&(size), &ret );
						free( source );
						
						ret = clBuildProgram( data->program, 1, &deviceID, nullptr, nullptr, nullptr );
						
						if( ret == 0 )
						{
							PrintError( ret );
							programs[fileName] = data;
							return GetFunction( fileName, function, error );
						}
						else
						{
							if( error )
								*error = ret;
							delete data;
							printf( "\n Can not load or build program from source file: \"%s\" ", fileName );
							return nullptr;
						}
					}
					else
					{
						printf( "\n Can not allocate memory for file buffer" );
						if( error )
							*error = 201;
						// error
						fclose( file );
						return nullptr;
					}
				}
				else
				{
					if( error )
						*error = 202;
					printf( "\n Can not open file: \"%s\" ", fileName );
				}
			}
			return nullptr;
		}
		if( error )
			*error = 200;
		return nullptr;
	}
	
	unsigned DestroyFunction( cl_kernel* kernel )
	{
		cl_int ret = 0;
		if( kernel )
		{
			for( auto it1 = programs.begin(); it1 != programs.end(); ++it1 )
			{
				for( auto it2 = it1->second->kernels.begin(); it2 != it1->second->kernels.end(); ++it2 )
				{
					if( it2->second == kernel )
					{
						ret = clReleaseKernel( *kernel );
						delete kernel;
						PrintError( ret );
						it1->second->kernels.erase( it2 );
						if( it1->second->kernels.empty() )
						{
							clReleaseProgram( it1->second->program );
							delete it1->second;
							programs.erase( it1 );
						}
						return ret;
					}
				}
			}
		}
		return 300;
	}
	
	
	
	unsigned For_( unsigned long long iterations, cl_kernel* kernel )
	{
		if( kernel )
		{
			cl_int ret = 0;
			size_t globalItemSize = iterations;
			size_t localItemSize = 64;
			ret = clEnqueueNDRangeKernel( commandQueue, *kernel, 1, nullptr, &globalItemSize, &localItemSize, 0, nullptr, nullptr );
			PrintError( ret );
			return ret;
		}
		printf( "\n No kernel given to For_" );
		return 666;
	}
	
	
	
	const char * GetErrorString( cl_int error )
	{
		switch(error)
		{
			// run-time and JIT compiler errors
			case 0: return "CL_SUCCESS";
			case -1: return "CL_DEVICE_NOT_FOUND";
			case -2: return "CL_DEVICE_NOT_AVAILABLE";
			case -3: return "CL_COMPILER_NOT_AVAILABLE";
			case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
			case -5: return "CL_OUT_OF_RESOURCES";
			case -6: return "CL_OUT_OF_HOST_MEMORY";
			case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
			case -8: return "CL_MEM_COPY_OVERLAP";
			case -9: return "CL_IMAGE_FORMAT_MISMATCH";
			case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
			case -11: return "CL_BUILD_PROGRAM_FAILURE";
			case -12: return "CL_MAP_FAILURE";
			case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
			case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
			case -15: return "CL_COMPILE_PROGRAM_FAILURE";
			case -16: return "CL_LINKER_NOT_AVAILABLE";
			case -17: return "CL_LINK_PROGRAM_FAILURE";
			case -18: return "CL_DEVICE_PARTITION_FAILED";
			case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
			
			// compile-time errors
			case -30: return "CL_INVALID_VALUE";
			case -31: return "CL_INVALID_DEVICE_TYPE";
			case -32: return "CL_INVALID_PLATFORM";
			case -33: return "CL_INVALID_DEVICE";
			case -34: return "CL_INVALID_CONTEXT";
			case -35: return "CL_INVALID_QUEUE_PROPERTIES";
			case -36: return "CL_INVALID_COMMAND_QUEUE";
			case -37: return "CL_INVALID_HOST_PTR";
			case -38: return "CL_INVALID_MEM_OBJECT";
			case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
			case -40: return "CL_INVALID_IMAGE_SIZE";
			case -41: return "CL_INVALID_SAMPLER";
			case -42: return "CL_INVALID_BINARY";
			case -43: return "CL_INVALID_BUILD_OPTIONS";
			case -44: return "CL_INVALID_PROGRAM";
			case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
			case -46: return "CL_INVALID_KERNEL_NAME";
			case -47: return "CL_INVALID_KERNEL_DEFINITION";
			case -48: return "CL_INVALID_KERNEL";
			case -49: return "CL_INVALID_ARG_INDEX";
			case -50: return "CL_INVALID_ARG_VALUE";
			case -51: return "CL_INVALID_ARG_SIZE";
			case -52: return "CL_INVALID_KERNEL_ARGS";
			case -53: return "CL_INVALID_WORK_DIMENSION";
			case -54: return "CL_INVALID_WORK_GROUP_SIZE";
			case -55: return "CL_INVALID_WORK_ITEM_SIZE";
			case -56: return "CL_INVALID_GLOBAL_OFFSET";
			case -57: return "CL_INVALID_EVENT_WAIT_LIST";
			case -58: return "CL_INVALID_EVENT";
			case -59: return "CL_INVALID_OPERATION";
			case -60: return "CL_INVALID_GL_OBJECT";
			case -61: return "CL_INVALID_BUFFER_SIZE";
			case -62: return "CL_INVALID_MIP_LEVEL";
			case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
			case -64: return "CL_INVALID_PROPERTY";
			case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
			case -66: return "CL_INVALID_COMPILER_OPTIONS";
			case -67: return "CL_INVALID_LINKER_OPTIONS";
			case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";
			
			// extension errors
			case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
			case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
			case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
			case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
			case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
			case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
			default: return "Unknown OpenCL error";
		}
		return "Unknown OpenCL error or CPU error";
	}
	
	void PrintError( cl_int error )
	{
		if( error < 0 )
			printf( "\n OpenCL error: %s ", GetErrorString( error ) );
	}
};

#endif

