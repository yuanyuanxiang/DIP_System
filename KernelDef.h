#ifndef _KERNEL_DEF
#define _KERNEL_DEF

typedef struct  kernel
{
	int Element[3][3];
	int Divisor;
	int Dimention;
}KERNEL, *LPKERNEL;

extern KERNEL Emboss[8];
extern KERNEL Line_Filter[4];
extern KERNEL OverRun_Filter;

#endif
