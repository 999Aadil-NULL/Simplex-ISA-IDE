; Unique Computation Sequence for SIMPLEX
; Final result expected in A: 50

    adj -5          ; Create a small safety buffer on the stack
    
    ; --- Stage 1: Base Addition ---
    ldc 10          ; A = 10, B = 0
    adc 5           ; A = 15, B = 10 (Immediate addition)
    
    ; --- Stage 2: Register Addition ---
    ldc 20          ; A = 20, B = 15
    add             ; A = B + A = 15 + 20 = 35
    
    ; --- Stage 3: Subtraction ---
    ldc 10          ; A = 10, B = 35
    sub             ; A = B - A = 35 - 10 = 25
    
    ; --- Stage 4: Logical Bit-Shifting ---
    ldc 2           ; A = 2, B = 25
    shl             ; A = B << A = 25 << 2 = 100
    
    ldc 1           ; A = 1, B = 100
    shr             ; A = B >> A = 100 >> 1 = 50

    HALT            ; Result 50 remains in Accumulator A