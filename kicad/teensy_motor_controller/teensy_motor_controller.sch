EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Teensy motor controller"
Date "2022-04-08"
Rev "1.1"
Comp "Satakunta University of Applied Sciences"
Comment1 "Hannu Henttinen"
Comment2 ""
Comment3 ""
Comment4 "To be used with TB6600 stepper motor driver"
$EndDescr
$Comp
L teensy:Teensy4.0 U1
U 1 1 6257E682
P 6650 3500
F 0 "U1" H 6650 5115 50  0000 C CNN
F 1 "Teensy4.0" H 6650 5024 50  0000 C CNN
F 2 "teensy:Teensy40_side_pins" H 6250 3700 50  0001 C CNN
F 3 "" H 6250 3700 50  0001 C CNN
	1    6650 3500
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x11 J1
U 1 1 6258167B
P 3900 2650
F 0 "J1" H 3818 3275 50  0000 C CNN
F 1 "Conn_01x11" H 3818 3276 50  0001 C CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-1,5-11-5.08_1x11_P5.08mm_Horizontal" H 3900 2650 50  0001 C CNN
F 3 "~" H 3900 2650 50  0001 C CNN
	1    3900 2650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4100 2150 4850 2150
Wire Wire Line
	5550 2250 4100 2250
Wire Wire Line
	4100 2350 5550 2350
Wire Wire Line
	5550 2450 4100 2450
Wire Wire Line
	4100 2550 5550 2550
Wire Wire Line
	5550 2650 4100 2650
Wire Wire Line
	4100 2750 5550 2750
Wire Wire Line
	5550 2850 4100 2850
Wire Wire Line
	5550 2950 4100 2950
Wire Wire Line
	4100 3050 5550 3050
Wire Wire Line
	5550 3150 4100 3150
$Comp
L Device:R R1
U 1 1 62583E85
P 3200 3250
F 0 "R1" H 3270 3296 50  0000 L CNN
F 1 "220" H 3270 3205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 3130 3250 50  0001 C CNN
F 3 "~" H 3200 3250 50  0001 C CNN
	1    3200 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 62584DCA
P 2750 3250
F 0 "R2" H 2820 3296 50  0000 L CNN
F 1 "220" H 2820 3205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 2680 3250 50  0001 C CNN
F 3 "~" H 2750 3250 50  0001 C CNN
	1    2750 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 62585057
P 2250 3250
F 0 "R3" H 2320 3296 50  0000 L CNN
F 1 "220" H 2320 3205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 2180 3250 50  0001 C CNN
F 3 "~" H 2250 3250 50  0001 C CNN
	1    2250 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 6258535D
P 1850 3250
F 0 "R4" H 1920 3296 50  0000 L CNN
F 1 "220" H 1920 3205 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 1780 3250 50  0001 C CNN
F 3 "~" H 1850 3250 50  0001 C CNN
	1    1850 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 2150 4850 1700
Wire Wire Line
	4850 1700 3200 1700
Wire Wire Line
	3200 1700 3200 3100
Connection ~ 4850 2150
Wire Wire Line
	4850 2150 5550 2150
Wire Wire Line
	2750 3100 2750 1700
Wire Wire Line
	2750 1700 3200 1700
Connection ~ 3200 1700
Wire Wire Line
	2250 3100 2250 1700
Wire Wire Line
	2250 1700 2750 1700
Connection ~ 2750 1700
Wire Wire Line
	1850 3100 1850 1700
Wire Wire Line
	1850 1700 2250 1700
Connection ~ 2250 1700
Wire Wire Line
	4850 1700 4850 1500
Connection ~ 4850 1700
$Comp
L power:PWR_FLAG #GND0101
U 1 1 62589F5F
P 4850 1500
F 0 "#GND0101" H 4918 1588 50  0000 L CNN
F 1 "PWR_FLAG" H 4918 1497 50  0000 L CNN
F 2 "" H 4850 1500 50  0001 C CNN
F 3 "~" H 4850 1500 50  0001 C CNN
	1    4850 1500
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 6258AB64
P 3700 4100
F 0 "D1" H 3693 4225 50  0000 C CNN
F 1 "LED" H 3693 4226 50  0001 C CNN
F 2 "LED_THT:LED_D3.0mm" H 3700 4100 50  0001 C CNN
F 3 "~" H 3700 4100 50  0001 C CNN
	1    3700 4100
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 6258AFF8
P 3700 4400
F 0 "D2" H 3693 4525 50  0000 C CNN
F 1 "LED" H 3693 4526 50  0001 C CNN
F 2 "LED_THT:LED_D3.0mm" H 3700 4400 50  0001 C CNN
F 3 "~" H 3700 4400 50  0001 C CNN
	1    3700 4400
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 6258B3FC
P 3700 4700
F 0 "D3" H 3693 4825 50  0000 C CNN
F 1 "LED" H 3693 4826 50  0001 C CNN
F 2 "LED_THT:LED_D3.0mm" H 3700 4700 50  0001 C CNN
F 3 "~" H 3700 4700 50  0001 C CNN
	1    3700 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D4
U 1 1 6258B964
P 3700 5000
F 0 "D4" H 3693 5125 50  0000 C CNN
F 1 "LED" H 3693 5126 50  0001 C CNN
F 2 "LED_THT:LED_D3.0mm" H 3700 5000 50  0001 C CNN
F 3 "~" H 3700 5000 50  0001 C CNN
	1    3700 5000
	1    0    0    -1  
$EndComp
Text Label 4200 2250 0    50   Italic 0
Z_stepper_pulse
Text Label 4200 2350 0    50   Italic 0
Z_stepper_direction
Text Label 4200 2450 0    50   Italic 0
Z_stepper_enable
Text Label 4200 2550 0    50   Italic 0
X_stepper_pulse
Text Label 4200 2650 0    50   Italic 0
X_stepper_direction
Text Label 4200 2750 0    50   Italic 0
X_stepper_enable
Text Label 4200 2850 0    50   Italic 0
Z_max_limit
Text Label 4200 2950 0    50   Italic 0
Z_min_limit
Text Label 4200 3050 0    50   Italic 0
X_max_limit
Text Label 4200 3150 0    50   Italic 0
X_min_limit
Wire Wire Line
	5550 4650 3850 4650
Wire Wire Line
	3850 4650 3850 4700
Wire Wire Line
	5550 4550 3850 4550
Wire Wire Line
	3850 4550 3850 4400
Wire Wire Line
	5550 4450 5050 4450
Wire Wire Line
	5050 4450 5050 4100
Wire Wire Line
	5050 4100 3850 4100
Wire Wire Line
	3550 4100 3200 4100
Wire Wire Line
	3200 4100 3200 3400
Wire Wire Line
	3550 4400 2750 4400
Wire Wire Line
	2750 4400 2750 3400
Wire Wire Line
	3550 4700 2250 4700
Wire Wire Line
	2250 4700 2250 3400
Wire Wire Line
	3550 5000 1850 5000
Wire Wire Line
	1850 5000 1850 3400
Text Label 4150 4100 0    50   Italic 0
Z_max_led
Text Label 4150 4550 0    50   Italic 0
Z_min_led
Text Label 4150 4650 0    50   Italic 0
X_max_led
Text Label 4150 4750 0    50   Italic 0
X_min_led
Wire Wire Line
	5550 4750 4000 4750
Wire Wire Line
	4000 4750 4000 5000
Wire Wire Line
	4000 5000 3850 5000
NoConn ~ 5550 3250
NoConn ~ 5550 3350
NoConn ~ 5550 3450
NoConn ~ 5550 3550
NoConn ~ 5550 3650
NoConn ~ 5550 3750
NoConn ~ 5550 3850
NoConn ~ 5550 3950
NoConn ~ 5550 4050
NoConn ~ 5550 4150
NoConn ~ 5550 4250
NoConn ~ 5550 4350
NoConn ~ 5550 4850
NoConn ~ 7750 4850
NoConn ~ 7750 4750
NoConn ~ 7750 4650
NoConn ~ 7750 4550
NoConn ~ 7750 4450
NoConn ~ 7750 4350
NoConn ~ 7750 4050
NoConn ~ 7750 3950
NoConn ~ 7750 3850
NoConn ~ 7750 3750
NoConn ~ 7750 3650
NoConn ~ 7750 3550
NoConn ~ 7750 3450
NoConn ~ 7750 3350
NoConn ~ 7750 3250
NoConn ~ 7750 3150
NoConn ~ 7750 3050
NoConn ~ 7750 2950
NoConn ~ 7750 2850
NoConn ~ 7750 2750
NoConn ~ 7750 2650
NoConn ~ 7750 2550
NoConn ~ 7750 2450
NoConn ~ 7750 2350
NoConn ~ 7750 2250
NoConn ~ 7750 2150
$EndSCHEMATC
