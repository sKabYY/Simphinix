.set	P_STACKBASE,	0
.set	GSREG,			P_STACKBASE
.set	FSREG,			GSREG + 4
.set	ESREG,			FSREG + 4
.set	DSREG,			ESREG + 4
.set	EDIREG,			DSREG + 4
.set	ESIREG,			EDIREG + 4
.set	EBPREG,			ESIREG + 4
.set	KERNELESPREG,	EBPREG + 4
.set	EBXREG,			KERNELESPREG + 4
.set	EDXREG,			EBXREG + 4
.set	ECXREG,			EDXREG + 4
.set	EAXREG,			ECXREG + 4
.set	RETADR,			EAXREG + 4
.set	EIPREG,			RETADR + 4
.set	CSREG,			EIPREG + 4
.set	EFLAGSREG,		CSREG + 4
.set	ESPREG,			EFLAGSREG + 4
.set	SSREG,			ESPREG + 4
.set	P_STACKTOP,		SSREG + 4
.set	P_PID,			P_STACKTOP
.set	P_LDT_SEL,		P_PID + 4
.set	P_LDT,			P_LDT_SEL + 4

.set	TSS3_S_SP0,		4

.set	SELECTOR_FLAT_C,	0x08
.set	SELECTOR_TSS,		0x20
.set	SELECTOR_KERNEL_CS,	SELECTOR_FLAT_C

.set	INT_M_CTL,		0x20
.set	INT_M_CTLMASK,	0x21
.set	INT_S_CTL,		0xa0
.set	INT_S_CTLMASK,	0xa1
.set	EOI,			0x20

.set	SYSVEC,			0x32

.set	KERNEL,			-1

