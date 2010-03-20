.code16
.text
	jmp		LABEL_START
	nop

#include "include/fat12hdr.h"
#include "include/load.h"
.set	BaseOfStack,	0x7c00

LABEL_START:
	mov		%cs, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		$BaseOfStack, %sp

	mov		$0x0600, %ax
	mov		$0x0700, %bx
	mov		$0, %cx
	mov		$0x184f, %dx
	int		$0x10
	mov		$0x2, %ah
	mov		$0x0, %bh
	mov		$0x0, %dx
	int		$0x10

	call	print
	.ascii	"Hello, OS world!\n\r\0"

	xor		%ah, %ah
	xor		%dl, %dl
	int		$0x13

	movw	$SectorNoOfRootDirectory, (wSectorNo)
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
	cmpw	$0, (wRootDirSizeForLoop)
	jz		LABEL_NO_LOADERBIN
	decw	(wRootDirSizeForLoop)
	mov		$BaseOfLoader, %ax
	mov		%ax, %es
	mov		$OffsetOfLoader, %bx
	mov		(wSectorNo), %ax
	mov		$1, %cl
	call	ReadSector

	mov		$LoaderFileName, %si
	mov		$OffsetOfLoader, %di

	cld
	mov		0x10, %dx

LABEL_SEARCH_FOR_LOADERBIN:
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
	mov		$LoaderFileName, %si
	jmp		LABEL_SEARCH_FOR_LOADERBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	addw	$1, (wSectorNo)
	jmp		LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_LOADERBIN:
	call	print
	.ascii	"No LOADER\0"
	jmp		.

LABEL_FILENAME_FOUND:
	call	print
	.ascii	"Booting\0"

	mov		$RootDirSectors, %ax
	and		$0xffe0, %di
	add		$0x1a, %di
	mov		%es:(%di), %cx
	push	%cx
	add		%ax, %cx
	add		$DeltaSectorNo, %cx
	mov		$OffsetOfLoader, %bx
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
	call	print
	.ascii	"OK\n\r\0"
	jmp		$BaseOfLoader,$OffsetOfLoader

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

	mov		$BaseOfLoader, %ax
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

#include "lib.h"

wRootDirSizeForLoop:	.2byte	RootDirSectors
wSectorNo:				.2byte	0
bOdd:					.byte	0

LoaderFileName:			.ascii	"LOADER  BIN"

.org 510
.word 0xaa55

