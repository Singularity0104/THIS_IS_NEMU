#include "cpu/exec/template-start.h"

#define instr ja

static void do_execute() {
	if(cpu.ZF == 1 && cpu.CF == 0) {
        DATA_TYPE_S offset = op_src->val;
        cpu.eip = cpu.eip + offset;
        if(DATA_BYTE == 2) {
            cpu.eip = cpu.eip & 0x0000ffff;
        }
    }
    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
