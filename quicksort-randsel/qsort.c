#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// Swap two elements of an array, at indices i and j
//
static inline void swap(int a[], const size_t i, const size_t j) {
	const int tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
}

// Get a random, bounded size_t. The result will be between 0 and arrayLen-1
// inclusive.
//
static inline size_t chooseRandomIndex(const size_t arrayLen) {
	const double fRand = (double)rand() / (1.0 + RAND_MAX);
	return arrayLen * fRand;
}

// The quick-sort partition algorithm. This partitions an array around a
// randomly-chosen pivot, such that the elements to the left of the pivot are
// less than or equal to the pivot element, and the elements to the right of the
// pivot are greater than the pivot element, so the pivot element itself ends up
// in its "rightful" ranked position.
//
static inline size_t partition(
	int a[], const size_t startIndex, const size_t endIndex)
{
	swap(a, startIndex, startIndex + chooseRandomIndex(endIndex-startIndex));
	size_t j = startIndex;
	const int pivot = a[j];
	for ( size_t i = j + 1; i < endIndex; i++ ) {
		if ( a[i] <= pivot ) {
			swap(a, i, ++j);
		}
	}
	swap(a, startIndex, j);
	return j;
}

// Linear-time randomized selection algorithm. There's a great description of
// the algorithm in Tim Roughgarden's video:
//
// https://www.youtube.com/watch?v=kg3XKGFObI4
//
int randSelect(int a[], const size_t len, const size_t ordinal) {
	size_t startIndex = 0, endIndex = len;
	size_t pivot = partition(a, startIndex, endIndex);
	while ( pivot != ordinal ) {
		if ( pivot > ordinal ) {
			// Look in the subarray to the left of the pivot
			endIndex = pivot;
		} else {
			// Look in the subarray to the right of the pivot
			startIndex = pivot + 1;
		}
		pivot = partition(a, startIndex, endIndex);
	}
	return a[pivot];
}

// Sort array using quick-sort algorithm, with special base-cases for arrays of
// length 2 & 3: these make a runtime difference of a few percent.
//
void sort(int a[], const size_t startIndex, const size_t endIndex) {
	const size_t len = endIndex - startIndex;
	if ( len > 3 ) {
		const size_t pivot = partition(a, startIndex, endIndex);
		sort(a, startIndex, pivot);
		sort(a, pivot+1, endIndex);
	} else if ( len == 3 ) {
		const size_t x = startIndex, y = x + 1, z = y + 1;
		const int ax = a[x], ay = a[y], az = a[z];
		if ( ax > ay ) {
			if ( ay > az ) {
				// 11: 321
				a[x] = az; a[z] = ax;
			} else {
				// 10: 312 or 213
				if ( ax > az ) {
					a[x] = ay; a[y] = az; a[z] = ax;  // 312
				} else {
					a[x] = ay; a[y] = ax;  // 213
				}
			}
		} else {
			if ( ay > az ) {
				// 01: 231 or 132
				if ( ax > az ) {
					a[x] = az; a[y] = ax; a[z] = ay;  // 231
				} else {
					a[y] = az; a[z] = ay;  // 132
				}
			} else {
				// 00: 123 - nothing to do
			}
		}
	} else if ( len == 2 ) {
		const size_t x = startIndex;
		const size_t y = x + 1;
		if ( a[x] > a[y] ) {
			swap(a, x, y);
		}
	}
}

// Populate the array with random integers
//
void populate(int a[], const size_t len) {
	for ( size_t i = 0; i < len; i++ ) {
		a[i] = rand();
	}
}

// The compare callback given to qsort()
//
int cmp(const void *x, const void *y) {
	const int *ix = (const int *)x;
	const int *iy = (const int *)y;
	return *ix - *iy;
}

// Sort an array of the given number of millions of random ints: first with the
// custom quick-sort, then with the general-purpose qsort() from the C standard
// library, and finally use the linear-time randomized selection algorithm to
// find the median. Verify the results and print a report-line.
//
void runTest(const int x) {
	const size_t arrayLen = x*1000000;
	int *a = (int*)malloc(arrayLen*sizeof(int));
	int *b = (int*)malloc(arrayLen*sizeof(int));
	int *c = (int*)malloc(arrayLen*sizeof(int));
	struct timeval tv1, tv2, tv3, tv4;
	double t1, t2, t3, t4;
	populate(a, arrayLen);
	memcpy(b, a, arrayLen*sizeof(int));
	memcpy(c, a, arrayLen*sizeof(int));
	gettimeofday(&tv1, NULL);
	sort(a, 0, arrayLen);
	gettimeofday(&tv2, NULL);
	qsort(b, arrayLen, sizeof(int), cmp);
	gettimeofday(&tv3, NULL);
	const int m = randSelect(c, arrayLen, arrayLen/2);
	gettimeofday(&tv4, NULL);
	t1 = (double)tv1.tv_sec + tv1.tv_usec/1000000.0;
	t2 = (double)tv2.tv_sec + tv2.tv_usec/1000000.0;
	t3 = (double)tv3.tv_sec + tv3.tv_usec/1000000.0;
	t4 = (double)tv4.tv_sec + tv4.tv_usec/1000000.0;
	printf(
		"%d %0.3f %0.3f %0.3f %s %s\n",
		x, t2-t1, t3-t2, t4-t3,
		(memcmp(a, b, arrayLen*sizeof(int)) == 0) ? "✓" : "✗",
		(m == a[arrayLen/2]) ? "✓" : "✗"
	);
	free(c);
	free(b);
	free(a);
}

int main(void) {
	printf("N t(X) t(Y) t(Z) X==Y? m(X)==m(Z)?\n");
	srand(0xC0DEFACE);
	for ( int i = 0; i < 3; i++ ) {
		runTest(100);
	}
	return 0;
}
