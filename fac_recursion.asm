; Factorial Loop: 5! = 120 (0x78)

    ldc 1000    ; Initialize Stack Pointer to a safe area
    a2sp        ; SP = 1000

    ldc 5
    stl 0       ; n (at SP+0) = 5
    ldc 1
    stl 1       ; result (at SP+1) = 1

fact_loop:
    ldl 0       ; Load n
    brz done    ; if n == 0, break to done
    
    ; Multiplication Logic (result * n)
    ldl 1       
    stl 2       ; temp_res (at SP+2) = result
    ldc 0
    stl 3       ; new_res (at SP+3) = 0
    ldl 0       
    stl 4       ; counter (at SP+4) = n
    
mult_loop:
    ldl 4       ; Check counter
    brz mult_done
    
    ldl 3       ; A = new_res
    ldl 2       ; B = temp_res
    add         ; A = new_res + temp_res
    stl 3       ; update new_res
    
    ldl 4       
    adc -1      ; counter--
    stl 4
    br mult_loop
    
mult_done:
    ldl 3       ; Load calculated product
    stl 1       ; result = new_res
    
    ldl 0       
    adc -1      ; n--
    stl 0
    br fact_loop

done:
    ldl 1       ; Final result (120) loaded into A for verification
    HALT