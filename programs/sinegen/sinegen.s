.text
.globl main
main:
    li      t1, 0x100ff
    addi    a0, zero, 0x0       # a0 is used for output
mloop:     
    li      a1, 0x10000         # a1 is the counter, init to base address of sine data.
iloop:
    lbu     a0, 0(a1)           # load a0 dmem[a1]
    addi    a1, a1, 1           # increment a1
    bne     a1, t1, iloop       # if a1 = 255, branch to iloop
    bne     t1, zero, mloop     #  ... else always brand to mloop
    