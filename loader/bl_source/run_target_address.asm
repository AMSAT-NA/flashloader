;-------------------------------------------------------------------------------
; run_target_address.asm
;
; Heimir Thor Sverrisson, w1ant, heimir.sverrisson@gmail.com
;
; Define an integer at start of SRAM to store the run target
;
;-------------------------------------------------------------------------------
; export reference
    .def run_target

; Target to run after reset
;
; 0x55555555 	Means enter Flashloader monitor
; 0xAAAAAAAA	Means enter Application (if loaded)
; other values	Unknown
;
run_target  .usect ".runTarget", 4
