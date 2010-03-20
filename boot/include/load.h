.set	BaseOfLoader,				0x9000
.set	OffsetOfLoader,				0x100

.set	BaseOfLoaderPhyAddr,		BaseOfLoader * 0x10

.set	BaseOfKernelFile,			0x8000
.set	OffsetOfKernelFile,			0x0

.set	BaseOfKernelFilePhyAddr,	BaseOfKernelFile * 0x10
.set	KernelEntryPointPhyAddr,	0x30400

.set	PageDirBase,				0x200000
.set	PageTblBase,				0x201000

