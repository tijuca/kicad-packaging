EESchema Schematic File Version 1
LIBS:pspice,.\pspice.cache
EELAYER 20  0
EELAYER END
$Descr A4 11700 8267
Sheet 1 1
Title ""
Date "5 oct 2005"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	5300 2800 4400 2800
Text Label 4500 2800 0    60   ~
VIN
Text Label 8350 2100 0    60   ~
VOUT
Connection ~ 9200 1800
Wire Wire Line
	8700 1800 9200 1800
Wire Wire Line
	7800 1800 8200 1800
Wire Wire Line
	7800 1500 7800 1800
$Comp
L R R12
U 1 1 3E9D53DA
P 8450 1800
F 0 "R12" V 8530 1800 50  0000 C C
F 1 "22K" V 8450 1800 50  0000 C C
	1    8450 1800
	0    1    1    0   
$EndComp
$Comp
L R R11
U 1 1 3E9D4D47
P 7700 1250
F 0 "R11" V 7780 1250 50  0000 C C
F 1 "100" V 7700 1250 50  0000 C C
	1    7700 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 1500 7700 1500
Wire Wire Line
	9200 2100 7700 2100
Wire Wire Line
	9200 1500 9200 2100
$Comp
L INDUCTOR L1
U 1 1 3E9D4A5D
P 8500 1500
F 0 "L1" H 8500 1600 70  0000 C C
F 1 "100mH" H 8500 1400 70  0000 C C
	1    8500 1500
	1    0    0    1   
$EndComp
Text Notes 3500 1200 0    60   ~
+gnucap .plot ac v(nodes) (-1,5)
Connection ~ 7700 2800
Wire Wire Line
	8250 2800 8400 2800
Wire Wire Line
	7750 2800 7450 2800
$Comp
L R R10
U 1 1 3E9D3DED
P 8000 2800
F 0 "R10" V 8080 2800 50  0000 C C
F 1 "220" V 8000 2800 50  0000 C C
	1    8000 2800
	0    1    1    0   
$EndComp
$Comp
L 0 #GND8
U 1 1 3E9D3DE2
P 8400 3300
F 0 "#GND8" H 8400 3200 40  0001 C C
F 1 "0" H 8400 3230 40  0000 C C
	1    8400 3300
	1    0    0    -1  
$EndComp
$Comp
L CAP C3
U 1 1 3E9D3DD3
P 8400 3050
F 0 "C3" V 8500 3200 50  0000 C C
F 1 "10uF" V 8500 2900 50  0000 C C
	1    8400 3050
	1    0    0    -1  
$EndComp
Text Notes 3500 1000 0    60   ~
-PSPICE .DC V1 10 12 0.5
Connection ~ 7050 2450
Wire Wire Line
	7050 2600 7050 2450
$Comp
L 0 #GND7
U 1 1 3E9D1EFF
P 7050 3100
F 0 "#GND7" H 7050 3000 40  0001 C C
F 1 "0" H 7050 3030 40  0000 C C
	1    7050 3100
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 3E9D1EF8
P 7050 2850
F 0 "C2" V 7150 3000 50  0000 C C
F 1 "1nF" V 7150 2700 50  0000 C C
	1    7050 2850
	1    0    0    -1  
$EndComp
Kmarq B 2000 5150 "Warning Pin power_in non pilot�e (Net 5)" F=1
Kmarq B 7700 3450 "Warning Pin power_in non pilot�e (Net 4)" F=1
Connection ~ 5750 3200
Connection ~ 6100 3200
Wire Wire Line
	6350 3200 6350 3150
Wire Wire Line
	5500 3200 6350 3200
Wire Wire Line
	5750 3200 5750 3150
Wire Wire Line
	5500 3150 5500 3200
Wire Wire Line
	3900 2800 3850 2800
Wire Wire Line
	7700 2950 7700 2800
$Comp
L 0 #GND6
U 1 1 32E8B852
P 7700 3450
F 0 "#GND6" H 7700 3350 40  0001 C C
F 1 "0" H 7700 3380 40  0000 C C
	1    7700 3450
	1    0    0    -1  
$EndComp
$Comp
L +12V #+12V6
U 1 1 32E8B84A
P 7700 1000
F 0 "#+12V6" H 7700 1200 40  0001 C C
F 1 "+12V" H 7700 1150 40  0000 C C
	1    7700 1000
	1    0    0    -1  
$EndComp
$Comp
L R R8
U 1 1 32E8B80E
P 7700 3200
F 0 "R8" V 7780 3200 50  0000 C C
F 1 "2.2K" V 7700 3200 50  0000 C C
	1    7700 3200
	1    0    0    -1  
$EndComp
$Comp
L QNPN Q3
U 1 1 32E8B7FC
P 7550 2450
F 0 "Q3" H 7450 2750 50  0000 C C
F 1 "Q2N2222" H 7450 2650 50  0000 C C
	1    7550 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2450 7250 2450
Text Notes 3500 800  0    60   ~
-PSPICE .model Q2N2222 npn (bf=200)
$Comp
L 0 #GND5
U 1 1 32CFC469
P 3850 4200
F 0 "#GND5" H 3850 4100 40  0001 C C
F 1 "0" H 3850 4130 40  0000 C C
	1    3850 4200
	1    0    0    -1  
$EndComp
$Comp
L VSOURCE V2
U 1 1 32CFC454
P 3850 3500
F 0 "V2" H 3850 3400 60  0000 C C
F 1 "AC 0.1" H 3850 3600 60  0000 C C
	1    3850 3500
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 32CFC413
P 4150 2800
F 0 "C1" V 4250 2950 50  0000 C C
F 1 "1UF" V 4250 2650 50  0000 C C
	1    4150 2800
	0    1    1    0   
$EndComp
Text Notes 3500 900  0    60   ~
-gnucap .AC 10 1Meg *1.2
Text Notes 3500 1100 0    60   ~
+PSPICE .print ac v(vout)
$Comp
L +12V #+12V5
U 1 1 32CFC34D
P 2000 5150
F 0 "#+12V5" H 2000 5350 40  0001 C C
F 1 "+12V" H 2000 5300 40  0000 C C
	1    2000 5150
	1    0    0    -1  
$EndComp
$Comp
L 0 #GND4
U 1 1 32CFC344
P 2000 6550
F 0 "#GND4" H 2000 6450 40  0001 C C
F 1 "0" H 2000 6480 40  0000 C C
	1    2000 6550
	1    0    0    -1  
$EndComp
$Comp
L VSOURCE V1
U 1 1 32CFC337
P 2000 5850
F 0 "V1" H 2000 5750 60  0000 C C
F 1 "DC 12V" H 2000 5950 60  0000 C C
	1    2000 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2450 6100 2300
Wire Wire Line
	5750 2450 5750 2300
$Comp
L +12V #+12V4
U 1 1 32CFC303
P 6700 1800
F 0 "#+12V4" H 6700 2000 40  0001 C C
F 1 "+12V" H 6700 1950 40  0000 C C
	1    6700 1800
	1    0    0    -1  
$EndComp
$Comp
L +12V #+12V3
U 1 1 32CFC2FF
P 6100 1800
F 0 "#+12V3" H 6100 2000 40  0001 C C
F 1 "+12V" H 6100 1950 40  0000 C C
	1    6100 1800
	1    0    0    -1  
$EndComp
$Comp
L +12V #+12V2
U 1 1 32CFC2FD
P 5750 1800
F 0 "#+12V2" H 5750 2000 40  0001 C C
F 1 "+12V" H 5750 1950 40  0000 C C
	1    5750 1800
	1    0    0    -1  
$EndComp
$Comp
L +12V #+12V1
U 1 1 32CFC2F7
P 5100 1800
F 0 "#+12V1" H 5100 2000 40  0001 C C
F 1 "+12V" H 5100 1950 40  0000 C C
	1    5100 1800
	1    0    0    -1  
$EndComp
Connection ~ 5100 2800
Connection ~ 6700 2800
Wire Wire Line
	6550 2800 6700 2800
Wire Wire Line
	6700 2300 6700 3300
Wire Wire Line
	6100 3300 6100 3150
Wire Wire Line
	5100 2300 5100 3300
$Comp
L 0 #GND3
U 1 1 32CFC2B8
P 6700 3800
F 0 "#GND3" H 6700 3700 40  0001 C C
F 1 "0" H 6700 3730 40  0000 C C
	1    6700 3800
	1    0    0    -1  
$EndComp
$Comp
L 0 #GND2
U 1 1 32CFC2B5
P 5100 3800
F 0 "#GND2" H 5100 3700 40  0001 C C
F 1 "0" H 5100 3730 40  0000 C C
	1    5100 3800
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 32CFC293
P 5100 3550
F 0 "R2" V 5180 3550 50  0000 C C
F 1 "10K" V 5100 3550 50  0000 C C
	1    5100 3550
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 32CFC288
P 5100 2050
F 0 "R6" V 5180 2050 50  0000 C C
F 1 "22K" V 5100 2050 50  0000 C C
	1    5100 2050
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 32CFC27F
P 6700 2050
F 0 "R5" V 6780 2050 50  0000 C C
F 1 "22K" V 6700 2050 50  0000 C C
	1    6700 2050
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 32CFC277
P 6700 3550
F 0 "R1" V 6780 3550 50  0000 C C
F 1 "10K" V 6700 3550 50  0000 C C
	1    6700 3550
	1    0    0    -1  
$EndComp
$Comp
L 0 #GND1
U 1 1 32CFC26C
P 6100 3800
F 0 "#GND1" H 6100 3700 40  0001 C C
F 1 "0" H 6100 3730 40  0000 C C
	1    6100 3800
	1    0    0    -1  
$EndComp
$Comp
L R R7
U 1 1 32CFC25A
P 6100 3550
F 0 "R7" V 6180 3550 50  0000 C C
F 1 "470" V 6100 3550 50  0000 C C
	1    6100 3550
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 32CFC254
P 6100 2050
F 0 "R4" V 6180 2050 50  0000 C C
F 1 "1K" V 6100 2050 50  0000 C C
	1    6100 2050
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 32CFC24C
P 5750 2050
F 0 "R3" V 5830 2050 50  0000 C C
F 1 "1K" V 5750 2050 50  0000 C C
	1    5750 2050
	1    0    0    -1  
$EndComp
$Comp
L QNPN Q2
U 1 1 32CFC230
P 6250 2800
F 0 "Q2" H 6150 3100 50  0000 C C
F 1 "Q2N2222" H 6150 3000 50  0000 C C
	1    6250 2800
	-1   0    0    -1  
$EndComp
$Comp
L QNPN Q1
U 1 1 32CFC227
P 5600 2800
F 0 "Q1" H 5500 3100 50  0000 C C
F 1 "Q2N2222" H 5500 3000 50  0000 C C
	1    5600 2800
	1    0    0    -1  
$EndComp
$EndSCHEMATC
