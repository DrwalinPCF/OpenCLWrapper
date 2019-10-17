
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
	
	class Variable
	{
	public:
		
		cl_uint numberOfPlatforms;
		cl_platform_id platformID;
		cl_uint numberOfDevices;
		cl_device_id deviceID;
		cl_context context;
		cl_command_queue commandQueue;
		std::map < void*, MemoryObjectData* > memoryObjects;
		std::map < std::string, ProgramData* > programs;
		
		Variable();
		~Variable();
	};
	
	extern Variable instance;
	
	
	
	unsigned Flush();
	unsigned Finish();
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
	
	cl_kernel * GetFunction( const char* fileName, const char* function, unsigned * error = NULL );
	unsigned DestroyFunction( cl_kernel* kernel );
	
	
	unsigned For_( unsigned long long iterations, cl_kernel* kernel );
	
	template < typename T >
	unsigned SetArgument_( cl_kernel* kernel, unsigned argIdOffset, const T& arg );
	
	template < typename T, typename... Args >
	unsigned SetArgument_( cl_kernel* kernel, unsigned argIdOffset, const T& arg, Args... args );
	
	template < typename... Args >
	unsigned For( unsigned long long iterations, cl_kernel* kernel, Args... args );
	
	
	template < typename... Args >
	class Function
	{
	public:
		cl_kernel * kernel;
		unsigned long long iterations;
		
		operator bool() const
		{
			return kernel!=NULL;
		}
		
		Function & operator[]( const unsigned long long iterations )
		{
			this->iterations = iterations;
			return *this;
		}
		
		unsigned operator()( Args... args )
		{
			return For( this->iterations, this->kernel, args... );
		}
		
		unsigned Destroy()
		{
			if( this->kernel )
			{
				unsigned ret = DestroyFunction( this->kernel );
				this->kernel = NULL;
				return ret;
			}
			return 0;
		}
		
		Function( const char* fileName, const char* function, unsigned * error = NULL )
		{
			this->kernel = GetFunction(fileName,function,error);
			this->iterations = 0;
		}
		
		~Function()
		{
			this->Destroy();
		}
	};
	template < typename... Args >
	unsigned DestroyFunction( Function<Args...> & function )
	{
		return function.Destroy();
	}
	
	
	
	
	template < typename T >
	T * Allocate( unsigned long long elements, cl_mem_flags flags )
	{
		cl_int error = 0;
		if( std::is_pointer<T>() )
		{
			printf( "\n Allocationg memory with CL::Allocate can not be a pointer type" );
			return NULL;
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
					mem->mem = clCreateBuffer( instance.context, flags, elements*sizeof(T), ( (flags&CL_MEM_USE_HOST_PTR) ? ret : NULL ), &error );
					if( error != 0 )
					{
						PrintError( error );
						free( mem );
						free( ret );
						return NULL;
					}
					instance.memoryObjects[ret] = mem;
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
		return NULL;
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

