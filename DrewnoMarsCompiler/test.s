.data
blah: .quad 0
.text
	cmpq %rbp, (blah)
