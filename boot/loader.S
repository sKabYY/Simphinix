.code16
.text
	jmp		LABEL_START

#include "include/fat12hdr.h"
#include "include/load.h"
#include "include/pm.h"
#include "lib.h"
.set BaseOfStack, 0x100

/* GDT  */
LABEL_GDT:
	Descriptor 0, 0, 0
LABEL_DESC_FLAT_C:
	Descriptor 0, 0xfffff, DA_CR|DA_32|DA_LIMIT_4K
LABEL_DESC_FLAT_RW:
	Descriptor 0, 0xfffff, DA_DRW|DA_32|DA_LIMIT_4K
LABEL_DESC_VIDEO:
	Descriptor 0xb8000, 0xfffff, DA_DRW|DA_DPL3

.set GdtLen, (. - LABEL_GDT)
GdtPtr:	.2byte	(GdtLen - 1)
		.4byte	(BaseOfLoaderPhyAddr + LABEL_GDT)

/* GDT Selector */
.set	SelectorFlatC,	(LABEL_DESC_FLAT_C  - LABEL_GDT)
.set	SelectorFlatRW,	(LABEL_DESC_FLAT_RW - LABEL_GDT)
.set	SelectorVideo,	(LABEL_DESC_VIDEO   - LABEL_GDT + SA_RPL3)

LABEL_START:
	mov		%cs, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		%ax, %ss
	mov		$BaseOfStack, %sp

	mov		$0, %ebx
	mov		$_MemChkBuf, %di
MemChkLoop:
	mov		$0xe820, %eax
	mov		$20, %ecx
	mov		$0x534d4150, %edx /* edx = 'SMAP' */
	int		$0x15
	jc		MemChkFail
	add		$20, %di
	incl	(_dwMCRNumber)
	cmp		$0, %ebx
	jne		MemChkLoop
	jmp		MemChkOK
MemChkFail:
	movl	$0, (_dwMCRNumber)
MemChkOK:
	xor		%ah, %ah
	xor		%dl, %dl
	int		$0x13

	movw 	$SectorNoOfRootDirectory, (wSectorNo)
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
	cmpw	$0, (wRootDirSizeForLoop)
	jz		LABEL_NO_KERNELBIN
	decw	(wRootDirSizeForLoop)
	mov		$BaseOfKernelFile, %ax
	mov		%ax, %es
	mov		$OffsetOfKernelFile, %bx
	mov		(wSectorNo), %ax
	mov		$1, %cl
	call	ReadSector

	mov		$KernelFileName, %si
	mov		$OffsetOfKernelFile, %di

	cld
	mov		$0x10, %dx

LABEL_SEARCH_FOR_KERNELBIN:
	cmp		$0, %dx
	jz		LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR
	dec		%dx
	mov		$11, %cx
LABEL_CMP_FILENAME:
	cmp		$0, %cx
	jz		LABEL_FILENAME_FOUND
	dec		%cx
	lodsb
	cmp		%es:(%di), %al
	jz		LABEL_GO_ON
	jmp		LABEL_DIFFERENT

LABEL_GO_ON:
	inc		%di
	jmp		LABEL_CMP_FILENAME

LABEL_DIFFERENT:
	and		$0xffe0, %di
	add		$0x20, %di
	mov		$KernelFileName, %si
	jmp		LABEL_SEARCH_FOR_KERNELBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	addw	$1, (wSectorNo)
	jmp		LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_KERNELBIN:
	call	print
	.ascii	"No KERNEL\0"
	jmp		.

LABEL_FILENAME_FOUND:
	call	print
	.ascii	"Loading\0"
	mov		$RootDirSectors, %ax
	and		$0xffe0, %di

	push	%eax
	mov		%es:0x1c(%di), %eax
	movl	%eax, (dwKernelSize)
	pop		%eax

	add		$0x1a, %di
	mov		%es:(%di), %cx
	push	%cx
	add		%ax, %cx
	add		$DeltaSectorNo, %cx
	mov		$OffsetOfKernelFile, %bx
	mov		%cx, %ax
LABEL_GOON_LOADING_FILE:
	call	print
	.ascii	".\0"
	mov		$1, %cl
	call	ReadSector
	pop		%ax
	call	GetFATEntry
	cmp		$0xfff, %ax
	jz		LABEL_FILE_LOADED
	push	%ax
	mov		$RootDirSectors, %dx
	add		%dx, %ax
	add		$DeltaSectorNo, %ax
	add		(BPB_BytsPerSec), %bx
	jmp		LABEL_GOON_LOADING_FILE
LABEL_FILE_LOADED:
	call	KillMotor
	call	print
	.ascii	"OK\n\r\0"

	lgdt	(GdtPtr)
	cli

	in		$0x92, %al
	or		$0b00000010, %al
	out		%al, $0x92

	mov		%cr0, %eax
	or		$1, %eax
	mov		%eax, %cr0

	ljmpl	$SelectorFlatC, $(BaseOfLoaderPhyAddr+LABEL_PM_START)

wRootDirSizeForLoop:	.2byte	RootDirSectors
wSectorNo:				.2byte	0
bOdd:					.byte	0
dwKernelSize:			.4byte	0

KernelFileName:			.ascii	"KERNEL  BIN"

ReadSector:
	push	%bp
	mov		%sp, %bp
	sub		$2, %sp
	mov		%cl, -2(%bp)
	push	%bx
	mov		(BPB_SecPerTrk), %bl
	div		%bl
	inc		%ah
	mov		%ah, %cl
	mov		%al, %dh
	shr		$1, %al
	mov		%al, %ch
	and		$1, %dh
	pop		%bx
	mov		(BS_DrvNum), %dl
GoOnReading:
	mov		$2, %ah
	mov		-2(%bp), %al
	int		$0x13
	jc		GoOnReading
	add		$2, %sp
	pop		%bp
	ret

GetFATEntry:
	push	%es
	push	%bx
	push	%ax

	mov		$BaseOfKernelFile, %ax
	sub		$0x100, %ax
	mov		%ax, %es

	pop		%ax
	movb	$0, (bOdd)
	mov		$3, %bx
	mul		%bx
	mov		$2, %bx
	div		%bx
	cmp		$0, %dx
	jz		LABEL_EVEN
	movb	$1, (bOdd)
LABEL_EVEN:
	xor		%dx, %dx
	mov		(BPB_BytsPerSec), %bx
	div		%bx
	push	%dx
	mov		$0, %bx
	add		$SectorNoOfFAT1, %ax

	mov		$2, %cl
	call	ReadSector
	pop		%dx
	add		%dx, %bx
	mov		%es:(%bx), %ax
	cmpb	$1, (bOdd)
	jnz		LABEL_EVEN_2
	shr		$4, %ax
LABEL_EVEN_2:
	and		$0xfff, %ax
LABEL_GET_FAT_ENTRY_OK:
	pop		%bx
	pop		%es
	ret

KillMotor:
	push	%dx
	mov		$0x3f2, %dx
	mov		$0, %al
	out		%al, %dx
	pop		%dx
	ret

.code32
LABEL_PM_START:
	mov		$SelectorVideo, %ax
	mov		%ax, %gs
	mov		$SelectorFlatRW, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		%ax, %fs
	mov		%ax, %ss
	mov		$TopOfStack, %esp

	call	DispStr
	.ascii	"BaseAddrL BaseAddrH LengthLow LengthHigh   Type\n\0"

	call	DispMemInfo
	call	SetupPaging

	call	InitKernel

	jmp		$SelectorFlatC, $KernelEntryPointPhyAddr

DispStr:
	push	%esi
	mov		4(%esp), %esi
	push	%ebx
	push	%edi

	mov		(dwDispPos), %edi
	mov		$0xf, %ah
DispStr_1:
	lodsb
	test	%al, %al
	jz		DispStr_2
	cmp		$0xa, %al
	jnz		DispStr_3
	push	%eax
	mov		%edi, %eax
	mov		$160, %bl
	div		%bl
	and		$0xff, %eax
	inc		%eax
	mov		$160, %bl
	mul		%bl
	mov		%eax, %edi
	pop		%eax
	jmp		DispStr_1
DispStr_3:
	mov		%ax, %gs:(%edi)
	add		$2, %edi
	jmp		DispStr_1
DispStr_2:
	mov		%edi, (dwDispPos)

	pop		%edi
	pop		%ebx
	mov		%esi, 4(%esp)
	pop		%esi
	ret

DispMemInfo:
	push	%esi
	push	%edi
	push	%ecx

	mov		$MemChkBuf, %esi
	mov		(dwMCRNumber), %ecx
DispMemInfo_loop:
	mov		$5, %edx
	mov		$ARDStruct, %edi
DispMemInfo_1:
	pushl	(%esi)
	call	DispInt
	call	DispStr
	.ascii	" \0"
	pop		%eax
	stosl
	add		$4, %esi
	dec		%edx
	cmp		$0, %edx
	jnz		DispMemInfo_1
	call	DispStr
	.ascii	"\n\0"
	cmpw	$1, (dwType)
	jne		DispMemInfo_2
	mov		(dwBaseAddrLow), %eax
	add		(dwLengthLow), %eax
	cmp		(dwMemSize), %eax
	jb		DispMemInfo_2
	mov		%eax, (dwMemSize)
DispMemInfo_2:
	loop	DispMemInfo_loop
	call	DispStr
	.ascii	"\n\0"
	call	DispStr
	.ascii	"RAM size:\0"

	pushl	(dwMemSize)
	call	DispInt
	add		$4, %esp

	pop		%ecx
	pop		%edi
	pop		%esi
	ret

DispInt:
	mov		4(%esp), %eax
	shr		$24, %eax
	call	DispAL

	mov		4(%esp), %eax
	shr		$16, %eax
	call	DispAL

	mov		4(%esp), %eax
	shr		$8, %eax
	call	DispAL

	mov		4(%esp), %eax
	call	DispAL

	call	DispStr
	.ascii	"h\0"

	ret

DispAL:
	push	%ecx
	push	%edx
	push	%edi

	mov		(dwDispPos), %edi

	mov		$0xf, %ah
	mov		%al, %dl
	shr		$4, %al
	mov		$2, %ecx
DispAL_begin:
	and		$0b1111, %al
	cmp		$9, %al
	ja		DispAL_1
	add		$'0', %al
	jmp		DispAL_2
DispAL_1:
	sub		$0xa, %al
	add		$'A', %al
DispAL_2:
	mov		%ax, %gs:(%edi)
	add		$2, %edi

	mov		%dl, %al
	loop	DispAL_begin

	mov		%edi, (dwDispPos)

	pop		%edi
	pop		%edx
	pop		%ecx

	ret

SetupPaging:
	xor		%edx, %edx
	mov		(dwMemSize), %eax
	mov		$0x400000, %ebx
	div		%ebx
	mov		%eax, %ecx
	test	%edx, %edx
	jz		no_remainder
	inc		%ecx
no_remainder:
	push	%ecx

	mov		$SelectorFlatRW, %ax
	mov		%ax, %es
	mov		$PageDirBase, %edi
	xor		%eax, %eax
	mov		$(PageTblBase|PG_P|PG_USU|PG_RWW), %eax
SetupPaging_1:
	stosl
	add		$4096, %eax
	loop	SetupPaging_1

	pop		%eax
	mov		$1024, %ebx
	mul		%ebx
	mov		%eax, %ecx
	mov		$PageTblBase, %edi
	xor		%eax, %eax
	mov		$(PG_P|PG_USU|PG_RWW), %eax
SetupPaging_2:
	stosl
	add		$4096, %eax
	loop	SetupPaging_2

	mov		$PageDirBase, %eax
	mov		%eax, %cr3
	mov		%cr0, %eax
	or		$0x80000000, %eax
	mov		%eax, %cr0
	ret

InitKernel:
	xor		%esi, %esi
	mov		(BaseOfKernelFilePhyAddr+0x2c), %cx
	movzwl	%cx, %ecx
	mov		(BaseOfKernelFilePhyAddr+0x1c), %esi
	add		$BaseOfKernelFilePhyAddr, %esi
InitKernel_Begin:
	mov		(%esi), %eax
	cmp		$0, %eax
	jz		InitKernel_NoAction
	pushl	0x10(%esi)
	mov		0x4(%esi), %eax
	add		$BaseOfKernelFilePhyAddr, %eax
	push	%eax
	pushl	0x8(%esi)
	call	MemCpy
	add		$12, %esp
InitKernel_NoAction:
	add		$0x20, %esi
	dec		%ecx
	jnz		InitKernel_Begin
	ret

/* void* MemCpy(void* es:pDest, void* ds:pSrc, int iSize) */
MemCpy:
	push	%ebp
	mov		%esp, %ebp

	push	%esi
	push	%edi
	push	%ecx

	mov		8(%ebp), %edi
	mov		12(%ebp), %esi
	mov		16(%ebp), %ecx
MemCpy_1:
	cmp		$0, %ecx
	jz		MemCpy_2

	mov		%ds:(%esi), %al
	inc		%esi

	mov		%al, %es:(%edi)
	inc		%edi

	dec		%ecx
	jmp		MemCpy_1
MemCpy_2:
	mov		8(%ebp), %eax

	pop		%ecx
	pop		%edi
	pop		%esi
	mov		%ebp, %esp
	pop		%ebp
	ret

LABEL_DATA:
_dwMCRNumber:	.4byte	0
_dwDispPos:		.4byte	(80*6+0)*2
_dwMemSize:		.4byte	0
_ARDStruct:
	_dwBaseAddrLow:		.4byte	0
	_dwBaseAddrHigh:	.4byte	0
	_dwLengthLow:		.4byte	0
	_dwLengthHigh:		.4byte	0
	_dwType:			.4byte	0
_MemChkBuf:		.space	256, 0

.set	dwMCRNumber, (BaseOfLoaderPhyAddr + _dwMCRNumber)
.set	dwDispPos,	(BaseOfLoaderPhyAddr + _dwDispPos)
.set	dwMemSize,	(BaseOfLoaderPhyAddr + _dwMemSize)
.set	dwMemChkBuf,	(BaseOfLoaderPhyAddr + _dwMemChkBuf)
.set	ARDStruct,	(BaseOfLoaderPhyAddr + _ARDStruct)
	.set	dwBaseAddrLow,	(BaseOfLoaderPhyAddr + _dwBaseAddrLow)
	.set	dwBaseAddrHigh,	(BaseOfLoaderPhyAddr + _dwBaseAddrHigh)
	.set	dwLengthLow,	(BaseOfLoaderPhyAddr + _dwLengthLow)
	.set	dwLengthHigh,	(BaseOfLoaderPhyAddr + _dwLengthHigh)
	.set	dwType,			(BaseOfLoaderPhyAddr + _dwType)
.set	MemChkBuf,	(BaseOfLoaderPhyAddr + _MemChkBuf)

StackSpace:		.space	512, 0
.set	TopOfStack,	(. - StackSpace)

