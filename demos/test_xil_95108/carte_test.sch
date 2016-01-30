EESchema Schematic File Version 1
LIBS:74xx,adc-dac,device,special,intel,cmos4000,conn,memory,linear,motorola,xilinx,power,philips,regul,.\carte_test.cache
EELAYER 23  0
EELAYER END
$Descr A3 16535 11700
Sheet 1 1
Title "CARTE d'EXPERIMENTATION a XC95108PC84"
Date "5 jan 2004"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 11000 1600
Wire Wire Line
	10600 1600 11700 1600
Wire Wire Line
	11000 1550 11000 1600
Connection ~ 11150 1850
Wire Wire Line
	4500 2500 4500 2100
Wire Wire Line
	4300 2600 4300 2100
Wire Wire Line
	4800 2600 4300 2600
Wire Wire Line
	4100 2400 4100 2100
Wire Wire Line
	4800 2400 4100 2400
Wire Wire Line
	3900 2700 4800 2700
Wire Wire Line
	3900 2100 3900 2700
Wire Wire Line
	3450 2100 3700 2100
Connection ~ 3800 2150
Connection ~ 4000 2150
Connection ~ 4200 2150
Wire Wire Line
	4000 2100 4000 2150
Wire Wire Line
	4200 2100 4200 2150
Wire Wire Line
	4400 2150 4400 2100
Wire Wire Line
	3800 2150 4400 2150
Wire Wire Line
	3800 2200 3800 2100
$Comp
L DB9 J2
U 1 1 3ECDEBEA
P 4100 1650
F 0 "J2" V 4100 2250 70  0000 C C
F 1 "DB9FEM" V 3950 2400 70  0000 C C
	1    4100 1650
	0    1    -1   0   
$EndComp
$Comp
L PWR_FLAG #FLG3
U 1 1 3ECDDD9A
P 2500 7850
F 0 "#FLG3" H 2500 8120 30  0001 C C
F 1 "PWR_FLAG" H 2500 8080 30  0000 C C
	1    2500 7850
	1    0    0    -1  
$EndComp
Connection ~ 2350 8050
Wire Wire Line
	2100 8350 2350 8350
Wire Wire Line
	2100 8450 2100 8350
Wire Wire Line
	1900 8050 1750 8050
Wire Wire Line
	2350 8350 2350 8050
Wire Wire Line
	2500 8050 2500 8100
Wire Wire Line
	2300 8050 2500 8050
Wire Wire Line
	2500 7850 2850 7850
Wire Wire Line
	1750 7850 2100 7850
$Comp
L DIODE D4
U 1 1 3ECDDCDD
P 2100 8050
F 0 "D4" H 2100 8150 40  0000 C C
F 1 "1N4004" H 2100 7951 40  0000 C C
	1    2100 8050
	-1   0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG2
U 1 1 3ECDDCBE
P 2100 8450
F 0 "#FLG2" H 2100 8720 30  0001 C C
F 1 "PWR_FLAG" H 2100 8680 30  0000 C C
	1    2100 8450
	1    0    0    1   
$EndComp
$Comp
L GND #PWR38
U 1 1 3ECDDCB7
P 2350 8750
F 0 "#PWR38" H 2350 8750 40  0001 C C
F 1 "GND" H 2350 8680 40  0001 C C
	1    2350 8750
	1    0    0    -1  
$EndComp
$Comp
L CP C13
U 1 1 3ECDDCAD
P 2350 8550
F 0 "C13" H 2400 8650 50  0000 L C
F 1 "10uF" H 2400 8450 50  0000 L C
	1    2350 8550
	1    0    0    1   
$EndComp
$Comp
L -12V #PWR4
U 1 1 3ECDDC9F
P 2500 8100
F 0 "#PWR4" H 2500 8300 40  0001 C C
F 1 "-12V" H 2500 8250 40  0000 C C
	1    2500 8100
	1    0    0    1   
$EndComp
Connection ~ 12300 4200
Connection ~ 12400 3850
NoConn ~ 13400 4200
NoConn ~ 13400 4100
NoConn ~ 13400 3900
$Comp
L -12V #PWR3
U 1 1 3ECDDC1A
P 12800 3600
F 0 "#PWR3" H 12800 3800 40  0001 C C
F 1 "-12V" H 12800 3750 40  0000 C C
	1    12800 3600
	1    0    0    -1  
$EndComp
$Comp
L +12V #PWR2
U 1 1 3ECDDC05
P 12800 4400
F 0 "#PWR2" H 12800 4600 40  0001 C C
F 1 "+12V" H 12800 4550 40  0000 C C
	1    12800 4400
	1    0    0    1   
$EndComp
Wire Wire Line
	2800 7900 2800 7800
$Comp
L +12V #PWR1
U 1 1 3ECDDBF2
P 2800 7800
F 0 "#PWR1" H 2800 8000 40  0001 C C
F 1 "+12V" H 2800 7950 40  0000 C C
	1    2800 7800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR37
U 1 1 3ECDDBE0
P 12300 4750
F 0 "#PWR37" H 12300 4750 40  0001 C C
F 1 "GND" H 12300 4680 40  0001 C C
	1    12300 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	12400 4200 12400 4100
Wire Wire Line
	12250 4200 12400 4200
Wire Wire Line
	12300 4200 12300 4250
Wire Wire Line
	13500 3300 13500 4000
Wire Wire Line
	13000 3300 13500 3300
Wire Wire Line
	12400 3300 12500 3300
Wire Wire Line
	12400 3900 12400 3300
Wire Wire Line
	12250 3850 12400 3850
Wire Wire Line
	11600 3850 11750 3850
Wire Wire Line
	14000 4000 14100 4000
Wire Wire Line
	13400 4000 13500 4000
$Comp
L LM318N U6
U 1 1 3ECDDB8B
P 12900 4000
F 0 "U6" H 13000 4300 60  0000 C C
F 1 "LM318N" H 13110 3751 60  0000 C C
	1    12900 4000
	1    0    0    1   
$EndComp
$Comp
L R R10
U 1 1 3ECDDB6D
P 13750 4000
F 0 "R10" V 13830 4000 50  0000 C C
F 1 "75" V 13750 4000 50  0000 C C
	1    13750 4000
	0    1    1    0   
$EndComp
$Comp
L R R7
U 1 1 3ECDDB60
P 12750 3300
F 0 "R7" V 12830 3300 50  0000 C C
F 1 "1,1K" V 12750 3300 50  0000 C C
	1    12750 3300
	0    1    1    0   
$EndComp
$Comp
L R R6
U 1 1 3ECDDB57
P 12300 4500
F 0 "R6" V 12380 4500 50  0000 C C
F 1 "1,1k" V 12300 4500 50  0000 C C
	1    12300 4500
	1    0    0    -1  
$EndComp
$Comp
L R R9
U 1 1 3ECDDB4B
P 12000 4200
F 0 "R9" V 12080 4200 50  0000 C C
F 1 "2,2K" V 12000 4200 50  0000 C C
	1    12000 4200
	0    1    1    0   
$EndComp
$Comp
L R R8
U 1 1 3ECDDB43
P 12000 3850
F 0 "R8" V 12080 3850 50  0000 C C
F 1 "2,2K" V 12000 3850 50  0000 C C
	1    12000 3850
	0    1    1    0   
$EndComp
$Comp
L CP C14
U 1 1 3ECDDACB
P 11400 3850
F 0 "C14" H 11450 3950 50  0000 L C
F 1 "10uF" H 11450 3750 50  0000 L C
	1    11400 3850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4150 3000 4800 3000
Wire Wire Line
	4150 2900 4800 2900
Text Label 4150 3000 0    60   ~
PIN_TEST1
Text Label 4150 2900 0    60   ~
PIN_TEST0
Connection ~ 13600 1600
Wire Wire Line
	13650 1600 13600 1600
Wire Wire Line
	13600 1500 13650 1500
Wire Wire Line
	13600 1650 13600 1500
$Comp
L GND #PWR36
U 1 1 3ECB3F8B
P 13600 1650
F 0 "#PWR36" H 13600 1650 40  0001 C C
F 1 "GND" H 13600 1580 40  0001 C C
	1    13600 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	14450 1600 15100 1600
Wire Wire Line
	14450 1500 15100 1500
Text Label 14500 1600 0    60   ~
PIN_TEST1
Text Label 14500 1500 0    60   ~
PIN_TEST0
$Comp
L CONN_2X2 P4
U 1 1 3ECB3F5E
P 14050 1550
F 0 "P4" H 14050 1700 50  0000 C C
F 1 "CONN_2X2" H 14060 1420 40  0000 C C
	1    14050 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 3400 4800 3400
Text Label 3900 3400 0    60   ~
PAR_AUX7/INIT
$Comp
L GND #PWR35
U 1 1 3ECB3A8F
P 1950 5000
F 0 "#PWR35" H 1950 5000 40  0001 C C
F 1 "GND" H 1950 4932 40  0001 C C
	1    1950 5000
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR34
U 1 1 3ECB3A8D
P 1950 4800
F 0 "#PWR34" H 1950 4800 40  0001 C C
F 1 "GND" H 1950 4732 40  0001 C C
	1    1950 4800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3700 3200 4800 3200
Text Label 3800 3200 0    60   ~
PAR_AUX8/SELECT
Wire Wire Line
	1950 5200 3050 5200
$Comp
L GND #PWR33
U 1 1 3ECB3A50
P 1950 4400
F 0 "#PWR33" H 1950 4400 40  0001 C C
F 1 "GND" H 1950 4332 40  0001 C C
	1    1950 4400
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR32
U 1 1 3ECB3A4D
P 1950 4200
F 0 "#PWR32" H 1950 4200 40  0001 C C
F 1 "GND" H 1950 4132 40  0001 C C
	1    1950 4200
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR31
U 1 1 3ECB3A4B
P 1950 4000
F 0 "#PWR31" H 1950 4000 40  0001 C C
F 1 "GND" H 1950 3932 40  0001 C C
	1    1950 4000
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR30
U 1 1 3ECB3A48
P 1950 3800
F 0 "#PWR30" H 1950 3800 40  0001 C C
F 1 "GND" H 1950 3732 40  0001 C C
	1    1950 3800
	0    -1   -1   0   
$EndComp
$Comp
L DB25 J1
U 1 1 3EC4C60F
P 1500 4700
F 0 "J1" H 1550 6050 70  0000 C C
F 1 "DB25" H 1450 3350 70  0000 C C
	1    1500 4700
	-1   0    0    -1  
$EndComp
NoConn ~ 4800 4900
NoConn ~ 4800 4800
NoConn ~ 4800 4700
NoConn ~ 4800 4600
NoConn ~ 4800 4500
NoConn ~ 4800 4400
NoConn ~ 4800 4300
NoConn ~ 4800 4200
Wire Wire Line
	1950 5400 2950 5400
Wire Wire Line
	1950 4100 3000 4100
Wire Wire Line
	1950 3900 3000 3900
Wire Wire Line
	1950 3700 2850 3700
Wire Wire Line
	7100 3400 8200 3400
Wire Wire Line
	1950 5900 3100 5900
Wire Wire Line
	1950 5800 3050 5800
Wire Wire Line
	1950 5600 3050 5600
Text Label 2050 5200 0    60   ~
PAR_AUX8/SELECT
Text Label 2050 5400 0    60   ~
PAR_AUX7/INIT
$Comp
L XC95108PC84 U2
U 1 1 3EC230AD
P 5950 4750
F 0 "U2" H 6500 7300 60  0000 L C
F 1 "XC95108PC84" H 6350 2800 60  0000 L C
	1    5950 4750
	1    0    0    -1  
$EndComp
NoConn ~ 7100 4700
NoConn ~ 7100 5200
Wire Wire Line
	7550 5500 7100 5500
Wire Wire Line
	11150 1850 11150 1550
Wire Wire Line
	10600 1850 11700 1850
Text Label 10600 1850 0    60   ~
SW1
Text Label 10600 1600 0    60   ~
SW0
$Comp
L VCC #PWR50
U 1 1 3EC8B981
P 11150 1050
F 0 "#PWR50" H 11150 1250 40  0001 C C
F 1 "VCC" H 11150 1200 40  0000 C C
	1    11150 1050
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR49
U 1 1 3EC8B97E
P 11000 1050
F 0 "#PWR49" H 11000 1250 40  0001 C C
F 1 "VCC" H 11000 1200 40  0000 C C
	1    11000 1050
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 3EC8B978
P 11000 1300
F 0 "R5" V 11080 1300 50  0000 C C
F 1 "4,7K" V 11000 1300 50  0000 C C
	1    11000 1300
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 3EC8B970
P 11150 1300
F 0 "R4" V 11230 1300 50  0000 C C
F 1 "4,7K" V 11150 1300 50  0000 C C
	1    11150 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 7900 3750 7850
Wire Wire Line
	7450 4000 7100 4000
Wire Wire Line
	7450 3900 7100 3900
Text Label 7150 4000 0    60   ~
SW1
Text Label 7150 3900 0    60   ~
SW0
$Comp
L GND #PWR29
U 1 1 3EC8B890
P 12300 1850
F 0 "#PWR29" H 12300 1850 40  0001 C C
F 1 "GND" H 12300 1780 40  0001 C C
	1    12300 1850
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR28
U 1 1 3EC8B88C
P 12300 1600
F 0 "#PWR28" H 12300 1600 40  0001 C C
F 1 "GND" H 12300 1530 40  0001 C C
	1    12300 1600
	0    -1   -1   0   
$EndComp
$Comp
L SW_PUSH SW2
U 1 1 3EC8B882
P 12000 1850
F 0 "SW2" H 12000 1700 50  0000 C C
F 1 "SW_PUSH" H 12000 1770 50  0000 C C
	1    12000 1850
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 3EC8B876
P 12000 1600
F 0 "SW1" H 12150 1750 50  0000 C C
F 1 "SW_PUSH" H 11800 1750 50  0000 C C
	1    12000 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 3600 3700 3600
Text Label 3750 3600 0    60   ~
PAR_AUX1/ERROR*
Wire Wire Line
	3700 4000 4800 4000
Text Label 3750 4000 0    60   ~
PAR_AUX0/STROBE*
Wire Wire Line
	7100 3300 8200 3300
Wire Wire Line
	7100 3500 8200 3500
Wire Wire Line
	7100 3700 8200 3700
Text Label 7150 3300 0    60   ~
PAR_AUX5/BUSY*
Text Label 7150 3500 0    60   ~
PAR_AUX4/PE
Text Label 7150 3700 0    60   ~
PAR_AUX3/SELECT*
$Comp
L GND #PWR27
U 1 1 3EC4F034
P 1950 4600
F 0 "#PWR27" H 1950 4600 40  0001 C C
F 1 "GND" H 1950 4532 40  0001 C C
	1    1950 4600
	0    -1   -1   0   
$EndComp
Text Label 7150 3800 0    60   ~
LED2
Text Label 7150 3600 0    60   ~
LED1
Wire Wire Line
	7450 3800 7100 3800
Wire Wire Line
	7450 3600 7100 3600
Text Label 10650 2600 0    60   ~
LED2
Text Label 10650 2300 0    60   ~
LED1
Wire Wire Line
	11000 2600 10650 2600
Wire Wire Line
	11000 2300 10650 2300
$Comp
L GND #PWR26
U 1 1 3EC4D095
P 11900 2600
F 0 "#PWR26" H 11900 2600 40  0001 C C
F 1 "GND" H 11900 2530 40  0001 C C
	1    11900 2600
	0    -1   -1   0   
$EndComp
$Comp
L LED D3
U 1 1 3EC4D08F
P 11700 2600
F 0 "D3" H 11700 2700 50  0000 C C
F 1 "LED" H 11700 2500 50  0000 C C
	1    11700 2600
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 3EC4D082
P 11250 2600
F 0 "R3" V 11330 2600 50  0000 C C
F 1 "330" V 11250 2600 50  0000 C C
	1    11250 2600
	0    1    1    0   
$EndComp
$Comp
L GND #PWR25
U 1 1 3EC4D095
P 11900 2300
F 0 "#PWR25" H 11900 2300 40  0001 C C
F 1 "GND" H 11900 2230 40  0001 C C
	1    11900 2300
	0    -1   -1   0   
$EndComp
$Comp
L LED D2
U 1 1 3EC4D08F
P 11700 2300
F 0 "D2" H 11700 2400 50  0000 C C
F 1 "LED" H 11700 2200 50  0000 C C
	1    11700 2300
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 3EC4D082
P 11250 2300
F 0 "R2" V 11330 2300 50  0000 C C
F 1 "330" V 11250 2300 50  0000 C C
	1    11250 2300
	0    1    1    0   
$EndComp
$Comp
L GND #PWR24
U 1 1 3EC4C7F6
P 4700 9450
F 0 "#PWR24" H 4700 9450 40  0001 C C
F 1 "GND" H 4700 9380 40  0001 C C
	1    4700 9450
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR46
U 1 1 3EC4C7EA
P 4700 9050
F 0 "#PWR46" H 4700 9250 40  0001 C C
F 1 "VCC" H 4700 9200 40  0000 C C
	1    4700 9050
	1    0    0    -1  
$EndComp
$Comp
L C C12
U 1 1 3EC4C7DB
P 4700 9250
F 0 "C12" H 4750 9350 50  0000 L C
F 1 "100nF" H 4750 9150 50  0000 L C
	1    4700 9250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR23
U 1 1 3EC4C7F6
P 12350 7950
F 0 "#PWR23" H 12350 7950 40  0001 C C
F 1 "GND" H 12350 7880 40  0001 C C
	1    12350 7950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR44
U 1 1 3EC4C7EA
P 12350 7550
F 0 "#PWR44" H 12350 7750 40  0001 C C
F 1 "VCC" H 12350 7700 40  0000 C C
	1    12350 7550
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 3EC4C7DB
P 12350 7750
F 0 "C7" H 12400 7850 50  0000 L C
F 1 "100nF" H 12400 7650 50  0000 L C
	1    12350 7750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR22
U 1 1 3EC4C7F6
P 7950 2100
F 0 "#PWR22" H 7950 2100 40  0001 C C
F 1 "GND" H 7950 2030 40  0001 C C
	1    7950 2100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR47
U 1 1 3EC4C7EA
P 7950 1700
F 0 "#PWR47" H 7950 1900 40  0001 C C
F 1 "VCC" H 7950 1850 40  0000 C C
	1    7950 1700
	1    0    0    -1  
$EndComp
$Comp
L C C11
U 1 1 3EC4C7DB
P 7950 1900
F 0 "C11" H 8000 2000 50  0000 L C
F 1 "100nF" H 8000 1800 50  0000 L C
	1    7950 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR20
U 1 1 3EC4C7F6
P 7600 2100
F 0 "#PWR20" H 7600 2100 40  0001 C C
F 1 "GND" H 7600 2030 40  0001 C C
	1    7600 2100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR48
U 1 1 3EC4C7EA
P 7600 1700
F 0 "#PWR48" H 7600 1900 40  0001 C C
F 1 "VCC" H 7600 1850 40  0000 C C
	1    7600 1700
	1    0    0    -1  
$EndComp
$Comp
L C C10
U 1 1 3EC4C7DB
P 7600 1900
F 0 "C10" H 7650 2000 50  0000 L C
F 1 "100nF" H 7650 1800 50  0000 L C
	1    7600 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR21
U 1 1 3EC4C7F6
P 7150 2100
F 0 "#PWR21" H 7150 2100 40  0001 C C
F 1 "GND" H 7150 2030 40  0001 C C
	1    7150 2100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR45
U 1 1 3EC4C7EA
P 7150 1700
F 0 "#PWR45" H 7150 1900 40  0001 C C
F 1 "VCC" H 7150 1850 40  0000 C C
	1    7150 1700
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 3EC4C7DB
P 7150 1900
F 0 "C8" H 7200 2000 50  0000 L C
F 1 "100nF" H 7200 1800 50  0000 L C
	1    7150 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR19
U 1 1 3EC4C76B
P 1950 3600
F 0 "#PWR19" H 1950 3600 40  0001 C C
F 1 "GND" H 1950 3532 40  0001 C C
	1    1950 3600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2650 3500 1950 3500
Text Label 2050 4100 0    60   ~
PAR_AUX6/ACK
Text Label 2050 3900 0    60   ~
PAR_AUX5/BUSY*
Text Label 2050 3700 0    60   ~
PAR_AUX4/PE
Text Label 2050 3500 0    60   ~
PAR_AUX3/SELECT*
Text Label 2050 5800 0    60   ~
PAR_AUX2/AUTOLF*
Text Label 2050 5600 0    60   ~
PAR_AUX1/ERROR*
Text Label 2050 5900 0    60   ~
PAR_AUX0/STROBE*
Wire Wire Line
	2650 5700 1950 5700
Wire Wire Line
	2650 5500 1950 5500
Wire Wire Line
	2650 5300 1950 5300
Wire Wire Line
	2650 5100 1950 5100
Wire Wire Line
	2650 4900 1950 4900
Wire Wire Line
	2650 4700 1950 4700
Wire Wire Line
	2650 4500 1950 4500
Wire Wire Line
	2650 4300 1950 4300
Text Label 2050 4300 0    60   ~
PARBUS7
Text Label 2050 4500 0    60   ~
PARBUS6
Text Label 2050 4700 0    60   ~
PARBUS5
Text Label 2050 4900 0    60   ~
PARBUS4
Text Label 2050 5100 0    60   ~
PARBUS3
Text Label 2050 5300 0    60   ~
PARBUS2
Text Label 2050 5500 0    60   ~
PARBUS1
Text Label 2050 5700 0    60   ~
PARBUS0
Wire Wire Line
	4800 3800 3700 3800
Text Label 7150 3400 0    60   ~
PAR_AUX6/ACK
Text Label 3750 3800 0    60   ~
PAR_AUX2/AUTOLF*
Text Label 7150 3100 0    60   ~
PARBUS7
Text Label 7150 3200 0    60   ~
PARBUS6
Text Label 7150 3000 0    60   ~
PARBUS5
Text Label 7150 2900 0    60   ~
PARBUS4
Text Label 4200 3100 0    60   ~
PARBUS3
Text Label 4200 3300 0    60   ~
PARBUS2
Text Label 4200 3500 0    60   ~
PARBUS1
Text Label 4200 3700 0    60   ~
PARBUS0
Wire Wire Line
	7700 3100 7100 3100
Wire Wire Line
	7700 3200 7100 3200
Wire Wire Line
	7700 3000 7100 3000
Wire Wire Line
	7700 2900 7100 2900
Wire Wire Line
	4800 3100 4200 3100
Wire Wire Line
	4800 3300 4200 3300
Wire Wire Line
	4800 3500 4200 3500
Wire Wire Line
	4800 3700 4200 3700
Text Label 7200 5800 0    60   ~
MD13
Text Label 7250 5600 0    60   ~
MD12
Text Label 7200 6500 0    60   ~
MD11
Text Label 4350 6300 0    60   ~
MD10
Text Label 7200 6300 0    60   ~
MD9
Text Label 7200 6000 0    60   ~
MD8
Text Label 7200 5900 0    60   ~
MD7
Text Label 7200 6100 0    60   ~
MD6
Text Label 7200 6200 0    60   ~
MD5
Text Label 7200 6400 0    60   ~
MD4
Text Label 4350 6500 0    60   ~
MD3
Text Label 4350 6200 0    60   ~
MD2
Text Label 4350 6000 0    60   ~
MD1
Text Label 4350 5800 0    60   ~
MD0
Wire Wire Line
	7650 5800 7100 5800
Wire Wire Line
	7550 5600 7100 5600
Wire Wire Line
	7650 6500 7100 6500
Wire Wire Line
	4800 6300 4250 6300
Wire Wire Line
	7650 6300 7100 6300
Wire Wire Line
	7650 6000 7100 6000
Wire Wire Line
	7650 5900 7100 5900
Wire Wire Line
	7650 6100 7100 6100
Wire Wire Line
	7650 6200 7100 6200
Wire Wire Line
	7650 6400 7100 6400
Wire Wire Line
	4800 6500 4250 6500
Wire Wire Line
	4800 6200 4250 6200
Wire Wire Line
	4800 6000 4250 6000
Wire Wire Line
	4800 5800 4250 5800
Text Label 7250 5500 0    60   ~
MD14
Text Label 7150 5700 0    60   ~
R/W-A14
Text Label 4350 6400 0    60   ~
OEMEM
Text Label 4350 6100 0    60   ~
CSMEM
Wire Wire Line
	7650 5700 7100 5700
Wire Wire Line
	4800 6400 4250 6400
Wire Wire Line
	4800 6100 4250 6100
Text Label 4500 5900 0    60   ~
MA7
Text Label 4500 5700 0    60   ~
MA6
Text Label 4500 5300 0    60   ~
MA5
Text Label 4500 5000 0    60   ~
MA4
Text Label 4500 5100 0    60   ~
MA3
Text Label 4500 5200 0    60   ~
MA2
Text Label 4500 5500 0    60   ~
MA1
Text Label 4500 5600 0    60   ~
MA0
Wire Wire Line
	4350 5900 4800 5900
Wire Wire Line
	4350 5700 4800 5700
Wire Wire Line
	4350 5300 4800 5300
Wire Wire Line
	4350 5000 4800 5000
Wire Wire Line
	4350 5100 4800 5100
Wire Wire Line
	4350 5200 4800 5200
Wire Wire Line
	4350 5500 4800 5500
Wire Wire Line
	4350 5600 4800 5600
Text Label 10200 7850 0    60   ~
R/W-A14
Text Label 10250 7750 0    60   ~
OEMEM
Text Label 10250 7650 0    60   ~
CSMEM
Wire Wire Line
	9700 7550 9400 7550
Text Label 9450 7550 0    60   ~
MD14
Text Label 10250 7350 0    60   ~
MD13
Text Label 10250 7250 0    60   ~
MD12
Text Label 10250 7150 0    60   ~
MD11
Text Label 10250 7050 0    60   ~
MD10
Text Label 10250 6950 0    60   ~
MD9
Text Label 10250 6850 0    60   ~
MD8
Text Label 10250 6750 0    60   ~
MD7
Text Label 10250 6650 0    60   ~
MD6
Text Label 10250 6550 0    60   ~
MD5
Text Label 10250 6450 0    60   ~
MD4
Text Label 10250 6350 0    60   ~
MD3
Text Label 10250 6250 0    60   ~
MD2
Text Label 10250 6150 0    60   ~
MD1
Text Label 10250 6050 0    60   ~
MD0
Text Label 12250 6750 0    60   ~
MA7
Text Label 12250 6650 0    60   ~
MA6
Text Label 12250 6550 0    60   ~
MA5
Text Label 12250 6450 0    60   ~
MA4
Text Label 12250 6350 0    60   ~
MA3
Text Label 12250 6250 0    60   ~
MA2
Text Label 12250 6150 0    60   ~
MA1
Text Label 12250 6050 0    60   ~
MA0
$Comp
L VCC #PWR43
U 1 1 3EC4C4B5
P 9900 7550
F 0 "#PWR43" H 9900 7750 40  0001 C C
F 1 "VCC" H 9900 7700 40  0000 C C
	1    9900 7550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 7450 9800 7550
Wire Wire Line
	10700 7450 9800 7450
$Comp
L CONN_3 K1
U 1 1 3EC4C49F
P 9800 7900
F 0 "K1" V 9750 7900 50  0000 C C
F 1 "CONN_3" V 9850 7900 40  0000 C C
	1    9800 7900
	0    1    1    0   
$EndComp
$Comp
L RAM_32KO U5
U 1 1 3EC4C463
P 11400 6850
F 0 "U5" H 11550 6550 70  0000 C C
F 1 "RAM_32KO" H 11500 6150 60  0000 C C
	1    11400 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10700 7850 10150 7850
Wire Wire Line
	10700 7750 10150 7750
Wire Wire Line
	10700 7650 10150 7650
Wire Wire Line
	10700 7350 10150 7350
Wire Wire Line
	10700 7250 10150 7250
Wire Wire Line
	10700 7150 10150 7150
Wire Wire Line
	10700 7050 10150 7050
Wire Wire Line
	10700 6950 10150 6950
Wire Wire Line
	10700 6850 10150 6850
Wire Wire Line
	10700 6750 10150 6750
Wire Wire Line
	10700 6650 10150 6650
Wire Wire Line
	10700 6550 10150 6550
Wire Wire Line
	10700 6450 10150 6450
Wire Wire Line
	10700 6350 10150 6350
Wire Wire Line
	10700 6250 10150 6250
Wire Wire Line
	10700 6150 10150 6150
Wire Wire Line
	10700 6050 10150 6050
Wire Wire Line
	12100 6750 12550 6750
Wire Wire Line
	12100 6650 12550 6650
Wire Wire Line
	12100 6550 12550 6550
Wire Wire Line
	12100 6450 12550 6450
Wire Wire Line
	12100 6350 12550 6350
Wire Wire Line
	12100 6250 12550 6250
Wire Wire Line
	12100 6150 12550 6150
Wire Wire Line
	12100 6050 12550 6050
Wire Wire Line
	4200 3900 4800 3900
Text Label 4300 3900 0    60   ~
XIN/CLK
Connection ~ 5150 9450
Connection ~ 6300 9450
Wire Wire Line
	5900 9450 6300 9450
Connection ~ 6300 8900
Wire Wire Line
	6300 9850 6300 8900
Wire Wire Line
	5850 9850 6300 9850
Wire Wire Line
	7650 8900 8250 8900
Wire Wire Line
	6200 8900 6750 8900
Wire Wire Line
	5150 8900 5300 8900
Wire Wire Line
	5150 9900 5150 8900
NoConn ~ 3100 10300
NoConn ~ 2000 10750
NoConn ~ 2000 10300
NoConn ~ 2000 9850
Connection ~ 1050 10750
Connection ~ 1050 10300
Wire Wire Line
	1100 10750 1050 10750
Wire Wire Line
	1100 10300 1050 10300
Wire Wire Line
	1050 9850 1100 9850
Wire Wire Line
	1050 10850 1050 9850
Wire Wire Line
	2200 10350 2200 10300
$Comp
L GND #PWR18
U 1 1 3EC4C328
P 2200 10350
F 0 "#PWR18" H 2200 10350 40  0001 C C
F 1 "GND" H 2200 10280 40  0001 C C
	1    2200 10350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR17
U 1 1 3EC4C322
P 1050 10850
F 0 "#PWR17" H 1050 10850 40  0001 C C
F 1 "GND" H 1050 10780 40  0001 C C
	1    1050 10850
	1    0    0    -1  
$EndComp
$Comp
L 74HC04 U4
U 6 1 3EC4C318
P 2650 10300
F 0 "U4" H 2845 10415 60  0000 C C
F 1 "74HC04" H 2840 10176 60  0000 C C
	6    2650 10300
	1    0    0    -1  
$EndComp
$Comp
L 74HC04 U4
U 5 1 3EC4C311
P 1550 10750
F 0 "U4" H 1745 10865 60  0000 C C
F 1 "74HC04" H 1740 10626 60  0000 C C
	5    1550 10750
	1    0    0    -1  
$EndComp
$Comp
L 74HC04 U4
U 4 1 3EC4C30C
P 1550 10300
F 0 "U4" H 1745 10415 60  0000 C C
F 1 "74HC04" H 1740 10176 60  0000 C C
	4    1550 10300
	1    0    0    -1  
$EndComp
$Comp
L 74HC04 U4
U 3 1 3EC4C30A
P 1550 9850
F 0 "U4" H 1745 9965 60  0000 C C
F 1 "74HC04" H 1740 9726 60  0000 C C
	3    1550 9850
	1    0    0    -1  
$EndComp
$Comp
L 74HC04 U4
U 2 1 3EC4C300
P 7200 8900
F 0 "U4" H 7395 9015 60  0000 C C
F 1 "74HC04" H 7390 8776 60  0000 C C
	2    7200 8900
	1    0    0    -1  
$EndComp
$Comp
L 74HC04 U4
U 1 1 3EC4C2F6
P 5750 8900
F 0 "U4" H 5945 9015 60  0000 C C
F 1 "74HC04" H 5940 8776 60  0000 C C
	1    5750 8900
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG1
U 1 1 3EC24491
P 3100 8700
F 0 "#FLG1" H 3100 8970 30  0001 C C
F 1 "PWR_FLAG" H 3100 8930 30  0000 C C
	1    3100 8700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 8750 3100 8700
$Comp
L GND #PWR14
U 1 1 3EC243BD
P 3100 8750
F 0 "#PWR14" H 3100 8750 40  0001 C C
F 1 "GND" H 3100 8680 40  0001 C C
	1    3100 8750
	1    0    0    -1  
$EndComp
Wire Wire Line
	11750 4200 11600 4200
$Comp
L GND #PWR9
U 1 1 3EC23A35
P 14100 4250
F 0 "#PWR9" H 14100 4250 40  0001 C C
F 1 "GND" H 14100 4180 40  0001 C C
	1    14100 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	14100 4250 14100 4200
$Comp
L CONN_2 P3
U 1 1 3EC2446D
P 14450 4100
F 0 "P3" V 14400 4100 40  0000 C C
F 1 "CONN_2" V 14500 4100 40  0000 C C
	1    14450 4100
	1    0    0    -1  
$EndComp
Connection ~ 2800 7850
Connection ~ 6050 2000
Connection ~ 5750 2000
Connection ~ 5850 2000
Connection ~ 6100 6900
Connection ~ 6000 6900
Connection ~ 5900 6900
Connection ~ 5800 6900
$Comp
L GND #PWR16
U 1 1 3EC243DC
P 2800 8300
F 0 "#PWR16" H 2800 8300 40  0001 C C
F 1 "GND" H 2800 8230 40  0001 C C
	1    2800 8300
	1    0    0    -1  
$EndComp
$Comp
L CP C6
U 1 1 3EC243D6
P 2800 8100
F 0 "C6" H 2850 8200 50  0000 L C
F 1 "47uF" H 2850 8000 50  0000 L C
	1    2800 8100
	1    0    0    -1  
$EndComp
$Comp
L DIODE D1
U 1 1 3EC243C8
P 2300 7850
F 0 "D1" H 2300 7950 40  0000 C C
F 1 "1N4004" H 2300 7750 40  0000 C C
	1    2300 7850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR15
U 1 1 3EC243BD
P 1750 7950
F 0 "#PWR15" H 1750 7950 40  0001 C C
F 1 "GND" H 1750 7881 40  0001 C C
	1    1750 7950
	0    -1   -1   0   
$EndComp
$Comp
L CONN_3 P2
U 1 1 3EC243AC
P 1400 7950
F 0 "P2" V 1352 7950 40  0000 C C
F 1 "CONN_3" V 1450 7950 40  0000 C C
	1    1400 7950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3650 7850 3750 7850
$Comp
L VCC #PWR42
U 1 1 3EC24382
P 3750 7850
F 0 "#PWR42" H 3750 8050 40  0001 C C
F 1 "VCC" H 3750 8000 40  0000 C C
	1    3750 7850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR13
U 1 1 3EC2437A
P 3250 8150
F 0 "#PWR13" H 3250 8150 40  0001 C C
F 1 "GND" H 3250 8080 40  0001 C C
	1    3250 8150
	1    0    0    -1  
$EndComp
$Comp
L LM7805 U3
U 1 1 3EC2436D
P 3250 7900
F 0 "U3" H 3400 7704 60  0000 C C
F 1 "LM7805" H 3250 8100 60  0000 C C
	1    3250 7900
	1    0    0    -1  
$EndComp
Text Label 7750 8900 0    60   ~
XIN/CLK
Connection ~ 5950 9850
Wire Wire Line
	5950 9900 5950 9850
$Comp
L GND #PWR6
U 1 1 3EC23A2D
P 5950 10350
F 0 "#PWR6" H 5950 10350 40  0001 C C
F 1 "GND" H 5950 10280 40  0001 C C
	1    5950 10350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 10350 5950 10300
$Comp
L C C4
U 1 1 3EC242AD
P 5950 10100
F 0 "C4" H 6000 10200 50  0000 C C
F 1 "22pF" H 6000 10000 50  0000 C C
	1    5950 10100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR7
U 1 1 3EC23A2D
P 5150 10350
F 0 "#PWR7" H 5150 10350 40  0001 C C
F 1 "GND" H 5150 10280 40  0001 C C
	1    5150 10350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 10350 5150 10300
Connection ~ 5150 9850
Wire Wire Line
	5350 9850 5150 9850
Wire Wire Line
	5150 9450 5300 9450
$Comp
L R R1
U 1 1 3EC242C1
P 5600 9850
F 0 "R1" V 5680 9850 50  0000 C C
F 1 "100K" V 5600 9850 50  0000 C C
	1    5600 9850
	0    1    1    0   
$EndComp
$Comp
L C C5
U 1 1 3EC242AD
P 5150 10100
F 0 "C5" H 5200 10200 50  0000 L C
F 1 "22pF" H 5200 10000 50  0000 L C
	1    5150 10100
	1    0    0    -1  
$EndComp
$Comp
L CRYSTAL X1
U 1 1 3EC2428D
P 5600 9450
F 0 "X1" H 5600 9600 60  0000 C C
F 1 "10MHz" H 5600 9300 60  0000 C C
	1    5600 9450
	1    0    0    -1  
$EndComp
Text Label 4550 2400 0    60   ~
TDI
Text Label 4550 2500 0    60   ~
TMS
Text Label 4550 2600 0    60   ~
TCK
Text Label 4550 2700 0    60   ~
TDO
Wire Wire Line
	4800 2500 4500 2500
$Comp
L GND #PWR12
U 1 1 3EC23E6E
P 3800 2200
F 0 "#PWR12" H 3800 2200 40  0001 C C
F 1 "GND" H 3800 2130 40  0001 C C
	1    3800 2200
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR41
U 1 1 3EC239FE
P 3450 2000
F 0 "#PWR41" H 3450 2200 40  0001 C C
F 1 "VCC" H 3450 2150 40  0000 C C
	1    3450 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 2100 3450 2000
$Comp
L GND #PWR11
U 1 1 3EC23DE5
P 3750 8300
F 0 "#PWR11" H 3750 8300 40  0001 C C
F 1 "GND" H 3750 8230 40  0001 C C
	1    3750 8300
	1    0    0    -1  
$EndComp
$Comp
L CP C3
U 1 1 3EC23DCD
P 3750 8100
F 0 "C3" H 3800 8200 50  0000 L C
F 1 "10uF" H 3800 8000 50  0000 L C
	1    3750 8100
	1    0    0    -1  
$EndComp
$Comp
L CP C2
U 1 1 3EC23B98
P 11400 4200
F 0 "C2" H 11450 4300 50  0000 L C
F 1 "10uF" H 11450 4100 50  0000 L C
	1    11400 4200
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR10
U 1 1 3EC23ADD
P 9500 5150
F 0 "#PWR10" H 9500 5150 40  0001 C C
F 1 "GND" H 9500 5080 40  0001 C C
	1    9500 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 4450 9500 4750
Wire Wire Line
	9600 4450 9500 4450
$Comp
L C C1
U 1 1 3EC23ABA
P 9500 4950
F 0 "C1" H 9550 5050 50  0000 L C
F 1 "100nF" H 9550 4850 50  0000 L C
	1    9500 4950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR8
U 1 1 3EC23A35
P 10500 5000
F 0 "#PWR8" H 10500 5000 40  0001 C C
F 1 "GND" H 10500 4930 40  0001 C C
	1    10500 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR5
U 1 1 3EC23A2D
P 6200 6950
F 0 "#PWR5" H 6200 6950 40  0001 C C
F 1 "GND" H 6200 6880 40  0001 C C
	1    6200 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 6900 5700 6900
Wire Wire Line
	6200 6950 6200 6900
Wire Wire Line
	10500 5000 10500 4950
Wire Wire Line
	10300 4950 10500 4950
Wire Wire Line
	10500 3150 10300 3150
$Comp
L VCC #PWR39
U 1 1 3EC239FE
P 10500 3050
F 0 "#PWR39" H 10500 3250 40  0001 C C
F 1 "VCC" H 10500 3200 40  0000 C C
	1    10500 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	10500 3150 10500 3050
Connection ~ 6050 2000
Connection ~ 5850 2000
Connection ~ 5750 2000
$Comp
L VCC #PWR40
U 1 1 3EC239FE
P 6150 1900
F 0 "#PWR40" H 6150 2100 40  0001 C C
F 1 "VCC" H 6150 2050 40  0000 C C
	1    6150 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 2000 6150 1900
Wire Wire Line
	5650 2000 6150 2000
Text Label 7150 4500 0    60   ~
DCLK
Wire Wire Line
	7400 4500 7100 4500
Text Label 9150 4650 0    60   ~
DCLK
Wire Wire Line
	9600 4650 9150 4650
Text Label 7150 4900 0    60   ~
DAT7
Text Label 7150 5100 0    60   ~
DAT6
Text Label 7150 5000 0    60   ~
DAT5
Text Label 7150 4800 0    60   ~
DAT4
Text Label 7150 4400 0    60   ~
DAT3
Text Label 7150 4200 0    60   ~
DAT2
Text Label 7150 4600 0    60   ~
DAT1
Text Label 7150 4300 0    60   ~
DAT0
Wire Wire Line
	7400 4900 7100 4900
Wire Wire Line
	7400 5100 7100 5100
Wire Wire Line
	7400 5000 7100 5000
Wire Wire Line
	7400 4800 7100 4800
Wire Wire Line
	7400 4400 7100 4400
Wire Wire Line
	7400 4200 7100 4200
Wire Wire Line
	7400 4600 7100 4600
Wire Wire Line
	7400 4300 7100 4300
Text Label 9300 4150 0    60   ~
DAT7
Text Label 9300 4050 0    60   ~
DAT6
Text Label 9300 3950 0    60   ~
DAT5
Text Label 9300 3850 0    60   ~
DAT4
Text Label 9300 3750 0    60   ~
DAT3
Text Label 9300 3650 0    60   ~
DAT2
Text Label 9300 3550 0    60   ~
DAT1
Text Label 9300 3450 0    60   ~
DAT0
Wire Wire Line
	9600 4150 9300 4150
Wire Wire Line
	9600 4050 9300 4050
Wire Wire Line
	9600 3950 9300 3950
Wire Wire Line
	9600 3850 9300 3850
Wire Wire Line
	9600 3750 9300 3750
Wire Wire Line
	9600 3650 9300 3650
Wire Wire Line
	9600 3550 9300 3550
Wire Wire Line
	9600 3450 9300 3450
$Comp
L TDA8702 U1
U 1 1 3EC238E5
P 10400 4050
F 0 "U1" H 10600 4850 60  0000 L C
F 1 "TDA8702" H 10600 3250 60  0000 L C
	1    10400 4050
	1    0    0    -1  
$EndComp
$EndSCHEMATC
