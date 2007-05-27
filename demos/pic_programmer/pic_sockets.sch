EESchema Schematic File Version 1
LIBS:power,device,conn,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,microcontrollers,dsp,microchip,analog_switches,motorola,texas,intel,audio,interface,digital-audio,philips,display,cypress,siliconi,contrib,valves,.\pic_programmer.cache
EELAYER 23  0
EELAYER END
$Descr A4 11700 8267
Sheet 2 2
Title "JDM - COM84 PIC Programmer with 13V DC/DC converter"
Date "16 may 2007"
Rev "3"
Comp "KiCad"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3700 7000 3700 6900
Wire Wire Line
	2350 2150 3100 2150
Wire Wire Line
	10250 1800 9400 1800
Wire Wire Line
	9400 1700 10200 1700
Wire Wire Line
	8200 1700 7450 1700
Connection ~ 2950 1100
Wire Wire Line
	2950 1100 3150 1100
Connection ~ 2950 1200
Wire Wire Line
	2950 1250 2950 1000
Wire Wire Line
	2950 1000 3150 1000
Wire Wire Line
	3150 1200 2950 1200
Wire Wire Line
	4550 1400 5350 1400
Wire Wire Line
	5400 1300 4550 1300
Wire Wire Line
	10500 4250 9650 4250
Wire Wire Line
	9650 4150 10450 4150
Wire Wire Line
	8450 4150 7700 4150
Wire Wire Line
	2500 4850 2300 4850
Wire Wire Line
	4900 5150 4050 5150
Wire Wire Line
	4050 5000 4850 5000
Wire Wire Line
	2500 4700 1750 4700
Wire Wire Line
	5400 2850 4550 2850
Wire Wire Line
	4550 2450 5350 2450
Wire Wire Line
	3100 3150 2350 3150
Wire Wire Line
	2300 4850 2300 4900
Wire Wire Line
	4850 4850 4050 4850
Wire Wire Line
	10200 2500 9400 2500
Wire Wire Line
	10450 4950 9650 4950
Wire Wire Line
	8450 5150 7650 5150
Wire Wire Line
	3700 6900 4550 6900
Wire Wire Line
	4550 6900 4550 7000
Text Label 3950 6900 0    60   ~
VCC_PIC
$Comp
L GND #PWR036
U 1 1 4639BE2E
P 4550 7400
F 0 "#PWR036" H 4550 7400 30  0001 C C
F 1 "GND" H 4550 7330 30  0001 C C
	1    4550 7400
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 4639BE2C
P 4550 7200
F 0 "C8" H 4600 7300 50  0000 L C
F 1 "100nF" H 4600 7100 50  0000 L C
	1    4550 7200
	1    0    0    -1  
$EndComp
Text GLabel 2350 2150 0    60   Input
VCC_PIC
Text Label 2550 2150 0    60   ~
VCC_PIC
Text Label 7650 5150 0    60   ~
VCC_PIC
Text Label 9900 4950 0    60   ~
VCC_PIC
Text Label 9650 2500 0    60   ~
VCC_PIC
$Comp
L GND #PWR037
U 1 1 442A8794
P 8450 5250
F 0 "#PWR037" H 8450 5250 30  0001 C C
F 1 "GND" H 8450 5180 30  0001 C C
	1    8450 5250
	0    1    1    0   
$EndComp
Text Label 4300 4850 0    60   ~
VCC_PIC
$Comp
L GND #PWR038
U 1 1 443CCA5D
P 8450 4850
F 0 "#PWR038" H 8450 4850 30  0001 C C
F 1 "GND" H 8450 4780 30  0001 C C
	1    8450 4850
	0    1    1    0   
$EndComp
NoConn ~ 8200 2700
NoConn ~ 9400 3000
NoConn ~ 9400 2900
NoConn ~ 9400 2800
NoConn ~ 9400 2700
NoConn ~ 9400 2400
NoConn ~ 9400 2300
NoConn ~ 9400 2200
NoConn ~ 9400 2100
NoConn ~ 9400 2000
NoConn ~ 9400 1900
NoConn ~ 8200 3000
NoConn ~ 8200 2900
NoConn ~ 8200 2800
NoConn ~ 8200 2600
NoConn ~ 8200 2500
NoConn ~ 8200 2300
NoConn ~ 8200 2200
NoConn ~ 8200 2100
NoConn ~ 8200 2000
NoConn ~ 8200 1900
NoConn ~ 8200 1800
$Comp
L GND #PWR039
U 1 1 443697C7
P 9400 2600
F 0 "#PWR039" H 9400 2600 30  0001 C C
F 1 "GND" H 9400 2530 30  0001 C C
	1    9400 2600
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR040
U 1 1 443697C3
P 8200 2400
F 0 "#PWR040" H 8200 2400 30  0001 C C
F 1 "GND" H 8200 2330 30  0001 C C
	1    8200 2400
	0    1    1    0   
$EndComp
Text Label 9550 1800 0    60   ~
CLOCK-RB6
Text Label 9550 1700 0    60   ~
DATA-RB7
Text Label 7550 1700 0    60   ~
VPP-MCLR
$Comp
L SUPP28 P2
U 1 1 4436967E
P 8800 2350
F 0 "P2" H 8850 3150 70  0000 C C
F 1 "SUPP28" H 8800 1550 70  0000 C C
	1    8800 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR041
U 1 1 442AA147
P 3700 7400
F 0 "#PWR041" H 3700 7400 30  0001 C C
F 1 "GND" H 3700 7330 30  0001 C C
	1    3700 7400
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 442AA145
P 3700 7200
F 0 "C7" H 3750 7300 50  0000 L C
F 1 "100nF" H 3750 7100 50  0000 L C
	1    3700 7200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR042
U 1 1 442AA138
P 2700 7400
F 0 "#PWR042" H 2700 7400 30  0001 C C
F 1 "GND" H 2700 7330 30  0001 C C
	1    2700 7400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR043
U 1 1 442AA134
P 2700 7000
F 0 "#PWR043" H 2700 7100 30  0001 C C
F 1 "VCC" H 2700 7100 30  0000 C C
	1    2700 7000
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 442AA12B
P 2700 7200
F 0 "C6" H 2750 7300 50  0000 L C
F 1 "100nF" H 2750 7100 50  0000 L C
	1    2700 7200
	1    0    0    -1  
$EndComp
NoConn ~ 9650 6050
NoConn ~ 9650 5950
NoConn ~ 9650 5850
NoConn ~ 9650 5750
NoConn ~ 9650 5650
NoConn ~ 9650 5550
NoConn ~ 9650 5450
NoConn ~ 9650 5350
NoConn ~ 9650 5250
NoConn ~ 9650 5150
NoConn ~ 9650 4850
NoConn ~ 9650 4750
NoConn ~ 9650 4650
NoConn ~ 9650 4550
NoConn ~ 9650 4450
NoConn ~ 9650 4350
NoConn ~ 8450 6050
NoConn ~ 8450 5950
NoConn ~ 8450 5850
NoConn ~ 8450 5750
NoConn ~ 8450 5650
NoConn ~ 8450 5550
NoConn ~ 8450 5450
NoConn ~ 8450 5350
NoConn ~ 8450 5050
NoConn ~ 8450 4950
NoConn ~ 8450 4750
NoConn ~ 8450 4650
NoConn ~ 8450 4550
NoConn ~ 8450 4450
NoConn ~ 8450 4350
NoConn ~ 8450 4250
$Comp
L GND #PWR044
U 1 1 442A896A
P 9650 5050
F 0 "#PWR044" H 9650 5050 30  0001 C C
F 1 "GND" H 9650 4980 30  0001 C C
	1    9650 5050
	0    -1   -1   0   
$EndComp
$Comp
L SUPP40 P3
U 1 1 442A88ED
P 9050 5100
F 0 "P3" H 9100 6200 70  0000 C C
F 1 "SUPP40" H 8900 4000 70  0000 C C
	1    9050 5100
	1    0    0    -1  
$EndComp
NoConn ~ 4550 1100
$Comp
L GND #PWR045
U 1 1 442A8838
P 2950 1250
F 0 "#PWR045" H 2950 1250 30  0001 C C
F 1 "GND" H 2950 1180 30  0001 C C
	1    2950 1250
	1    0    0    -1  
$EndComp
Text Label 4700 1400 0    60   ~
DATA-RB7
Text Label 4700 1300 0    60   ~
CLOCK-RB6
$Comp
L 24C16 U1
U 1 1 442A87F7
P 3850 1200
F 0 "U1" H 4000 1550 60  0000 C C
F 1 "24Cxx" H 4050 850 60  0000 C C
	1    3850 1200
	1    0    0    -1  
$EndComp
Text Label 9800 4250 0    60   ~
CLOCK-RB6
Text Label 9800 4150 0    60   ~
DATA-RB7
Text Label 7800 4150 0    60   ~
VPP-MCLR
NoConn ~ 2500 4400
NoConn ~ 4050 5450
NoConn ~ 4050 5300
NoConn ~ 4050 4550
NoConn ~ 4050 4700
NoConn ~ 4050 4400
NoConn ~ 4050 4250
NoConn ~ 2500 5450
NoConn ~ 2500 5300
NoConn ~ 2500 5150
NoConn ~ 2500 5000
NoConn ~ 2500 4550
NoConn ~ 2500 4250
$Comp
L PIC16F54 U5
U 1 1 442A81A7
P 3300 4850
F 0 "U5" H 3600 5650 60  0000 C C
F 1 "PIC_18_PINS" H 3750 4050 60  0000 C C
	1    3300 4850
	1    0    0    -1  
$EndComp
NoConn ~ 4550 3150
NoConn ~ 3100 2850
NoConn ~ 3100 2450
Text Label 4200 5150 0    60   ~
CLOCK-RB6
Text Label 4200 5000 0    60   ~
DATA-RB7
Text Label 1850 4700 0    60   ~
VPP-MCLR
Text Label 2450 3150 0    60   ~
VPP-MCLR
Text Label 4700 2850 0    60   ~
CLOCK-RB6
Text Label 4700 2450 0    60   ~
DATA-RB7
Text GLabel 1350 4450 0    60   Input
VPP-MCLR
Text GLabel 1350 4250 0    60   Input
CLOCK-RB6
Text GLabel 1350 4050 0    60   Input
DATA-RB7
$Comp
L GND #PWR046
U 1 1 442A820F
P 2300 4900
F 0 "#PWR046" H 2300 4900 30  0001 C C
F 1 "GND" H 2300 4830 30  0001 C C
	1    2300 4900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR047
U 1 1 442A8205
P 4550 2150
F 0 "#PWR047" H 4550 2150 30  0001 C C
F 1 "GND" H 4550 2080 30  0001 C C
	1    4550 2150
	0    -1   -1   0   
$EndComp
$Comp
L PIC12C508A U6
U 1 1 442A81A5
P 3850 2650
F 0 "U6" H 3800 3350 60  0000 C C
F 1 "PIC_8_PINS" H 3850 1950 60  0000 C C
	1    3850 2650
	1    0    0    -1  
$EndComp
$EndSCHEMATC
