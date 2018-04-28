SECTION .TEXT
    GLOBAL get_bareflank_status 

get_bareflank_status:
	mov eax, 0x1
	cpuid
	shr eax, 30
	cmp eax, 0
	jmp _fail
	mov rax, 0x1
	ret

_fail:
	mov rax, 0x0
	ret
