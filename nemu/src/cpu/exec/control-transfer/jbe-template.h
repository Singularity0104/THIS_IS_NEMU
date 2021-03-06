#include "cpu/exec/template-start.h"

#define instr jbe

static void do_execute() {
	if(cpu.ZF == 1 || cpu.CF == 1) {
        // uint32_t s = 0;
        // if((op_src->val & (1 << (DATA_BYTE * 8 - 1))) >> (DATA_BYTE * 8 - 1) == 1) {
        //     s = ((1u << (DATA_BYTE * 8 - 1)) - 1) + (1u << (DATA_BYTE * 8 - 1));
        //     s = ~s;
        // }
        // cpu.eip = cpu.eip + op_src->val + s;
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
