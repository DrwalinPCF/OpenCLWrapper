# 1 "VecAdd.cl"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "VecAdd.cl"

# 1 "Add.cl" 1

void Add( __global int2 *A, __global int2*B, __global int2*C )
{
 *C = *A + *B;
}
# 3 "VecAdd.cl" 2

__kernel void VectorAdd( __global int2 *A, __global int2 *B, __global int2 *C )
{
 int id = get_global_id(0);

 Add( &(A[id]), &(B[id]), &(C[id]) );
}
