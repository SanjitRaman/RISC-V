module reg_file_m #(
    parameter ADDRESS_WIDTH = 5,
    parameter DATA_WIDTH = 32,
    parameter FUNCT3_WIDTH = 3
)(
    input  logic                            CLK,
    input  logic                            EN,

    input  logic                            RegWriteE,
    input  logic [1:0]                      ResultSrcE,
    input  logic                            MemWriteE, MemReadE,
    input  logic [DATA_WIDTH-1:0]           ALUResultE,
    input  logic [DATA_WIDTH-1:0]           WriteDataE,
    input  logic [ADDRESS_WIDTH-1:0]        RdE,
    input  logic [DATA_WIDTH-1:0]           PCPlus4E, 
    input  logic [FUNCT3_WIDTH-1:0]         funct3E,

    output  logic                            RegWriteM,
    output  logic [1:0]                      ResultSrcM, 
    output  logic                            MemWriteM, MemReadM,
    output  logic [DATA_WIDTH-1:0]           ALUResultM,
    output  logic [DATA_WIDTH-1:0]           WriteDataM,
    output  logic [ADDRESS_WIDTH-1:0]        RdM,
    output  logic [DATA_WIDTH-1:0]           PCPlus4M, 
    output  logic [FUNCT3_WIDTH-1:0]         funct3M


);

always_ff @(posedge CLK) begin
    if (!EN) begin
    RegWriteM    <= RegWriteE;
    ResultSrcM   <= ResultSrcE;
    MemWriteM    <= MemWriteE;
    MemReadM     <= MemReadE;
    ALUResultM   <= ALUResultE;
    WriteDataM   <= WriteDataE;
    RdM          <= RdE;
    PCPlus4M     <= PCPlus4E;
    funct3M      <= funct3E;
    end
end

endmodule
