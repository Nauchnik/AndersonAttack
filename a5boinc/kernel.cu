
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <helper_cuda.h>


#define R1LEN 19
#define R2LEN 22
#define R3LEN 23

#define R2MAJ 11

#define BLOCKSIZE 8


#define m_CopyToGPU(d, s,  bytes){ checkCudaErrors(cudaMalloc((void**) &d, bytes)); checkCudaErrors(cudaMemcpy((void*) d, (void*)s, bytes, cudaMemcpyHostToDevice)); }
typedef unsigned regtype;


inline void CUDA_CHECK_ERROR2(cudaError_t err)
{
	if (err != cudaSuccess) { printf("Cuda error %i: %s\n ", (int)err, cudaGetErrorString(err)); printf("Error in file: %s, line: %i\n", __FILE__, __LINE__); }
}

static inline double cpuTime(void) { return (double)clock() / CLOCKS_PER_SEC; }

const unsigned KSTREAMLEN = 64; // длина ключевого потока в байтах
//char stream[KSTREAMLEN] = { 2, 3, 2, 4, 3, 6, 1, 1, 8, 10,2, 2, 2, 4, 4, 6, 2, 8, 9, 10,1, 2, 3, 4, 5, 6, 7, 8, 9, 10,2, 2, 3, 4, 5, 7, 7, 8, 2, 10,1, 2, 3, 4, 5, 6, 7, 8, 9, 10,1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1,2,3,4 };

__constant__ char stream_gpu[KSTREAMLEN] = { 2, 3, 2, 4, 3, 6, 1, 1, 8, 10,2, 2, 2, 4, 4, 6, 2, 8, 9, 10,1, 2, 3, 4, 5, 6, 7, 8, 9, 10,2, 2, 3, 4, 5, 7, 7, 8, 2, 10,1, 2, 3, 4, 5, 6, 7, 8, 9, 10,1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1,2,3,4 };

inline unsigned expand_cpu(bool b) { return (0 - b); }
__device__ inline unsigned expand(bool b) { return (0 - b); }
__device__ unsigned   gpu_result = 0;

#define DEF_REGS_R1 regtype& b01, regtype& b02, regtype& b03, regtype& b04, regtype& b05, regtype& b06, regtype& b07, regtype& b08, regtype& b09, regtype& b10, regtype& b11, regtype& b12, regtype& b13, regtype& b14, regtype& b15, regtype& b16, regtype& b17, regtype& b18, regtype& b19

#define DEF_REGS_R2 regtype& b01, regtype& b02, regtype& b03, regtype& b04, regtype& b05, regtype& b06, regtype& b07, regtype& b08, regtype& b09, regtype& b10, regtype& b11, regtype& b12, regtype& b13, regtype& b14, regtype& b15, regtype& b16, regtype& b17, regtype& b18, regtype& b19, regtype& b20, regtype& b21, regtype& b22

#define DEF_REGS_R3 regtype& b01, regtype& b02, regtype& b03, regtype& b04, regtype& b05, regtype& b06, regtype& b07, regtype& b08, regtype& b09, regtype& b10, regtype& b11, regtype& b12, regtype& b13, regtype& b14, regtype& b15, regtype& b16, regtype& b17, regtype& b18, regtype& b19, regtype& b20, regtype& b21, regtype& b22, regtype& b23

#define REGS_R1 r1b01, r1b02, r1b03, r1b04, r1b05, r1b06, r1b07, r1b08, r1b09, r1b10, r1b11, r1b12, r1b13, r1b14, r1b15, r1b16, r1b17, r1b18, r1b19
#define REGS_R2	r2b01, r2b02, r2b03, r2b04, r2b05, r2b06, r2b07, r2b08, r2b09, r2b10, r2b11, r2b12, r2b13, r2b14, r2b15, r2b16, r2b17, r2b18, r2b19, r2b20, r2b21, r2b22
#define REGS_R3	r3b01, r3b02, r3b03, r3b04, r3b05, r3b06, r3b07, r3b08, r3b09, r3b10, r3b11, r3b12, r3b13, r3b14, r3b15, r3b16, r3b17, r3b18, r3b19, r3b20, r3b21, r3b22, r3b23




#define  DEF_R1(n) regtype \
r1b01=expand(1&(n>>0)),\
r1b02=expand(1&(n>>1)),\
r1b03=expand(1&(n>>2)),\
r1b04=expand(1&(n>>3)),\
r1b05=expand(1&(n>>4)),\
r1b06=expand(1&(n>>5)),\
r1b07=expand(1&(n>>6)),\
r1b08=expand(1&(n>>7)),\
r1b09=expand(1&(n>>8)),\
r1b10=expand(1&(n>>9)),\
r1b11=expand(1&(n>>10)),\
r1b12=expand(1&(n>>11)),\
r1b13=expand(1&(n>>12)),\
r1b14=expand(1&(n>>13)),\
r1b15=expand(1&(n>>14)),\
r1b16=expand(1&(n>>15)),\
r1b17=expand(1&(n>>16)),\
r1b18=expand(1&(n>>17)),\
r1b19=expand(1&(n>>18))



#define DEF_R2(n) regtype \
r2b01=0xffff,\
r2b02=0xff00ff,\
r2b03=0xf0f0f0f0,\
r2b04=0x33333333,\
r2b05=0x55555555,\
r2b06=expand(1&(n>>5)),\
r2b07=expand(1&(n>>6)),\
r2b08=expand(1&(n>>7)),\
r2b09=expand(1&(n>>8)),\
r2b10=expand(1&(n>>9)),\
r2b11=expand(1&(n>>10)),\
r2b12=0,\
r2b13=0,\
r2b14=0,\
r2b15=0,\
r2b16=0,\
r2b17=0,\
r2b18=0,\
r2b19=0,\
r2b20=0,\
r2b21=0,\
r2b22=0

#define DEF_R3(n) regtype \
r3b01=expand(1&(n>>0)),\
r3b02=expand(1&(n>>1)),\
r3b03=expand(1&(n>>2)),\
r3b04=expand(1&(n>>3)),\
r3b05=expand(1&(n>>4)),\
r3b06=expand(1&(n>>5)),\
r3b07=expand(1&(n>>6)),\
r3b08=expand(1&(n>>7)),\
r3b09=expand(1&(n>>8)),\
r3b10=expand(1&(n>>9)),\
r3b11=expand(1&(n>>10)),\
r3b12=expand(1&(n>>11)),\
r3b13=expand(1&(n>>12)),\
r3b14=expand(1&(n>>13)),\
r3b15=expand(1&(n>>14)),\
r3b16=expand(1&(n>>15)),\
r3b17=expand(1&(n>>16)),\
r3b18=expand(1&(n>>17)),\
r3b19=expand(1&(n>>18)),\
r3b20=expand(1&(n>>19)),\
r3b21=expand(1&(n>>20)),\
r3b22=expand(1&(n>>21)),\
r3b23=expand(1&(n>>22))

__device__ inline regtype bitselect(regtype a, regtype b, regtype c) {
#if (__CUDA_ARCH__ >= 500)
	regtype out;
	asm("lop3.b32 %0, %1, %2, %3, 0xe4;" : "=r"(out) : "r"(a), "r"(b), "r"(c));
	return out;
#else
	return ((a&c)|(b&(~c)));
#endif
}
//__device__ inline regtype bitselect(regtype a, regtype b, regtype c) {return (a ^ ((c ^ a) & b));}

regtype bitselect_cpu(regtype a, regtype b, regtype c) { return ((a&c) | (b&(~c))); }
inline bool getbit_cpu(char* data, unsigned bnum)
{
	return (bool)(data[bnum >> 3] & (1 << (7 - (bnum & 0x7))));
	//return (bool)(data[bnum]);
}
__device__ inline void ClockR1(DEF_REGS_R1, regtype& f)
{

	// New bit
	regtype nv = b19 ^ b18 ^ b17 ^ b14;
	b19 = bitselect(b18, b19, f);
	b18 = bitselect(b17, b18, f);
	b17 = bitselect(b16, b17, f);
	b16 = bitselect(b15, b16, f);
	b15 = bitselect(b14, b15, f);
	b14 = bitselect(b13, b14, f);
	b13 = bitselect(b12, b13, f);
	b12 = bitselect(b11, b12, f);
	b11 = bitselect(b10, b11, f);
	b10 = bitselect(b09, b10, f);
	b09 = bitselect(b08, b09, f);
	b08 = bitselect(b07, b08, f);
	b07 = bitselect(b06, b07, f);
	b06 = bitselect(b05, b06, f);
	b05 = bitselect(b04, b05, f);
	b04 = bitselect(b03, b04, f);
	b03 = bitselect(b02, b03, f);
	b02 = bitselect(b01, b02, f);
	b01 = bitselect(nv, b01, f);

}

__device__ inline void ClockR2(DEF_REGS_R2, regtype& f)
{
	// New bit
	regtype nv = b22 ^ b21;
	b22 = bitselect(b21, b22, f);
	b21 = bitselect(b20, b21, f);
	b20 = bitselect(b19, b20, f);
	b19 = bitselect(b18, b19, f);
	b18 = bitselect(b17, b18, f);
	b17 = bitselect(b16, b17, f);
	b16 = bitselect(b15, b16, f);
	b15 = bitselect(b14, b15, f);
	b14 = bitselect(b13, b14, f);
	b13 = bitselect(b12, b13, f);
	b12 = bitselect(b11, b12, f);
	b11 = bitselect(b10, b11, f);
	b10 = bitselect(b09, b10, f);
	b09 = bitselect(b08, b09, f);
	b08 = bitselect(b07, b08, f);
	b07 = bitselect(b06, b07, f);
	b06 = bitselect(b05, b06, f);
	b05 = bitselect(b04, b05, f);
	b04 = bitselect(b03, b04, f);
	b03 = bitselect(b02, b03, f);
	b02 = bitselect(b01, b02, f);
	b01 = bitselect(nv, b01, f);
}

__device__ inline void ClockR3(DEF_REGS_R3, regtype& f)
{
	// New bit
	regtype nv = b23 ^ b22 ^ b21 ^ b08;
	b23 = bitselect(b22, b23, f);
	b22 = bitselect(b21, b22, f);
	b21 = bitselect(b20, b21, f);
	b20 = bitselect(b19, b20, f);
	b19 = bitselect(b18, b19, f);
	b18 = bitselect(b17, b18, f);
	b17 = bitselect(b16, b17, f);
	b16 = bitselect(b15, b16, f);
	b15 = bitselect(b14, b15, f);
	b14 = bitselect(b13, b14, f);
	b13 = bitselect(b12, b13, f);
	b12 = bitselect(b11, b12, f);
	b11 = bitselect(b10, b11, f);
	b10 = bitselect(b09, b10, f);
	b09 = bitselect(b08, b09, f);
	b08 = bitselect(b07, b08, f);
	b07 = bitselect(b06, b07, f);
	b06 = bitselect(b05, b06, f);
	b05 = bitselect(b04, b05, f);
	b04 = bitselect(b03, b04, f);
	b03 = bitselect(b02, b03, f);
	b02 = bitselect(b01, b02, f);
	b01 = bitselect(nv, b01, f);

}


__device__ inline void AssertR3Nozeros(DEF_REGS_R3)
{
	assert(0xffffffff == (b01 | b02 | b03 | b04 | b05 | b06 | b07 | b08 | b09 | b10 | b11 | b12 | b13 | b14 | b15 | b16 | b17 | b18 | b19 | b20 | b21 | b22 | b23));
}

__device__ inline void AssertR2Nozeros(DEF_REGS_R2)
{
	assert(0xffffffff == (b01 | b02 | b03 | b04 | b05 | b06 | b07 | b08 | b09 | b10 | b11 | b12 | b13 | b14 | b15 | b16 | b17 | b18 | b19 | b20 | b21 | b22));
}
__device__ inline void AssertR1Nozeros(DEF_REGS_R1)
{
	assert(0xffffffff == (b01 | b02 | b03 | b04 | b05 | b06 | b07 | b08 | b09 | b10 | b11 | b12 | b13 | b14 | b15 | b16 | b17 | b18 | b19));
}

__device__ inline regtype AddAndCheckBitwise(regtype& b0, regtype& b1, regtype& b2, regtype& b3, regtype in)
{
	const int VALUE = 11;
	const regtype y0 = expand(1 & (VALUE >> 0));
	const regtype y1 = expand(1 & (VALUE >> 1));
	const regtype y2 = expand(1 & (VALUE >> 2));
	const regtype y3 = expand(1 & (VALUE >> 3));

	regtype c0 = 0, c1 = 0, c2 = 0, c3 = 0;
	//Check value bitwise if we need to sum futher
	c0 = in & ((b0^y0) | (b1^y1) | (b2^y2) | (b3^y3));

	// Add bitwise
	c1 = b0 & c0; //carry bit 0->1
	c2 = b1 & c1; //carry bit 1->2
	c3 = b2 & c2; //carry bit 2->3
	b0 ^= c0; // assign bit it's new value
	b1 ^= c1;
	b2 ^= c2;
	b3 ^= c3;

	//		printf ("%x ", (b0^y0) | (b1^y1) | (b2^y2) | (b3^y3));
	//Check value again
	return (in & ((b0^y0) | (b1^y1) | (b2^y2) | (b3^y3)));
}

__device__ inline regtype CheckBitwise(regtype& b0, regtype& b1, regtype& b2, regtype& b3)
{
	const int VALUE = 11;
	const regtype y0 = expand(1 & (VALUE >> 0));
	const regtype y1 = expand(1 & (VALUE >> 1));
	const regtype y2 = expand(1 & (VALUE >> 2));
	const regtype y3 = expand(1 & (VALUE >> 3));
	return ((b0^y0) | (b1^y1) | (b2^y2) | (b3^y3));
}

__device__ inline unsigned int bslc2uint(int i)
{
	unsigned int out = 0x0;

	out |=
		((0x0000ffff>>i)&1)<<0 | 
		((0x00ff00ff>>i)&1)<<1 |
		((0xf0f0f0f0>>i)&1)<<2 |
		((0x33333333>>i)&1)<<3 |
		((0x55555555>>i)&1)<<4 ;
	return out;
}

__global__ void kernel_gpu(regtype* stream_expanded_gpu, unsigned rnum)
{

	assert(0x1 == bitselect(0x1, 0x0, 0x1));
	assert(0x0 == bitselect(0x1, 0x0, 0x0));
	assert(0x0 == bitselect(0x0, 0x1, 0x1));
	assert(0x1 == bitselect(0x0, 0x1, 0x0));
	assert(0x0 == bitselect(0x0, 0x0, 0x0));
	assert(0x1 == bitselect(0x1, 0x1, 0x1));
	assert(0x0 == bitselect(0x0, 0x0, 0x1));
	assert(0x1 == bitselect(0x1, 0x1, 0x0));
	if (((blockIdx.x << BLOCKSIZE) + threadIdx.x) == 0) return;
	const unsigned int tid = (blockIdx.x << BLOCKSIZE) + threadIdx.x;
	DEF_R1(rnum);
	DEF_R2((rnum>>19) << 5);
	DEF_R3(tid);

	// STAGE 1 - compute R2 register
	unsigned dead_lanes = 0x0;
	regtype s0 = 0, s1 = 0, s2 = 0, s3 = 0;

	regtype cval_old = r2b22 = r1b19 ^ stream_expanded_gpu[0] ^ r3b23;

	unsigned long long tmp = 0; 
	unsigned long long tmp2 = 0; 
	for (int i = 0; i<64; ++i) {
		// majority
		regtype maj = ~(r1b10 ^ r2b11 ^ r3b11);

		//AssertR1Nozeros(REGS_R1);
		regtype m1 = r1b10 ^ maj;
		ClockR1(REGS_R1, m1);

		//AssertR3Nozeros(REGS_R3);
		regtype m3 = r3b11 ^ maj;
		ClockR3(REGS_R3, m3);

		regtype cval_next = r1b19 ^ stream_expanded_gpu[i] ^ r3b23;

		//AssertR2Nozeros(REGS_R2);
		regtype m2 = r2b11 ^ maj;
		regtype st1mode = AddAndCheckBitwise(s0, s1, s2, s3, m2); // Stage 1 or 2 flag
		dead_lanes |= st1mode & (~m2 & (cval_old ^ cval_next)); // Early conflict!
		r2b21 = bitselect(cval_next, r2b21, st1mode);
		ClockR2(REGS_R2, m2);
		cval_old = cval_next;

		regtype o = r1b19 ^ r2b22 ^ r3b23;
		/*
		if (tid == 0x123)
		{
			tmp |= (unsigned long long)((o & 1))<<i;
			tmp2 |= (unsigned long long)((stream_expanded_gpu[i] & 1))<<i;
		}
		*/

		dead_lanes |= (stream_expanded_gpu[i] ^ o)&(~st1mode); // only for stage 2
		//printf ("%x:", dead_lanes);
		if (dead_lanes == 0xffffffff )
			break;
	}

	if (dead_lanes != 0xffffffff)
	{
		for (int i=0; i<32; ++i)
			if(((dead_lanes >> i)&1) == 0)
			{
				gpu_result =
				(((unsigned)bslc2uint(i)) << 23)|
				((unsigned)tid);
				//if (tid == 0x123) printf ("\n gpures %lx", gpu_result);
				break;
			}
	}

	/*
	if (tid == 0x123)
	{

		printf ("\n %lx", gpu_result);
		printf ("\n %llx", tmp);
		printf ("\n %llx", tmp2);
	}
	*/
	//printf("\n");
	
}

unsigned long long RunKernel(unsigned long long c_stream, unsigned long long start, unsigned long long stop, int device)
{
	//test key 53 bit 0x948000ad800123
	//test stream 0xb3f4c70cdd61313f

	regtype stream_expanded[8*sizeof(c_stream)];
	for (int i = 0; i<8*sizeof(c_stream); ++i)
	{
		stream_expanded[i] = ((c_stream >> i)&1 ? 0xffffffff : 0x0);
		//printf("%i", stream_expanded[i]&1);
	}

	cudaDeviceProp dp;
	cudaGetDeviceProperties(&dp, device);
	//printf(" %i %i ",dp.major, dp.minor);
	if (dp.major < 2)
	{
		printf("\nGPU compute capability too low!");
		exit(1);
	}
	cudaSetDevice(device);

	regtype* stream_expanded_gpu;
	m_CopyToGPU(stream_expanded_gpu, stream_expanded, KSTREAMLEN * 2 * 8 * sizeof(regtype));
	assert(expand_cpu(1) == 0xffffffff);

	//  R1 - 19 bits | R2 - 22 bits                                          | R3 - 23 bits
	//  R1 - 19 bits | 11 anderson bits | 6 bits - R2 bits | 5 bitslice bits | R3 - 23 bits 
	//  ^ start      |                  | ^ start          | ^  processed in kernel  ^
	unsigned start_regs = start>>(23+5);
	unsigned stop_regs = stop>>(23+5);

	
	cudaFuncSetCacheConfig(kernel_gpu, cudaFuncCachePreferL1);
	cudaEvent_t kernel_start, kernel_stop;
	cudaEventCreate(&kernel_start);
	cudaEventCreate(&kernel_stop);
	cudaEventRecord(kernel_start);
	//printf("start-stop %x %x", start_regs, stop_regs);
	unsigned h_result = 0;
	unsigned long long out = 0;
	for (unsigned i = start_regs; i < stop_regs; i++) 
	{
		kernel_gpu <<<(1 << (R3LEN - BLOCKSIZE)), 1<<BLOCKSIZE >>> (stream_expanded_gpu, i);
		#ifndef NDEBUG
		cudaThreadSynchronize();
		cudaError_t err = cudaGetLastError();
		checkCudaErrors(err);
		#endif
		checkCudaErrors(cudaMemcpyFromSymbol(&h_result, gpu_result, sizeof(gpu_result), 0, cudaMemcpyDeviceToHost));
		if (h_result != 0)
		{
			out =(((unsigned long long)i) << (23+5)) | ((unsigned long long)h_result) ;
			break;
		}
	}
	//printf("\nResult: %llx", out);
	cudaThreadSynchronize();
	cudaEventRecord(kernel_stop);
	cudaEventSynchronize(kernel_stop);
	float preprocessTime;
	cudaEventElapsedTime(&preprocessTime, kernel_start, kernel_stop);


	float tm = preprocessTime / 1000;

	return out;
}
