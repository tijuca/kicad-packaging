EESchema Schematic File Version 1
LIBS:power,device,conn,brooktre,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,analog_switches,philips,.\video.cache
EELAYER 20  0
EELAYER END
$Descr A3 16535 11700
Sheet 7 8
Title "Video"
Date "31 dec 2006"
Rev "2.0B"
Comp "Kicad EDA"
Comment1 "Interface Bus PCI"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 1250 10750
Wire Wire Line
	1050 10750 1500 10750
$Comp
L PWR_FLAG #FLG076
U 1 1 4174D9FB
P 1500 10750
F 0 "#FLG076" H 1500 11020 30  0001 C C
F 1 "PWR_FLAG" H 1500 10980 30  0000 C C
	1    1500 10750
	1    0    0    -1  
$EndComp
Connection ~ 1150 8850
Connection ~ 1050 10200
Wire Wire Line
	1250 10200 600  10200
$Comp
L PWR_FLAG #FLG077
U 1 1 4174D80F
P 1250 10200
F 0 "#FLG077" H 1250 10470 30  0001 C C
F 1 "PWR_FLAG" H 1250 10430 30  0000 C C
	1    1250 10200
	1    0    0    -1  
$EndComp
Connection ~ 4500 1000
Wire Wire Line
	4250 1000 4500 1000
Wire Wire Line
	1350 8850 700  8850
$Comp
L PWR_FLAG #FLG078
U 1 1 4174D820
P 4250 1000
F 0 "#FLG078" H 4250 1270 30  0001 C C
F 1 "PWR_FLAG" H 4250 1230 30  0000 C C
	1    4250 1000
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG079
U 1 1 4174D80F
P 1350 8850
F 0 "#FLG079" H 1350 9120 30  0001 C C
F 1 "PWR_FLAG" H 1350 9080 30  0000 C C
	1    1350 8850
	1    0    0    -1  
$EndComp
$Comp
L VSS #PWR080
U 1 1 33BA4D4A
P 1250 10800
F 0 "#PWR080" H 1250 10800 40  0001 C C
F 1 "VSS" H 1250 10730 40  0000 C C
	1    1250 10800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 10750 1250 10800
Connection ~ 1050 10750
$Comp
L VCC #PWR081
U 1 1 33B3DC6A
P 1150 8850
F 0 "#PWR081" H 1150 9050 40  0001 C C
F 1 "VCC" H 1150 9000 40  0000 C C
	1    1150 8850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1050 10200 1050 10300
Wire Wire Line
	600  10200 600  10300
Wire Wire Line
	1150 8850 1150 8950
$Comp
L +3.3V #PWR082
U 1 1 33AFD455
P 600 10200
F 0 "#PWR082" H 600 10400 40  0001 C C
F 1 "+3.3V" H 600 10350 40  0000 C C
	1    600  10200
	1    0    0    -1  
$EndComp
$Comp
L GND #GND083
U 1 1 2820F094
P 1050 10800
F 0 "#GND083" H 1050 10900 60  0001 C C
F 1 "GND" H 1050 10700 60  0000 C C
	1    1050 10800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1050 10700 1050 10800
$Comp
L GND #GND084
U 1 1 2820F094
P 600 10800
F 0 "#GND084" H 600 10900 60  0001 C C
F 1 "GND" H 600 10700 60  0000 C C
	1    600  10800
	1    0    0    -1  
$EndComp
Wire Wire Line
	600  10700 600  10800
$Comp
L GND #GND085
U 1 1 2820F094
P 1150 9450
F 0 "#GND085" H 1150 9550 60  0001 C C
F 1 "GND" H 1150 9350 60  0000 C C
	1    1150 9450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 9350 1150 9450
$Comp
L CP C69
U 1 1 33AFD43A
P 1050 10500
F 0 "C69" H 1100 10600 50  0000 L C
F 1 "4,7uF" H 1100 10400 50  0000 L C
	1    1050 10500
	1    0    0    -1  
$EndComp
$Comp
L CP C68
U 1 1 33AFD420
P 600 10500
F 0 "C68" H 650 10600 50  0000 L C
F 1 "4,7uF" H 650 10400 50  0000 L C
	1    600  10500
	1    0    0    -1  
$EndComp
$Comp
L CP C67
U 1 1 33AFD420
P 1150 9150
F 0 "C67" H 1200 9250 50  0000 L C
F 1 "4,7uF" H 1200 9050 50  0000 L C
	1    1150 9150
	1    0    0    -1  
$EndComp
NoConn ~ 2200 1150
Wire Wire Line
	4400 5550 4500 5550
Wire Wire Line
	4400 3150 4500 3150
Text GLabel 10800 5050 2    60   Output
PTADR-
Wire Wire Line
	10600 5050 10800 5050
Wire Bus Line
	11300 4850 11250 4850
Wire Bus Line
	11250 4850 11250 4750
Entry Wire Line
	11150 4950 11250 4850
Entry Wire Line
	11150 4850 11250 4750
Entry Wire Line
	11150 4750 11250 4850
Entry Wire Line
	11150 4650 11250 4750
Wire Wire Line
	10600 4950 11150 4950
Wire Wire Line
	10600 4850 11150 4850
Wire Wire Line
	10600 4750 11150 4750
Wire Wire Line
	10600 4650 11150 4650
Text Label 10700 4950 0    60   ~
PTBE-3
Text Label 10700 4850 0    60   ~
PTBE-2
Text Label 10700 4750 0    60   ~
PTBE-1
Text Label 10700 4650 0    60   ~
PTBE-0
Text GLabel 11200 4250 2    60   Output
PTATN-
Wire Bus Line
	11100 6150 11100 5850
Wire Bus Line
	11100 5850 11150 5850
Wire Bus Line
	11100 5750 11100 5350
Wire Bus Line
	11100 5350 11150 5350
Entry Wire Line
	11000 6250 11100 6150
Entry Wire Line
	11000 6150 11100 6050
Entry Wire Line
	11000 6050 11100 5950
Entry Wire Line
	11000 5950 11100 5850
Entry Wire Line
	11000 5850 11100 5750
Entry Wire Line
	11000 5750 11100 5650
Entry Wire Line
	11000 5650 11100 5550
Entry Wire Line
	11000 5550 11100 5450
Entry Wire Line
	11000 5450 11100 5350
Text Label 10700 6250 0    60   ~
BE-3
Text Label 10700 6150 0    60   ~
BE-2
Text Label 10700 6050 0    60   ~
BE-1
Text Label 10700 5950 0    60   ~
BE-0
Wire Wire Line
	10600 6250 11000 6250
Wire Wire Line
	10600 6150 11000 6150
Wire Wire Line
	10600 6050 11000 6050
Wire Wire Line
	10600 5950 11000 5950
Wire Wire Line
	10600 5850 11000 5850
Wire Wire Line
	10600 5750 11000 5750
Wire Wire Line
	10600 5650 11000 5650
Wire Wire Line
	10600 5550 11000 5550
Wire Wire Line
	10600 5450 11000 5450
Text Label 10700 5850 0    60   ~
ADR6
Text Label 10700 5750 0    60   ~
ADR5
Text Label 10700 5650 0    60   ~
ADR4
Text Label 10700 5550 0    60   ~
ADR3
Text Label 10700 5450 0    60   ~
ADR2
Wire Bus Line
	11600 4050 11600 850 
Entry Wire Line
	11500 4150 11600 4050
Entry Wire Line
	11500 4050 11600 3950
Entry Wire Line
	11500 3950 11600 3850
Entry Wire Line
	11500 3850 11600 3750
Entry Wire Line
	11500 3750 11600 3650
Entry Wire Line
	11500 3650 11600 3550
Entry Wire Line
	11500 3550 11600 3450
Entry Wire Line
	11500 3450 11600 3350
Entry Wire Line
	11500 3350 11600 3250
Entry Wire Line
	11500 3250 11600 3150
Entry Wire Line
	11500 3150 11600 3050
Entry Wire Line
	11500 3050 11600 2950
Entry Wire Line
	11500 2950 11600 2850
Entry Wire Line
	11500 2850 11600 2750
Entry Wire Line
	11500 2750 11600 2650
Entry Wire Line
	11500 2650 11600 2550
Wire Wire Line
	10600 4150 11500 4150
Wire Wire Line
	10600 4050 11500 4050
Wire Wire Line
	10600 3950 11500 3950
Wire Wire Line
	10600 3850 11500 3850
Wire Wire Line
	10600 3750 11500 3750
Wire Wire Line
	10600 3650 11500 3650
Wire Wire Line
	10600 3550 11500 3550
Wire Wire Line
	10600 3450 11500 3450
Wire Wire Line
	10600 3350 11500 3350
Wire Wire Line
	10600 3250 11500 3250
Wire Wire Line
	10600 3150 11500 3150
Wire Wire Line
	10600 3050 11500 3050
Wire Wire Line
	10600 2950 11500 2950
Wire Wire Line
	10600 2850 11500 2850
Wire Wire Line
	10600 2750 11500 2750
Wire Wire Line
	10600 2650 11500 2650
Wire Wire Line
	2200 1550 2200 1650
Wire Wire Line
	4500 6950 4500 7250
Wire Wire Line
	2100 6950 2100 7250
Wire Wire Line
	4950 9600 4950 9350
Text Label 1500 6850 0    60   ~
P_AD1
Text Label 1500 6650 0    60   ~
P_AD3
Text Label 1500 6550 0    60   ~
P_AD5
Text Label 1500 6350 0    60   ~
P_AD7
Text Label 1500 6250 0    60   ~
P_AD8
Text Label 1500 5850 0    60   ~
P_AD10
Text Label 1500 5750 0    60   ~
P_AD12
Text Label 1500 5550 0    60   ~
P_AD14
Text Label 1450 4250 0    60   ~
P_AD17
Text Label 1450 4050 0    60   ~
P_AD19
Text Label 1450 3950 0    60   ~
P_AD21
Text Label 1450 3750 0    60   ~
P_AD23
Text Label 1450 3450 0    60   ~
P_AD25
Text Label 1450 3350 0    60   ~
P_AD27
Text Label 1450 3150 0    60   ~
P_AD29
Text Label 1450 3050 0    60   ~
P_AD31
Text Label 1450 5450 0    60   ~
P_C/BE1#
Text Label 1450 4350 0    60   ~
P_C/BE2#
Text Label 1450 3650 0    60   ~
P_C/BE3#
Text Label 1450 2650 0    60   ~
P_CLK
Text Label 1450 4550 0    60   ~
P_IRDY#
Text Label 1450 4950 0    60   ~
P_LOCK#
Text Label 1450 4750 0    60   ~
P_DEVSEL#
Text Label 1450 2850 0    60   ~
P_REQ#
Text Label 1450 5050 0    60   ~
P_PERR#
Text Label 1450 5250 0    60   ~
P_SERR#
Wire Wire Line
	1350 2650 2200 2650
Wire Wire Line
	1350 2850 2200 2850
Wire Wire Line
	1350 3050 2200 3050
Wire Wire Line
	1350 3150 2200 3150
Wire Wire Line
	1350 3350 2200 3350
Wire Wire Line
	1350 3450 2200 3450
Wire Wire Line
	1350 3650 2200 3650
Wire Wire Line
	1350 3750 2200 3750
Wire Wire Line
	1350 3950 2200 3950
Wire Wire Line
	1350 4050 2200 4050
Wire Wire Line
	1350 4250 2200 4250
Wire Wire Line
	1350 4350 2200 4350
Wire Wire Line
	1350 4550 2200 4550
Wire Wire Line
	1350 4750 2200 4750
Wire Wire Line
	1350 4950 2200 4950
Wire Wire Line
	1350 5050 2200 5050
Wire Wire Line
	1350 5250 2200 5250
Wire Wire Line
	1350 5450 2200 5450
Wire Wire Line
	1350 5550 2200 5550
Wire Wire Line
	1350 5750 2200 5750
Wire Wire Line
	1350 5850 2200 5850
Wire Wire Line
	1350 6250 2200 6250
Wire Wire Line
	1350 6350 2200 6350
Wire Wire Line
	1350 6550 2200 6550
Wire Wire Line
	1350 6650 2200 6650
Wire Wire Line
	1350 6850 2200 6850
Text Notes 2300 10850 0    60   ~
Decouplage du "S5933"
Wire Notes Line
	1700 10950 1700 9050
Wire Notes Line
	1700 10950 4600 10950
Wire Notes Line
	4600 10950 4600 9050
Wire Notes Line
	4600 9050 1700 9050
$Comp
L S5933_PQ160 U11
U 1 1 21FA8347
P 9450 5100
F 0 "U11" H 9450 7350 60  0000 C C
F 1 "S5933_PQ160" H 9900 950 60  0000 C C
	1    9450 5100
	1    0    0    -1  
$EndComp
$Comp
L GND #GND086
U 1 1 2691B5BF
P 2200 1350
F 0 "#GND086" H 2200 1450 60  0001 C C
F 1 "GND" H 2200 1250 60  0000 C C
	1    2200 1350
	0    1    1    0   
$EndComp
$Comp
L GND #GND089
U 1 1 2691B5CE
P 2200 2550
F 0 "#GND089" H 2200 2650 60  0001 C C
F 1 "GND" H 2200 2450 60  0000 C C
	1    2200 2550
	0    1    1    0   
$EndComp
$Comp
L GND #GND090
U 1 1 2691B5D3
P 2200 2750
F 0 "#GND090" H 2200 2850 60  0001 C C
F 1 "GND" H 2200 2650 60  0000 C C
	1    2200 2750
	0    1    1    0   
$EndComp
$Comp
L GND #GND091
U 1 1 2691B5D8
P 2200 3250
F 0 "#GND091" H 2200 3350 60  0001 C C
F 1 "GND" H 2200 3150 60  0000 C C
	1    2200 3250
	0    1    1    0   
$EndComp
$Comp
L GND #GND092
U 1 1 2691B5DD
P 2200 3850
F 0 "#GND092" H 2200 3950 60  0001 C C
F 1 "GND" H 2200 3750 60  0000 C C
	1    2200 3850
	0    1    1    0   
$EndComp
$Comp
L GND #GND093
U 1 1 2691B5E2
P 2200 4450
F 0 "#GND093" H 2200 4550 60  0001 C C
F 1 "GND" H 2200 4350 60  0000 C C
	1    2200 4450
	0    1    1    0   
$EndComp
$Comp
L GND #GND094
U 1 1 2691B5E7
P 2200 4850
F 0 "#GND094" H 2200 4950 60  0001 C C
F 1 "GND" H 2200 4750 60  0000 C C
	1    2200 4850
	0    1    1    0   
$EndComp
$Comp
L GND #GND095
U 1 1 2691B5EC
P 2200 5650
F 0 "#GND095" H 2200 5750 60  0001 C C
F 1 "GND" H 2200 5550 60  0000 C C
	1    2200 5650
	0    1    1    0   
$EndComp
$Comp
L GND #GND096
U 1 1 2691B5F1
P 2200 5950
F 0 "#GND096" H 2200 6050 60  0001 C C
F 1 "GND" H 2200 5850 60  0000 C C
	1    2200 5950
	0    1    1    0   
$EndComp
$Comp
L GND #GND097
U 1 1 2691B5F6
P 2200 6750
F 0 "#GND097" H 2200 6850 60  0001 C C
F 1 "GND" H 2200 6650 60  0000 C C
	1    2200 6750
	0    1    1    0   
$EndComp
$Comp
L GND #GND098
U 1 1 2691B5FB
P 4500 6650
F 0 "#GND098" H 4500 6750 60  0001 C C
F 1 "GND" H 4500 6550 60  0000 C C
	1    4500 6650
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND099
U 1 1 2691B600
P 4500 5850
F 0 "#GND099" H 4500 5950 60  0001 C C
F 1 "GND" H 4500 5750 60  0000 C C
	1    4500 5850
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND0100
U 1 1 2691B605
P 4500 5250
F 0 "#GND0100" H 4500 5350 60  0001 C C
F 1 "GND" H 4500 5150 60  0000 C C
	1    4500 5250
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND0101
U 1 1 2691B60A
P 4500 4750
F 0 "#GND0101" H 4500 4850 60  0001 C C
F 1 "GND" H 4500 4650 60  0000 C C
	1    4500 4750
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND0102
U 1 1 2691B60F
P 4500 4550
F 0 "#GND0102" H 4500 4650 60  0001 C C
F 1 "GND" H 4500 4450 60  0000 C C
	1    4500 4550
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND0103
U 1 1 2691B614
P 4500 4050
F 0 "#GND0103" H 4500 4150 60  0001 C C
F 1 "GND" H 4500 3950 60  0000 C C
	1    4500 4050
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND0104
U 1 1 2691B619
P 4500 3450
F 0 "#GND0104" H 4500 3550 60  0001 C C
F 1 "GND" H 4500 3350 60  0000 C C
	1    4500 3450
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND0105
U 1 1 2691B61E
P 4500 2850
F 0 "#GND0105" H 4500 2850 40  0001 C C
F 1 "GND" H 4500 2780 40  0000 C C
	1    4500 2850
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND0108
U 1 1 2691B63C
P 1500 2250
F 0 "#GND0108" H 1500 2250 40  0001 C C
F 1 "GND" H 1500 2180 40  0001 C C
	1    1500 2250
	1    0    0    -1  
$EndComp
$Comp
L TEST W4
U 1 1 2691B632
P 1800 2150
F 0 "W4" H 1800 2210 40  0000 C C
F 1 "TEST" H 1800 2080 40  0000 C C
	1    1800 2150
	1    0    0    -1  
$EndComp
$Comp
L TEST W5
U 1 1 2691B637
P 1800 1950
F 0 "W5" H 1800 2010 40  0000 C C
F 1 "TEST" H 1800 1880 40  0000 C C
	1    1800 1950
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0109
U 1 1 26A799F2
P 12050 6650
F 0 "#GND0109" H 12050 6750 60  0001 C C
F 1 "GND" H 12050 6550 60  0000 C C
	1    12050 6650
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0110
U 1 1 26A799FC
P 11900 7950
F 0 "#GND0110" H 11900 8050 60  0001 C C
F 1 "GND" H 11900 7850 60  0000 C C
	1    11900 7950
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0111
U 1 1 26A79A06
P 11900 8850
F 0 "#GND0111" H 11900 8950 60  0001 C C
F 1 "GND" H 11900 8750 60  0000 C C
	1    11900 8850
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 26A799E8
P 12500 6150
F 0 "R5" V 12580 6150 50  0000 C C
F 1 "10K" V 12500 6150 50  0000 C C
	1    12500 6150
	0    -1   -1   0   
$EndComp
$Comp
L R R6
U 1 1 26A79A01
P 12350 7350
F 0 "R6" V 12430 7350 50  0000 C C
F 1 "10K" V 12350 7350 50  0000 C C
	1    12350 7350
	0    -1   -1   0   
$EndComp
$Comp
L R R7
U 1 1 26A79A0B
P 12350 8250
F 0 "R7" V 12430 8250 50  0000 C C
F 1 "10K" V 12350 8250 50  0000 C C
	1    12350 8250
	0    -1   -1   0   
$EndComp
$Comp
L GND #GND0112
U 1 1 26B211BB
P 4950 9600
F 0 "#GND0112" H 4950 9700 60  0001 C C
F 1 "GND" H 4950 9500 60  0000 C C
	1    4950 9600
	1    0    0    -1  
$EndComp
$Comp
L R R28
U 1 1 26B211C0
P 6850 9300
F 0 "R28" V 6930 9300 50  0000 C C
F 1 "2,2K" V 6850 9300 50  0000 C C
	1    6850 9300
	1    0    0    -1  
$EndComp
$Comp
L R R29
U 1 1 26B211CF
P 7100 9300
F 0 "R29" V 7180 9300 50  0000 C C
F 1 "2,2K" V 7100 9300 50  0000 C C
	1    7100 9300
	1    0    0    -1  
$EndComp
$Comp
L C C24
U 1 1 26B211D9
P 1900 9600
F 0 "C24" H 1950 9700 50  0000 L C
F 1 "100nF" H 1950 9500 50  0000 L C
	1    1900 9600
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0113
U 1 1 26B211DE
P 1900 9800
F 0 "#GND0113" H 1900 9900 60  0001 C C
F 1 "GND" H 1900 9700 60  0000 C C
	1    1900 9800
	1    0    0    -1  
$EndComp
$Comp
L C C25
U 1 1 26B211E3
P 2600 9600
F 0 "C25" H 2650 9700 50  0000 L C
F 1 "100nF" H 2650 9500 50  0000 L C
	1    2600 9600
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0114
U 1 1 26B211E8
P 2600 9800
F 0 "#GND0114" H 2600 9900 60  0001 C C
F 1 "GND" H 2600 9700 60  0000 C C
	1    2600 9800
	1    0    0    -1  
$EndComp
$Comp
L C C26
U 1 1 26B211ED
P 3300 9600
F 0 "C26" H 3350 9700 50  0000 L C
F 1 "100nF" H 3350 9500 50  0000 L C
	1    3300 9600
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0115
U 1 1 26B211F2
P 3300 9800
F 0 "#GND0115" H 3300 9900 60  0001 C C
F 1 "GND" H 3300 9700 60  0000 C C
	1    3300 9800
	1    0    0    -1  
$EndComp
$Comp
L C C27
U 1 1 26B211F7
P 4000 9600
F 0 "C27" H 4050 9700 50  0000 L C
F 1 "100nF" H 4050 9500 50  0000 L C
	1    4000 9600
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0116
U 1 1 26B211FC
P 4000 9800
F 0 "#GND0116" H 4000 9900 60  0001 C C
F 1 "GND" H 4000 9700 60  0000 C C
	1    4000 9800
	1    0    0    -1  
$EndComp
$Comp
L C C28
U 1 1 26B21201
P 1900 10400
F 0 "C28" H 1950 10500 50  0000 L C
F 1 "100nF" H 1950 10300 50  0000 L C
	1    1900 10400
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0117
U 1 1 26B21206
P 1900 10600
F 0 "#GND0117" H 1900 10700 60  0001 C C
F 1 "GND" H 1900 10500 60  0000 C C
	1    1900 10600
	1    0    0    -1  
$EndComp
$Comp
L C C29
U 1 1 26B2120B
P 2600 10400
F 0 "C29" H 2650 10500 50  0000 L C
F 1 "100nF" H 2650 10300 50  0000 L C
	1    2600 10400
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0118
U 1 1 26B21210
P 2600 10600
F 0 "#GND0118" H 2600 10700 60  0001 C C
F 1 "GND" H 2600 10500 60  0000 C C
	1    2600 10600
	1    0    0    -1  
$EndComp
$Comp
L C C30
U 1 1 26B21215
P 3300 10400
F 0 "C30" H 3350 10500 50  0000 L C
F 1 "100nF" H 3350 10300 50  0000 L C
	1    3300 10400
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0119
U 1 1 26B2121A
P 3300 10600
F 0 "#GND0119" H 3300 10700 60  0001 C C
F 1 "GND" H 3300 10500 60  0000 C C
	1    3300 10600
	1    0    0    -1  
$EndComp
$Comp
L C C31
U 1 1 26B2121F
P 4000 10400
F 0 "C31" H 4050 10500 50  0000 L C
F 1 "100nF" H 4050 10300 50  0000 L C
	1    4000 10400
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0120
U 1 1 26B21224
P 4000 10600
F 0 "#GND0120" H 4000 10700 60  0001 C C
F 1 "GND" H 4000 10500 60  0000 C C
	1    4000 10600
	1    0    0    -1  
$EndComp
$Comp
L GND #GND0121
U 1 1 2820F094
P 700 9450
F 0 "#GND0121" H 700 9550 60  0001 C C
F 1 "GND" H 700 9350 60  0000 C C
	1    700  9450
	1    0    0    -1  
$EndComp
$Comp
L CP C38
U 1 1 2820F08A
P 700 9150
F 0 "C38" H 750 9250 50  0000 L C
F 1 "4,7uF" H 750 9050 50  0000 L C
	1    700  9150
	1    0    0    -1  
$EndComp
$Comp
L BUSPCI-5V BUS1
U 1 1 269C6109
P 3300 4200
F 0 "BUS1" H 3300 7400 60  0000 C C
F 1 "BUSPCI_5V" H 3300 1000 60  0000 C C
	1    3300 4200
	1    0    0    -1  
$EndComp
$Comp
L 24C16 U1
U 1 1 2F5F7E5C
P 5750 9550
F 0 "U1" H 5900 9900 60  0000 C C
F 1 "24C16" H 5950 9200 60  0000 C C
	1    5750 9550
	1    0    0    -1  
$EndComp
$Comp
L RR8 RR1
U 1 1 C8AF8090
P 14350 1300
F 0 "RR1" H 14400 1850 70  0000 C C
F 1 "8x10K" V 14380 1300 70  0000 C C
	1    14350 1300
	1    0    0    -1  
$EndComp
$Comp
L RR8 RR2
U 1 1 C8B01EF2
P 14350 2300
F 0 "RR2" H 14400 2850 70  0000 C C
F 1 "8x10K" V 14380 2300 70  0000 C C
	1    14350 2300
	1    0    0    -1  
$EndComp
$Comp
L RR8 RR3
U 1 1 C8B23B9F
P 14350 3300
F 0 "RR3" H 14400 3850 70  0000 C C
F 1 "8x10K" V 14380 3300 70  0000 C C
	1    14350 3300
	1    0    0    -1  
$EndComp
$Comp
L RR8 RR4
U 1 1 C8B2B4CE
P 14350 4300
F 0 "RR4" H 14400 4850 70  0000 C C
F 1 "8x10K" V 14380 4300 70  0000 C C
	1    14350 4300
	1    0    0    -1  
$EndComp
$Comp
L RR8 RR5
U 1 1 C8B2B4E3
P 14350 5300
F 0 "RR5" H 14400 5850 70  0000 C C
F 1 "8x10K" V 14380 5300 70  0000 C C
	1    14350 5300
	1    0    0    -1  
$EndComp
$Comp
L RR8 RR6
U 1 1 C93123CE
P 14350 6300
F 0 "RR6" H 14400 6850 70  0000 C C
F 1 "8x10K" V 14380 6300 70  0000 C C
	1    14350 6300
	1    0    0    -1  
$EndComp
$Comp
L RR8 RR7
U 1 1 C931248E
P 14350 7300
F 0 "RR7" H 14400 7850 70  0000 C C
F 1 "8x10K" V 14380 7300 70  0000 C C
	1    14350 7300
	1    0    0    -1  
$EndComp
$Comp
L RR8 RR8
U 1 1 C9363A3F
P 14350 8300
F 0 "RR8" H 14400 8850 70  0000 C C
F 1 "8x10K" V 14380 8300 70  0000 C C
	1    14350 8300
	1    0    0    -1  
$EndComp
$Comp
L TEST W2
U 1 1 26A799F7
P 11900 7650
F 0 "W2" H 11900 7750 60  0000 C C
F 1 "FLOAT#" H 11900 7550 60  0000 C C
	1    11900 7650
	0    -1   -1   0   
$EndComp
$Comp
L TEST W3
U 1 1 26A79A10
P 11900 8550
F 0 "W3" H 11900 8610 40  0000 C C
F 1 "SERNV" H 11900 8480 40  0000 C C
	1    11900 8550
	0    -1   -1   0   
$EndComp
$Comp
L TEST W1
U 1 1 26A799ED
P 12050 6450
F 0 "W1" H 12050 6550 60  0000 C C
F 1 "16/32" H 12050 6350 60  0000 C C
	1    12050 6450
	0    -1   -1   0   
$EndComp
Text GLabel 11700 850  2    60   BiDi
DQ[0..31]
Text GLabel 7100 6650 0    60   BiDi
IRQ_SRL
Text GLabel 7100 7450 0    60   BiDi
X_IRQ
Text GLabel 11200 4350 2    60   Input
PTRDY-
Text GLabel 11200 4450 2    60   Output
PTNUM0
Text GLabel 11200 4550 2    60   Output
PTNUM1
Text GLabel 11300 4850 2    60   Output
PTBE-[0..3]
Text GLabel 10800 5150 2    60   Output
PTWR
Text GLabel 10800 5250 2    60   Output
PTBURST-
Text GLabel 11150 5350 2    60   Input
ADR[2..6]
Text GLabel 11150 5850 2    60   Input
BE-[0..3]
Text GLabel 11400 6450 2    60   Input
SELECT-
Text GLabel 11400 6550 2    60   Input
WR-
Text GLabel 11400 6650 2    60   Input
RD-
Text GLabel 10800 7650 2    60   Output
BPCLK
Text GLabel 10800 7750 2    60   Output
SYSRST-
Text GLabel 10800 7150 2    60   Output
WRFULL
Text GLabel 10800 7250 2    60   Output
RDEMPTY
Text GLabel 11400 6850 2    60   Input
WRFIFO-
Text GLabel 11400 6950 2    60   Input
RDFIFO-
Text GLabel 10800 7550 2    60   Output
IRQ-
Text Label 7600 1050 0    60   ~
P_AD0
Text Label 7600 1150 0    60   ~
P_AD1
Text Label 7600 1250 0    60   ~
P_AD2
Text Label 7600 1350 0    60   ~
P_AD3
Text Label 7600 1450 0    60   ~
P_AD4
Text Label 7600 1550 0    60   ~
P_AD5
Text Label 7600 1650 0    60   ~
P_AD6
Text Label 7600 1750 0    60   ~
P_AD7
Text Label 7600 1850 0    60   ~
P_AD8
Text Label 7600 1950 0    60   ~
P_AD9
Text Label 7600 2050 0    60   ~
P_AD10
Text Label 7600 2150 0    60   ~
P_AD11
Text Label 7600 2250 0    60   ~
P_AD12
Text Label 7600 2350 0    60   ~
P_AD13
Text Label 7600 2450 0    60   ~
P_AD14
Text Label 7600 2550 0    60   ~
P_AD15
Text Label 7600 2650 0    60   ~
P_AD16
Text Label 7600 2750 0    60   ~
P_AD17
Text Label 7600 2850 0    60   ~
P_AD18
Text Label 7600 2950 0    60   ~
P_AD19
Text Label 7600 3050 0    60   ~
P_AD20
Text Label 7600 3150 0    60   ~
P_AD21
Text Label 7600 3250 0    60   ~
P_AD22
Text Label 7600 3350 0    60   ~
P_AD23
Text Label 7600 3450 0    60   ~
P_AD24
Text Label 7600 3550 0    60   ~
P_AD25
Text Label 7600 3650 0    60   ~
P_AD26
Text Label 7600 3750 0    60   ~
P_AD27
Text Label 7600 3850 0    60   ~
P_AD28
Text Label 7600 3950 0    60   ~
P_AD29
Text Label 7600 4050 0    60   ~
P_AD30
Text Label 7600 4150 0    60   ~
P_AD31
Text Label 4800 6850 0    60   ~
P_AD0
Text Label 4800 6750 0    60   ~
P_AD2
Text Label 4800 6550 0    60   ~
P_AD4
Text Label 4800 6450 0    60   ~
P_AD6
Text Label 4800 5950 0    60   ~
P_AD9
Text Label 4800 5750 0    60   ~
P_AD11
Text Label 4800 5650 0    60   ~
P_AD13
Text Label 4800 5450 0    60   ~
P_AD15
Text Label 4800 4250 0    60   ~
P_AD16
Text Label 4800 4150 0    60   ~
P_AD18
Text Label 4800 3950 0    60   ~
P_AD20
Text Label 4800 3850 0    60   ~
P_AD22
Text Label 4800 3550 0    60   ~
P_AD24
Text Label 4800 3350 0    60   ~
P_AD26
Text Label 4800 3250 0    60   ~
P_AD28
Text Label 4800 3050 0    60   ~
P_AD30
Text Label 7500 4250 0    60   ~
P_C/BE0#
Text Label 7500 4350 0    60   ~
P_C/BE1#
Text Label 7500 4450 0    60   ~
P_C/BE2#
Text Label 7500 4550 0    60   ~
P_C/BE3#
Text Label 4800 6250 0    60   ~
P_C/BE0#
Text Label 7600 4650 0    60   ~
P_PAR
Text Label 7600 4750 0    60   ~
P_CLK
Text Label 7600 4850 0    60   ~
P_RST#
Text Label 4800 5350 0    60   ~
P_PAR
Text Label 4800 2550 0    60   ~
P_RST#
Text Label 7500 5050 0    60   ~
P_FRAME#
Text Label 7500 5150 0    60   ~
P_IRDY#
Text Label 7500 5250 0    60   ~
P_TRDY#
Text Label 7500 5350 0    60   ~
P_STOP#
Text Label 7500 5450 0    60   ~
P_LOCK#
Text Label 4800 4450 0    60   ~
P_FRAME#
Text Label 4800 4650 0    60   ~
P_TRDY#
Text Label 4800 4850 0    60   ~
P_STOP#
Text Label 7500 5650 0    60   ~
P_IDSEL
Text Label 7500 5750 0    60   ~
P_DEVSEL#
Text Label 7500 5850 0    60   ~
P_REQ#
Text Label 7500 5950 0    60   ~
P_GNT#
Text Label 7500 6050 0    60   ~
P_PERR#
Text Label 7500 6150 0    60   ~
P_SERR#
Text Label 7500 6350 0    60   ~
P_INTA#
Text Label 4800 3650 0    60   ~
P_IDSEL
Text Label 4800 2750 0    60   ~
P_GNT#
Text Label 4800 1650 0    60   ~
P_INTA#
Text Label 10800 1050 0    60   ~
DQ0
Text Label 10800 1150 0    60   ~
DQ1
Text Label 10800 1250 0    60   ~
DQ2
Text Label 10800 1350 0    60   ~
DQ3
Text Label 10800 1450 0    60   ~
DQ4
Text Label 10800 1550 0    60   ~
DQ5
Text Label 10800 1650 0    60   ~
DQ6
Text Label 10800 1750 0    60   ~
DQ7
Text Label 10800 1850 0    60   ~
DQ8
Text Label 10800 1950 0    60   ~
DQ9
Text Label 10800 2050 0    60   ~
DQ10
Text Label 10800 2150 0    60   ~
DQ11
Text Label 10800 2250 0    60   ~
DQ12
Text Label 10800 2350 0    60   ~
DQ13
Text Label 10800 2450 0    60   ~
DQ14
Text Label 10800 2550 0    60   ~
DQ15
Text Label 10800 2650 0    60   ~
DQ16
Text Label 10800 2750 0    60   ~
DQ17
Text Label 10800 2850 0    60   ~
DQ18
Text Label 10800 2950 0    60   ~
DQ19
Text Label 10800 3050 0    60   ~
DQ20
Text Label 10800 3150 0    60   ~
DQ21
Text Label 10800 3250 0    60   ~
DQ22
Text Label 10800 3350 0    60   ~
DQ23
Text Label 10800 3450 0    60   ~
DQ24
Text Label 10800 3550 0    60   ~
DQ25
Text Label 10800 3650 0    60   ~
DQ26
Text Label 10800 3750 0    60   ~
DQ27
Text Label 10800 3850 0    60   ~
DQ28
Text Label 10800 3950 0    60   ~
DQ29
Text Label 10800 4050 0    60   ~
DQ30
Text Label 10800 4150 0    60   ~
DQ31
Text Label 7800 6750 0    60   ~
EA1
Text Label 7800 6850 0    60   ~
EA2
Text Label 7800 6950 0    60   ~
EA3
Text Label 7800 7050 0    60   ~
EA4
Text Label 7800 7150 0    60   ~
EA5
Text Label 7800 7250 0    60   ~
EA6
Text Label 7800 7350 0    60   ~
EA7
Text Label 7800 7550 0    60   ~
EA9
Text Label 7800 7650 0    60   ~
EA10
Text Label 7800 7750 0    60   ~
EA11
Text Label 7800 7850 0    60   ~
EA12
Text Label 7800 7950 0    60   ~
EA13
Text Label 7800 8050 0    60   ~
EA14
Text Label 7800 8150 0    60   ~
EA15
Text Label 7800 8250 0    60   ~
EQ0
Text Label 7800 8350 0    60   ~
EQ1
Text Label 7800 8450 0    60   ~
EQ2
Text Label 7800 8550 0    60   ~
EQ3
Text Label 7800 8650 0    60   ~
EQ4
Text Label 7800 8750 0    60   ~
EQ5
Text Label 7800 8850 0    60   ~
EQ6
Text Label 7800 8950 0    60   ~
EQ7
Text Label 13500 1150 0    60   ~
EQ1
Text Label 13500 1350 0    60   ~
EQ2
Text Label 13500 1450 0    60   ~
EQ3
Text Label 13500 1650 0    60   ~
EQ4
Text Label 13500 2150 0    60   ~
EQ5
Text Label 13500 1950 0    60   ~
EQ6
Text Label 13500 2650 0    60   ~
EQ7
Text Label 13500 950  0    60   ~
EQ0
Text Label 13500 4450 0    60   ~
DQ16
Text Label 13500 4650 0    60   ~
DQ17
Text Label 13500 6350 0    60   ~
DQ18
Text Label 13500 6050 0    60   ~
DQ20
Text Label 13500 5350 0    60   ~
DQ21
Text Label 13500 4050 0    60   ~
DQ22
Text Label 13500 4250 0    60   ~
DQ23
Text Label 13500 3050 0    60   ~
DQ24
Text Label 13500 3650 0    60   ~
DQ25
Text Label 13500 2550 0    60   ~
DQ26
Text Label 13500 2250 0    60   ~
DQ27
Text Label 13500 2050 0    60   ~
DQ28
Text Label 13500 1550 0    60   ~
DQ29
Text Label 13500 1250 0    60   ~
DQ30
Text Label 13500 1050 0    60   ~
DQ31
Text Label 13400 2450 0    60   ~
IRQ_SRL
Text Label 13400 5150 0    60   ~
X_IRQ
Text Label 13500 2350 0    60   ~
EA1
Text Label 13500 3550 0    60   ~
EA2
Text Label 13500 3350 0    60   ~
EA3
Text Label 13500 2950 0    60   ~
EA4
Text Label 13500 4150 0    60   ~
EA5
Text Label 13500 3950 0    60   ~
EA6
Text Label 13500 5650 0    60   ~
EA7
Text Label 13500 5050 0    60   ~
EA9
Text Label 13500 6650 0    60   ~
EA10
Text Label 13500 6450 0    60   ~
EA11
Text Label 13500 6250 0    60   ~
EA12
Text Label 13500 6150 0    60   ~
EA13
Text Label 13500 4550 0    60   ~
EA14
Text Label 13500 4350 0    60   ~
EA15
Text Label 13500 6950 0    60   ~
DQ0
Text Label 13500 7050 0    60   ~
DQ1
Text Label 13500 7150 0    60   ~
DQ2
Text Label 13500 7250 0    60   ~
DQ3
Text Label 13500 7350 0    60   ~
DQ4
Text Label 13500 7450 0    60   ~
DQ5
Text Label 13500 7550 0    60   ~
DQ6
Text Label 13500 7650 0    60   ~
DQ7
Text Label 13500 8650 0    60   ~
DQ8
Text Label 13500 8550 0    60   ~
DQ9
Text Label 13500 8450 0    60   ~
DQ10
Text Label 13500 8350 0    60   ~
DQ11
Text Label 13500 8250 0    60   ~
DQ12
Text Label 13500 8150 0    60   ~
DQ13
Text Label 13500 8050 0    60   ~
DQ14
Text Label 13500 7950 0    60   ~
DQ15
Text Label 13400 3150 0    60   ~
SELECT-
Text Label 13500 3250 0    60   ~
WR-
Text Label 13500 3450 0    60   ~
RD-
Text Label 13400 5550 0    60   ~
WRFIFO-
Text Label 13400 5450 0    60   ~
RDFIFO-
Text Label 13400 4950 0    60   ~
PTATN-
Text Label 13400 5950 0    60   ~
PTRDY-
Text Label 13400 5250 0    60   ~
PTADR-
Text Label 13500 6550 0    60   ~
DQ19
$Comp
L +5V #+5V0122
U 1 1 00000000
P 6550 8850
F 0 "#+5V0122" H 6550 9050 40  0001 C C
F 1 "+5V" H 6550 9000 40  0000 C C
	1    6550 8850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0123
U 1 1 00000000
P 1900 9400
F 0 "#+5V0123" H 1900 9600 40  0001 C C
F 1 "+5V" H 1900 9550 40  0000 C C
	1    1900 9400
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0124
U 1 1 00000000
P 2600 9400
F 0 "#+5V0124" H 2600 9600 40  0001 C C
F 1 "+5V" H 2600 9550 40  0000 C C
	1    2600 9400
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0125
U 1 1 00000000
P 3300 9400
F 0 "#+5V0125" H 3300 9600 40  0001 C C
F 1 "+5V" H 3300 9550 40  0000 C C
	1    3300 9400
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0126
U 1 1 00000000
P 4000 9400
F 0 "#+5V0126" H 4000 9600 40  0001 C C
F 1 "+5V" H 4000 9550 40  0000 C C
	1    4000 9400
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0127
U 1 1 00000000
P 1900 10200
F 0 "#+5V0127" H 1900 10400 40  0001 C C
F 1 "+5V" H 1900 10350 40  0000 C C
	1    1900 10200
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0128
U 1 1 00000000
P 2600 10200
F 0 "#+5V0128" H 2600 10400 40  0001 C C
F 1 "+5V" H 2600 10350 40  0000 C C
	1    2600 10200
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0129
U 1 1 00000000
P 3300 10200
F 0 "#+5V0129" H 3300 10400 40  0001 C C
F 1 "+5V" H 3300 10350 40  0000 C C
	1    3300 10200
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0130
U 1 1 00000000
P 4000 10200
F 0 "#+5V0130" H 4000 10400 40  0001 C C
F 1 "+5V" H 4000 10350 40  0000 C C
	1    4000 10200
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #+3.3V0131
U 1 1 00000000
P 4500 3150
F 0 "#+3.3V0131" H 4500 3350 40  0001 C C
F 1 "+3.3V" H 4500 3300 40  0000 C C
	1    4500 3150
	0    1    1    0   
$EndComp
$Comp
L +3.3V #+3.3V0132
U 1 1 00000000
P 4500 5550
F 0 "#+3.3V0132" H 4500 5750 40  0001 C C
F 1 "+3.3V" H 4500 5700 40  0000 C C
	1    4500 5550
	0    1    1    0   
$EndComp
$Comp
L +5V #+5V0133
U 1 1 00000000
P 700 8850
F 0 "#+5V0133" H 700 9050 40  0001 C C
F 1 "+5V" H 700 9000 40  0000 C C
	1    700  8850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0134
U 1 1 00000000
P 4900 1150
F 0 "#+5V0134" H 4900 1240 20  0001 C C
F 1 "+5V" H 4900 1240 30  0000 C C
	1    4900 1150
	1    0    0    -1  
$EndComp
$Comp
L +12V #+12V0135
U 1 1 00000000
P 4500 950
F 0 "#+12V0135" H 4500 1150 40  0001 C C
F 1 "+12V" H 4500 1100 40  0000 C C
	1    4500 950 
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0136
U 1 1 00000000
P 14000 850
F 0 "#+5V0136" H 14000 1050 40  0001 C C
F 1 "+5V" H 14000 1000 40  0000 C C
	1    14000 850 
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0137
U 1 1 00000000
P 14000 1850
F 0 "#+5V0137" H 14000 2050 40  0001 C C
F 1 "+5V" H 14000 2000 40  0000 C C
	1    14000 1850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0138
U 1 1 00000000
P 14000 2850
F 0 "#+5V0138" H 14000 3050 40  0001 C C
F 1 "+5V" H 14000 3000 40  0000 C C
	1    14000 2850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0139
U 1 1 00000000
P 14000 3850
F 0 "#+5V0139" H 14000 4050 40  0001 C C
F 1 "+5V" H 14000 4000 40  0000 C C
	1    14000 3850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0140
U 1 1 00000000
P 14000 4850
F 0 "#+5V0140" H 14000 5050 40  0001 C C
F 1 "+5V" H 14000 5000 40  0000 C C
	1    14000 4850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0141
U 1 1 00000000
P 14000 5850
F 0 "#+5V0141" H 14000 6050 40  0001 C C
F 1 "+5V" H 14000 6000 40  0000 C C
	1    14000 5850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0142
U 1 1 00000000
P 14000 6850
F 0 "#+5V0142" H 14000 7050 40  0001 C C
F 1 "+5V" H 14000 7000 40  0000 C C
	1    14000 6850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0143
U 1 1 00000000
P 14000 7850
F 0 "#+5V0143" H 14000 8050 40  0001 C C
F 1 "+5V" H 14000 8000 40  0000 C C
	1    14000 7850
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #+3.3V0144
U 1 1 00000000
P 2100 3550
F 0 "#+3.3V0144" H 2100 3750 40  0001 C C
F 1 "+3.3V" H 2100 3700 40  0000 C C
	1    2100 3550
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #+3.3V0145
U 1 1 00000000
P 2100 4150
F 0 "#+3.3V0145" H 2100 4350 40  0001 C C
F 1 "+3.3V" H 2100 4300 40  0000 C C
	1    2100 4150
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #+3.3V0146
U 1 1 00000000
P 2100 4650
F 0 "#+3.3V0146" H 2100 4850 40  0001 C C
F 1 "+3.3V" H 2100 4800 40  0000 C C
	1    2100 4650
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #+3.3V0147
U 1 1 00000000
P 2100 5150
F 0 "#+3.3V0147" H 2100 5350 40  0001 C C
F 1 "+3.3V" H 2100 5300 40  0000 C C
	1    2100 5150
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #+3.3V0148
U 1 1 00000000
P 2100 5350
F 0 "#+3.3V0148" H 2100 5550 40  0001 C C
F 1 "+3.3V" H 2100 5500 40  0000 C C
	1    2100 5350
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #+3.3V0149
U 1 1 00000000
P 2100 6450
F 0 "#+3.3V0149" H 2100 6650 40  0001 C C
F 1 "+3.3V" H 2100 6600 40  0000 C C
	1    2100 6450
	0    -1   -1   0   
$EndComp
$Comp
L +5V #+5V0150
U 1 1 00000000
P 2000 6950
F 0 "#+5V0150" H 2000 7150 40  0001 C C
F 1 "+5V" H 2000 7100 40  0000 C C
	1    2000 6950
	0    -1   -1   0   
$EndComp
$Comp
L +5V #+5V0151
U 1 1 00000000
P 2100 1650
F 0 "#+5V0151" H 2100 1850 40  0001 C C
F 1 "+5V" H 2100 1800 40  0000 C C
	1    2100 1650
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0152
U 1 1 00000000
P 2100 2950
F 0 "#+5V0152" H 2100 3150 40  0001 C C
F 1 "+5V" H 2100 3100 40  0000 C C
	1    2100 2950
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #+3.3V0153
U 1 1 00000000
P 4500 6350
F 0 "#+3.3V0153" H 4500 6550 40  0001 C C
F 1 "+3.3V" H 4500 6500 40  0000 C C
	1    4500 6350
	0    1    1    0   
$EndComp
$Comp
L +3.3V #+3.3V0154
U 1 1 00000000
P 4500 4950
F 0 "#+3.3V0154" H 4500 5150 40  0001 C C
F 1 "+3.3V" H 4500 5100 40  0000 C C
	1    4500 4950
	0    1    1    0   
$EndComp
$Comp
L +3.3V #+3.3V0155
U 1 1 00000000
P 4500 4350
F 0 "#+3.3V0155" H 4500 4550 40  0001 C C
F 1 "+3.3V" H 4500 4500 40  0000 C C
	1    4500 4350
	0    1    1    0   
$EndComp
$Comp
L +3.3V #+3.3V0156
U 1 1 00000000
P 4500 3750
F 0 "#+3.3V0156" H 4500 3950 40  0001 C C
F 1 "+3.3V" H 4500 3900 40  0000 C C
	1    4500 3750
	0    1    1    0   
$EndComp
$Comp
L +5V #+5V0157
U 1 1 00000000
P 4600 6950
F 0 "#+5V0157" H 4600 7150 40  0001 C C
F 1 "+5V" H 4600 7100 40  0000 C C
	1    4600 6950
	0    1    1    0   
$EndComp
$Comp
L +5V #+5V0158
U 1 1 00000000
P 12850 6150
F 0 "#+5V0158" H 12850 6350 40  0001 C C
F 1 "+5V" H 12850 6300 40  0000 C C
	1    12850 6150
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0159
U 1 1 00000000
P 12700 7350
F 0 "#+5V0159" H 12700 7550 40  0001 C C
F 1 "+5V" H 12700 7500 40  0000 C C
	1    12700 7350
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0160
U 1 1 00000000
P 12700 8250
F 0 "#+5V0160" H 12700 8450 40  0001 C C
F 1 "+5V" H 12700 8400 40  0000 C C
	1    12700 8250
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0161
U 1 1 00000000
P 4500 1850
F 0 "#+5V0161" H 4500 2050 40  0001 C C
F 1 "+5V" H 4500 2000 40  0000 C C
	1    4500 1850
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0162
U 1 1 00000000
P 4500 2050
F 0 "#+5V0162" H 4500 2250 40  0001 C C
F 1 "+5V" H 4500 2200 40  0000 C C
	1    4500 2050
	1    0    0    -1  
$EndComp
$Comp
L +5V #+5V0163
U 1 1 00000000
P 4500 2650
F 0 "#+5V0163" H 4500 2850 40  0001 C C
F 1 "+5V" H 4500 2800 40  0000 C C
	1    4500 2650
	0    1    1    0   
$EndComp
Entry Wire Line
	11500 1050 11600 950 
Entry Wire Line
	11500 1150 11600 1050
Entry Wire Line
	11500 1250 11600 1150
Entry Wire Line
	11500 1350 11600 1250
Entry Wire Line
	11500 1450 11600 1350
Entry Wire Line
	11500 1550 11600 1450
Entry Wire Line
	11500 1650 11600 1550
Entry Wire Line
	11500 1750 11600 1650
Entry Wire Line
	11500 1850 11600 1750
Entry Wire Line
	11500 1950 11600 1850
Entry Wire Line
	11500 2050 11600 1950
Entry Wire Line
	11500 2150 11600 2050
Entry Wire Line
	11500 2250 11600 2150
Entry Wire Line
	11500 2350 11600 2250
Entry Wire Line
	11500 2450 11600 2350
Entry Wire Line
	11500 2550 11600 2450
Connection ~ 1500 2150
Connection ~ 12050 6150
Connection ~ 2100 6950
Connection ~ 4500 6950
Connection ~ 2100 7150
Connection ~ 4500 7150
Connection ~ 11900 7350
Connection ~ 11900 8250
Connection ~ 6550 8950
Connection ~ 6850 8950
Connection ~ 4950 9450
Connection ~ 4950 9550
Connection ~ 6850 9650
Connection ~ 7100 9750
NoConn ~ 4400 1150
NoConn ~ 2200 1250
NoConn ~ 4400 1350
NoConn ~ 2200 1750
NoConn ~ 4400 1750
NoConn ~ 2200 1850
NoConn ~ 4400 1950
NoConn ~ 2200 2050
NoConn ~ 4400 2150
NoConn ~ 2200 2450
NoConn ~ 4400 2450
NoConn ~ 4400 2950
NoConn ~ 4400 5050
NoConn ~ 4400 5150
NoConn ~ 2200 7050
NoConn ~ 4400 7050
Wire Wire Line
	1900 750  4700 750 
Wire Wire Line
	13300 950  14000 950 
Wire Wire Line
	7400 1050 8300 1050
Wire Wire Line
	10600 1050 11500 1050
Wire Wire Line
	13300 1050 14000 1050
Wire Wire Line
	7400 1150 8300 1150
Wire Wire Line
	10600 1150 11500 1150
Wire Wire Line
	13300 1150 14000 1150
Wire Wire Line
	4400 1250 4500 1250
Wire Wire Line
	7400 1250 8300 1250
Wire Wire Line
	10600 1250 11500 1250
Wire Wire Line
	13300 1250 14000 1250
Wire Wire Line
	7400 1350 8300 1350
Wire Wire Line
	10600 1350 11500 1350
Wire Wire Line
	13300 1350 14000 1350
Wire Wire Line
	2200 1450 1900 1450
Wire Wire Line
	4400 1450 4700 1450
Wire Wire Line
	7400 1450 8300 1450
Wire Wire Line
	10600 1450 11500 1450
Wire Wire Line
	13300 1450 14000 1450
Wire Wire Line
	4400 1550 4900 1550
Wire Wire Line
	7400 1550 8300 1550
Wire Wire Line
	10600 1550 11500 1550
Wire Wire Line
	13300 1550 14000 1550
Wire Wire Line
	2200 1650 2100 1650
Wire Wire Line
	4400 1650 5300 1650
Wire Wire Line
	7400 1650 8300 1650
Wire Wire Line
	10600 1650 11500 1650
Wire Wire Line
	13300 1650 14000 1650
Wire Wire Line
	7400 1750 8300 1750
Wire Wire Line
	10600 1750 11500 1750
Wire Wire Line
	4400 1850 4500 1850
Wire Wire Line
	7400 1850 8300 1850
Wire Wire Line
	10600 1850 11500 1850
Wire Wire Line
	1600 1950 1500 1950
Wire Wire Line
	2000 1950 2200 1950
Wire Wire Line
	7400 1950 8300 1950
Wire Wire Line
	10600 1950 11500 1950
Wire Wire Line
	13300 1950 14000 1950
Wire Wire Line
	4400 2050 4500 2050
Wire Wire Line
	7400 2050 8300 2050
Wire Wire Line
	10600 2050 11500 2050
Wire Wire Line
	13300 2050 14000 2050
Wire Wire Line
	1500 2150 1600 2150
Wire Wire Line
	2000 2150 2200 2150
Wire Wire Line
	7400 2150 8300 2150
Wire Wire Line
	10600 2150 11500 2150
Wire Wire Line
	13300 2150 14000 2150
Wire Wire Line
	7400 2250 8300 2250
Wire Wire Line
	10600 2250 11500 2250
Wire Wire Line
	13300 2250 14000 2250
Wire Wire Line
	7400 2350 8300 2350
Wire Wire Line
	10600 2350 11500 2350
Wire Wire Line
	13300 2350 14000 2350
Wire Wire Line
	7400 2450 8300 2450
Wire Wire Line
	10600 2450 11500 2450
Wire Wire Line
	13300 2450 14000 2450
Wire Wire Line
	4400 2550 5300 2550
Wire Wire Line
	7400 2550 8300 2550
Wire Wire Line
	10600 2550 11500 2550
Wire Wire Line
	13300 2550 14000 2550
Wire Wire Line
	4400 2650 4500 2650
Wire Wire Line
	7400 2650 8300 2650
Wire Wire Line
	13300 2650 14000 2650
Wire Wire Line
	4400 2750 5300 2750
Wire Wire Line
	7400 2750 8300 2750
Wire Wire Line
	4400 2850 4500 2850
Wire Wire Line
	7400 2850 8300 2850
Wire Wire Line
	2100 2950 2200 2950
Wire Wire Line
	7400 2950 8300 2950
Wire Wire Line
	13300 2950 14000 2950
Wire Wire Line
	4400 3050 5300 3050
Wire Wire Line
	7400 3050 8300 3050
Wire Wire Line
	13300 3050 14000 3050
Wire Wire Line
	7400 3150 8300 3150
Wire Wire Line
	13300 3150 14000 3150
Wire Wire Line
	4400 3250 5300 3250
Wire Wire Line
	7400 3250 8300 3250
Wire Wire Line
	13300 3250 14000 3250
Wire Wire Line
	4400 3350 5300 3350
Wire Wire Line
	7400 3350 8300 3350
Wire Wire Line
	13300 3350 14000 3350
Wire Wire Line
	4400 3450 4500 3450
Wire Wire Line
	7400 3450 8300 3450
Wire Wire Line
	13300 3450 14000 3450
Wire Wire Line
	2100 3550 2200 3550
Wire Wire Line
	4400 3550 5300 3550
Wire Wire Line
	7400 3550 8300 3550
Wire Wire Line
	13300 3550 14000 3550
Wire Wire Line
	4400 3650 5300 3650
Wire Wire Line
	7400 3650 8300 3650
Wire Wire Line
	13300 3650 14000 3650
Wire Wire Line
	4400 3750 4500 3750
Wire Wire Line
	7400 3750 8300 3750
Wire Wire Line
	4400 3850 5300 3850
Wire Wire Line
	7400 3850 8300 3850
Wire Wire Line
	4400 3950 5300 3950
Wire Wire Line
	7400 3950 8300 3950
Wire Wire Line
	13300 3950 14000 3950
Wire Wire Line
	4400 4050 4500 4050
Wire Wire Line
	7400 4050 8300 4050
Wire Wire Line
	13300 4050 14000 4050
Wire Wire Line
	2100 4150 2200 4150
Wire Wire Line
	4400 4150 5300 4150
Wire Wire Line
	7400 4150 8300 4150
Wire Wire Line
	13300 4150 14000 4150
Wire Wire Line
	4400 4250 5300 4250
Wire Wire Line
	7400 4250 8300 4250
Wire Wire Line
	10600 4250 11200 4250
Wire Wire Line
	13300 4250 14000 4250
Wire Wire Line
	4400 4350 4500 4350
Wire Wire Line
	7400 4350 8300 4350
Wire Wire Line
	10600 4350 11200 4350
Wire Wire Line
	13300 4350 14000 4350
Wire Wire Line
	4400 4450 5300 4450
Wire Wire Line
	7400 4450 8300 4450
Wire Wire Line
	10600 4450 11200 4450
Wire Wire Line
	13300 4450 14000 4450
Wire Wire Line
	4400 4550 4500 4550
Wire Wire Line
	7400 4550 8300 4550
Wire Wire Line
	10600 4550 11200 4550
Wire Wire Line
	13300 4550 14000 4550
Wire Wire Line
	2100 4650 2200 4650
Wire Wire Line
	4400 4650 5300 4650
Wire Wire Line
	7400 4650 8300 4650
Wire Wire Line
	13300 4650 14000 4650
Wire Wire Line
	4400 4750 4500 4750
Wire Wire Line
	7400 4750 8300 4750
Wire Wire Line
	4400 4850 5300 4850
Wire Wire Line
	7400 4850 8300 4850
Wire Wire Line
	4400 4950 4500 4950
Wire Wire Line
	13300 4950 14000 4950
Wire Wire Line
	7400 5050 8300 5050
Wire Wire Line
	13300 5050 14000 5050
Wire Wire Line
	2100 5150 2200 5150
Wire Wire Line
	7400 5150 8300 5150
Wire Wire Line
	10600 5150 10800 5150
Wire Wire Line
	13300 5150 14000 5150
Wire Wire Line
	4400 5250 4500 5250
Wire Wire Line
	7400 5250 8300 5250
Wire Wire Line
	10600 5250 10800 5250
Wire Wire Line
	13300 5250 14000 5250
Wire Wire Line
	2100 5350 2200 5350
Wire Wire Line
	4400 5350 5400 5350
Wire Wire Line
	7400 5350 8300 5350
Wire Wire Line
	13300 5350 14000 5350
Wire Wire Line
	4400 5450 5400 5450
Wire Wire Line
	7400 5450 8300 5450
Wire Wire Line
	12250 6150 11600 6150
Wire Wire Line
	12750 6150 12850 6150
Wire Wire Line
	13300 5450 14000 5450
Wire Wire Line
	13300 5550 14000 5550
Wire Wire Line
	4400 5650 5400 5650
Wire Wire Line
	7400 5650 8300 5650
Wire Wire Line
	13300 5650 14000 5650
Wire Wire Line
	4400 5750 5400 5750
Wire Wire Line
	7400 5750 8300 5750
Wire Wire Line
	4400 5850 4500 5850
Wire Wire Line
	7400 5850 8300 5850
Wire Wire Line
	4400 5950 5400 5950
Wire Wire Line
	7400 5950 8300 5950
Wire Wire Line
	13300 5950 14000 5950
Wire Wire Line
	7400 6050 8300 6050
Wire Wire Line
	13300 6050 14000 6050
Wire Wire Line
	7400 6150 8300 6150
Wire Wire Line
	13300 6150 14000 6150
Wire Wire Line
	4400 6250 5400 6250
Wire Wire Line
	13300 6250 14000 6250
Wire Wire Line
	4400 6350 4500 6350
Wire Wire Line
	7400 6350 8300 6350
Wire Wire Line
	10600 6350 11600 6350
Wire Wire Line
	13300 6350 14000 6350
Wire Wire Line
	2100 6450 2200 6450
Wire Wire Line
	4400 6450 5400 6450
Wire Wire Line
	10600 6450 11400 6450
Wire Wire Line
	13300 6450 14000 6450
Wire Wire Line
	4400 6550 5400 6550
Wire Wire Line
	10600 6550 11400 6550
Wire Wire Line
	13300 6550 14000 6550
Wire Wire Line
	4400 6650 4500 6650
Wire Wire Line
	7100 6650 8300 6650
Wire Wire Line
	10600 6650 11400 6650
Wire Wire Line
	13300 6650 14000 6650
Wire Wire Line
	4400 6750 5400 6750
Wire Wire Line
	7700 6750 8300 6750
Wire Wire Line
	4400 6850 5400 6850
Wire Wire Line
	7700 6850 8300 6850
Wire Wire Line
	10600 6850 11400 6850
Wire Wire Line
	2000 6950 2200 6950
Wire Wire Line
	4400 6950 4600 6950
Wire Wire Line
	7700 6950 8300 6950
Wire Wire Line
	10600 6950 11400 6950
Wire Wire Line
	13300 6950 14000 6950
Wire Wire Line
	7700 7050 8300 7050
Wire Wire Line
	13300 7050 14000 7050
Wire Wire Line
	2100 7150 2200 7150
Wire Wire Line
	4400 7150 4500 7150
Wire Wire Line
	7700 7150 8300 7150
Wire Wire Line
	10600 7150 10800 7150
Wire Wire Line
	13300 7150 14000 7150
Wire Wire Line
	2100 7250 2200 7250
Wire Wire Line
	4500 7250 4400 7250
Wire Wire Line
	7700 7250 8300 7250
Wire Wire Line
	10600 7250 10800 7250
Wire Wire Line
	13300 7250 14000 7250
Wire Wire Line
	7700 7350 8300 7350
Wire Wire Line
	10600 7350 12100 7350
Wire Wire Line
	12600 7350 12700 7350
Wire Wire Line
	13300 7350 14000 7350
Wire Wire Line
	7100 7450 8300 7450
Wire Wire Line
	10600 7450 11600 7450
Wire Wire Line
	13300 7450 14000 7450
Wire Wire Line
	7700 7550 8300 7550
Wire Wire Line
	10600 7550 10800 7550
Wire Wire Line
	13300 7550 14000 7550
Wire Wire Line
	7700 7650 8300 7650
Wire Wire Line
	10600 7650 10800 7650
Wire Wire Line
	13300 7650 14000 7650
Wire Wire Line
	7700 7750 8300 7750
Wire Wire Line
	10600 7750 10800 7750
Wire Wire Line
	7700 7850 8300 7850
Wire Wire Line
	7700 7950 8300 7950
Wire Wire Line
	13300 7950 14000 7950
Wire Wire Line
	7700 8050 8300 8050
Wire Wire Line
	13300 8050 14000 8050
Wire Wire Line
	7700 8150 8300 8150
Wire Wire Line
	13300 8150 14000 8150
Wire Wire Line
	7700 8250 8300 8250
Wire Wire Line
	12100 8250 11600 8250
Wire Wire Line
	12600 8250 12700 8250
Wire Wire Line
	13300 8250 14000 8250
Wire Wire Line
	7700 8350 8300 8350
Wire Wire Line
	13300 8350 14000 8350
Wire Wire Line
	7700 8450 8300 8450
Wire Wire Line
	13300 8450 14000 8450
Wire Wire Line
	7700 8550 8300 8550
Wire Wire Line
	13300 8550 14000 8550
Wire Wire Line
	7700 8650 8300 8650
Wire Wire Line
	13300 8650 14000 8650
Wire Wire Line
	7700 8750 8300 8750
Wire Wire Line
	7700 8850 8300 8850
Wire Wire Line
	6550 8950 7100 8950
Wire Wire Line
	7700 8950 8300 8950
Wire Wire Line
	8300 9050 8000 9050
Wire Wire Line
	8300 9150 8100 9150
Wire Wire Line
	4950 9350 5050 9350
Wire Wire Line
	4950 9450 5050 9450
Wire Wire Line
	6450 9450 6550 9450
Wire Wire Line
	4950 9550 5050 9550
Wire Wire Line
	6450 9650 8000 9650
Wire Wire Line
	6450 9750 8100 9750
Wire Bus Line
	11600 850  11700 850 
Wire Wire Line
	700  8850 700  8950
Wire Wire Line
	700  9350 700  9450
Wire Wire Line
	1500 1950 1500 2250
Wire Wire Line
	1900 1450 1900 750 
Wire Wire Line
	4500 1250 4500 950 
Wire Wire Line
	4700 1450 4700 750 
Wire Wire Line
	4900 1550 4900 1150
Wire Wire Line
	6550 9450 6550 8850
Wire Wire Line
	6850 8950 6850 9050
Wire Wire Line
	6850 9550 6850 9650
Wire Wire Line
	7100 8950 7100 9050
Wire Wire Line
	7100 9550 7100 9750
Wire Wire Line
	8000 9650 8000 9050
Wire Wire Line
	8100 9750 8100 9150
Wire Wire Line
	11600 6350 11600 6150
Wire Wire Line
	11600 8250 11600 7450
Wire Wire Line
	12050 6150 12050 6250
Wire Wire Line
	11900 7350 11900 7450
Wire Wire Line
	11900 7850 11900 7950
Wire Wire Line
	11900 8250 11900 8350
Wire Wire Line
	11900 8750 11900 8850
$EndSCHEMATC
