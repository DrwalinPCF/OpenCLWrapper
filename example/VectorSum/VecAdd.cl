
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

__kernel void VectorAdd( __global const int *A, __global const int *B, __global int *C, const int multiplier )
{
    // Get the index of the current element to be processed
    int i = get_global_id(0);
 
    // Do the operation
    C[i] = power( A[i] + B[i], multiplier );
}

