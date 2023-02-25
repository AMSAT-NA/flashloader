# Exception Handling

In order to switch handling of exceptions from the Flashloader to an application that has been loaded, interrupt service handler addresses cannot be stored in Flash.

## Memory structure

When an exception happens on the ARM processor control is transferred to addresses in the
bottom of the address space. The `TMS570` processors have flash memory starting at address
`0x00000000` and in case of the `'0914` we have 1 MByte of flash, up to address `0x00100000`.

SRAM on the `'0914` starts at the 128 MByte boundary, at address `0x08000000` and is 128 kBytes
covering up to address `0x08020000`.

## Interrupt Vectors in SRAM

When a single program is running on the SoC it is fine to have interrupt vectors contain branch
commands into service routines in flash memory.

In case of a program loaded with a Flashloader the situation gets complicated. When there is
no application loaded, exceptions must be handled by the Flashloader code, but as soon as we
load one, the interrupt vectors should branch to code in the application, not the Flashloader.

Now we cannot have the interrupt vectors hardwired anymore. The solution to this problem
chosen here is to create two jump tables, one which is permanent in flash in low memory
and another one that is dynamic in SRAM in upper memory.

### Low memory contents

Let's take a look at the code needed at address `0x00000000`.

```assembly
resetEntry:     b   _c_int00        ; Power-On-Reset Vector, is a constant target
undefEntry:     ldr pc, tab_undef   ; Load program counter from Flash table
swiEntry:       ldr pc, tab_swi
prefetchEntry:  ldr pc, tab_pref
dabtEntry:      ldr pc, tab_dabt
phantomEntry:   b   phantomEntry    ; Endless Loop, reserved entry should never be hit
irqEntry:       ldr pc,[pc,#-0x1b0] ; Load from VIM, no need to relocate handled by VIM
fiqEntry:       ldr pc,[pc,#-0x1b0] ; Load from VIM, no need to relocate handled by VIM
```

Each entry can only take up a single instruction (32 bits) by the ARM architecture.
The first entry, `resetEntry`, does a simple unconditional branch to the initialization
routine, `_c_int00`. This path is executed on Power-On-Reset condition and sets up the
hardware followed by a call to the `main()` function of the Flashloader.

An important addition to the `_c_int00` function is to set up the rest of the interrupt
handling, as described below.

The the four following interrupt vectors dispatch indirectly to an address in SRAM
by loading a 32-bit address straight into the Program Counter from the table below:

```assembly
tab_undef:      .word ram_undef
tab_swi:        .word ram_swi
tab_pref:       .word ram_pref
tab_dabt:       .word ram_dabt
```

The values with the `ram_` prefix refer to addresses at the very top of the 128
kByte SRAM. Thereby branching through to an address more than 128 MByte away.
This pc load trick is needed, because the ARM instruction set only allows
for a 24-bit (+/- 8 MByte) offset for regular relative branch instructions.

### High memory contents

The addresses referenced in low flash memory as described above are set up to
contain the following code, with instructions added to the `_c_int00` function
which is called on hard reset in the Flashloader and is the entry point for
the actual application, located at `0x00010100`.

```assembly
ram_undef:      ldr pc, ram_tab_undef
ram_swi:        ldr pc, ram_tab_swi
ram_pref:       ldr pc, ram_tab_pref
ram_dabt:       ldr pc, ram_tab_dabt
```

Again the pc loader trick is used to indirectly jump into the correct interrupt
handler via the `ram_tab` entries, back into the correct routine in flash!

```assembly
ram_tab_undef:  .word undef_handler
ram_tab_swi:    .word swi_handler
ram_tab_pref:   .word pabt_handler
ram_tab_dabt:   .word _dabort
```

Now the addresses stored in the table are addresses of the actual service routines
put there by the linker at compile time.

## Conclusion

By creating two jump tables, one in flash and another in SRAM, interrupt service routines
can switch between going to code in the Flashloader and the actual loaded application.

For a more generic discussion see [Application Note spna236.pdf](spna236.pdf)
