


typedef struct {
    /** Number of elements. */
    long n;
    /** Array of integer values. */
    int *val;
} iftIntArray;

%extend iftIntArray {

	PyObject* AsNumPy(void)
	{
	    iftIntArray *a = ($self);
	    npy_intp dims[1] = {a->n};
	    PyArrayObject* result = PyArray_SimpleNew(1, dims, NPY_INT32);
	
	    int *data = array_data(result);
	    memcpy(data, a->val, a->n * sizeof (*data));
	
	    return PyArray_Return(result);
	}
	
	void FromNumPy(PyObject* input) {
	
	    PyArrayObject *ary = obj_to_array_no_conversion(input, NPY_INT32);
	
	    if (ary == NULL) return;
	
	    if (!require_contiguous(ary))
	        SWIG_Error(0, "Input numpy array is not contiguous");
	
	    iftIntArray *a = ($self);
	    float *ptr = array_data(ary);
	
	    int n_dims = array_numdims(ary);
	    if (n_dims != 1)
	        SWIG_Error(0, "Input is not a 1D array");
	
	    int ary_size = array_size(ary, 0);
	
	    if (ary_size != a->n)
	    {
	        a->n = ary_size;
	        a->val = iftRealloc(a->val, a->n * sizeof *a->val);
	            if (a->val == NULL)
	            iftError(MSG_MEMORY_ALLOC_ERROR, "FromNumpy");
	    }
	    memcpy(a->val, ptr, a->n * sizeof *a->val);
	}
	
	

	~iftIntArray() {
		iftIntArray* ptr = ($self);
		iftDestroyIntArray(&ptr);
	}
};

%newobject iftCreateIntArray;
%feature("autodoc", "2");
iftIntArray *iftCreateIntArray(long n);

%inline %{

iftIntArray *CreateIntArrayFromNumPy(PyObject *input) {
    PyArrayObject *arr = obj_to_array_no_conversion(input, NPY_INT32);
    if (!require_contiguous(arr))
        SWIG_Error(0, "Input numpy array is not contiguous");
    
    int n_dims = array_numdims(input);
    if (n_dims != 1) {
        SWIG_Error(0, "Input Integer Array must be 1-dimensional");
        return NULL;
    }
    
    npy_intp *shape = array_dimensions(input);
    
    iftIntArray *iarr = iftCreateIntArray(shape[0]);
    
    int *ptr = array_data(input);
    memcpy(iarr->val, ptr, sizeof (*ptr) * iarr->n);
    
    return iarr;
}

%}

%newobject CreateIntArrayFromNumPy;
%feature("autodoc", "2");
iftIntArray *CreateIntArrayFromNumPy(PyObject *input) ;

