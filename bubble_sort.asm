; Optimized Bubble Sort for SIMPLEX
; Sorts 'arr' in ascending order

init:
    adj -7          ; Reserve 4 slots on stack for locals
    ldc arr         
    stl 3           ; Local 3: Array Base Address
    ldc 5           
    stl 2           ; Local 2: N (Array size)
    ldc 0           
    stl 1           ; Local 1: i (Outer counter)

outer:
    ldc 0           
    stl 0           ; Local 0: j (Inner counter)

inner:
    ; Calculate current element pointer (p = arr + j)
    ldl 3           
    ldl 0           
    add             
    stl 4           ; Temporary Local 4: p

    ; Load arr[j] and arr[j+1]
    ldl 4           
    ldnl 0          
    stl 5           ; Local 5: arr[j]
    ldl 4           
    ldnl 1          
    stl 6           ; Local 6: arr[j+1]

    ; Comparison: if arr[j] <= arr[j+1] then skip swap
    ldl 5           
    ldl 6           
    sub             ; A = arr[j] - arr[j+1]
    brlz noswap     ; Jump if arr[j] < arr[j+1]
    brz noswap      ; Jump if arr[j] == arr[j+1] 

    ; Perform Swap 
    ldl 6           ; Load arr[j+1]
    ldl 4           ; Load p
    stnl 0          ; memory[p] = arr[j+1]
    ldl 5           ; Load arr[j]
    ldl 4           ; Load p
    stnl 1          ; memory[p+1] = arr[j]

noswap:
    ldl 0           
    adc 1           
    stl 0           ; j++

    ; Inner loop condition: j < (N - 1)
    ldl 0           
    ldl 2           
    adc -1          ; N-1
    sub             
    brlz inner      ; Repeat if j < N-1

    ldl 1           
    adc 1           
    stl 1           ; i++
    
    ; Outer loop condition: i < N
    ldl 1           
    ldl 2           
    sub             
    brlz outer      ; Repeat if i < N

    adj 7          ; Clean up stack
    HALT            ; Stop emulator

arr:
    data 45         ; Test values
    data 12
    data 89
    data 34
    data 2