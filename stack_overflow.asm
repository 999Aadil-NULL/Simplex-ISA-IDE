ldc 16383   ; Load maximum valid index
a2sp        ; SP = 16383
ldc 99      ; Dummy value
stl 1       ; Target = 16383 + 1 = 16384 (OUT OF BOUNDS)
HALT