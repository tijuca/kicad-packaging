EESchema Schematic File Version 1
LIBS:power,device,conn,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,microcontrollers,microchip,microchip1,analog_switches,motorola,intel,audio,interface,digital-audio,philips,display,cypress,siliconi,contrib,.\sonde xilinx.cache
EELAYER 23  0
EELAYER END
$Descr A4 11700 8267
Sheet 1 1
Title "CABLE PARALLELE III"
Date "3 may 2007"
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	6900 4200 7350 4200
Wire Wire Line
	7350 4200 7350 3050
Connection ~ 6300 4200
Wire Wire Line
	6300 4250 6300 4200
Wire Wire Line
	6400 4200 5900 4200
$Comp
L GND #PWR01
U 1 1 3EBF81B1
P 6300 4650
F 0 "#PWR01" H 6300 4650 40  0001 C C
F 1 "GND" H 6300 4580 40  0001 C C
	1    6300 4650
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 3EBF81A7
P 6300 4450
F 0 "C4" H 6350 4550 50  0000 L C
F 1 "100pF" H 6350 4350 50  0000 L C
	1    6300 4450
	1    0    0    -1  
$EndComp
$Comp
L R R13
U 1 1 3EBF819B
P 6650 4200
F 0 "R13" V 6730 4200 50  0000 C C
F 1 "100" V 6650 4200 50  0000 C C
	1    6650 4200
	0    1    1    0   
$EndComp
Wire Wire Line
	5000 4200 4200 4200
Wire Wire Line
	5450 4500 4800 4500
$Comp
L 74LS125 U1
U 4 1 3EBF7DBD
P 5450 4200
F 0 "U1" H 5450 4300 50  0000 L B
F 1 "74LS125" H 5500 4051 40  0000 L T
	4    5450 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 4500 4800 2900
Text Label 8250 3800 1    60   ~
TMS-PROG
Wire Wire Line
	10250 4100 8300 4100
Wire Wire Line
	10250 4300 8250 4300
Wire Wire Line
	10250 3900 8350 3900
Wire Wire Line
	10250 3700 8400 3700
Wire Wire Line
	10250 3600 10050 3600
Connection ~ 10200 3600
Connection ~ 10200 4000
Connection ~ 10200 3800
Wire Wire Line
	10250 4000 10200 4000
Wire Wire Line
	10250 3800 10200 3800
Wire Wire Line
	10200 3600 10200 4200
Wire Wire Line
	10200 4200 10250 4200
$Comp
L DB9 J2
U 1 1 3ECDE5C8
P 10700 3900
F 0 "J2" H 10700 4450 70  0000 C C
F 1 "DB9MALE" H 10700 3353 70  0000 C C
	1    10700 3900
	1    0    0    1   
$EndComp
Connection ~ 8250 3050
Connection ~ 8300 2950
Connection ~ 8350 2850
Connection ~ 8400 2750
Wire Wire Line
	8250 4300 8250 3050
Wire Wire Line
	8300 4100 8300 2950
Wire Wire Line
	8350 3900 8350 2850
Wire Wire Line
	8400 3700 8400 2750
$Comp
L PWR_FLAG #FLG02
U 1 1 3EBF848F
P 6200 1050
F 0 "#FLG02" H 6200 1320 30  0001 C C
F 1 "PWR_FLAG" H 6200 1280 30  0000 C C
	1    6200 1050
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR03
U 1 1 3EBF8479
P 5900 1050
F 0 "#PWR03" H 5900 1150 30  0001 C C
F 1 "VCC" H 5900 1150 30  0000 C C
	1    5900 1050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 3EBF8458
P 800 7300
F 0 "#PWR04" H 800 7300 40  0001 C C
F 1 "GND" H 800 7230 40  0001 C C
	1    800  7300
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG05
U 1 1 3EBF843C
P 800 7300
F 0 "#FLG05" H 800 7570 30  0001 C C
F 1 "PWR_FLAG" H 800 7530 30  0000 C C
	1    800  7300
	1    0    0    -1  
$EndComp
NoConn ~ 2200 4000
NoConn ~ 2200 3900
NoConn ~ 2200 3500
NoConn ~ 2200 3300
NoConn ~ 2200 3100
Wire Wire Line
	2500 2750 2500 2700
Wire Wire Line
	2500 2700 2200 2700
$Comp
L GND #PWR06
U 1 1 3EBF8100
P 2500 2750
F 0 "#PWR06" H 2500 2750 40  0001 C C
F 1 "GND" H 2500 2680 40  0001 C C
	1    2500 2750
	1    0    0    -1  
$EndComp
NoConn ~ 2200 2900
NoConn ~ 2200 2800
NoConn ~ 2200 2500
NoConn ~ 2200 2400
NoConn ~ 2200 2300
NoConn ~ 2200 2200
NoConn ~ 2200 2100
NoConn ~ 2200 1900
Wire Wire Line
	10250 3500 9400 3500
Text Label 9450 3500 0    60   ~
PWR(3,3-5V)
$Comp
L GND #PWR07
U 1 1 3EBF8376
P 10050 3600
F 0 "#PWR07" H 10050 3600 40  0001 C C
F 1 "GND" H 10050 3530 40  0001 C C
	1    10050 3600
	0    1    1    0   
$EndComp
Text Label 7550 3050 0    60   ~
TMS-PROG
Text Label 7550 2950 0    60   ~
TCK-CCLK
Text Label 7550 2850 0    60   ~
TDI-DIN
Text Label 7550 2750 0    60   ~
TD0-DONE/P
Text Label 7700 2550 0    60   ~
PWR(3,3-5V)
$Comp
L GND #PWR08
U 1 1 3EBF8376
P 8500 2650
F 0 "#PWR08" H 8500 2650 40  0001 C C
F 1 "GND" H 8500 2580 40  0001 C C
	1    8500 2650
	0    1    1    0   
$EndComp
Wire Wire Line
	7350 3050 8500 3050
Wire Wire Line
	8500 2950 7250 2950
Wire Wire Line
	7250 2950 7250 3300
Wire Wire Line
	7250 3300 6900 3300
Wire Wire Line
	8500 2850 7250 2850
Wire Wire Line
	7250 2850 7250 2600
Wire Wire Line
	7250 2600 6900 2600
Wire Wire Line
	8500 2750 7450 2750
Wire Wire Line
	7450 2750 7450 1900
Wire Wire Line
	7450 1900 6900 1900
Wire Wire Line
	8500 2550 7650 2550
Wire Wire Line
	7650 2550 7650 1050
Wire Wire Line
	7650 1050 7300 1050
$Comp
L CONN_6 P1
U 1 1 3EBF830C
P 8850 2800
F 0 "P1" V 8800 2800 60  0000 C C
F 1 "CONN_6" V 8900 2800 60  0000 C C
	1    8850 2800
	1    0    0    -1  
$EndComp
Connection ~ 6500 1050
Wire Wire Line
	6500 1100 6500 1050
$Comp
L GND #PWR09
U 1 1 3EBF82CF
P 6500 1500
F 0 "#PWR09" H 6500 1500 40  0001 C C
F 1 "GND" H 6500 1430 40  0001 C C
	1    6500 1500
	1    0    0    -1  
$EndComp
$Comp
L CP C1
U 1 1 3EBF82C6
P 6500 1300
F 0 "C1" H 6550 1400 50  0000 L C
F 1 "1uF" H 6550 1201 50  0000 L C
	1    6500 1300
	1    0    0    -1  
$EndComp
Connection ~ 6300 3300
Connection ~ 6300 2600
Connection ~ 6300 1900
Wire Wire Line
	6300 1950 6300 1900
Wire Wire Line
	6300 2650 6300 2600
Wire Wire Line
	6300 3350 6300 3300
Wire Wire Line
	6400 3300 5900 3300
Wire Wire Line
	5900 2600 6400 2600
Wire Wire Line
	6400 1900 5900 1900
$Comp
L GND #PWR010
U 1 1 3EBF81B1
P 6300 3750
F 0 "#PWR010" H 6300 3750 40  0001 C C
F 1 "GND" H 6300 3680 40  0001 C C
	1    6300 3750
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 3EBF81A7
P 6300 3550
F 0 "C5" H 6350 3650 50  0000 L C
F 1 "100pF" H 6350 3450 50  0000 L C
	1    6300 3550
	1    0    0    -1  
$EndComp
$Comp
L R R12
U 1 1 3EBF819B
P 6650 3300
F 0 "R12" V 6730 3300 50  0000 C C
F 1 "100" V 6650 3300 50  0000 C C
	1    6650 3300
	0    1    1    0   
$EndComp
$Comp
L GND #PWR011
U 1 1 3EBF81B1
P 6300 3050
F 0 "#PWR011" H 6300 3050 40  0001 C C
F 1 "GND" H 6300 2980 40  0001 C C
	1    6300 3050
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 3EBF81A7
P 6300 2850
F 0 "C2" H 6350 2950 50  0000 L C
F 1 "100pF" H 6350 2750 50  0000 L C
	1    6300 2850
	1    0    0    -1  
$EndComp
$Comp
L R R11
U 1 1 3EBF819B
P 6650 2600
F 0 "R11" V 6730 2600 50  0000 C C
F 1 "100" V 6650 2600 50  0000 C C
	1    6650 2600
	0    1    1    0   
$EndComp
Connection ~ 5900 1050
Connection ~ 6200 1050
Wire Wire Line
	4300 1050 4200 1050
Wire Wire Line
	2300 1050 2300 3700
Wire Wire Line
	6900 1050 4700 1050
Wire Wire Line
	5900 1050 5900 1100
Wire Wire Line
	6200 1050 6200 1100
$Comp
L GND #PWR012
U 1 1 3EBF81B3
P 6200 1600
F 0 "#PWR012" H 6200 1600 40  0001 C C
F 1 "GND" H 6200 1530 40  0001 C C
	1    6200 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 3EBF81B1
P 6300 2350
F 0 "#PWR013" H 6300 2350 40  0001 C C
F 1 "GND" H 6300 2280 40  0001 C C
	1    6300 2350
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 3EBF81A7
P 6300 2150
F 0 "C3" H 6350 2250 50  0000 L C
F 1 "100pF" H 6350 2050 50  0000 L C
	1    6300 2150
	1    0    0    -1  
$EndComp
$Comp
L R R14
U 1 1 3EBF819B
P 6650 1900
F 0 "R14" V 6730 1900 50  0000 C C
F 1 "100" V 6650 1900 50  0000 C C
	1    6650 1900
	0    1    1    0   
$EndComp
$Comp
L R R5
U 1 1 3EBF818E
P 6200 1350
F 0 "R5" V 6280 1350 50  0000 C C
F 1 "1K" V 6200 1350 50  0000 C C
	1    6200 1350
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 3EBF8187
P 5900 1350
F 0 "R2" V 5980 1350 50  0000 C C
F 1 "5,1K" V 5900 1350 50  0000 C C
	1    5900 1350
	1    0    0    -1  
$EndComp
$Comp
L DIODESCH D2
U 1 1 3EBF8176
P 7100 1050
F 0 "D2" H 7100 1150 40  0000 C C
F 1 "BAT46" H 7100 950 40  0000 C C
	1    7100 1050
	-1   0    0    -1  
$EndComp
$Comp
L DIODESCH D1
U 1 1 3EBF815E
P 4500 1050
F 0 "D1" H 4500 1150 40  0000 C C
F 1 "BAT46" H 4500 950 40  0000 C C
	1    4500 1050
	-1   0    0    -1  
$EndComp
Connection ~ 2350 2000
Wire Wire Line
	2350 2600 2350 1800
Wire Wire Line
	2350 2600 2200 2600
Wire Wire Line
	2200 2000 2350 2000
Wire Wire Line
	2350 1800 2200 1800
Wire Wire Line
	2500 1750 2500 1700
Wire Wire Line
	2500 1700 2200 1700
$Comp
L GND #PWR014
U 1 1 3EBF8100
P 2500 1750
F 0 "#PWR014" H 2500 1750 40  0001 C C
F 1 "GND" H 2500 1680 40  0001 C C
	1    2500 1750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 3EBF80DD
P 4650 2000
F 0 "#PWR015" H 4650 2000 40  0001 C C
F 1 "GND" H 4650 1930 40  0001 C C
	1    4650 2000
	1    0    0    -1  
$EndComp
Connection ~ 4800 3600
Connection ~ 4800 2900
Text Label 2500 1050 0    60   ~
VCC_SENSE(/ERROR)
Text Label 2400 3800 0    60   ~
TDI/DIN(D0)
Text Label 2400 3600 0    60   ~
CLK(D1)
Text Label 2400 3400 0    60   ~
TMS/PROG(D2)
Text Label 2400 3200 0    60   ~
CTRL(D3)
Text Label 2400 3000 0    60   ~
TD0/PROG(D4)
Wire Wire Line
	2300 1050 3700 1050
Wire Wire Line
	2300 3700 2200 3700
Wire Wire Line
	3700 4200 3600 4200
Wire Wire Line
	3600 4200 3600 3400
Wire Wire Line
	3600 3400 2200 3400
Wire Wire Line
	2200 3600 3450 3600
Wire Wire Line
	3450 3600 3450 3300
Wire Wire Line
	3450 3300 3700 3300
Wire Wire Line
	2200 3200 3450 3200
Wire Wire Line
	3450 3200 3450 2900
Wire Wire Line
	3450 2900 3700 2900
Wire Wire Line
	2200 3800 3400 3800
Wire Wire Line
	3400 3800 3400 2600
Wire Wire Line
	3400 2600 3700 2600
Wire Wire Line
	2200 3000 3350 3000
Wire Wire Line
	3350 3000 3350 2250
Wire Wire Line
	3350 2250 3700 2250
Text Label 2500 1600 0    60   ~
DONE(/SELECT)
NoConn ~ 2150 6900
NoConn ~ 2150 6250
NoConn ~ 2150 5600
Connection ~ 1150 7200
Connection ~ 1150 6900
Connection ~ 1150 6550
Connection ~ 1150 5900
Wire Wire Line
	1700 5900 1150 5900
Wire Wire Line
	1700 6550 1150 6550
Wire Wire Line
	1700 7200 1150 7200
$Comp
L GND #PWR016
U 1 1 3EBF7F0D
P 1150 7300
F 0 "#PWR016" H 1150 7300 40  0001 C C
F 1 "GND" H 1150 7230 40  0001 C C
	1    1150 7300
	1    0    0    -1  
$EndComp
Connection ~ 1150 6250
Wire Wire Line
	1250 6250 1150 6250
Wire Wire Line
	1250 6900 1150 6900
Wire Wire Line
	1150 7300 1150 5600
Wire Wire Line
	1150 5600 1250 5600
$Comp
L 74LS125 U2
U 4 1 3EBF7EEC
P 1700 5600
F 0 "U2" H 1700 5700 50  0000 L B
F 1 "74LS125" H 1750 5450 40  0000 L T
	4    1700 5600
	1    0    0    -1  
$EndComp
$Comp
L 74LS125 U2
U 3 1 3EBF7EDF
P 1700 6250
F 0 "U2" H 1700 6350 50  0000 L B
F 1 "74LS125" H 1750 6100 40  0000 L T
	3    1700 6250
	1    0    0    -1  
$EndComp
$Comp
L 74LS125 U2
U 2 1 3EBF7EDD
P 1700 6900
F 0 "U2" H 1700 7000 50  0000 L B
F 1 "74LS125" H 1750 6750 40  0000 L T
	2    1700 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 3600 4800 3600
Wire Wire Line
	5450 2900 4200 2900
Wire Wire Line
	5900 1900 5900 1600
Wire Wire Line
	5900 1600 5100 1600
Wire Wire Line
	5000 3300 4200 3300
Wire Wire Line
	5000 2600 4200 2600
Wire Wire Line
	4200 2250 5450 2250
Wire Wire Line
	5450 2250 5450 2200
Wire Wire Line
	4650 2000 4650 1900
Wire Wire Line
	4650 1900 5000 1900
$Comp
L 74LS125 U1
U 3 1 3EBF7DB3
P 5450 3300
F 0 "U1" H 5450 3400 50  0000 L B
F 1 "74LS125" H 5500 3151 40  0000 L T
	3    5450 3300
	1    0    0    -1  
$EndComp
$Comp
L 74LS125 U1
U 2 1 3EBF7DAD
P 5450 2600
F 0 "U1" H 5450 2700 50  0000 L B
F 1 "74LS125" H 5500 2451 40  0000 L T
	2    5450 2600
	1    0    0    -1  
$EndComp
$Comp
L 74LS125 U1
U 1 1 3EBF7D9F
P 5450 1900
F 0 "U1" H 5450 2000 50  0000 L B
F 1 "74LS125" H 5500 1750 40  0000 L T
	1    5450 1900
	1    0    0    -1  
$EndComp
$Comp
L 74LS125 U2
U 1 1 3EBF7D92
P 4650 1600
F 0 "U2" H 4650 1700 50  0000 L B
F 1 "74LS125" H 4700 1450 40  0000 L T
	1    4650 1600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2200 1600 3700 1600
$Comp
L R R9
U 1 1 3EBF7D33
P 3950 4200
F 0 "R9" V 4030 4200 50  0000 C C
F 1 "100" V 3950 4200 50  0000 C C
	1    3950 4200
	0    1    1    0   
$EndComp
$Comp
L R R10
U 1 1 3EBF7D31
P 3950 3300
F 0 "R10" V 4030 3300 50  0000 C C
F 1 "100" V 3950 3300 50  0000 C C
	1    3950 3300
	0    1    1    0   
$EndComp
$Comp
L R R8
U 1 1 3EBF7D33
P 3950 2900
F 0 "R8" V 4030 2900 50  0000 C C
F 1 "100" V 3950 2900 50  0000 C C
	1    3950 2900
	0    1    1    0   
$EndComp
$Comp
L R R7
U 1 1 3EBF7D31
P 3950 2600
F 0 "R7" V 4030 2600 50  0000 C C
F 1 "100" V 3950 2600 50  0000 C C
	1    3950 2600
	0    1    1    0   
$EndComp
$Comp
L R R6
U 1 1 3EBF7D26
P 3950 2250
F 0 "R6" V 4030 2250 50  0000 C C
F 1 "100" V 3950 2250 50  0000 C C
	1    3950 2250
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 3EBF7D22
P 3950 1600
F 0 "R4" V 4030 1600 50  0000 C C
F 1 "47" V 3950 1600 50  0000 C C
	1    3950 1600
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 3EBF7D16
P 3950 1050
F 0 "R1" V 4030 1050 50  0000 C C
F 1 "100" V 3950 1050 50  0000 C C
	1    3950 1050
	0    1    1    0   
$EndComp
$Comp
L DB25 J1
U 1 1 3EBF7D04
P 1750 2800
F 0 "J1" H 1800 4150 70  0000 C C
F 1 "DB25MALE" H 1700 1450 70  0000 C C
	1    1750 2800
	-1   0    0    -1  
$EndComp
$EndSCHEMATC
