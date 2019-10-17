
#include "Add.cl"

__kernel void VectorAdd( __global int2 *A, __global int2 *B, __global int2 *C )
{
	int id = get_global_id(0);
	//C[id] = (A[id] + B[id]);
	Add( &(A[id]), &(B[id]), &(C[id]) );
}

