
#ifndef OPENCL_WRAPPER_H
#define OPENCL_WRAPPER_H

#include <CL/cl.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <vector>
#include <map>
#include <string>
#include <type_traits>		// for std::is_pointer

namespace CL
{
	struct ProgramData
	{
		cl_program program;
		std::map < std::string, cl_kernel* > kernels;
	};
	
	struct MemoryObjectData
	{
		void* ptr;
		unsigned long long elements;
		unsigned long long bytesPerElement;
		cl_mem mem;
	};
	
	// global instances:
		extern cl_uint numberOfPlatforms;
		extern cl_platform_id platformID;
		extern cl_uint numberOfDevices;
		extern cl_device_id deviceID;
		extern cl_context context;
		extern cl_command_queue commandQueue;
		extern std::map < void*, MemoryObjectData* > memoryObjects;
		extern std::map < std::string, ProgramData* > programs;
	
	
	
	cl_mem * GetMemoryObject( void* ptr );
	
	
	const char * GetErrorString( cl_int error );
	void PrintError( cl_int error );
	
	unsigned Init();
	void DeInit();
	
	
	template < typename T >
	T * Allocate( unsigned long long elements, cl_mem_flags flags = CL_MEM_READ_WRITE );
	
	
	unsigned Free( void* ptr );
	unsigned CopyToVRAM( void* ptr );
	unsigned CopyFromVRAM( void* ptr );
	
	
	cl_kernel* GetFunction( const char* fileName, const char* function, unsigned * error = nullptr );
	unsigned DestroyFunction( cl_kernel* kernel );
	
	
	unsigned For_( unsigned long long iterations, cl_kernel* kernel );
	
	template < typename T >
	unsigned SetArgument_( cl_kernel* kernel, unsigned argIdOffset, const T& arg );
	
	template < typename T, typename... Args >
	unsigned SetArgument_( cl_kernel* kernel, unsigned argIdOffset, const T& arg, Args... args );
	
	template < typename... Args >
	unsigned For( unsigned long long iterations, cl_kernel* kernel, Args... args );
	
	
	
	
	
	template < typename T >
	T * Allocate( unsigned long long elements, cl_mem_flags flags )
	{
		cl_int error = 0;
		if( std::is_pointer<T>() )
		{
			printf( "\n Allocationg memory with CL::Allocate can not be a pointer type" );
			return nullptr;
		}
		else
		{
			T * ret = (T*)malloc( elements * sizeof(T) );
			if( ret )
			{
				MemoryObjectData* mem = (MemoryObjectData*)malloc( sizeof(MemoryObjectData) );
				if( mem )
				{
					mem->elements = elements;
					mem->bytesPerElement = sizeof(T);
					mem->ptr = ret;
					mem->mem = clCreateBuffer( context, flags, elements*sizeof(T), ( (flags&CL_MEM_USE_HOST_PTR) ? ret : nullptr ), &error );
					if( error != 0 )
					{
						PrintError( error );
						free( mem );
						free( ret );
						return nullptr;
					}
					memoryObjects[ret] = mem;
					return ret;
				}
				else
				{
					printf( "\n Can not allocate memory object" );
				}
			}
			else
			{
				printf( "\n Can not allocate memory buffer with size: %llu ", (unsigned long long)(elements * sizeof(T)) );
			}
		}
		return nullptr;
	}
	
	
	
	template < typename T >
	unsigned SetArgument_( cl_kernel* kernel, unsigned argIdOffset, const T& arg )
	{
		if( kernel )
		{
			cl_int ret = 0;
			if( std::is_pointer<T>() && sizeof(T) == sizeof(void*) )
			{
				void * memObj = 0;
				memcpy( &memObj, &arg, sizeof(void*) );
				ret = clSetKernelArg( *kernel, argIdOffset, sizeof(cl_mem), (void *)GetMemoryObject( (void*)memObj ) );
			}
			else
			{
				ret = clSetKernelArg( *kernel, argIdOffset, sizeof(T), (void *)&arg );
			}
			PrintError( ret );
			return ret;
		}
		printf( "\n No kernel given to short SetArgument_" );
		return 900;
	}
	
	template < typename T, typename... Args >
	unsigned SetArgument_( cl_kernel* kernel, unsigned argIdOffset, const T& arg, Args... args )
	{
		if( kernel )
		{
			cl_int ret = 0;
			ret = SetArgument_( kernel, argIdOffset, arg );
			PrintError( ret );
			ret = SetArgument_( kernel, argIdOffset+1, args... );
			PrintError( ret );
			return ret;
		}
		printf( "\n No kernel given to SetArgument_" );
		return 700;
	}
	
	template < typename... Args >
	unsigned For( unsigned long long iterations, cl_kernel* kernel, Args... args )
	{
		if( kernel )
		{
			cl_int ret = 0;
			ret = SetArgument_( kernel, 0, args... );
			if( ret == 0 )
			{
				ret = For_( iterations, kernel );
			}
			return ret;
		}
		printf( "\n No given kernel to For" );
		return 800;
	}
	
	
	
};

#endif

