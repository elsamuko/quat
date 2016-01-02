#define forall(i,n) for ((i)=0; (i)<(n); (i)++)
#define scalarprod(a,b) (a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2]

typedef double vec3[3];                    /* vector in 3d */
struct basestruct                            /* origin and 3 base vectors */
{
   vector O, x, y, z;
} ;



