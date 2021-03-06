module sled_4(clk,LED7S,dig);
input clk;
output [7:0] LED7S;
output [7:0] dig; //数码管使能寄存器
reg [7:0] LED7S; //数码管段码输出寄存器
reg [7:0] dig;
reg[1:0] a;
reg[10:0] b;

always @(posedge clk)
begin
	if(b<10'b1111111111)b=b+1;//每来10'b1111111111
	else begin a=a+1;b=0;end//个时钟使a自加1
end
always @ (a) //通过a判断显示不同数码管
begin case(a)
		2'b00:begin
			dig=8'b11111110; //控制第1个数码管显示
			LED7S = 8'b11111001;//显示"1" 
			end
		2'b01:begin
			dig=8'b11111101; //控制第2个数码管显示
			LED7S = 8'b10100100;//显示"2" 
			end
		2'b10:begin
			dig=8'b11111011; //控制第3个数码管显示
			LED7S = 8'b10110000;//显示"3" 
			end
		2'b11:begin
			dig=8'b11110111; //控制第4个数码管显示
			LED7S = 8'b10011001;//显示"4" 
			end
		default: a=2'b00;
		endcase
end 
endmodule
