
#include "../src/OpenCLWrapper.h"

#include <ctime>
#include <cmath>

int main()
{
	unsigned beg;
	CL::Init();
	
	unsigned size = 1024*1024*80 + 1;
	
	float * a = CL::Allocate<float>( size, CL_MEM_READ_ONLY );
	float * b = CL::Allocate<float>( size, CL_MEM_READ_ONLY );
	float * c = CL::Allocate<float>( size, CL_MEM_WRITE_ONLY );
	float * d = new float[size];
	
	for( unsigned i = 0; i < size; ++i )
	{
		a[i] = i+1;
		b[i] = i+1;
	}
	
	float multiplier = 15.1;
	
	beg = clock();
	for( unsigned i = 0; i < size; ++i )
		d[i] = pow( a[i]+b[i], multiplier );
	printf( "\n For using CPU: %u ", unsigned(clock()-beg) );
	
	cl_kernel * function = CL::GetFunction( "VecAdd.cl", "VectorAdd" );
	
	
	CL::CopyToVRAM( a );
	CL::CopyToVRAM( b );
	
	clFlush( CL::commandQueue );
	clFinish( CL::commandQueue );
	beg = clock();
	CL::For( size, function, a, b, c, multiplier );
	clFlush( CL::commandQueue );
	clFinish( CL::commandQueue );
	printf( "\n For using GPU: %u ", unsigned(clock()-beg) );
	
	CL::CopyFromVRAM( c );
	
	for( unsigned i = 0; i < size; ++i )
	{
		if( c[i] != d[i] )
			printf( "\n c[%u] != d[%u]  :  %f != %f ", i, i, c[i], d[i] );
	}
	
	for( unsigned i = 0; i < 16; ++i )
		printf( "\n c[%u] = %f ", i, c[i] );
	
	CL::DestroyFunction( function );
	
	CL::Free( a );
	CL::Free( b );
	CL::Free( c );
	
	CL::DeInit();
	
	delete[] d;
	
	return 0;
}



