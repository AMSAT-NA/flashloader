;-------------------------------------------------------------------------------
; intvecs.asm
;
; Heimir Thor Sverrisson, w1ant, heimir.sverrisson@gmail.com
;
; Interrupt vectors in SRAM as described in TI Application Note spna236 - March 2017
;
;-------------------------------------------------------------------------------
; import reference for interrupt routines
    .ref _c_int00

	.ref _dabort
	.ref phantomInterrupt

;-------------------------------------------------------------------------------
; export reference
    .def resetEntry

;-------------------------------------------------------------------------------
; interrupt vectors in Flash
    .sect   ".intvecs"
    .retain ".intvecs"
    .arm

resetEntry:     b   _c_int00        ; Reset Vector, thuss constant target
undefEntry:     ldr pc, tab_undef
swiEntry:       ldr pc, tab_swi
prefetchEntry:  ldr pc, tab_pref
dabtEntry:      ldr pc, tab_dabt
phantomEntry:   b   phantomEntry    ; Endless Loop, reserved entry should never been hit, optinaly branch to a "hard fault" handler
irqEntry:       ldr pc,[pc,#-0x1b0] ; Load from VIM, no need to relocate this entry as dispatiching of the interrupts is usually done by the VIM
fiqEntry:       ldr pc,[pc,#-0x1b0] ; Load from VIM, no need to relocate this entry as dispatiching of the interrupts is usually done by the VIM
;-------------------------------------------------------------------------------

; Table with the addresses of the vectors in the SRAM
tab_undef:      .word ram_undef
tab_swi:        .word ram_swi
tab_pref:       .word ram_pref
tab_dabt:       .word ram_dabt
;-------------------------------------------------------------------------------

; Dummy entries to padd memory to 64bytes
dummy1:         .word 0x00000000
dummy2:         .word 0x00000000
dummy3:         .word 0x00000000
dummy4:         .word 0x00000000

; import reference for interrupt routines
    .ref _dabort
    .ref undef_handler
    .ref swi_handler
    .ref pabt_handler
;-------------------------------------------------------------------------------

; export reference
    .def ram_tab_undef
    .def ram_tab_swi
    .def ram_tab_pref
    .def ram_tab_dabt
;-------------------------------------------------------------------------------


; Table in RAM
    .sect   ".ramIntvecs"
    .retain ".ramIntvecs"
    .arm

ram_undef:      ldr pc, ram_tab_undef
ram_swi:        ldr pc, ram_tab_swi
ram_pref:       ldr pc, ram_tab_pref
ram_dabt:       ldr pc, ram_tab_dabt
;-------------------------------------------------------------------------------

; Table with the addresses of the exception handlers
ram_tab_undef:  .word undef_handler
ram_tab_swi:    .word swi_handler
ram_tab_pref:   .word pabt_handler
ram_tab_dabt:   .word _dabort
;-------------------------------------------------------------------------------

