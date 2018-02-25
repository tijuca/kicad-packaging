EESchema Schematic File Version 4
LIBS:ecc83-pp_v2-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "ECC Push-Pull"
Date "Sat 21 Mar 2015"
Rev "0.1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ecc83_schlib:ECC83 U1
U 3 1 48B4F266
P 3900 3050
F 0 "U1" H 4200 3150 50  0000 C CNN
F 1 "ECC83" H 4250 3050 50  0000 C CNN
F 2 "" H 4400 2950 30  0000 C CNN
F 3 "" H 3900 3050 60  0001 C CNN
	3    3900 3050
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:ECC83 U1
U 2 1 48B4F263
P 6500 4200
F 0 "U1" H 7000 4300 50  0000 C CNN
F 1 "ECC83" H 7000 4200 50  0000 C CNN
F 2 "" H 7000 4100 30  0000 C CNN
F 3 "" H 6500 4200 60  0001 C CNN
	2    6500 4200
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:ECC83 U1
U 1 1 48B4F256
P 6600 2300
F 0 "U1" H 7100 2400 50  0000 C CNN
F 1 "ECC83" H 7100 2300 50  0000 C CNN
F 2 "Valve:Valve_ECC-83-2" H 7100 2200 30  0000 C CNN
F 3 "" H 6600 2300 60  0001 C CNN
	1    6600 2300
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:GND #PWR01
U 1 1 457DBAF8
P 7850 3250
F 0 "#PWR01" H 7850 3250 30  0001 C CNN
F 1 "GND" H 7850 3180 30  0001 C CNN
F 2 "" H 7850 3250 60  0001 C CNN
F 3 "" H 7850 3250 60  0001 C CNN
	1    7850 3250
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:GND #PWR02
U 1 1 457DBAF5
P 7600 3800
F 0 "#PWR02" H 7600 3800 30  0001 C CNN
F 1 "GND" H 7600 3730 30  0001 C CNN
F 2 "" H 7600 3800 60  0001 C CNN
F 3 "" H 7600 3800 60  0001 C CNN
	1    7600 3800
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:GND #PWR03
U 1 1 457DBAF1
P 6400 5500
F 0 "#PWR03" H 6400 5500 30  0001 C CNN
F 1 "GND" H 6400 5430 30  0001 C CNN
F 2 "" H 6400 5500 60  0001 C CNN
F 3 "" H 6400 5500 60  0001 C CNN
	1    6400 5500
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:GND #PWR04
U 1 1 457DBAEF
P 6000 5500
F 0 "#PWR04" H 6000 5500 30  0001 C CNN
F 1 "GND" H 6000 5430 30  0001 C CNN
F 2 "" H 6000 5500 60  0001 C CNN
F 3 "" H 6000 5500 60  0001 C CNN
	1    6000 5500
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:PWR_FLAG #FLG05
U 1 1 457DBAC0
P 3700 2200
F 0 "#FLG05" H 3700 2470 30  0001 C CNN
F 1 "PWR_FLAG" H 3700 2430 30  0000 C CNN
F 2 "" H 3700 2200 60  0001 C CNN
F 3 "" H 3700 2200 60  0001 C CNN
	1    3700 2200
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:CONN_2 P4
U 1 1 456A8ACC
P 3050 3700
F 0 "P4" V 3000 3700 40  0000 C CNN
F 1 "CONN_2" V 3100 3700 40  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3050 3500 30  0000 C CNN
F 3 "" H 3050 3700 60  0001 C CNN
	1    3050 3700
	-1   0    0    -1  
$EndComp
$Comp
L ecc83_schlib:CP C1
U 1 1 4549F4BE
P 4100 2100
F 0 "C1" H 4250 2150 50  0000 L CNN
F 1 "10uF" H 4250 2050 50  0000 L CNN
F 2 "discret:C2V10" H 4300 1900 30  0000 C CNN
F 3 "" H 4100 2100 60  0001 C CNN
	1    4100 2100
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:CONN_2 P3
U 1 1 4549F4A5
P 3050 1900
F 0 "P3" V 3000 1900 40  0000 C CNN
F 1 "POWER" V 3100 1900 40  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3050 1700 30  0000 C CNN
F 3 "" H 3050 1900 60  0001 C CNN
	1    3050 1900
	-1   0    0    -1  
$EndComp
$Comp
L ecc83_schlib:CONN_2 P2
U 1 1 4549F46C
P 8350 3000
F 0 "P2" V 8300 3000 40  0000 C CNN
F 1 "OUT" V 8400 3000 40  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 8350 2800 30  0000 C CNN
F 3 "" H 8350 3000 60  0001 C CNN
	1    8350 3000
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:CONN_2 P1
U 1 1 4549F464
P 5350 4300
F 0 "P1" V 5300 4300 40  0000 C CNN
F 1 "IN" V 5400 4300 40  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 5350 4500 30  0000 C CNN
F 3 "" H 5350 4300 60  0001 C CNN
	1    5350 4300
	-1   0    0    1   
$EndComp
$Comp
L ecc83_schlib:C C2
U 1 1 4549F3BE
P 7300 2900
F 0 "C2" V 7150 2900 50  0000 C CNN
F 1 "680nF" V 7450 2900 50  0000 C CNN
F 2 "Capacitors_THT:C_Axial_L12.0mm_D6.5mm_P20.00mm_Horizontal" V 7500 2900 10  0000 C CNN
F 3 "" H 7300 2900 60  0001 C CNN
	1    7300 2900
	0    1    1    0   
$EndComp
$Comp
L ecc83_schlib:R R3
U 1 1 4549F3AD
P 7600 3550
F 0 "R3" H 7500 3750 50  0000 C CNN
F 1 "100K" V 7600 3550 50  0000 C CNN
F 2 "discret:R3" V 7700 3550 30  0000 C CNN
F 3 "" H 7600 3550 60  0001 C CNN
	1    7600 3550
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:R R4
U 1 1 4549F3A2
P 6000 5300
F 0 "R4" H 5900 5500 50  0000 C CNN
F 1 "47K" V 6000 5300 50  0000 C CNN
F 2 "discret:R3" V 6100 5300 30  0000 C CNN
F 3 "" H 6000 5300 60  0001 C CNN
	1    6000 5300
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:R R2
U 1 1 4549F39D
P 6400 5300
F 0 "R2" H 6300 5500 50  0000 C CNN
F 1 "1.5K" V 6400 5300 50  0000 C CNN
F 2 "discret:R3" V 6500 5300 30  0000 C CNN
F 3 "" H 6400 5300 60  0001 C CNN
	1    6400 5300
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:R R1
U 1 1 4549F38A
P 6500 3250
F 0 "R1" H 6600 3050 50  0000 C CNN
F 1 "1.5K" V 6500 3250 50  0000 C CNN
F 2 "discret:R3" V 6400 3250 30  0000 C CNN
F 3 "" H 6500 3250 60  0001 C CNN
	1    6500 3250
	-1   0    0    1   
$EndComp
$Comp
L ecc83_schlib:CONN_1 P5
U 1 1 54A5830A
P 6300 6600
F 0 "P5" H 6380 6600 40  0000 L CNN
F 1 "CONN_1" H 6300 6655 30  0001 C CNN
F 2 "connect:1pin" H 6300 6600 60  0001 C CNN
F 3 "" H 6300 6600 60  0000 C CNN
	1    6300 6600
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:CONN_1 P6
U 1 1 54A58363
P 6300 6700
F 0 "P6" H 6380 6700 40  0000 L CNN
F 1 "CONN_1" H 6300 6755 30  0001 C CNN
F 2 "connect:1pin" H 6300 6700 60  0001 C CNN
F 3 "" H 6300 6700 60  0000 C CNN
	1    6300 6700
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:CONN_1 P7
U 1 1 54A5837A
P 6300 6800
F 0 "P7" H 6380 6800 40  0000 L CNN
F 1 "CONN_1" H 6300 6855 30  0001 C CNN
F 2 "connect:1pin" H 6300 6800 60  0001 C CNN
F 3 "" H 6300 6800 60  0000 C CNN
	1    6300 6800
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:CONN_1 P8
U 1 1 54A58391
P 6300 6900
F 0 "P8" H 6380 6900 40  0000 L CNN
F 1 "CONN_1" H 6300 6955 30  0001 C CNN
F 2 "connect:1pin" H 6300 6900 60  0001 C CNN
F 3 "" H 6300 6900 60  0000 C CNN
	1    6300 6900
	1    0    0    -1  
$EndComp
NoConn ~ 6150 6600
NoConn ~ 6150 6700
NoConn ~ 6150 6800
NoConn ~ 6150 6900
$Comp
L ecc83_schlib:PWR_FLAG #FLG06
U 1 1 550E41D9
P 3700 1700
F 0 "#FLG06" H 3700 1970 30  0001 C CNN
F 1 "PWR_FLAG" H 3700 1930 30  0000 C CNN
F 2 "" H 3700 1700 60  0001 C CNN
F 3 "" H 3700 1700 60  0001 C CNN
	1    3700 1700
	1    0    0    -1  
$EndComp
$Comp
L ecc83_schlib:GND #PWR07
U 1 1 550E42C4
P 4100 2500
F 0 "#PWR07" H 4100 2500 30  0001 C CNN
F 1 "GND" H 4100 2430 30  0001 C CNN
F 2 "" H 4100 2500 60  0000 C CNN
F 3 "" H 4100 2500 60  0000 C CNN
	1    4100 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 3800 3900 3800
Wire Wire Line
	3900 3800 3900 3500
Wire Wire Line
	6400 4600 6400 5150
Wire Wire Line
	7450 2900 7600 2900
Wire Wire Line
	3400 3600 3800 3600
Wire Wire Line
	3800 3600 3800 3500
Wire Wire Line
	6500 3400 6500 3600
Connection ~ 3800 3600
Wire Wire Line
	4000 3600 4000 3500
Connection ~ 6500 3600
Wire Wire Line
	6500 2700 6500 2900
Wire Wire Line
	6500 3600 6100 3600
Wire Wire Line
	6100 3600 6100 2300
Wire Wire Line
	6100 2300 6300 2300
Connection ~ 6500 2900
Wire Wire Line
	3400 1800 3700 1800
Wire Wire Line
	6600 1800 6600 1900
Wire Wire Line
	3700 1700 3700 1800
Connection ~ 3700 1800
Wire Wire Line
	3400 2000 3500 2000
Wire Wire Line
	3500 2000 3500 2400
Wire Wire Line
	3500 2400 3700 2400
Connection ~ 3700 2400
Wire Wire Line
	3700 2400 3700 2200
Wire Wire Line
	4100 2250 4100 2400
Connection ~ 4100 2400
Wire Wire Line
	4100 1950 4100 1800
Connection ~ 4100 1800
Wire Wire Line
	6500 2900 7150 2900
Connection ~ 7600 2900
Wire Wire Line
	7600 3700 7600 3800
Wire Wire Line
	7600 3400 7600 2900
Wire Wire Line
	8000 3100 7850 3100
Wire Wire Line
	7850 3100 7850 3250
Wire Wire Line
	5700 4200 6000 4200
Wire Wire Line
	6000 5150 6000 4200
Connection ~ 6000 4200
$Comp
L ecc83_schlib:GND #PWR08
U 1 1 550E4C85
P 5800 4550
F 0 "#PWR08" H 5800 4550 30  0001 C CNN
F 1 "GND" H 5800 4480 30  0001 C CNN
F 2 "" H 5800 4550 60  0000 C CNN
F 3 "" H 5800 4550 60  0000 C CNN
	1    5800 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 4550 5800 4400
Wire Wire Line
	5800 4400 5700 4400
Wire Wire Line
	6400 5450 6400 5500
Wire Wire Line
	6000 5450 6000 5500
Wire Wire Line
	3800 3600 4000 3600
Wire Wire Line
	6500 3600 6500 3800
Wire Wire Line
	6500 2900 6500 3100
Wire Wire Line
	3700 1800 4100 1800
Wire Wire Line
	3700 2400 4100 2400
Wire Wire Line
	4100 2400 4100 2500
Wire Wire Line
	4100 1800 6600 1800
Wire Wire Line
	7600 2900 8000 2900
Wire Wire Line
	6000 4200 6200 4200
$EndSCHEMATC
