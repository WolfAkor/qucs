<Qucs Schematic 0.0.20>
<Properties>
  <DataDisplay=HF_BPF.dpl>
  <DataSet=HF_BPF.dat>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
  <OpenDisplay=1>
  <PosX=0>
  <PosY=0>
  <RunScript=0>
  <Script=HF_BPF.m>
  <ViewScale=0.977584>
  <ViewX1=-34>
  <ViewX2=1000>
  <ViewY1=-74>
  <ViewY2=729>
  <showFrame=0>
</Properties>
<Symbol>
</Symbol>
<Components>
    <GND * 5 800 220 0 0 0 0>
    <Pac P1 1 800 170 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 W" 0 "1 GHz" 0 "-273.15" 1>
    <GND * 5 310 220 0 0 0 0>
    <.DC DC1 1 20 310 0 43  0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0 "" 1>
    <Eqn Eqn2 1 450 320 -28 15 0 0 "S21_dB=dB(S[2,1])" 1 "S11_dB=dB(S[1,1])" 1 "yes" 0>
    <Eqn Eqn1 1 450 440 -31 17 0 0 "Ql=200" 1 "Qc=5000" 1 "f0=14e6" 1 "yes" 0>
    <.AC AC1 1 20 410 0 43  0 0 "log" 1 "1.2 MHz" 1 "170 MHz" 1 "218" 1 "yes" 1 "" 1>
    <GND * 5 100 220 0 0 0 0>
    <Pac P2 1 100 150 -122 -26 1 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "-273.15" 1>
    <GND * 5 630 290 0 0 0 0>
    <Vdc V1 1 630 250 -75 -22 1 1 "1.1 V" 1>
</Components>
<Wires>
<630 90 630 140 "" 0 0 0 "">
<590 140 630 140 "" 0 0 0 "">
<590 200 630 200 "" 0 0 0 "">
<630 200 630 220 "" 0 0 0 "">
<580 90 630 90 "" 0 0 0 "">
<630 90 800 90 "" 0 0 0 "">
<800 90 800 140 "" 0 0 0 "">
<800 200 800 220 "" 0 0 0 "">
<440 90 520 90 "" 0 0 0 "">
<310 90 310 140 "" 0 0 0 "">
<270 140 310 140 "" 0 0 0 "">
<270 200 310 200 "" 0 0 0 "">
<310 90 380 90 "" 0 0 0 "">
<310 200 310 220 "" 0 0 0 "">
<100 90 310 90 "" 0 0 0 "">
<100 90 100 120 "" 0 0 0 "">
<100 180 100 220 "" 0 0 0 "">
<630 280 630 290 "" 0 0 0 "">
<800 90 800 90 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 40 -40 12 #ff5500 0 "Chebyshev band-pass filter \n 12MHz...17MHz, pi-type, \n impedance matching 50 Ohm">
</Paintings>
