
#include "../../src/OpenCLWrapper.h"

#include <ctime>
#include <cmath>

#include <iostream>

int main()
{
	int i;
	int size = 13;
	
	cl_int2 * a = CL::Allocate<cl_int2>( size, CL_MEM_READ_WRITE );//| CL_MEM_USE_HOST_PTR );
	cl_int2 * b = CL::Allocate<cl_int2>( size, CL_MEM_READ_WRITE );//| CL_MEM_USE_HOST_PTR );
	cl_int2 * c = CL::Allocate<cl_int2>( size, CL_MEM_READ_WRITE );//| CL_MEM_USE_HOST_PTR );
	
	for( i = 0; i < size; ++i )
	{
		a[i].x = i+1;
		a[i].y = i-2;
		b[i].x = size-i;
		b[i].y = size-i+3;
		c[i].x = 0;
		c[i].y = 0;
	}
	
	CL::Function<cl_int2*,cl_int2*,cl_int2*> VectorAdd( "VecAdd.cl", "VectorAdd" );
	
	CL::CopyToVRAM( a );
	CL::CopyToVRAM( b );
	VectorAdd[size]( a, b, c );
	CL::CopyFromVRAM( c );
	
	for( i = 0; i < size; ++i )
	{
		printf( "\n [%i,%i] + [%i,%i] = [%i,%i]", a[i].x, a[i].y, b[i].x, b[i].y, c[i].x, c[i].y );
	}
	
	CL::DestroyFunction( VectorAdd );
	
	CL::Free( a );
	CL::Free( b );
	CL::Free( c );
	
	return 0;
}



