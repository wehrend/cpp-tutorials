	.file	"main.cpp"
	.text
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.section	.rodata
	.align 8
	.type	_ZL11SAMPLE_RATE, @object
	.size	_ZL11SAMPLE_RATE, 8
_ZL11SAMPLE_RATE:
	.long	0
	.long	1088909312
	.align 8
	.type	_ZL9FREQUENCY, @object
	.size	_ZL9FREQUENCY, 8
_ZL9FREQUENCY:
	.long	0
	.long	1081835520
	.align 8
	.type	_ZL6TWO_PI, @object
	.size	_ZL6TWO_PI, 8
_ZL6TWO_PI:
	.long	1413754136
	.long	1075388923
	.globl	phase
	.bss
	.align 8
	.type	phase, @object
	.size	phase, 8
phase:
	.zero	8
	.text
	.globl	_Z13audioCallbackPvPhi
	.type	_Z13audioCallbackPvPhi, @function
_Z13audioCallbackPvPhi:
.LFB7174:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movl	%edx, -52(%rbp)
	movq	-48(%rbp), %rax
	movq	%rax, -16(%rbp)
	movl	-52(%rbp), %eax
	cltq
	shrq	$2, %rax
	movl	%eax, -20(%rbp)
	movsd	.LC0(%rip), %xmm0
	movsd	%xmm0, -32(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L2
.L5:
	movq	phase(%rip), %rax
	movq	%rax, %xmm0
	call	sin@PLT
	pxor	%xmm1, %xmm1
	cvtsd2ss	%xmm0, %xmm1
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movss	.LC1(%rip), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, (%rax)
	movsd	phase(%rip), %xmm0
	addsd	-32(%rbp), %xmm0
	movsd	%xmm0, phase(%rip)
	movsd	phase(%rip), %xmm0
	comisd	.LC2(%rip), %xmm0
	jb	.L3
	movsd	phase(%rip), %xmm0
	movsd	.LC2(%rip), %xmm1
	subsd	%xmm1, %xmm0
	movsd	%xmm0, phase(%rip)
.L3:
	addl	$1, -4(%rbp)
.L2:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L5
	nop
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7174:
	.size	_Z13audioCallbackPvPhi, .-_Z13audioCallbackPvPhi
	.section	.rodata
.LC3:
	.string	"SDL_Init failed: "
.LC4:
	.string	"\n"
.LC5:
	.string	"Audio open failed: "
.LC6:
	.string	"Playing 440 Hz tone...\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB7175:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset 3, -24
	movl	$16, %edi
	call	SDL_Init@PLT
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	.L8
	leaq	.LC3(%rip), %rax
	movq	%rax, %rsi
	leaq	_ZSt4cerr(%rip), %rax
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movq	%rax, %rbx
	call	SDL_GetError@PLT
	movq	%rax, %rsi
	movq	%rbx, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movq	%rax, %rdx
	leaq	.LC4(%rip), %rax
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movl	$1, %eax
	jmp	.L11
.L8:
	pxor	%xmm0, %xmm0
	movaps	%xmm0, -64(%rbp)
	movaps	%xmm0, -48(%rbp)
	movl	$48000, -64(%rbp)
	movw	$-32480, -60(%rbp)
	movb	$1, -58(%rbp)
	movw	$512, -56(%rbp)
	leaq	_Z13audioCallbackPvPhi(%rip), %rax
	movq	%rax, -48(%rbp)
	pxor	%xmm0, %xmm0
	movaps	%xmm0, -96(%rbp)
	movaps	%xmm0, -80(%rbp)
	leaq	-96(%rbp), %rdx
	leaq	-64(%rbp), %rax
	movl	$0, %r8d
	movq	%rdx, %rcx
	movq	%rax, %rdx
	movl	$0, %esi
	movl	$0, %edi
	call	SDL_OpenAudioDevice@PLT
	movl	%eax, -20(%rbp)
	cmpl	$0, -20(%rbp)
	jne	.L10
	leaq	.LC5(%rip), %rax
	movq	%rax, %rsi
	leaq	_ZSt4cerr(%rip), %rax
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movq	%rax, %rbx
	call	SDL_GetError@PLT
	movq	%rax, %rsi
	movq	%rbx, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movq	%rax, %rdx
	leaq	.LC4(%rip), %rax
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movl	$1, %eax
	jmp	.L11
.L10:
	leaq	.LC6(%rip), %rax
	movq	%rax, %rsi
	leaq	_ZSt4cout(%rip), %rax
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movl	-20(%rbp), %eax
	movl	$0, %esi
	movl	%eax, %edi
	call	SDL_PauseAudioDevice@PLT
	movl	$3000, %edi
	call	SDL_Delay@PLT
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	SDL_CloseAudioDevice@PLT
	call	SDL_Quit@PLT
	movl	$0, %eax
.L11:
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7175:
	.size	main, .-main
	.type	_Z41__static_initialization_and_destruction_0ii, @function
_Z41__static_initialization_and_destruction_0ii:
.LFB7816:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$1, -4(%rbp)
	jne	.L14
	cmpl	$65535, -8(%rbp)
	jne	.L14
	leaq	_ZStL8__ioinit(%rip), %rax
	movq	%rax, %rdi
	call	_ZNSt8ios_base4InitC1Ev@PLT
	leaq	__dso_handle(%rip), %rax
	movq	%rax, %rdx
	leaq	_ZStL8__ioinit(%rip), %rax
	movq	%rax, %rsi
	movq	_ZNSt8ios_base4InitD1Ev@GOTPCREL(%rip), %rax
	movq	%rax, %rdi
	call	__cxa_atexit@PLT
.L14:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7816:
	.size	_Z41__static_initialization_and_destruction_0ii, .-_Z41__static_initialization_and_destruction_0ii
	.type	_GLOBAL__sub_I_phase, @function
_GLOBAL__sub_I_phase:
.LFB7817:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$65535, %esi
	movl	$1, %edi
	call	_Z41__static_initialization_and_destruction_0ii
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7817:
	.size	_GLOBAL__sub_I_phase, .-_GLOBAL__sub_I_phase
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I_phase
	.section	.rodata
	.align 8
.LC0:
	.long	-1948967675
	.long	1068334388
	.align 4
.LC1:
	.long	1008981770
	.align 8
.LC2:
	.long	1413754136
	.long	1075388923
	.hidden	__dso_handle
	.ident	"GCC: (Debian 12.2.0-14+deb12u1) 12.2.0"
	.section	.note.GNU-stack,"",@progbits
