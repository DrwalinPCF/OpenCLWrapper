
__kernel void VectorAdd( __global const float *A, __global const float *B, __global float *C, const float multiplier )
{
    // Get the index of the current element to be processed
    int i = get_global_id(0);
 
    // Do the operation
    C[i] = pow( A[i] + B[i], multiplier );
    
}

