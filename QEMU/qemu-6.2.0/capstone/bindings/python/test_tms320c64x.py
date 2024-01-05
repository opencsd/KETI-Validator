#!/usr/bin/env python

# Capstone Python bindings, by Fotis Loukos <me@fotisl.com>

from __future__ import print_function
from capstone import *
from capstone.tms320c64x import *
from xprint import to_x, to_hex, to_x_32


TMS320C64X_CODE = b"\x01\xac\x88\x40\x81\xac\x88\x43\x00\x00\x00\x00\x02\x90\x32\x96\x02\x80\x46\x9e\x05\x3c\x83\xe6\x0b\x0c\x8b\x24"

all_tests = (
        (CS_ARCH_TMS320C64X, 0, TMS320C64X_CODE, "TMS320C64x"),
)


def print_insn_detail(insn):
    # print address, mnemonic and operands
    print("0x%x:\t%s\t%s" % (insn.address, insn.mnemonic, insn.op_str))

    # "data" instruction generated by SKIPDATA option has no detail
    if insn.id == 0:
        return

    if len(insn.operands) > 0:
        print("\top_count: %u" % len(insn.operands))
        c = 0
        for i in insn.operands:
            if i.type == TMS320C64X_OP_REG:
                print("\t\toperands[%u].type: REG = %s" % (c, insn.reg_name(i.reg)))
            if i.type == TMS320C64X_OP_IMM:
                print("\t\toperands[%u].type: IMM = 0x%s" % (c, to_x(i.imm)))
            if i.type == TMS320C64X_OP_MEM:
                print("\t\toperands[%u].type: MEM" % c)
                if i.mem.base != 0:
                    print("\t\t\toperands[%u].mem.base: REG = %s" \
                        % (c, insn.reg_name(i.mem.base)))
                if i.mem.disptype == TMS320C64X_MEM_DISP_INVALID:
                    print("\t\t\toperands[%u].mem.disptype: Invalid" % (c))
                    print("\t\t\toperands[%u].mem.disp: 0x%s" \
                        % (c, to_x(i.mem.disp)))
                if i.mem.disptype == TMS320C64X_MEM_DISP_CONSTANT:
                    print("\t\t\toperands[%u].mem.disptype: Constant" % (c))
                    print("\t\t\toperands[%u].mem.disp: 0x%s" \
                        % (c, to_x(i.mem.disp)))
                if i.mem.disptype == TMS320C64X_MEM_DISP_REGISTER:
                    print("\t\t\toperands[%u].mem.disptype: Register" % (c))
                    print("\t\t\toperands[%u].mem.disp: %s" \
                        % (c, insn.reg_name(i.mem.disp)))
                print("\t\t\toperands[%u].mem.unit: %u" % (c, i.mem.unit))
                if i.mem.direction == TMS320C64X_MEM_DIR_INVALID:
                    print("\t\t\toperands[%u].mem.direction: Invalid" % (c))
                if i.mem.direction == TMS320C64X_MEM_DIR_FW:
                    print("\t\t\toperands[%u].mem.direction: Forward" % (c))
                if i.mem.direction == TMS320C64X_MEM_DIR_BW:
                    print("\t\t\toperands[%u].mem.direction: Backward" % (c))
                if i.mem.modify == TMS320C64X_MEM_MOD_INVALID:
                    print("\t\t\toperands[%u].mem.modify: Invalid" % (c))
                if i.mem.modify == TMS320C64X_MEM_MOD_NO:
                    print("\t\t\toperands[%u].mem.modify: No" % (c))
                if i.mem.modify == TMS320C64X_MEM_MOD_PRE:
                    print("\t\t\toperands[%u].mem.modify: Pre" % (c))
                if i.mem.modify == TMS320C64X_MEM_MOD_POST:
                    print("\t\t\toperands[%u].mem.modify: Post" % (c))
                print("\t\t\toperands[%u].mem.scaled: %u" % (c, i.mem.scaled))
            if i.type == TMS320C64X_OP_REGPAIR:
                print("\t\toperands[%u].type: REGPAIR = %s:%s" % (c, insn.reg_name(i.reg + 1), insn.reg_name(i.reg)))
            c += 1


# ## Test class Cs
def test_class():

    for (arch, mode, code, comment) in all_tests:
        print("*" * 16)
        print("Platform: %s" %comment)
        print("Code: %s" % to_hex(code))
        print("Disasm:")

        try:
            md = Cs(arch, mode)
            md.detail = True
            for insn in md.disasm(code, 0x1000):
                print_insn_detail(insn)
                print ()
            print("0x%x:\n" % (insn.address + insn.size))
        except CsError as e:
            print("ERROR: %s" %e)


if __name__ == '__main__':
    test_class()
