Fork de la Biblioteca de Arduino del Procesador de Movimiento Digital (DMP) SparkFun MPU-9250
========================================
Take a look to this guide in [English](README.md)

Modificado para tener en cuenta la declinación magnética
========================================
* Compatible con Arduino Dúo
* Utilizable para el seguimiento 3DoF del HMD

Encontrar tu declinación magnética y convertirla a grados decimales

* Primero, toma tu teléfono inteligente y abre:
https://www.ngdc.noaa.gov/geomag/calculators/mobileDeclination.shtml
mostrará una brújula con direcciones magnéticas y el ángulo de declinación que necesitarás.

* Nota que el signo de la declinación puede ser negativo o positivo (OESTE o ESTE)
Dirá si la declinación es NEGATIVA (OESTE) o POSITIVA (ESTE)

* La declinación magnética cambia con el tiempo, y el grado de cambio varía 
entre ubicaciones. Esto significa que debes actualizar tu declinación magnética 
una o dos veces al año y volver a flashear tu sketch de seguimiento.

--------------------------------

Londres da:
Declinación Magnética: -0° 15'
La declinación es NEGATIVA (OESTE)

Cálculo: 0 + (15/60) = 0.25
Resultado: MENOS 0.25
es decir, -0.25

--------------------------------

París da:
Declinación Magnética: +0° 46'
La declinación es POSITIVA (ESTE)

Cálculo: 0 + (46/60) = 0.767
Resultado: MÁS 0.767
es decir, 0.767

--------------------------------

Nueva York da:
Declinación Magnética: -12° 52'
La declinación es NEGATIVA (OESTE)

Cálculo: 12 + (52/60) = 12.867
Resultado: MENOS 12.867
es decir, -12.867

--------------------------------
