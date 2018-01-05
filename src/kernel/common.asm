[global disable_interrupts]
disable_interrupts:
    cli
    ret

[global enable_interrupts]
enable_interrupts:
    sti
    ret

[global int_wait]
int_wait:
    hlt
    ret
