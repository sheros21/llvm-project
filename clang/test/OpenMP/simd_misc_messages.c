// RUN: %clang_cc1 -fsyntax-only -fopenmp -fopenmp-version=45 -verify=expected,omp45 %s -Wuninitialized
// RUN: %clang_cc1 -fsyntax-only -fopenmp -fopenmp-version=50 -verify=expected,omp50 %s -Wuninitialized
// RUN: %clang_cc1 -fsyntax-only -fopenmp -fopenmp-version=51 -verify=expected,omp51 %s -Wuninitialized

// RUN: %clang_cc1 -fsyntax-only -fopenmp-simd -fopenmp-version=45 -verify=expected,omp45 %s -Wuninitialized
// RUN: %clang_cc1 -fsyntax-only -fopenmp-simd -fopenmp-version=50 -verify=expected,omp50 %s -Wuninitialized
// RUN: %clang_cc1 -fsyntax-only -fopenmp-simd -fopenmp-version=51 -verify=expected,omp51 %s -Wuninitialized

void xxx(int argc) {
  int x; // expected-note {{initialize the variable 'x' to silence this warning}}
#pragma omp simd
  for (int i = 0; i < 10; ++i)
    argc = x; // expected-warning {{variable 'x' is uninitialized when used here}}
}

// expected-error@+1 {{unexpected OpenMP directive '#pragma omp simd'}}
#pragma omp simd

// expected-error@+1 {{unexpected OpenMP directive '#pragma omp simd'}}
#pragma omp simd foo

// expected-error@+1 {{unexpected OpenMP directive '#pragma omp simd'}}
#pragma omp simd safelen(4)

void test_no_clause(void) {
  int i;
#pragma omp simd
  for (i = 0; i < 16; ++i)
    ;

// expected-error@+2 {{statement after '#pragma omp simd' must be a for loop}}
#pragma omp simd
  ++i;
}

void test_branch_protected_scope(void) {
  int i = 0;
L1:
  ++i;

  int x[24];

#pragma omp simd
  for (i = 0; i < 16; ++i) {
    if (i == 5)
      goto L1; // expected-error {{use of undeclared label 'L1'}}
    else if (i == 6)
      return; // expected-error {{cannot return from OpenMP region}}
    else if (i == 7)
      goto L2;
    else if (i == 8) {
    L2:
      x[i]++;
    }
  }

  if (x[0] == 0)
    goto L2; // expected-error {{use of undeclared label 'L2'}}
  else if (x[1] == 1)
    goto L1;
}

void test_invalid_clause(void) {
  int i;
// expected-warning@+1 {{extra tokens at the end of '#pragma omp simd' are ignored}}
#pragma omp simd foo bar
  for (i = 0; i < 16; ++i)
    ;
}

void test_non_identifiers(void) {
  int i, x;

// expected-warning@+1 {{extra tokens at the end of '#pragma omp simd' are ignored}}
#pragma omp simd;
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{unexpected OpenMP clause 'firstprivate' in directive '#pragma omp simd'}}
// expected-warning@+1 {{extra tokens at the end of '#pragma omp simd' are ignored}}
#pragma omp simd firstprivate(x);
  for (i = 0; i < 16; ++i)
    ;

// expected-warning@+1 {{extra tokens at the end of '#pragma omp simd' are ignored}}
#pragma omp simd private(x);
  for (i = 0; i < 16; ++i)
    ;

// expected-warning@+1 {{extra tokens at the end of '#pragma omp simd' are ignored}}
#pragma omp simd, private(x);
  for (i = 0; i < 16; ++i)
    ;
}

extern int foo(void);
void test_safelen(void) {
  int i;
// expected-error@+1 {{expected '('}}
#pragma omp simd safelen
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd safelen()
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}  expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(, )
  for (i = 0; i < 16; ++i)
    ;
// expected-warning@+2 {{extra tokens at the end of '#pragma omp simd' are ignored}}
// expected-error@+1 {{expected '('}}
#pragma omp simd safelen 4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(4
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(4,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(4, )
  for (i = 0; i < 16; ++i)
    ;
// xxpected-error@+1 {{expected expression}}
#pragma omp simd safelen(4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(4 4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(4, , 4)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd safelen(4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd safelen(4, 8)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{integer constant expression}}
#pragma omp simd safelen(2.5)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{integer constant expression}}
#pragma omp simd safelen(foo())
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'safelen' clause must be a strictly positive integer value}}
#pragma omp simd safelen(-5)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'safelen' clause must be a strictly positive integer value}}
#pragma omp simd safelen(0)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'safelen' clause must be a strictly positive integer value}}
#pragma omp simd safelen(5 - 5)
  for (i = 0; i < 16; ++i)
    ;
}

void test_simdlen(void) {
  int i;
// expected-error@+1 {{expected '('}}
#pragma omp simd simdlen
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd simdlen()
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}  expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(, )
  for (i = 0; i < 16; ++i)
    ;
// expected-warning@+2 {{extra tokens at the end of '#pragma omp simd' are ignored}}
// expected-error@+1 {{expected '('}}
#pragma omp simd simdlen 4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(4
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(4,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(4, )
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd simdlen(4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(4 4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(4, , 4)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd simdlen(4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}}
#pragma omp simd simdlen(4, 8)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{integer constant expression}}
#pragma omp simd simdlen(2.5)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{integer constant expression}}
#pragma omp simd simdlen(foo())
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'simdlen' clause must be a strictly positive integer value}}
#pragma omp simd simdlen(-5)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'simdlen' clause must be a strictly positive integer value}}
#pragma omp simd simdlen(0)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'simdlen' clause must be a strictly positive integer value}}
#pragma omp simd simdlen(5 - 5)
  for (i = 0; i < 16; ++i)
    ;
}

void test_safelen_simdlen(void) {
  int i;
// expected-error@+1 {{the value of 'simdlen' parameter must be less than or equal to the value of the 'safelen' parameter}}
#pragma omp simd simdlen(6) safelen(5)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{the value of 'simdlen' parameter must be less than or equal to the value of the 'safelen' parameter}}
#pragma omp simd safelen(5) simdlen(6)
  for (i = 0; i < 16; ++i)
    ;
}

void test_collapse(void) {
  int i;
// expected-error@+1 {{expected '('}}
#pragma omp simd collapse
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd collapse(
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd collapse()
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd collapse(,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}  expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd collapse(, )
  for (i = 0; i < 16; ++i)
    ;
// expected-warning@+2 {{extra tokens at the end of '#pragma omp simd' are ignored}}
// expected-error@+1 {{expected '('}}
#pragma omp simd collapse 4)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}} expected-note@+1 {{as specified in 'collapse' clause}}
#pragma omp simd collapse(4
  for (i = 0; i < 16; ++i)
    ; // expected-error {{expected 4 for loops after '#pragma omp simd', but found only 1}}
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}} expected-note@+1 {{as specified in 'collapse' clause}}
#pragma omp simd collapse(4,
  for (i = 0; i < 16; ++i)
    ; // expected-error {{expected 4 for loops after '#pragma omp simd', but found only 1}}
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}} expected-note@+1 {{as specified in 'collapse' clause}}
#pragma omp simd collapse(4, )
  for (i = 0; i < 16; ++i)
    ; // expected-error {{expected 4 for loops after '#pragma omp simd', but found only 1}}
// xxpected-error@+1 {{expected expression}} expected-note@+1 {{as specified in 'collapse' clause}}
#pragma omp simd collapse(4)
  for (i = 0; i < 16; ++i)
    ; // expected-error {{expected 4 for loops after '#pragma omp simd', but found only 1}}
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}} expected-note@+1 {{as specified in 'collapse' clause}}
#pragma omp simd collapse(4 4)
  for (i = 0; i < 16; ++i)
    ; // expected-error {{expected 4 for loops after '#pragma omp simd', but found only 1}}
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}} expected-note@+1 {{as specified in 'collapse' clause}}
#pragma omp simd collapse(4, , 4)
  for (i = 0; i < 16; ++i)
    ; // expected-error {{expected 4 for loops after '#pragma omp simd', but found only 1}}
#pragma omp simd collapse(4)
  for (int i1 = 0; i1 < 16; ++i1)
    for (int i2 = 0; i2 < 16; ++i2)
      for (int i3 = 0; i3 < 16; ++i3)
        for (int i4 = 0; i4 < 16; ++i4)
          foo();
// expected-error@+2 {{expected ')'}}
// expected-note@+1 {{to match this '('}} expected-note@+1 {{as specified in 'collapse' clause}}
#pragma omp simd collapse(4, 8)
  for (i = 0; i < 16; ++i)
    ; // expected-error {{expected 4 for loops after '#pragma omp simd', but found only 1}}
// expected-error@+1 {{integer constant expression}}
#pragma omp simd collapse(2.5)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{integer constant expression}}
#pragma omp simd collapse(foo())
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'collapse' clause must be a strictly positive integer value}}
#pragma omp simd collapse(-5)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'collapse' clause must be a strictly positive integer value}}
#pragma omp simd collapse(0)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument to 'collapse' clause must be a strictly positive integer value}}
#pragma omp simd collapse(5 - 5)
  for (i = 0; i < 16; ++i)
    ;
// expected-note@+2 2 {{defined as reduction}}
#pragma omp parallel
#pragma omp simd collapse(2) reduction(+ : i)
  for (i = 0; i < 16; ++i) // expected-error {{loop iteration variable in the associated loop of 'omp simd' directive may not be reduction, predetermined as lastprivate}}
    // expected-note@+1 {{variable with automatic storage duration is predetermined as private; perhaps you forget to enclose 'omp for' directive into a parallel or another task region?}}
    for (int j = 0; j < 16; ++j)
// expected-error@+2 2 {{reduction variable must be shared}}
// expected-error@+1 {{OpenMP constructs may not be nested inside a simd region}}
#pragma omp for reduction(+ : i, j)
      for (int k = 0; k < 16; ++k)
        i += j;
#pragma omp parallel
#pragma omp for
  for (i = 0; i < 16; ++i)
    for (int j = 0; j < 16; ++j)
#pragma omp simd reduction(+ : i, j)
      for (int k = 0; k < 16; ++k)
        i += j;
}

void test_linear(void) {
  int i;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd linear(
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected expression}}
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd linear(,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected expression}}
// expected-error@+1 {{expected expression}}
#pragma omp simd linear(, )
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd linear()
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd linear(int)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected variable name}}
#pragma omp simd linear(0)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{use of undeclared identifier 'x'}}
#pragma omp simd linear(x)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{use of undeclared identifier 'x'}}
// expected-error@+1 {{use of undeclared identifier 'y'}}
#pragma omp simd linear(x, y)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+3 {{use of undeclared identifier 'x'}}
// expected-error@+2 {{use of undeclared identifier 'y'}}
// expected-error@+1 {{use of undeclared identifier 'z'}}
#pragma omp simd linear(x, y, z)
  for (i = 0; i < 16; ++i)
    ;

  int x, y;
// expected-error@+1 {{expected expression}}
#pragma omp simd linear(x :)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd linear(x :, )
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd linear(x : 1)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd linear(x : 2 * 2)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd linear(x : 1, y)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd linear(x : 1, y, z : 1)
  for (i = 0; i < 16; ++i)
    ;

// expected-note@+2 {{defined as linear}}
// expected-error@+1 {{linear variable cannot be linear}}
#pragma omp simd linear(x) linear(x)
  for (i = 0; i < 16; ++i)
    ;

// expected-note@+2 {{defined as private}}
// expected-error@+1 {{private variable cannot be linear}}
#pragma omp simd private(x) linear(x)
  for (i = 0; i < 16; ++i)
    ;

// expected-note@+2 {{defined as linear}}
// expected-error@+1 {{linear variable cannot be private}}
#pragma omp simd linear(x) private(x)
  for (i = 0; i < 16; ++i)
    ;

// expected-warning@+1 {{zero linear step ('x' and other variables in clause should probably be const)}}
#pragma omp simd linear(x, y : 0)
  for (i = 0; i < 16; ++i)
    ;

// expected-note@+2 {{defined as linear}}
// expected-error@+1 {{linear variable cannot be lastprivate}}
#pragma omp simd linear(x) lastprivate(x)
  for (i = 0; i < 16; ++i)
    ;

// expected-note@+2 {{defined as lastprivate}}
// expected-error@+1 {{lastprivate variable cannot be linear}}
#pragma omp simd lastprivate(x) linear(x)
  for (i = 0; i < 16; ++i)
    ;
}

void test_aligned(void) {
  int i;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd aligned(
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected expression}}
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd aligned(,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected expression}}
// expected-error@+1 {{expected expression}}
#pragma omp simd aligned(, )
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd aligned()
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd aligned(int)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected variable name}}
#pragma omp simd aligned(0)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{use of undeclared identifier 'x'}}
#pragma omp simd aligned(x)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{use of undeclared identifier 'x'}}
// expected-error@+1 {{use of undeclared identifier 'y'}}
#pragma omp simd aligned(x, y)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+3 {{use of undeclared identifier 'x'}}
// expected-error@+2 {{use of undeclared identifier 'y'}}
// expected-error@+1 {{use of undeclared identifier 'z'}}
#pragma omp simd aligned(x, y, z)
  for (i = 0; i < 16; ++i)
    ;

  int *x, y, z[25]; // expected-note 4 {{'y' defined here}}
#pragma omp simd aligned(x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd aligned(z)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd aligned(x :)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd aligned(x :, )
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd aligned(x : 1)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd aligned(x : 2 * 2)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd aligned(x : 1, y)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd aligned(x : 1, y, z : 1)
  for (i = 0; i < 16; ++i)
    ;

// expected-error@+1 {{argument of aligned clause should be array or pointer, not 'int'}}
#pragma omp simd aligned(x, y)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{argument of aligned clause should be array or pointer, not 'int'}}
#pragma omp simd aligned(x, y, z)
  for (i = 0; i < 16; ++i)
    ;

// expected-note@+2 {{defined as aligned}}
// expected-error@+1 {{a variable cannot appear in more than one aligned clause}}
#pragma omp simd aligned(x) aligned(z, x)
  for (i = 0; i < 16; ++i)
    ;

// expected-note@+3 {{defined as aligned}}
// expected-error@+2 {{a variable cannot appear in more than one aligned clause}}
// expected-error@+1 2 {{argument of aligned clause should be array or pointer, not 'int'}}
#pragma omp simd aligned(x, y, z) aligned(y, z)
  for (i = 0; i < 16; ++i)
    ;
}

void test_private(void) {
  int i;
// expected-error@+2 {{expected expression}}
// expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd private(
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected ')'}} expected-note@+2 {{to match this '('}}
// expected-error@+1 2 {{expected expression}}
#pragma omp simd private(,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 2 {{expected expression}}
#pragma omp simd private(, )
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd private()
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd private(int)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected variable name}}
#pragma omp simd private(0)
  for (i = 0; i < 16; ++i)
    ;

  int x, y, z;
#pragma omp simd private(x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd private(x, y)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd private(x, y, z)
  for (i = 0; i < 16; ++i) {
    x = y * i + z;
  }
}

void test_firstprivate(void) {
  int i;
// expected-error@+3 {{expected ')'}} expected-note@+3 {{to match this '('}}
// expected-error@+2 {{unexpected OpenMP clause 'firstprivate' in directive '#pragma omp simd'}}
// expected-error@+1 {{expected expression}}
#pragma omp simd firstprivate(
  for (i = 0; i < 16; ++i)
    ;
}

void test_lastprivate(void) {
  int i;
// expected-error@+2 {{expected ')'}} expected-note@+2 {{to match this '('}}
// expected-error@+1 {{expected expression}}
#pragma omp simd lastprivate(
  for (i = 0; i < 16; ++i)
    ;

// expected-error@+2 {{expected ')'}} expected-note@+2 {{to match this '('}}
// expected-error@+1 2 {{expected expression}}
#pragma omp simd lastprivate(,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 2 {{expected expression}}
#pragma omp simd lastprivate(, )
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd lastprivate()
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd lastprivate(int)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected variable name}}
#pragma omp simd lastprivate(0)
  for (i = 0; i < 16; ++i)
    ;

  int x, y, z;
#pragma omp simd lastprivate(x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd lastprivate(x, y)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd lastprivate(x, y, z)
  for (i = 0; i < 16; ++i)
    ;
}

void test_reduction(void) {
  int i, x, y;
// expected-error@+3 {{expected ')'}} expected-note@+3 {{to match this '('}}
// expected-error@+2 {{expected identifier}}
// expected-warning@+1 {{missing ':' after reduction identifier - ignoring}}
#pragma omp simd reduction(
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected identifier}}
// expected-warning@+1 {{missing ':' after reduction identifier - ignoring}}
#pragma omp simd reduction()
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{expected expression}}
// expected-warning@+1 {{missing ':' after reduction identifier - ignoring}}
#pragma omp simd reduction(x)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected identifier}}
#pragma omp simd reduction( : x)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+3 {{expected ')'}} expected-note@+3 {{to match this '('}}
// expected-error@+2 {{expected identifier}}
// expected-warning@+1 {{missing ':' after reduction identifier - ignoring}}
#pragma omp simd reduction(,
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+3 {{expected ')'}} expected-note@+3 {{to match this '('}}
// expected-error@+2 {{expected expression}}
// expected-warning@+1 {{missing ':' after reduction identifier - ignoring}}
#pragma omp simd reduction(+
  for (i = 0; i < 16; ++i)
    ;

// expected-error@+3 {{expected ')'}} expected-note@+3 {{to match this '('}}
//
// expected-error@+1 {{expected expression}}
#pragma omp simd reduction(+:
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd reduction(+ :)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd reduction(+ :, y)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected expression}}
#pragma omp simd reduction(+ : x, + : y)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected identifier}}
#pragma omp simd reduction(% : x)
  for (i = 0; i < 16; ++i)
    ;

#pragma omp simd reduction(+ : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(* : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(- : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(& : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(| : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(^ : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(&& : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(|| : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(max : x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd reduction(min : x)
  for (i = 0; i < 16; ++i)
    ;
  struct X {
    int x;
  };
  struct X X;
// expected-error@+1 {{expected variable name}}
#pragma omp simd reduction(+ : X.x)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+1 {{expected variable name}}
#pragma omp simd reduction(+ : x + x)
  for (i = 0; i < 16; ++i)
    ;
}

void test_loop_messages(void) {
  float a[100], b[100], c[100];
// expected-error@+2 {{variable must be of integer or pointer type}}
#pragma omp simd
  for (float fi = 0; fi < 10.0; fi++) {
    c[(int)fi] = a[(int)fi] + b[(int)fi];
  }
// expected-error@+2 {{variable must be of integer or pointer type}}
#pragma omp simd
  for (double fi = 0; fi < 10.0; fi++) {
    c[(int)fi] = a[(int)fi] + b[(int)fi];
  }
}

void linear_modifiers(int argc) {
  int f;
  #pragma omp simd linear(f)
  for (int k = 0; k < argc; ++k) ++k;
  #pragma omp simd linear(val(f))
  for (int k = 0; k < argc; ++k) ++k;
  #pragma omp simd linear(uval(f)) // expected-error {{expected 'val' modifier}}
  for (int k = 0; k < argc; ++k) ++k;
  #pragma omp simd linear(ref(f)) // expected-error {{expected 'val' modifier}}
  for (int k = 0; k < argc; ++k) ++k;
  #pragma omp simd linear(foo(f)) // expected-error {{expected 'val' modifier}}
  for (int k = 0; k < argc; ++k) ++k;
}

void test_nontemporal(void) {
  int i;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd nontemporal(
  for (i = 0; i < 16; ++i)
    ;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 2 {{expected expression}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd nontemporal(,
  for (i = 0; i < 16; ++i)
    ;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 2 {{expected expression}}
#pragma omp simd nontemporal(, )
  for (i = 0; i < 16; ++i)
    ;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 {{expected expression}}
#pragma omp simd nontemporal()
  for (i = 0; i < 16; ++i)
    ;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 {{expected expression}}
#pragma omp simd nontemporal(int)
  for (i = 0; i < 16; ++i)
    ;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} omp50-error@+1 {{expected variable name}} omp51-error@+1 {{expected variable name}}
#pragma omp simd nontemporal(0)
  for (i = 0; i < 16; ++i)
    ;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 {{use of undeclared identifier 'x'}}
#pragma omp simd nontemporal(x)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+2 {{use of undeclared identifier 'x'}}
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 {{use of undeclared identifier 'y'}}
#pragma omp simd nontemporal(x, y)
  for (i = 0; i < 16; ++i)
    ;
// expected-error@+3 {{use of undeclared identifier 'x'}}
// expected-error@+2 {{use of undeclared identifier 'y'}}
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 {{use of undeclared identifier 'z'}}
#pragma omp simd nontemporal(x, y, z)
  for (i = 0; i < 16; ++i)
    ;

  int x, y;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 {{expected ',' or ')' in 'nontemporal' clause}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}}
#pragma omp simd nontemporal(x :)
  for (i = 0; i < 16; ++i)
    ;
// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-error@+1 {{expected ')'}} expected-note@+1 {{to match this '('}} expected-error@+1 {{expected ',' or ')' in 'nontemporal' clause}}
#pragma omp simd nontemporal(x :, )
  for (i = 0; i < 16; ++i)
    ;

// omp51-note@+3 {{defined as nontemporal}}
// omp50-note@+2 {{defined as nontemporal}}
// omp45-error@+1 2 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} omp50-error@+1 {{a variable cannot appear in more than one nontemporal clause}} omp51-error@+1 {{a variable cannot appear in more than one nontemporal clause}}
#pragma omp simd nontemporal(x) nontemporal(x)
  for (i = 0; i < 16; ++i)
    ;

// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}}
#pragma omp simd private(x) nontemporal(x)
  for (i = 0; i < 16; ++i)
    ;

// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}}
#pragma omp simd nontemporal(x) private(x)
  for (i = 0; i < 16; ++i)
    ;

// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}} expected-note@+1 {{to match this '('}} expected-error@+1 {{expected ',' or ')' in 'nontemporal' clause}} expected-error@+1 {{expected ')'}}
#pragma omp simd nontemporal(x, y : 0)
  for (i = 0; i < 16; ++i)
    ;

// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}}
#pragma omp simd nontemporal(x) lastprivate(x)
  for (i = 0; i < 16; ++i)
    ;

// omp45-error@+1 {{unexpected OpenMP clause 'nontemporal' in directive '#pragma omp simd'}}
#pragma omp simd lastprivate(x) nontemporal(x)
  for (i = 0; i < 16; ++i)
    ;
#pragma omp simd order // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} expected-error {{expected '(' after 'order'}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order( // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} expected-error {{expected ')'}} expected-note {{to match this '('}} omp50-error {{expected 'concurrent' in OpenMP clause 'order'}} omp51-error {{expected 'concurrent' in OpenMP clause 'order'}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order(none // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} expected-error {{expected ')'}} expected-note {{to match this '('}} omp50-error {{expected 'concurrent' in OpenMP clause 'order'}} omp51-error {{expected 'concurrent' in OpenMP clause 'order'}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order(concurrent // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order(concurrent) // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order(unconstrained:) // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} omp50-error {{expected 'concurrent' in OpenMP clause 'order'}} omp51-error {{expected 'concurrent' in OpenMP clause 'order'}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order(reproducible:concurrent // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} expected-error {{expected ')'}} expected-note {{to match this '('}} omp50-error {{expected 'concurrent' in OpenMP clause 'order'}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order(reproducible:concurrent) // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} omp50-error {{expected 'concurrent' in OpenMP clause 'order'}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order(unconstrained:concurrent) // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} omp50-error {{expected 'concurrent' in OpenMP clause 'order'}}
  for (int i = 0; i < 10; ++i)
    ;
#pragma omp simd order(concurrent) order(concurrent) // omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} omp45-error {{unexpected OpenMP clause 'order' in directive '#pragma omp simd'}} omp51-error {{directive '#pragma omp simd' cannot contain more than one 'order' clause}}
  for (int i = 0; i < 10; ++i)
    ;
}

