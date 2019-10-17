
#include "../../src/OpenCLWrapper.h"

#include <ctime>
#include <cmath>

#include <iostream>

struct Object
{
	int a;
	int b;
	int c;
};

int main()
{
	unsigned i, beg;
	
	unsigned size = 1024*1024*20;
	
	int * a = (int*)CL::Allocate<Object>( size, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR );
	
	for( i = 0; i < size*3; ++i )
		a[i] = i;
	
	cl_kernel * function = CL::GetFunction( "VecAdd.cl", "VectorAdd" );
	
	CL::CopyToVRAM( a );
	CL::For( size-1, function, a );
	CL::Finish();
	
	for( i = 0; i < size && i < 500; ++i )
		printf( "\n a[%i] = %i", i/3, a[i] );
	
	CL::DestroyFunction( function );
	CL::Free( a );
	
	return 0;
}



