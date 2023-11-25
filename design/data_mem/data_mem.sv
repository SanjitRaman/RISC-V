module data_mem #(
    parameter ADDRESS_WIDTH = 9,
    parameter BYTE_WIDTH    = 8,
    parameter DATA_WIDTH    = 32

)(
    input logic CLK,
    input logic WE,
    input logic [ADDRESS_WIDTH-1:0] A,
    input logic [DATA_WIDTH-1:0] WD,
    output logic [DATA_WIDTH-1:0] RD
);

logic [BYTE_WIDTH-1:0] ram_array [2**ADDRESS_WIDTH-1:0];

initial begin
    $display("Loading ram.");
  $readmemh("sineram.mem", ram_array);
end;

always_ff @(posedge CLK)
    if (WE == 1'b1) begin
        ram_array[A] <= WD[BYTE_WIDTH-1:0];
        ram_array[A+1] <= WD[2*BYTE_WIDTH-1:BYTE_WIDTH];
        ram_array[A+2] <= WD[3*BYTE_WIDTH-1:2*BYTE_WIDTH];
        ram_array[A+3] <= WD[4*BYTE_WIDTH-1:3*BYTE_WIDTH]; // this will be a problem for sw: individual bytes are not written to.
    end
assign RD = {{24{1'b0}}, ram_array[A]};
endmodule
