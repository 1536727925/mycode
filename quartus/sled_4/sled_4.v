module sled_4(clk,LED7S,dig);
input clk;
output [7:0] LED7S;
output [7:0] dig; //�����ʹ�ܼĴ���
reg [7:0] LED7S; //����ܶ�������Ĵ���
reg [7:0] dig;
reg[1:0] a;
reg[10:0] b;

always @(posedge clk)
begin
	if(b<10'b1111111111)b=b+1;//ÿ��10'b1111111111
	else begin a=a+1;b=0;end//��ʱ��ʹa�Լ�1
end
always @ (a) //ͨ��a�ж���ʾ��ͬ�����
begin case(a)
		2'b00:begin
			dig=8'b11111110; //���Ƶ�1���������ʾ
			LED7S = 8'b11111001;//��ʾ"1" 
			end
		2'b01:begin
			dig=8'b11111101; //���Ƶ�2���������ʾ
			LED7S = 8'b10100100;//��ʾ"2" 
			end
		2'b10:begin
			dig=8'b11111011; //���Ƶ�3���������ʾ
			LED7S = 8'b10110000;//��ʾ"3" 
			end
		2'b11:begin
			dig=8'b11110111; //���Ƶ�4���������ʾ
			LED7S = 8'b10011001;//��ʾ"4" 
			end
		default: a=2'b00;
		endcase
end 
endmodule