#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
	if(cpu.ZF == 1) {
        if((op_src->val & (1 << (DATA_BYTE * 8 - 1))) >> (DATA_BYTE * 8 - 1) == 1) {
            cpu.eip = cpu.eip - ((~op_src->val) + 1);
            printf("aaaaaaaaaa\n");
        }
        else {
            cpu.eip = cpu.eip + op_src->val;
        }
        if(DATA_BYTE == 2) {
            cpu.eip = cpu.eip & 0x0000ffff;
            printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
        }
    }
    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"