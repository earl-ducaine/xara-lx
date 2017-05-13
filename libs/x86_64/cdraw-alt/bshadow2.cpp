




#define CONST const
#define TRUE true

typedef unsigned char		BYTE;

typedef unsigned short		WORD;
typedef signed short		INT16;
typedef unsigned short		UINT16;

typedef unsigned int		DWORD;
typedef signed int			INT32;
typedef unsigned int		UINT32;



struct BITMAPINFOHEADER
{
	WORD				biSize;
	INT32				biWidth;
	INT32				biHeight;
	WORD				biPlanes;
	WORD				biBitCount;
	DWORD				biCompression;
	DWORD				biSizeImage;
	INT32				biXPelsPerMeter;
	INT32				biYPelsPerMeter;
	DWORD				biClrUsed;
	DWORD				biClrImportant;
};


//#include "camtypes.h"

// const UINT32 MAX_SHADOW_BLUR = 100; // Maximum amount of blur
// const UINT32 MAX_CONTOUR = 100; // Maximum contour pixel size.
// const double MIN_BLUR_DIAMETER = 0.71; // Minimum blur diameter is sqrt(0.5), rounded up
// 											// a little to safely avoid boundary conditions.
// // enum {
// //   MAX_ROW_OFFSETS = MAX_SHADOW_BLUR*2+1, // Maximum size of shadow mask.
// //   TABLE_SIZE = 0x800 // Maximum lookup table size for shadow generation.
// // };

// bool GenerateWallShadow(CONST BITMAPINFOHEADER* pSourceInfo,
// 			CONST BYTE* pSourceBitmap,
// 			CONST BITMAPINFOHEADER* pDestinationInfo,
// 			BYTE* pDestinationBitmap,
// 			CONST UINT32 nRowOffsets,
// 			CONST UINT32* pLeftOffsets,
// 			CONST UINT32* pRightOffsets,
// 			CONST UINT32 nColumnOffsets,
// 			CONST UINT32* pLowOffsets,
// 			CONST UINT32* pHighOffsets,
// 			CONST UINT32 nTranslationShift,
// 			CONST BYTE* pTranslationTable);


#pragma warning(push)
#pragma GCC diagnostic ignored "-Wreturn-type"

bool GenerateWallShadow(CONST BITMAPINFOHEADER* pSourceInfo,
			CONST BYTE* pSourceBitmap,
			CONST BITMAPINFOHEADER* pDestinationInfo,
			BYTE* pDestinationBitmap,
			CONST UINT32 nRowOffsets,
			CONST UINT32* pLeftOffsets,
			CONST UINT32* pRightOffsets,
			CONST UINT32 nColumnOffsets,
			CONST UINT32* pLowOffsets,
			CONST UINT32* pHighOffsets,
			CONST UINT32 nTranslationShift,
			CONST BYTE* pTranslationTable) {
  
  asm ("push   %r15\n\t"
     "push   %r14\n\t"
     "push   %r13\n\t"
     "mov    %rdx,%r13\n\t"
     "push   %r12\n\t"
     "push   %rbp\n\t"
     "mov    %r9,%rbp\n\t"
     "push   %rbx\n\t"
     "mov    %rsi,%rbx\n\t"
     "mov    0x38(%rsp),%r12\n\t"
     "mov    %rcx,-0x10(%rsp)\n\t"
     "cmpl   $0x80001,0xc(%rdi)\n\t"
     "je     _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x33\n\t"
     "pop    %rbx\n\t"
     "pop    %rbp\n\t"
     "pop    %r12\n\t"
     "pop    %r13\n\t"
     "pop    %r14\n\t"
     "pop    %r15\n\t"
     "xor    %eax,%eax\n\t"
     "retq\n\t"
     "cmpl   $0x80001,0xc(%rdx)\n\t"
     "setne  %dl\n\t"
     "cmp    $0xc9,%r8d\n\t"
     "seta   %al\n\t"
     "or     %al,%dl\n\t"
     "jne    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x26\n\t"
     "mov    0x4(%rdi),%edi\n\t"
     "mov    0x4(%r13),%r11d\n\t"
     "xor    %r14d,%r14d\n\t"
     "xor    %eax,%eax\n\t"
     "add    $0x3,%edi\n\t"
     "add    $0x3,%r11d\n\t"
     "and    $0xfffffffc,%edi\n\t"
     "and    $0xfffffffc,%r11d\n\t"
     "test   %r8d,%r8d\n\t"
     "je     _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0xab\n\t"
     "xor    %r9d,%r9d\n\t"
     "xor    %esi,%esi\n\t"
     "lea    0x0(,%rsi,4),%rax\n\t"
     "mov    0x0(%rbp,%rax,1),%edx\n\t"
     "mov    (%r12,%rax,1),%eax\n\t"
     "cmp    %eax,%edx\n\t"
     "jae    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x96\n\t"
     "mov    %eax,%r10d\n\t"
     "mov    %edx,%eax\n\t"
     "inc    %edx\n\t"
     "movzbl (%rbx,%rax,1),%eax\n\t"
     "add    %eax,%r14d\n\t"
     "cmp    %r10d,%edx\n\t"
     "jne    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x86\n\t"
     "inc    %r9d\n\t"
     "inc    %rsi\n\t"
     "cmp    %r9d,%r8d\n\t"
     "jne    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x6f \n\t"
     "movzbl 0x58(%rsp),%ecx\n\t"
     "mov    %r14d,%eax\n\t"
     "shr    %cl,%eax\n\t"
     "mov    %edi,%edi\n\t"
     "mov    %r11d,%r11d\n\t"
     "movl   $0x1,-0x4(%rsp)\n\t"
     "mov    %rdi,-0x20(%rsp)\n\t"
     "mov    %r11,-0x18(%rsp)\n\t"
     "cmpl   $0x1,0x8(%r13)\n\t"
     "jbe    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x263\n\t"
     "movzbl 0x58(%rsp),%ecx\n\t"
     "mov    0x60(%rsp),%r15\n\t"
     "mov    %r14d,%eax\n\t"
     "mov    -0x10(%rsp),%rdx\n\t"
     "mov    %r14d,%esi\n\t"
     "mov    $0x1,%r9d\n\t"
     "shr    %cl,%eax\n\t"
     "mov    %rdx,%rdi\n\t"
     "mov    %eax,%eax\n\t"
     "add    %rax,%r15\n\t"
     "movzbl (%r15),%eax\n\t"
     "mov    %al,(%rdx)\n\t"
     "mov    %rbx,-0x28(%rsp)\n\t"
     "cmpl   $0x1,0x4(%r13)\n\t"
     "jbe    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x163\n\t"
     "xor    %r11d,%r11d\n\t"
     "xor    %r10d,%r10d\n\t"
     "test   %r8d,%r8d\n\t"
     "je     _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x13b\n\t"
     "mov    -0x28(%rsp),%rcx\n\t"
     "lea    0x0(,%r10,4),%rdx\n\t"
     "inc    %r11d\n\t"
     "inc    %r10\n\t"
     "mov    (%r12,%rdx,1),%eax\n\t"
     "mov    0x0(%rbp,%rdx,1),%edx\n\t"
     "movzbl (%rcx,%rax,1),%eax\n\t"
     "movzbl (%rcx,%rdx,1),%edx\n\t"
     "sub    %edx,%eax\n\t"
     "add    %eax,%esi\n\t"
     "cmp    %r11d,%r8d\n\t"
     "jne    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x114\n\t"
     "movzbl 0x58(%rsp),%ecx\n\t"
     "mov    0x60(%rsp),%rdx\n\t"
     "mov    %esi,%eax\n\t"
     "incq   -0x28(%rsp)\n\t"
     "inc    %r9d\n\t"
     "shr    %cl,%eax\n\t"
     "mov    %eax,%eax\n\t"
     "movzbl (%rdx,%rax,1),%eax\n\t"
     "mov    %al,0x1(%rdi)\n\t"
     "inc    %rdi\n\t"
     "cmp    0x4(%r13),%r9d\n\t"
     "jb     _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x104\n\t"
     "mov    0x40(%rsp),%eax\n\t"
     "test   %eax,%eax\n\t"
     "je     _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x1b7\n\t"
     "xor    %r11d,%r11d\n\t"
     "xor    %r10d,%r10d\n\t"
     "mov    0x50(%rsp),%rcx\n\t"
     "lea    0x0(,%r10,4),%rdx\n\t"
     "inc    %r11d\n\t"
     "inc    %r10\n\t"
     "mov    (%rcx,%rdx,1),%eax\n\t"
     "mov    0x48(%rsp),%rcx\n\t"
     "mov    (%rcx,%rdx,1),%edx\n\t"
     "movzbl (%rbx,%rax,1),%eax\n\t"
     "movzbl (%rbx,%rdx,1),%edx\n\t"
     "sub    %edx,%eax\n\t"
     "add    %eax,%r14d\n\t"
     "cmp    %r11d,0x40(%rsp)\n\t"
     "jne    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x171\n\t"
     "movzbl 0x58(%rsp),%ecx\n\t"
     "mov    %r14d,%eax\n\t"
     "mov    0x60(%rsp),%r15\n\t"
     "shr    %cl,%eax\n\t"
     "mov    %eax,%eax\n\t"
     "add    %rax,%r15\n\t"
     "incl   -0x4(%rsp)\n\t"
     "mov    -0x18(%rsp),%rax\n\t"
     "add    %rax,-0x10(%rsp)\n\t"
     "add    -0x20(%rsp),%rbx\n\t"
     "mov    -0x4(%rsp),%edx\n\t"
     "cmp    %edx,0x8(%r13)\n\t"
     "ja     _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0xcd\n\t"
     "movzbl (%r15),%eax\n\t"
     "mov    -0x10(%rsp),%rcx\n\t"
     "mov    %r14d,%r11d\n\t"
     "mov    %rbx,%rsi\n\t"
     "mov    $0x1,%r9d\n\t"
     "mov    %rcx,%r15\n\t"
     "mov    %al,(%rcx)\n\t"
     "cmpl   $0x1,0x4(%r13)\n\t"
     "jbe    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x253\n\t"
     "xor    %r10d,%r10d\n\t"
     "xor    %edi,%edi\n\t"
     "test   %r8d,%r8d\n\t"
     "je     _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x22b\n\t"
     "lea    0x0(,%rdi,4),%rdx\n\t"
     "inc    %r10d\n\t"
     "inc    %rdi\n\t"
     "mov    (%r12,%rdx,1),%eax\n\t"
     "mov    0x0(%rbp,%rdx,1),%edx\n\t"
     "movzbl (%rsi,%rax,1),%eax\n\t"
     "movzbl (%rsi,%rdx,1),%edx\n\t"
     "sub    %edx,%eax\n\t"
     "add    %eax,%r11d\n\t"
     "cmp    %r10d,%r8d\n\t"
     "jne    _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x203\n\t"
     "movzbl 0x58(%rsp),%ecx\n\t"
     "mov    0x60(%rsp),%rdx\n\t"
     "mov    %r11d,%eax\n\t"
     "inc    %rsi\n\t"
     "inc    %r9d\n\t"
     "shr    %cl,%eax\n\t"
     "mov    %eax,%eax\n\t"
     "movzbl (%rdx,%rax,1),%eax\n\t"
     "mov    %al,0x1(%r15)\n\t"
     "inc    %r15\n\t"
     "cmp    0x4(%r13),%r9d\n\t"
     "jb     _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x1f9\n\t"
     "pop    %rbx\n\t"
     "pop    %rbp\n\t"
     "pop    %r12\n\t"
     "pop    %r13\n\t"
     "pop    %r14\n\t"
     "pop    %r15\n\t"
     "mov    $0x1,%eax\n\t"
     "retq\n\t"
     "mov    0x60(%rsp),%r15\n\t"
     "mov    %eax,%eax\n\t"
     "add    %rax,%r15\n\t"
     "jmp   _Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_ + 0x1d8\n\t"
     );
}

#pragma warning(pop)

// 0000000000000690 <_Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_>:
// 0000000000000000 <_Z18GenerateWallShadowPK16BITMAPINFOHEADERPKhS1_PhjPKjS6_jS6_S6_jS3_>:






// asm ("mov %1, %0\n\t"
//     "add $1, %0"
//     : "=r" (dst) 
//     : "r" (src));



