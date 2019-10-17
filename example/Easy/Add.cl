
void Add( __global int2 *A, __global int2*B, __global int2*C )
{
	*C = *A + *B;
}