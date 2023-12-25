# This file is written by Ermia Ghaffari and Teoman. 
# Any coping and pasting is not allowed without by naming the source
.macro    PUSH reg
  addi    $sp,$sp,-4
  sw    \reg,0($sp)
.endm

.macro    POP reg
  lw    \reg,0($sp)
  addi    $sp,$sp,4
.endm



.global delay
.global enableInterrupt



.text


  delay:
     ble   $a0, $0, delayed    # While 0 <= a0, otherwise branch to delayed (when a0 (ms) < 0)
     nop
     addi  $a0, $a0,-1         #a0--  (This should not be running aftrer for loop is done)
     
     add   $t1, $0, $0         #Put  $t1 to 0  (loading variable)
     li    $t0, 4500           # Loading constant for ms in$t0
  
      
 dL: ble    $t0, $t1, delay  # while t1 (i) < t0 we go down, otherwise branch to delayed
     addi   $t1, $t1, 1      #t++
     nop
     j      dL     
     nop
  
  
  delayed:
      jr   $ra   // End of the function
      nop
      

enableInterrupt:  # Enbaling all the intrupts 
            ei
            jr  $ra
            nop 
            
             

            
