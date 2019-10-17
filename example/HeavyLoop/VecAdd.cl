
__kernel void VectorAdd( __global const float *A, __global const float *B, __global float *C, const int elements )
{
    int id = get_global_id(0);
 	
 	int end = id+elements;
 	float sum = 0.0;
 	int i;
 	for( i = id; i < end; ++i )
 	{
 		sum += A[i] * B[i];
 	}
 	C[id] = sum;
}

