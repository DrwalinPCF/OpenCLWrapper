
#include "../../src/OpenCLWrapper.h"

#include <ctime>
#include <cmath>

#include <iostream>


float mult( int id, float * a, float * b, int elements )
{
 	int end = id+elements;
 	float sum = 0.0f;
 	int i;
 	for( i = id; i < end; ++i )
 	{
 		sum += a[i] * b[i];
 	}
 	return sum;
}

int main()
{
	unsigned i, beg;
	
	unsigned size = 1024*1024;
	std::cin >> size;
	
	float * a = CL::Allocate<float>( size, CL_MEM_READ_WRITE );//| CL_MEM_USE_HOST_PTR );
	float * b = CL::Allocate<float>( size, CL_MEM_READ_WRITE );//| CL_MEM_USE_HOST_PTR );
	float * c = CL::Allocate<float>( size, CL_MEM_READ_WRITE );//| CL_MEM_USE_HOST_PTR );
	float * d = new float[size];
	
	for( i = 0; i < size; ++i )
	{
		a[i] = i+1;
		b[i] = size-i;
		a[i] /= 100.0f;
		b[i] /= 100.0f;
	}
	
	int multiplier = 1024;
	std::cin >> multiplier;
	
	beg = clock();
	for( int j = 0; j < 10; ++j )
		for( i = 0; i < size-multiplier; ++i )
			d[i] = mult( i, a, b, multiplier );
	printf( "\n For using CPU: %u ", unsigned(clock()-beg) );
	
	cl_kernel * function = CL::GetFunction( "VecAdd.cl", "VectorAdd" );
	
	
	CL::CopyToVRAM( a );
	CL::CopyToVRAM( b );
	
	unsigned sumTime = 0;
	CL::Finish();
	for( i = 0; i < 20; ++i )
	{
		beg = clock();
		for( int j = 0; j < 10; ++j )
		{
			CL::For( size-multiplier, function, a, b, c, multiplier );
			CL::Flush();
		}
		CL::Finish();
		sumTime += unsigned(clock()-beg);
		printf( "\n For using GPU: %u ", unsigned(clock()-beg) );
	}
	printf( "\n For using GPU mean time: %u ", sumTime / i );
	
	CL::CopyFromVRAM( c );
	
	
	for( i = 0; i < size-multiplier; ++i )
	{
		if( abs(c[i]-d[i]) > 0.01 )
		{
			printf( "\n c[%u] != d[%u]  :  %f != %f ", i, i, c[i], d[i] );
		}
	}
	
	
	CL::DestroyFunction( function );
	
	CL::Free( a );
	CL::Free( b );
	CL::Free( c );
	
	delete[] d;
	
	return 0;
}



