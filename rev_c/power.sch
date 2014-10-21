EESchema Schematic File Version 2
LIBS:bus_bridge_encoders_sonar
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:bus_bridge_encoders_sonar-cache
EELAYER 25 0
EELAYER END
$Descr A 11000 8500
encoding utf-8
Sheet 2 3
Title ""
Date "Sun 12 Oct 2014"
Rev "C"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 1200 6800 0    50   ~ 0
BPWR
Text Label 1200 7600 0    50   ~ 0
BGND
$Comp
L VOLTAGE_REGULATOR U5
U 1 1 543A2449
P 2350 6850
F 0 "U5" H 2150 7000 50  0000 C CNN
F 1 "LM2940" H 2375 6700 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:TO-220_Neutral123_Horizontal_LargePads" H 2350 6900 60  0001 C CNN
F 3 "" H 2350 6900 60  0000 C CNN
	1    2350 6850
	1    0    0    -1  
$EndComp
$Comp
L C_VERT C1
U 1 1 543A2450
P 1800 7100
F 0 "C1" H 1820 7190 50  0000 L CNN
F 1 "22uF" H 1820 7010 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Capacitor3MMDiscRM2.5" H 1838 6950 30  0001 C CNN
F 3 "" H 1800 7100 60  0000 C CNN
	1    1800 7100
	1    0    0    -1  
$EndComp
$Comp
L C_VERT C2
U 1 1 543A2457
P 2900 7100
F 0 "C2" H 2920 7190 50  0000 L CNN
F 1 ".1uF" H 2920 7010 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Capacitor4x3RM2-5_RevB" H 2938 6950 30  0001 C CNN
F 3 "" H 2900 7100 60  0000 C CNN
	1    2900 7100
	1    0    0    -1  
$EndComp
$Comp
L R_HORIZ R1
U 1 1 543A245E
P 2650 7400
F 0 "R1" H 2650 7500 50  0000 C CNN
F 1 ".47" H 2650 7300 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x02" H 2675 7400 60  0001 C CNN
F 3 "" H 2675 7400 60  0000 C CNN
	1    2650 7400
	1    0    0    -1  
$EndComp
$Comp
L C_VERT C3
U 1 1 543A2465
P 3300 7100
F 0 "C3" H 3320 7190 50  0000 L CNN
F 1 "22uF" H 3320 7010 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Capacitor3MMDiscRM2.5" H 3338 6950 30  0001 C CNN
F 3 "" H 3300 7100 60  0000 C CNN
	1    3300 7100
	1    0    0    -1  
$EndComp
$Comp
L POWER_SELECT J1
U 1 1 543A246C
P 4950 6700
F 0 "J1" H 4900 6900 50  0000 C CNN
F 1 "POWER_SELECT" H 4950 6500 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x03" H 4950 6700 60  0001 C CNN
F 3 "" H 4950 6700 60  0000 C CNN
	1    4950 6700
	-1   0    0    1   
$EndComp
$Comp
L OPTO_ISOLATOR U1
U 1 1 543A2481
P 2300 900
F 0 "U1" H 2050 1200 50  0000 C CNN
F 1 "LTV-826" H 2450 600 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-8__300" H 2300 900 60  0001 C CNN
F 3 "" H 2300 900 60  0000 C CNN
	1    2300 900 
	1    0    0    -1  
$EndComp
Text Label 8800 550  0    50   ~ 0
MOTOR_POWER
$Comp
L C_VERT C4
U 1 1 543A249A
P 3700 7100
F 0 "C4" H 3720 7190 50  0000 L CNN
F 1 ".1uF" H 3720 7010 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Capacitor4x3RM2-5_RevB" H 3738 6950 30  0001 C CNN
F 3 "" H 3700 7100 60  0000 C CNN
	1    3700 7100
	1    0    0    -1  
$EndComp
Text Label 6600 900  0    50   ~ 0
M1+
Text Label 6600 1300 0    50   ~ 0
M1-
Text Label 6600 1700 0    50   ~ 0
M2+
Text Label 6600 2100 0    50   ~ 0
M2-
Text Notes 1725 7050 0    50   ~ 0
+
Text Notes 3200 7050 0    50   ~ 0
+
$Comp
L R_VERT_SIP7 R3
U 1 1 543A2628
P 3300 5400
F 0 "R3" H 3345 5470 50  0000 L CNN
F 1 "10Kx6" H 3345 5370 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x07" H 3300 5400 50  0001 C CNN
F 3 "" H 3300 5400 50  0000 C CNN
	1    3300 5400
	1    0    0    -1  
$EndComp
$Comp
L R_VERT_SIP7 R3
U 2 1 543A262F
P 3700 5400
F 0 "R3" H 3745 5470 50  0000 L CNN
F 1 "10Kx6" H 3745 5370 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x07" H 3700 5400 50  0001 C CNN
F 3 "" H 3700 5400 50  0000 C CNN
	2    3700 5400
	1    0    0    -1  
$EndComp
$Comp
L R_VERT_SIP7 R3
U 3 1 543A2636
P 4100 5400
F 0 "R3" H 4145 5470 50  0000 L CNN
F 1 "10Kx6" H 4145 5370 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x07" H 4100 5400 50  0001 C CNN
F 3 "" H 4100 5400 50  0000 C CNN
	3    4100 5400
	1    0    0    -1  
$EndComp
$Comp
L R_VERT_SIP7 R3
U 4 1 543A263D
P 4500 5400
F 0 "R3" H 4545 5470 50  0000 L CNN
F 1 "10Kx6" H 4545 5370 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x07" H 4500 5400 50  0001 C CNN
F 3 "" H 4500 5400 50  0000 C CNN
	4    4500 5400
	1    0    0    -1  
$EndComp
$Comp
L R_VERT_SIP7 R3
U 5 1 543A2644
P 4900 5400
F 0 "R3" H 4945 5470 50  0000 L CNN
F 1 "10Kx6" H 4945 5370 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x07" H 4900 5400 50  0001 C CNN
F 3 "" H 4900 5400 50  0000 C CNN
	5    4900 5400
	1    0    0    -1  
$EndComp
$Comp
L R_VERT_SIP7 R3
U 6 1 543A264B
P 5300 5400
F 0 "R3" H 5345 5470 50  0000 L CNN
F 1 "10Kx6" H 5345 5370 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x07" H 5300 5400 50  0001 C CNN
F 3 "" H 5300 5400 50  0000 C CNN
	6    5300 5400
	1    0    0    -1  
$EndComp
$Comp
L 754410 U6
U 1 1 543A2660
P 6000 1500
F 0 "U6" H 6000 2400 50  0000 C CNN
F 1 "754410" H 6000 500 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-16__300" H 5975 75  60  0001 C CNN
F 3 "" H 5975 75  60  0000 C CNN
	1    6000 1500
	1    0    0    -1  
$EndComp
NoConn ~ 1800 5700
NoConn ~ 1800 6100
NoConn ~ 2800 5700
NoConn ~ 2800 6100
Text HLabel 1100 700  0    50   Input ~ 0
1A
$Comp
L OPTO_ISOLATOR U1
U 2 1 543ABB21
P 2300 1600
F 0 "U1" H 2050 1900 50  0000 C CNN
F 1 "LTV-826" H 2450 1300 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-8__300" H 2300 1600 60  0000 C CNN
F 3 "" H 2300 1600 60  0000 C CNN
	2    2300 1600
	1    0    0    -1  
$EndComp
$Comp
L OPTO_ISOLATOR U2
U 1 1 543ABB5F
P 2300 2300
F 0 "U2" H 2050 2600 50  0000 C CNN
F 1 "LTV-826" H 2450 2000 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-8__300" H 2300 2300 60  0000 C CNN
F 3 "" H 2300 2300 60  0000 C CNN
	1    2300 2300
	1    0    0    -1  
$EndComp
$Comp
L OPTO_ISOLATOR U2
U 2 1 543ABB9E
P 2300 3000
F 0 "U2" H 2050 3300 50  0000 C CNN
F 1 "LTV-826" H 2450 2700 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-8__300" H 2300 3000 60  0000 C CNN
F 3 "" H 2300 3000 60  0000 C CNN
	2    2300 3000
	1    0    0    -1  
$EndComp
$Comp
L OPTO_ISOLATOR U3
U 1 1 543ABE70
P 2300 3700
F 0 "U3" H 2050 4000 50  0000 C CNN
F 1 "LTV-826" H 2450 3400 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-8__300" H 2300 3700 60  0000 C CNN
F 3 "" H 2300 3700 60  0000 C CNN
	1    2300 3700
	1    0    0    -1  
$EndComp
$Comp
L OPTO_ISOLATOR U3
U 2 1 543ABEC1
P 2300 4400
F 0 "U3" H 2050 4700 50  0000 C CNN
F 1 "LTV-826" H 2450 4100 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-8__300" H 2300 4400 60  0000 C CNN
F 3 "" H 2300 4400 60  0000 C CNN
	2    2300 4400
	1    0    0    -1  
$EndComp
$Comp
L OPTO_ISOLATOR U4
U 1 1 543ABF91
P 2300 5100
F 0 "U4" H 2050 5400 50  0000 C CNN
F 1 "LTV-826" H 2450 4800 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-8__300" H 2300 5100 60  0000 C CNN
F 3 "" H 2300 5100 60  0000 C CNN
	1    2300 5100
	1    0    0    -1  
$EndComp
$Comp
L OPTO_ISOLATOR U4
U 2 1 543ABFCE
P 2300 5900
F 0 "U4" H 2050 6200 50  0000 C CNN
F 1 "LTV-826" H 2450 5600 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:DIP-8__300" H 2300 5900 60  0000 C CNN
F 3 "" H 2300 5900 60  0000 C CNN
	2    2300 5900
	1    0    0    -1  
$EndComp
Text HLabel 1100 1400 0    50   Input ~ 0
EN12
Text HLabel 1100 2100 0    50   Input ~ 0
2A
Text HLabel 1100 2800 0    50   Input ~ 0
3A
Text HLabel 1100 3500 0    50   Input ~ 0
EN34
Text HLabel 1100 4200 0    50   Input ~ 0
4A
Text HLabel 1100 4900 0    50   Input ~ 0
ENC_EN
Text HLabel 1100 5300 0    50   Input ~ 0
LGND
Text HLabel 1100 6800 0    50   Input ~ 0
BPWR
Text HLabel 1100 7600 0    50   Input ~ 0
BGND
$Comp
L NPN Q1
U 1 1 543B0BA3
P 3400 6250
F 0 "Q1" H 3500 6250 50  0000 C CNN
F 1 "PN2222" H 3400 6000 50  0000 C CNN
F 2 "Bus_Bridge_Encoders_Sonar:TO92_123" H 3400 6250 60  0001 C CNN
F 3 "" H 3400 6250 60  0000 C CNN
	1    3400 6250
	1    0    0    -1  
$EndComp
$Comp
L R_VERT R2
U 1 1 543B0C2C
P 3100 5900
F 0 "R2" H 3170 5950 50  0000 L CNN
F 1 "220" H 3170 5850 50  0000 L CNN
F 2 "Bus_Bridge_Encoders_Sonar:Pin_Header_Straight_1x02" H 3100 5950 60  0001 C CNN
F 3 "" H 3100 5950 60  0000 C CNN
	1    3100 5900
	1    0    0    -1  
$EndComp
Text HLabel 7200 900  2    50   Input ~ 0
MOTOR1_A
Text HLabel 7200 1300 2    50   Input ~ 0
MOTOR1_B
Text HLabel 7200 1700 2    50   Input ~ 0
MOTOR2_A
Text HLabel 7200 2100 2    50   Input ~ 0
MOTOR2_B
Text HLabel 7200 6100 2    50   Input ~ 0
LED_EN
Text Label 1200 5300 0    50   ~ 0
LGND
Text Label 1200 4900 0    50   ~ 0
ENC_EN
Text Label 1200 4200 0    50   ~ 0
4A
Text Label 1200 3500 0    50   ~ 0
EN34
Text Label 1200 2800 0    50   ~ 0
3A
Text Label 1200 2100 0    50   ~ 0
2A
Text Label 1200 1400 0    50   ~ 0
EN12
Text Label 1200 700  0    50   ~ 0
1A
Text Label 6600 6100 0    50   ~ 0
LED_EN
Text HLabel 7200 6300 2    50   Input ~ 0
LED_5V
Connection ~ 1800 7600
Connection ~ 2900 700 
Wire Wire Line
	2900 2100 2800 2100
Connection ~ 1700 4600
Wire Wire Line
	1800 7600 1800 7300
Connection ~ 1700 1800
Wire Wire Line
	1800 1800 1700 1800
Connection ~ 1700 2500
Wire Wire Line
	1700 2500 1800 2500
Connection ~ 1700 3200
Wire Wire Line
	1700 3200 1800 3200
Connection ~ 1700 3900
Wire Wire Line
	1700 3900 1800 3900
Wire Wire Line
	1700 1100 1800 1100
Wire Wire Line
	1800 4200 1100 4200
Wire Wire Line
	1100 3500 1800 3500
Wire Wire Line
	1100 2800 1800 2800
Wire Wire Line
	1100 2100 1800 2100
Wire Wire Line
	1100 1400 1800 1400
Wire Wire Line
	1100 700  1800 700 
Connection ~ 4100 2500
Connection ~ 4500 3200
Wire Wire Line
	3300 900  3300 5200
Wire Wire Line
	3300 5700 3300 5600
Wire Wire Line
	4100 2500 2800 2500
Wire Wire Line
	4500 3200 2800 3200
Wire Wire Line
	5500 1700 4500 1700
Connection ~ 2900 3500
Wire Wire Line
	2900 3500 2800 3500
Connection ~ 2900 2800
Wire Wire Line
	2900 2800 2800 2800
Connection ~ 2900 2100
Connection ~ 2900 1400
Wire Wire Line
	2900 1400 2800 1400
Wire Wire Line
	2800 700  5500 700 
Wire Wire Line
	2900 700  2900 6900
Wire Wire Line
	2900 4200 2800 4200
Wire Wire Line
	4900 1900 5500 1900
Wire Wire Line
	5300 2100 5500 2100
Connection ~ 3300 6800
Connection ~ 6600 2400
Wire Wire Line
	6600 2300 6500 2300
Connection ~ 3700 5700
Connection ~ 4100 5700
Wire Wire Line
	3700 5700 3700 5600
Connection ~ 4500 5700
Wire Wire Line
	4100 5700 4100 5600
Wire Wire Line
	4500 5700 4500 5600
Wire Wire Line
	2800 6800 4600 6800
Wire Wire Line
	3300 7600 3300 7300
Connection ~ 2900 6800
Wire Wire Line
	2900 7400 2900 7300
Wire Wire Line
	2850 7400 2900 7400
Connection ~ 2350 7400
Connection ~ 1800 6800
Wire Wire Line
	1100 6800 1900 6800
Wire Wire Line
	1700 4600 1800 4600
Wire Wire Line
	5500 900  3300 900 
Wire Wire Line
	5500 2300 5400 2300
Connection ~ 5400 2400
Wire Wire Line
	5300 2100 5300 5200
Connection ~ 5300 4600
Wire Wire Line
	2800 4600 5300 4600
Wire Wire Line
	2800 3900 4900 3900
Connection ~ 4900 3900
Wire Wire Line
	4900 1900 4900 5200
Wire Wire Line
	4500 1700 4500 5200
Wire Wire Line
	4100 1300 4100 5200
Wire Wire Line
	6600 2600 6600 2300
Wire Wire Line
	1100 5300 1800 5300
Connection ~ 1700 5300
Wire Wire Line
	1700 1100 1700 5300
Wire Wire Line
	1800 4900 1100 4900
Wire Wire Line
	5500 2400 5400 2400
Wire Wire Line
	6600 2400 6500 2400
Connection ~ 5300 5700
Wire Wire Line
	2900 4900 2800 4900
Connection ~ 2900 4200
Wire Wire Line
	5400 2300 5400 2600
Connection ~ 4900 5700
Wire Wire Line
	4900 5700 4900 5600
Wire Wire Line
	5300 5700 5300 5600
Connection ~ 3300 1100
Wire Wire Line
	2800 1100 3300 1100
Wire Wire Line
	3700 1100 3700 5200
Connection ~ 3700 1800
Wire Wire Line
	3700 1800 2800 1800
Wire Wire Line
	5500 1100 3700 1100
Wire Wire Line
	5500 1300 4100 1300
Wire Wire Line
	5700 7600 5700 2600
Connection ~ 5700 2600
Wire Wire Line
	5400 2600 6600 2600
Wire Wire Line
	1100 7600 5700 7600
Wire Wire Line
	2350 7150 2350 7600
Connection ~ 2350 7600
Wire Wire Line
	2350 7400 2450 7400
Wire Wire Line
	3300 6800 3300 6900
Wire Wire Line
	1800 6600 1800 6900
Connection ~ 3300 7600
Connection ~ 2900 4900
Wire Wire Line
	3700 6800 3700 6900
Wire Wire Line
	3700 7300 3700 7600
Connection ~ 3700 7600
Wire Wire Line
	3300 5700 5700 5700
Connection ~ 5700 5700
Connection ~ 3700 6800
Wire Wire Line
	4600 6600 1800 6600
Wire Wire Line
	4600 6700 4500 6700
Wire Wire Line
	4500 6700 4500 5900
Wire Wire Line
	4500 5900 6900 5900
Wire Wire Line
	6900 5900 6900 700 
Wire Wire Line
	6900 700  6500 700 
Wire Wire Line
	2800 5300 3100 5300
Wire Wire Line
	3100 5300 3100 5700
Wire Wire Line
	3200 6250 3100 6250
Wire Wire Line
	3100 6250 3100 6100
Wire Wire Line
	3700 6400 4000 6400
Wire Wire Line
	4000 6400 4000 7600
Connection ~ 4000 7600
Wire Wire Line
	6500 900  7200 900 
Wire Wire Line
	7200 1300 6500 1300
Wire Wire Line
	6500 1700 7200 1700
Wire Wire Line
	3700 6100 7200 6100
Wire Wire Line
	7200 6300 4200 6300
Wire Wire Line
	4200 6300 4200 6800
Connection ~ 4200 6800
Wire Wire Line
	7200 2100 6500 2100
Wire Notes Line
	500  5500 2250 5500
Wire Notes Line
	2250 5500 2250 5400
Wire Notes Line
	2250 600  2250 500 
Text Notes 650  5600 0    50   ~ 0
Electrically Isolated
Text Notes 1950 6400 0    50   ~ 0
Unused
Text Notes 1950 7800 0    50   ~ 0
5V Power Supply
Text Notes 3500 6000 0    50   ~ 0
Encoder LED Enable
Text Notes 750  3150 0    50   ~ 0
Opto-isolators
Text Notes 7250 2500 0    50   ~ 0
Motor Driver
Text Notes 7250 2800 0    50   ~ 0
Note: The 754410 does not have fly-back diodes.\nIf this is an issue, substitute in an L293 instead.
Text Label 6600 6300 0    50   ~ 0
LED_5V
Text Label 6100 5900 0    50   ~ 0
MOTOR_POWER
$EndSCHEMATC
