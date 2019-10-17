
#include "../../src/OpenCLWrapper.h"

#include <ctime>
#include <cmath>

#include <iostream>


int power( int a, int b )
{
	int i;
	int ret = 1;
	for( i = 0; i < b; ++i )
	{
		ret *= a;
	}
	return ret;
	/*
	for( i = 1; i <= b; i = i << 1 )
	{
		if( i & b )
		{
			ret *= a;
		}
		a *= a;
	}
	return ret;
	*/
}

int main()
{
	unsigned i, beg;
	
	unsigned size = 1024*1024*80;
	
	int * a = CL::Allocate<int>( size, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR );//CL_MEM_READ_ONLY );
	int * b = CL::Allocate<int>( size, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR );//CL_MEM_READ_ONLY );
	int * c = CL::Allocate<int>( size, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR );//CL_MEM_WRITE_ONLY );
	int * d = new int[size];
	
	for( i = 0; i < size; ++i )
	{
		a[i] = i+1;
		b[i] = i+1;
	}
	
	int multiplier = 100;
	std::cin >> multiplier;
	
	beg = clock();
	for( i = 0; i < size; ++i )
		d[i] = power( a[i] + b[i], multiplier );
	printf( "\n For using CPU: %u ", unsigned(clock()-beg) );
	
	cl_kernel * function = CL::GetFunction( "VecAdd.cl", "VectorAdd" );
	
	
	CL::CopyToVRAM( a );
	CL::CopyToVRAM( b );
	
	unsigned sumTime = 0;
	CL::Finish();
	for( i = 0; i < 20; ++i )
	{
		beg = clock();
		CL::For( size, function, a, b, c, multiplier );
		CL::Finish();
		sumTime += unsigned(clock()-beg);
		printf( "\n For using GPU: %u ", unsigned(clock()-beg) );
		/*
		CL::CopyFromVRAM( c );
		memcpy( a, c, sizeof(int)*size );
		CL::CopyToVRAM( a );
		*/
	}
	printf( "\n For using GPU mean time: %u ", sumTime / i );
	
	CL::CopyFromVRAM( c );
	
	
	for( i = 0; i < size; ++i )
	{
		if( abs(c[i]-d[i]) > 0.001 )
		{
			printf( "\n c[%u] != d[%u]  :  %i != %i ", i, i, c[i], d[i] );
		}
	}
	
	
	CL::DestroyFunction( function );
	
	CL::Free( a );
	CL::Free( b );
	CL::Free( c );
	
	delete[] d;
	
	return 0;
}



