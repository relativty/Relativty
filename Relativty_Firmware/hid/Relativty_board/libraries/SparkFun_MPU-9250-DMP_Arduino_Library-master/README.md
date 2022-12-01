Fork of SparkFun MPU-9250 Digital Motion Processor (DMP) Arduino Library
========================================
Modified to  account for magnetic declination
========================================
* Compatible with Arduino Due
* usable for hmd 3dof tracking

Finding your Magnetic Declination and converting it to decimal degrees


* First, pick up your smartphone and open:
https://www.ngdc.noaa.gov/geomag/calculators/mobileDeclination.shtml
it will show you a compass with magnetic directions and the declination angle which you will need 

* Note the sign on declination is either negative or positive (WEST or EAST)
It will say either Declination is NEGATIVE (WEST) or POSITIVE (EAST)

* Magnetic Declination changes over time, and the degree of change varies 
between locations. This means that you should update your Magnetic 
Declination once or twice a year and re-flash your Tracker sketch

--------------------------------

London gives:
Magnetic Declination: -0° 15'
Declination is NEGATIVE (WEST)

Calc: 0 + (15/60) = 0.25
Result: MINUS 0.25
i.e. -0.25

--------------------------------

Paris gives:
Magnetic Declination: +0° 46'
Declination is POSITIVE (EAST)

Calc: 0 + (46/60) = 0.767
Result: PLUS 0.767
i.e. 0.767

--------------------------------

New York gives:
Magnetic Declination: -12° 52'
Declination is NEGATIVE (WEST)

Calc: 12 + (52/60) = 12.867
Result: MINUS 12.867
i.e. -12.867

--------------------------------

