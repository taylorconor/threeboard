	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 14	sdk_version 10, 14
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	leaq	-8(%rbp), %rdi
	callq	__ZN8MyStructC1Ev
	leaq	-8(%rbp), %rdi
	callq	__ZN8MyStruct6MyFuncEv
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
                                        ## -- End function
	.globl	__ZN8MyStructC1Ev       ## -- Begin function _ZN8MyStructC1Ev
	.weak_def_can_be_hidden	__ZN8MyStructC1Ev
	.p2align	4, 0x90
__ZN8MyStructC1Ev:                      ## @_ZN8MyStructC1Ev
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	callq	__ZN8MyStructC2Ev
	addq	$16, %rsp
	popq	%rbp
	retq
                                        ## -- End function
	.globl	__ZN8MyStruct6MyFuncEv  ## -- Begin function _ZN8MyStruct6MyFuncEv
	.weak_def_can_be_hidden	__ZN8MyStruct6MyFuncEv
	.p2align	4, 0x90
__ZN8MyStruct6MyFuncEv:                 ## @_ZN8MyStruct6MyFuncEv
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	%rdi, -8(%rbp)
	popq	%rbp
	retq
                                        ## -- End function
	.globl	__ZN8MyStructC2Ev       ## -- Begin function _ZN8MyStructC2Ev
	.weak_def_can_be_hidden	__ZN8MyStructC2Ev
	.p2align	4, 0x90
__ZN8MyStructC2Ev:                      ## @_ZN8MyStructC2Ev
## %bb.0:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	__ZTV8MyStruct@GOTPCREL(%rip), %rax
	addq	$16, %rax
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movq	%rax, (%rdi)
	popq	%rbp
	retq
                                        ## -- End function
	.section	__DATA,__const
	.globl	__ZTV8MyStruct          ## @_ZTV8MyStruct
	.weak_def_can_be_hidden	__ZTV8MyStruct
	.p2align	3
__ZTV8MyStruct:
	.quad	0
	.quad	0
	.quad	__ZN8MyStruct6MyFuncEv


.subsections_via_symbols
