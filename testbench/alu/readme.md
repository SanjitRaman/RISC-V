# Test Methodology for ALU

## Test Environment
In this file, [Google Test](/testbench/readme.md) is used to implement unit tests for the `alu` module. 

## Test Cases

The following testcases are calculated by hand based on the specification of the ALU module found [here](/rtl/alu/readme.md).

| Testcase Name | Operation                  | Expected ALUResult | Expected Zero | Expected N | Expected C | Expected V |
|--------------|----------------------------|--------------------|---------------|------------|------------|------------|
| ADDFlags0    | 0x0000003F + 0x0000014A    | 0x00000189         | 0b0           | 0b0        | 0b0        | 0b0        |
| ADDFlags1    | 0xFFFFFFFF + 0xFFFFFFFF    | 0xFFFFFFFE         | 0b0           | 0b1        | 0b1        | 0b0        |
| SUBFlags0    | 0x000037D8 - 0x0000025B    | 0x0000357D         | 0b0           | 0b0        | 0b0        | 0b0        |
| SUBFlags1    | 0xFFFFF021 - 0xFFFFF021    | 0x00000000         | 0b1           | 0b0        | 0b0        | 0b1        |
| SLL          | 0x000037D8 << 3            | 0x0001BEC0         | 0b0           | 0b0        | 0b0        | 0b0        |
| SLTGPP       | 0x000037D8 > 0x0000025B    | 0x00000000         | 0b1           | 0b0        | 0b0        | 0b0        |
| SLTGPN       | 0x29AB0E38 > 0xE010F021    | 0x00000000         | 0b1           | 0b0        | 0b0        | 0b0        |
| SLTGNN       | 0xFFFFFFFF > 0xF01FF021    | 0x00000000         | 0b1           | 0b0        | 0b0        | 0b1        |
| SLTLPP       | 0x0000025B < 0x00007901    | 0x00000001         | 0b0           | 0b0        | 0b0        | 0b0        |
| SLTLNP       | 0xF01FF021 < 0x00AB0E38    | 0x00000001         | 0b0           | 0b0        | 0b0        | 0b0        |
| SLTLNN       | 0xA0AB0E38 < 0xF01FF021    | 0x00000001         | 0b0           | 0b0        | 0b0        | 0b1        |
| SLTU0        | 0x735a1099 < 0x30d2f191    | 0x00000000         | 0b1           | 0b0        | 0b0        | 0b0        |
| SLTU1        | 0x30d2f191 < 0x735a1099    | 0x00000001         | 0b0           | 0b0        | 0b0        | 0b0        |
| XOR          | 0x6047D6D3 ^ 0xD471D368    | 0xB43605BB         | 0b0           | 0b1        | 0b0        | 0b0        |
| SRL          | 0xA020025B >> 8            | 0x00A02002         | 0b0           | 0b0        | 0b0        | 0b0        |
| SRA0         | 0x030D37D8 >> 5            | 0x001869BE         | 0b0           | 0b0        | 0b0        | 0b0        |
| SRA1         | 0xF0000000 >> 4            | 0xFF000000         | 0b0           | 0b1        | 0b0        | 0b0        |
| OR           | 0x030D37D8 \| 0xA020025B    | 0xA32D37DB         | 0b0           | 0b1        | 0b0        | 0b0        |
| AND          | 0x29AB0E38 & 0xE010F021    | 0x20000020         | 0b0           | 0b0        | 0b0        | 0b0        |
| LUI          | 0x01234567                 | 0x01234567         | 0b0           | 0b0        | 0b0        | 0b0        |

## Results:

Upon running the command:
`make runtest RUN=module MODULE=alu MODULE.INCLUDE_DIRS="" GTEST=1 VBUDDY=0`

The following output is generated:

![alu_test_output](/images/gtest-results-alu-testbench.png)

With the following code coverage report upon running the command:

`make genhtml RUN=module MODULE=alu MODULE.INCLUDE_DIRS="" GTEST=1 VBUDDY=0`


![alu_test_coverage](/images/code-coverage-alu.png)

With all lines being covered by the test cases.

## Conclusion:
The ALU module is has been verified and passes all the test cases.

## Further Work:
It is not possible to test the ALU module with all the possible combinations of inputs. Hence, it is recommended to test the ALU module with random inputs in a UVM-style methodology to ensure that the module works as expected, defining covergroups to ensure that all functional coverage points are have been sufficiently tested. Given the limited time for this project, this was not possible to implement.