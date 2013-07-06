%module myret

%{
struct Foo;
struct Foo *createFoo1(void) {
	struct Foo *retVal = (struct Foo *)malloc(16);
	printf("malloc() returned 0x%010X\n", retVal);
	return retVal;
};
void createFoo2(struct Foo **out) {
	struct Foo *retVal = (struct Foo *)malloc(16);
	printf("malloc() returned 0x%010X\n", retVal);
	*out = retVal;
};
void destroyFoo(struct Foo *p) {
	printf("About to free(0x%010X)\n", p);
	free((void*)p);
};
int possibleError(int x, const char **error) {
	if ( x == 10 ) {
		const char *msg = "You cannot call this with 10";
		char *const str = (char*)malloc(strlen(msg));
		if ( str ) {
			strcpy(str, msg);
		}
		*error = str;
		return 1;
	}
	printf("possibleError(%d)\n");
	return 0;
}
%}

%typemap(in, numinputs=0) struct Foo **OUTPUT {
	$1 = &temp;
}
%typemap(out) void createFoo2(struct Foo *temp = NULL) {
	ST(argvi) = SWIG_NewPointerObj(SWIG_as_voidptr(temp), SWIGTYPE_p_Foo, 0 | 0);
	argvi++;
}
struct Foo *createFoo1(void);
void createFoo2(struct Foo **OUTPUT);
void destroyFoo(struct Foo *);

%typemap(in, numinputs=0) const char **OUTPUT {
	$1 = &error;
}
%typemap(out) int possibleError (char *error = NULL) {
	if ( result ) {
		SWIG_Error(SWIG_ArgError(result), error);
		free(error);
		SWIG_fail;
	}
}
int possibleError(int, const char **OUTPUT);
