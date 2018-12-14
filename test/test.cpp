
#include "../src/OpenCLWrapper.h"

#include <iostream>

int main()
{
	CL::Init();
	
	unsigned size = 256;
	
	float * a = CL::Allocate<float>( size, CL_MEM_READ_ONLY );
	float * b = CL::Allocate<float>( size, CL_MEM_READ_ONLY );
	float * c = CL::Allocate<float>( size, CL_MEM_WRITE_ONLY );
	
	for( unsigned i = 0; i < size; ++i )
	{
		a[i] = i+1;
		b[i] = i+1;
	}
	
	cl_kernel * function = CL::GetFunction( "VecAdd.cl", "VectorAdd" );
	
	CL::CopyToVRAM( a );
	CL::CopyToVRAM( b );
	
	CL::For( size, function, a, b, c, float(15) );
	
	CL::CopyFromVRAM( c );
	
	for( unsigned i = 0; i < size; ++i )
		printf( "\n c[%u] = %f ", i, c[i] );
	
	CL::DestroyFunction( function );
	
	CL::Free( a );
	CL::Free( b );
	CL::Free( c );
	
	CL::DeInit();
	
	return 0;
}



