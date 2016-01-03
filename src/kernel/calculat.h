#ifdef __cplusplus
extern "C" {
#endif
typedef char ( *funcptr )( double* a, const double* b );
/* applies operation on a and b and stores result in a */
/* e.g.: a = a/b; (operation = division) */
/* or: a = sin(b) */

struct function_decl {
    const char* name;  /* string, name of function e.g. "sin" */
    funcptr func;      /* pointer to function that performs operation */
};

struct progtype {
    /* None of these members has to be referenced by the user, they are filled automatically */
    struct function_decl avail_functions[30];   /* pointers to defined functions (e.g. "sin") */
    int vardef, availfncount;            /* how many defined variables, and functions */
    char varnames[30][12];               /* names of defined variables (max 30, max 12 char.) */
    double varvalues[30];                /* here the values of the variables are stored */
    funcptr func[80];                   /* 100 instructions (function calls) which a program consists of */
    double* a[80], *b[80], z[80];     /* the data passed to the above functions */
};

int Init( struct progtype* prog );
/* Resets variables in program "prog" */

int SetVariable( const char* name, unsigned char* handle, double value, struct progtype* prog );
/* Sets the variable referenced by "name" or "handle" to the value "value" */
/* in the program "prog". */
/* To define a new variable, set handle to 255 */
/* in further calls use the returned handle, not the name (for speed reasons) */

int DeclareFunction( const char* name, funcptr f, struct progtype* prog );
/* Declares an user defined function */
/* name: Name of function, e.g. "sin" */
/* f: pointer to a function that is connected to name */
/* note: it has a special calling syntax: f(double *a, double *b) */
/*       the function has to calculate something with a and b and to store the result in a */
/* prog: program for which the function is declared */

int Translate( char* wrong, char* aa, struct progtype* prog );
/* function to translate a formula given as a string (aa) to a program (prog)  */
/* for use with  "calculate". */
/* Must be called before function "calculate"! */
/* returns 0, if successful, else an error message (in wrong) */

double calculate( char* notdef, struct progtype* prog );
/* function to calculate a value by a formula */
/* The formula has to be translated to a program "prog" first (see "Translate") */
/* values for variables in the formula-string may be set by function */
/* "SetVariable" */
/* The calculated value will be returned. */

int IsNumber( const char* s );
/* returns 1 if s is a valid number */
#ifdef __cplusplus
}
#endif
