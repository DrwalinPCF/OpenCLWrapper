
struct Object
{
	int a;
	int b;
	int c;
};

struct Object2
{
	float a;
	float b;
	float c;
};

__kernel void VectorAdd( __global struct Object *A )
{
    int i = get_global_id(0);
    A[i].a = 1*i-2;
    A[i].b = 2*i-3;
    A[i].c = 3*i-4;
    
    struct Object2 * ptr = (struct Object2*)&(A[i]);
    
    ptr->a = A[i].a;
    ptr->b = A[i].b;
    ptr->c = A[i].c;
    
    /*
    A[i]+1.a += 3;
    A[i+1].b += A[i+1].a;
    A[i+1].c += A[i].b;
    */
}

