start:
    ldc 10      ; Load 10 into Accumulator A
    adc 5       ; Add 5 (A = 15)
    br next     ; Branch to the label 'next'
    
next:
    ldc 0       ; Just a dummy instruction
    ; NO HALT HERE - The PC will keep incrementing until 
    ; it hits the MEM_SIZE (16384) and triggers your SEGFAULT.