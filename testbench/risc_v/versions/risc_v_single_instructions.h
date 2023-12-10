#include <risc_v.h>
#include <verilated.h>
#include "verilated_vcd_c.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

enum class RiscVRegisters : uint32_t {
    zero, ra, sp, gp, tp, t0, t1, t2, \
    s0, s1, a0, a1, a2, a3, a4, a5, \
    a6, a7, s2, s3, s4, s5, s6, s7, \
    s8, s9, s10, s11, t3, t4, t5, t6
};

class RiscVTest: public ::testing::Test {
protected:
    risc_v * top;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    const uint32_t simcyc = 10'000'000;
    uint32_t curr_cyc = 0;

    void assert_reg(RiscVRegisters reg, uint32_t value) {
        top->address_to_view = static_cast<uint32_t>(reg);
        top->eval();
        EXPECT_EQ(top->reg_output, value);
    }

    void clock_tick(void)
    {
        // Make sure the tickcount is greater than zero before
        // we do this
        curr_cyc++;

        // Allow any combinatorial logic to settle before we tick
        // the clock.  This becomes necessary in the case where
        // we may have modified or adjusted the inputs prior to
        // coming into here, since we need all combinatorial logic
        // to be settled before we call for a clock tick.
        //
        top->CLK = 0;
        top->eval();

        //
        // Here's the new item:
        //
        //	Dump values to our trace file
        //
        if (tfp)
            tfp->dump(10 * curr_cyc - 2);

        // Repeat for the positive edge of the clock
        top->CLK = 1;
        top->eval();
        if (tfp)
            tfp->dump(10 * curr_cyc);

        // Now the negative edge
        top->CLK = 0;
        top->eval();
        if (tfp)
        {
            // This portion, though, is a touch different.
            // After dumping our values as they exist on the
            // negative clock edge ...
            tfp->dump(10 * curr_cyc + 5);
            //
            // We'll also need to make sure we flush any I/O to
            // the trace file, so that we can use the assert()
            // function between now and the next tick if we want to.
            tfp->flush();
        }
    }

    void n_clock_ticks(int N = 1) {
        for (int i = 1; i <= N; i++) {
            clock_tick();
        }
    }

    // Sends a synchronous reset pulse.
    void reset(void)
    {
        top->RST = 1;
        this->clock_tick();
        top->RST = 0;
    }

    void SetUp( ) {
    top = new risc_v;
    Verilated::traceEverOn(true);
    }

    void TearDown( ) {
        top->final();
        if(tfp) {
            tfp->close();
            tfp = NULL;
        }
        delete top;
    }

    void set_tfp(std::string filename) {
        top->trace (tfp, 99);
        tfp->open (filename.c_str());
    }
};

TEST_F(RiscVTest, LW) {
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/i-type/lw");
    set_tfp("risc_v_lw.vcd");
    reset();

    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a0, 0xFFFFFFFF);

    n_clock_ticks(3);
}

TEST_F(RiscVTest, ADDI) {
    // read the instruction memory
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/i-type/addi");
    set_tfp("risc_v_addi.vcd");
    reset();

    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 5);
    n_clock_ticks(1);
}

TEST_F(RiscVTest, BEQ) {
// read the instruction memory
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/b-type/beq");
    set_tfp("risc_v_beq.vcd");
    reset();

    // check lw worked
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 1);

    // check the second lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 2);
    
    //check the beq not taken
    n_clock_ticks(1);
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    
    // check next lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 1);
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 1);

    // check the beq taken.
    n_clock_ticks(1); // do the beq
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    n_clock_ticks(1);
}

TEST_F(RiscVTest, BNE) {
// read the instruction memory
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/b-type/bne");
    set_tfp("risc_v_bne.vcd");
    reset();

    // check lw worked
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 1);

    // check the second lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 1);
    
    //check the bne not taken
    n_clock_ticks(1);
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    
    // check next lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 1);
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 2);

    // check the bne taken.
    n_clock_ticks(1); // do the bne
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    n_clock_ticks(1);
}

TEST_F(RiscVTest, BGE) {
// read the instruction memory
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/b-type/bge");
    set_tfp("risc_v_bge.vcd");
    reset();

    // check lw worked
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 1);

    // check the second lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 2);
    
    //check the bge not taken
    n_clock_ticks(1);
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    
    // check next lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 2);
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 0xFFFFFFFF);

    // check the bge taken.
    n_clock_ticks(1); // do the bge
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    n_clock_ticks(1);
}

TEST_F(RiscVTest, BGEU) {
// read the instruction memory
    int reg = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/b-type/bgeu");
    set_tfp("risc_v_bgeu.vcd");
    reset();

    // check lw worked
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 1);

    // check the second lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 2);
    
    //check the bgeu not taken
    n_clock_ticks(1);
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    
    // check next lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 0xFFFFFFFF);
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 1);

    // check the bgeu taken.
    n_clock_ticks(1); // do the bgeu
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    n_clock_ticks(1);
}

TEST_F(RiscVTest, BLT) {
// read the instruction memory
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/b-type/blt");
    set_tfp("risc_v_blt.vcd");
    reset();

    // check lw worked
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 2);

    // check the second lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 1);
    
    //check the bge not taken
    n_clock_ticks(1);
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    
    // check next lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 0xFFFFFFFF);
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 2);

    // check the bge taken.
    n_clock_ticks(1); // do the bge
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    n_clock_ticks(1);
}

TEST_F(RiscVTest, BLTU) {
// read the instruction memory
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/b-type/bltu");
    set_tfp("risc_v_bltu.vcd");
    reset();

    // check lw worked
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 2);

    // check the second lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 1);
    
    //check the bgeu not taken
    n_clock_ticks(1);
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    
    // check next lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 1);
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 0xFFFFFFFF);

    // check the bgeu taken.
    n_clock_ticks(1); // do the bgeu
    ASSERT_EQ(top->pc_viewer, 0xBFC0000C);
    n_clock_ticks(1);
}

// Test the add instruction
// We know that addi, lw works.
TEST_F(RiscVTest, ADD) {
// read the instruction memory
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/r-type/add");
    set_tfp("risc_v_add.vcd");
    reset();

    // check lw worked
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 1);

    // check the second lw
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a3, 2);
    
    //check the add worked.
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 3);

    // Test overflow:
    // lw big numbers from data memory.
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a2, 0xFFFFFFFF);

    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a3, 0x00000001);

    // check the add worked.
    n_clock_ticks(1);
    assert_reg(RiscVRegisters::a1, 0x00000000);
    n_clock_ticks(5);
}

TEST_F(RiscVTest, SUB) {
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/r-type/sub");
    set_tfp("risc_v_sub.vcd");
    reset();
    
    // load first two numbers
    // and check the sub worked.
    n_clock_ticks(3);
    assert_reg(RiscVRegisters::a1, 1);
    

    // Test overflow:
    // check the sub worked.
    n_clock_ticks(3);
    assert_reg(RiscVRegisters::a1, -5);
    n_clock_ticks(5);
}

TEST_F(RiscVTest, SLL) {
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/r-type/sll");
    set_tfp("risc_v_sll.vcd");
    reset();
    
    // load first two operands
    // and check the operation worked.
    n_clock_ticks(3);
    assert_reg(RiscVRegisters::a1, 0x118);
    
    n_clock_ticks(3);
    assert_reg(RiscVRegisters::a1, 0x98);
    n_clock_ticks(5);
}


TEST_F(RiscVTest, AND) {
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/r-type/and");
    set_tfp("risc_v_and.vcd");
    reset();
    std::vector<uint32_t> expected_results = {0xFFFFFFFF, 0x02244220, 0x8000821A, 0x04240002,
                                              0x105080A1};
    n_clock_ticks(1);

    for(int i = 0; i < 5; i++) {
        n_clock_ticks(5);
        assert_reg(RiscVRegisters::a1, expected_results[i]);
        n_clock_ticks(2);
    }
}

TEST_F(RiscVTest, OR) {
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/r-type/or");
    set_tfp("risc_v_or.vcd");
    reset();
    std::vector<uint32_t> expected_results = {0xFFFFFFFF, 0x736DF97D, 0xBF73AFF3, 0x3F3EF2B9, 0xFE7AFFFF, 0x8FB78CFE};
    n_clock_ticks(1);

    for(int i = 0; i < 6; i++) {
        n_clock_ticks(5);
        assert_reg(RiscVRegisters::a1, expected_results[i]);
        n_clock_ticks(2);
    }
}

TEST_F(RiscVTest, XOR) {
    int ret = system("make -C ../ assemble PROGRAM_NAME=single_instruction_tests/r-type/xor");
    set_tfp("risc_v_xor.vcd");
    reset();
    std::vector<uint32_t> expected_results = {0x0, 0x95511559, 0x6433891B, 0x7B05859D, 0x87DF334E, 0xF4976244, 0xE0B20A37};
    n_clock_ticks(1);

    for(int i = 0; i < 6; i++) {
        n_clock_ticks(5);
        assert_reg(RiscVRegisters::a1, expected_results[i]);
        n_clock_ticks(2);
    }
}

int main(int argc, char **argv) {
    std::cout << "Verilated Command Args" << std::endl;
    Verilated::commandArgs(argc, argv);
    std::cout << "Init Google Test" << std::endl;
    testing::InitGoogleTest(&argc, argv);
    std::cout << "Run All Tests" << std::endl;
    auto res = RUN_ALL_TESTS();
    std::cout << "Making Logs Directory" << std::endl;
    Verilated::mkdir("logs");

    std::cout << "Write Coverage" << std::endl;
    //VerilatedCov::write("logs/coverage_risc_v.dat");
    return res;
}
