// VS::printSubckt main
module main();
Vac #(.U(1 uV), .f(2 kHz), .Phase(0), .Theta(0)) V4(net_40_200, net_40_260);
GND #() anonymous_gnd_hack_0(net_40_270);
R #(.R(50k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R2(net_290_310, net_230_310);
GND #() anonymous_gnd_hack_1(net_100_310);
R #(.R(100k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_360_300, net_360_240);
GND #() anonymous_gnd_hack_2(net_360_300);
GND #() anonymous_gnd_hack_3(net_130_150);
Vdc #(.U(2.5 V)) V3(net_130_90, net_130_150);
GND #() anonymous_gnd_hack_4(net_210_400);
Vdc #(.U(-2.5 V)) V6(net_210_340, net_210_400);
//AC1
//DC1
Eqn #(.Gain(dB(Output.v/Input.v)), .Phase(phase(Output.v/Input.v)), .Export(yes)) Eqn1();
Sub$singleOPV #(.File(singleOPV.sch)) OPV1(net_160_240, net_160_200, net_210_170, net_210_270, net_260_220);
R #(.R(1.7k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R3(net_160_310, net_100_310);
wire #() noname(net_260_220, net_290_220);
wire #() noname(net_40_260, net_40_270);
wire #() noname(net_40_200, net_160_200);
wire #() noname(net_160_240, net_160_310);
wire #() noname(net_160_310, net_230_310);
wire #() noname(net_290_220, net_290_310);
wire #() noname(net_360_220, net_360_240);
wire #() noname(net_290_220, net_360_220);
wire #() noname(net_130_80, net_210_80);
wire #() noname(net_130_80, net_130_90);
wire #() noname(net_210_80, net_210_170);
wire #() noname(net_210_270, net_210_340);
wire #() noname(net_40_200, net_40_200);
place #(.$xposition(40),.$yposition(200)) place_40_200(net_40_200);
place #(.$xposition(40),.$yposition(260)) place_40_260(net_40_260);
place #(.$xposition(40),.$yposition(270)) place_40_270(net_40_270);
place #(.$xposition(100),.$yposition(310)) place_100_310(net_100_310);
place #(.$xposition(130),.$yposition(80)) place_130_80(net_130_80);
place #(.$xposition(130),.$yposition(90)) place_130_90(net_130_90);
place #(.$xposition(130),.$yposition(150)) place_130_150(net_130_150);
place #(.$xposition(160),.$yposition(200)) place_160_200(net_160_200);
place #(.$xposition(160),.$yposition(240)) place_160_240(net_160_240);
place #(.$xposition(160),.$yposition(310)) place_160_310(net_160_310);
place #(.$xposition(210),.$yposition(80)) place_210_80(net_210_80);
place #(.$xposition(210),.$yposition(170)) place_210_170(net_210_170);
place #(.$xposition(210),.$yposition(270)) place_210_270(net_210_270);
place #(.$xposition(210),.$yposition(340)) place_210_340(net_210_340);
place #(.$xposition(210),.$yposition(400)) place_210_400(net_210_400);
place #(.$xposition(230),.$yposition(310)) place_230_310(net_230_310);
place #(.$xposition(260),.$yposition(220)) place_260_220(net_260_220);
place #(.$xposition(290),.$yposition(220)) place_290_220(net_290_220);
place #(.$xposition(290),.$yposition(310)) place_290_310(net_290_310);
place #(.$xposition(360),.$yposition(220)) place_360_220(net_360_220);
place #(.$xposition(360),.$yposition(240)) place_360_240(net_360_240);
place #(.$xposition(360),.$yposition(300)) place_360_300(net_360_300);
endmodule // main

