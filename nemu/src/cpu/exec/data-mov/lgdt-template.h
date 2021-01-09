#include "cpu/exec/template-start.h"

#define instr lgdt

static void do_execute() {
	swaddr_t gdt_addr_ptr = op_src->addr;
    cpu.gdtr.limit = lnaddr_read(gdt_addr_ptr, 2);
    gdt_addr_ptr += 2;
#if DATA_BYTE == 2
    cpu.gdtr.base = lnaddr_read(gdt_addr_ptr, 3);
#endif
#if DATA_BYTE == 4
    cpu.gdtr.base = lnaddr_read(gdt_addr_ptr, 4);
#endif
    print_asm_template1();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
