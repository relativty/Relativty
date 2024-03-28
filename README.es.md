<h1 align="center">
	<img
		width="400"
		alt="Relativty"
		src="ressources/img/title.png">
</h1>

<h3 align="center">
	Casco VR de código abierto con soporte SteamVR.
</h3>
<p align="center">
	<strong>
		<a href="https://relativty.com">Website</a>
		•
		<a href="https://discord.gg/jARCsVb">Discord</a>
	</strong>
</p>

Soy <a href="https://twitter.com/maxim_xyz?lang=es">Maxim xyz</a> y cuando mi mejor amigo Gabriel Combe y yo teníamos 15 años construimos nuestros propios casco VR porque No podía permitirme el lujo de comprar uno.

5 años después: estos cascos se convirtieron en Relativty.

* Totalmente de código abierto: **hardware**, **software**, **firmware**.
* **Soporte de Steam VR**.
* Muestra de forma nativa una resolución **2K** a **120 FPS**.
* Compatible con **Arduino**.
* Experimental **Seguimiento corporal**


**Este repositorio sirve como guía de construcción. Para obtener más información sobre los cascos, nos vemos en <a href="https://relativty.com">Relativty.com</a>**

Hecho para Hackers
-
Relativty **NO ES UN PRODUCTO DE CONSUMO**. Hicimos Relativty en mi habitación con un soldador y una impresora 3D y esperamos que tú hagas lo mismo: **constrúyelo tú mismo**.

Relativty en su esencia es un visor de realidad virtual de 3 grados de libertad (3-DoF) sin soporte de controlador incorporado, diseñado para funcionar con SteamVR.
Esto puede limitar su usabilidad práctica para juegos que exigen controles de realidad virtual tipo nudillo.
Si está buscando un proyecto DIY VR con más funciones, seguimiento de 6 grados y compatibilidad con controles, le recomendamos que consulte <a href="https://github.com/HadesVR/HadesVR">HadesVR</a>.

Ya sea que estés construyendo Relativty o HadesVR o no, o si simplemente quieres unirte a nuestra comunidad y pasar el rato, te invitamos a <a href="https://discord.gg/F8GNKjy6RF">El gremio de Relativty en Discord</a>.

Es un lugar para que la comunidad de hagalo usted mismo/VR de código abierto aprenda, crezca, comparta conocimientos y solicite ayuda.
# Start Building Relativty

<p align="center"> <img src="ressources/img/open.jpg"> </p>

# Construyendo el hardware
## 1. CONSTRUCCIÓN RECOMENDADA ACTUAL - A partir de mayo de 2023
### 1.1 Introducción
Esta es una guía de construcción actualizada para ayudar a las personas que descubrieron recientemente Relativty. Su objetivo es explicar cómo construir el producto funcional mínimo, que puede mejorarse y modificarse según el gusto del individuo. La guía de construcción original aún se puede encontrar más abajo.

		Seguir esta guía para construir unos cascos Relativty expecta y supone un conocimiento básico de la electrónica y la programación de placas Arduino.
		Un visor de realidad virtual es un sistema complejo y puede resultar difícil descubrir qué es lo que no funciona si te quedas atascado en la construcción.
		Si tiene algún problema, únase a nuestro Discord y solicite ayuda, estaremos encantados de ayudarle.

### 1.2 ELECTRÓNICOS

Se requieren los siguientes componentes electrónicos:

- Microcontrolador (MCU) ATmega32U4 con soporte USB HID, como un <a href="https://www.amazon.co.uk/diymore-Atmega32U4-Development-Microcontroller-Header/dp/B0BKGSVX2X">Arduino Pro Micro< /a>
- Unidad de medición inercial (IMU) compatible con la <a href="https://github.com/LiquidCGS/FastIMU">Biblioteca FastIMU</a>
- Una pantalla de PC pequeña, de alta resolución y alta frecuencia de actualización + placa de controlador. <a href="https://github.com/HadesVR/HadesVR/blob/main/docs/Headset.md#displays">Este segmento de la documentación de HadesVR presenta posibles buenas opciones entre las que puede elegir</a>. Es importante tener en cuenta que estas pantallas se conectan a una TARJETA DE CONTROLADOR, que luego se conecta a su computadora. SIEMPRE compre la placa del controlador y las pantallas en el mismo paquete si puede para asegurarse de que sean compatibles entre sí. Si tiene más preguntas sobre las pantallas, únase a nosotros en <a href="https://discord.gg/F8GNKjy6RF">El gremio de Relativty en Discord</a>.

```
NOTAS sobre PANTALLAS VR:
- La pantalla no se conecta al microcontrolador, solo se conecta a la computadora que ejecuta sus aplicaciones de realidad virtual.
- Técnicamente, cualquier pantalla/monitor de PC se puede configurar como la pantalla utilizada por Relativty. Por lo tanto, puedes simplemente probar tu compilación en el monitor de tu PC.
primero para asegurarse de que funciona, antes de decidir gastar una cantidad significativa de dinero en lentes, pantallas y otras piezas.
- Las pantallas de alto rendimiento y factor de forma pequeño son caras y, a menudo, muy delicadas. ¡Manéjalos con cuidado! :)
```

### 1.2.1 Cableado de la IMU y la MCU
La IMU debe estar conectada a la MCU para alimentación y comunicación.
La MCU se conecta a su computadora a través de USB para enviar las lecturas de IMU a SteamVR.

En el caso de un Arduino Pro Micro, necesitas conectar los siguientes pines:

```
Pro Micro       IMU
VCC         ->  VCC  
GND         ->  GND  
SDA(pin 2)  ->  SDA  
SCL(pin 3)  ->  SCL  
```  
Si utiliza una MCU diferente, los pines SDA y SCL pueden asignarse a números de pin diferentes.

Además, asegúrese de que el VCC de su MCU sea compatible con el voltaje de funcionamiento nominal de su IMU.

Suministrar voltaje incorrecto a los componentes electrónicos puede causarles daños.

La propia MCU simplemente se conecta a su computadora a través del puerto USB.

### 1.2.2 Conexión de la pantalla
Como se mencionó anteriormente, cualquier pantalla que sea compatible con una computadora personal debería poder funcionar como su pantalla de realidad virtual.

En caso de que esté utilizando el componente recomendado o similar que se alimenta a través de micro-USB u otro estándar USB, es posible que encuentre situaciones en las que la placa no se encienda. Esto podría deberse a que el cable micro-USB que está utilizando es demasiado largo (por lo que la placa no puede encenderse porque el voltaje cayó demasiado) o a que la placa no puede encenderse debido al puerto USB al que se conecta en el lado de la computadora. simplemente no puede suministrar suficiente energía. En este caso, es posible que tengas que probar diferentes puertos en tu computadora o en un concentrador USB con alimentación.

### 1.3 CONSTRUCCIÓN MECÁNICA

Se requieren las siguientes piezas para la construcción mecánica:

- Carcasa: archivos .STL para el modelo imprimible en 3D proporcionados en la carpeta Relativty_Mechanical_build.
- Lentes: para construir los auriculares imprimibles en 3D. Se requieren lentes con un diámetro de 40 mm y una distancia focal de 50 mm. A menudo puedes encontrarlos en Aliexpress o similares.
- Correa e interfaz facial - p.e. Correa de repuesto + espuma para HTC Vive. A menudo puedes encontrarlos en Aliexpress o similares.

<p align="center"> <img src="ressources/img/front.jpg"> </p>

Si no tiene acceso a la impresión 3D, también es posible (y MUCHO más sencillo) simplemente usar una funda de teléfono Android VR y modificarla para que se ajuste a su pantalla y poder conectarle su IMU y MCU.

La ventaja de este enfoque es que obtiene todo en un solo paquete, que a menudo incluye el ajuste IPD.

<p align="center"> <img src="ressources/img/android-vr.jpg"> </p>

### 1.4 CONFIGURACIÓN DEL SOFTWARE

### 1.4.1 Introducción

La relatividad depende de 2 componentes principales de software:
- el firmware de Arduino
- el controlador SteamVR

Como el sistema está diseñado para funcionar con SteamVR, debes tener Steam instalado y descargado SteamVR en tu computadora.

### 1.4.2 Programando su MCU

Como se mencionó anteriormente, le recomendamos utilizar un Arduino Pro Micro y una IMU compatible con la biblioteca FastIMU.
FastIMU es un paquete increíble que admite muchas IMU de uso común y viene con un boceto de Arduino preescrito que funciona con Relativty.

Primero, deberá instalar el IDE de Arduino y conectar su MCU a su computadora mediante el conector USB.

Una vez que lo haya conectado y verificado que su IDE de Arduino puede funcionar con su MCU, descargue FastIMU desde el administrador de la biblioteca.

<p align="center"> <img src="ressources/img/FastIMU-lib.jpg"> </p>

Conecte su IMU como se recomienda en la Sección 1.2.1.

Encuentre el boceto Ejemplos/Fastimu/Calibrado_relativty en su IDE de Arduino:
<p align="center"> <img src="ressources/img/FastIMU-sketch.jpg"> </p>

Y finalmente, cárgalo a tu MCU.

FastIMU también incluye una herramienta de calibración incorporada que puede almacenar los datos de calibración en la EEPROM de la IMU.

Calibrar la IMU puede ayudar con la desviación del sensor que puede experimentar con el tiempo mientras usa Relativty.

Después de cargar el boceto Calibrated_relativty en la IMU, puede abrir el Monitor serie Arduino para iniciar una secuencia de calibración:

<p align="center"> <img src="ressources/img/FastIMU-calib.jpg"> </p>

	NOTA: sólo tienes que hacer esto una vez, pero asegúrate de seguir las instrucciones que te dan en el monitor serial.


### 1.4.3 Instalación del controlador SteamVR

Para instalar el controlador Relativty SteamVR:
- descargue el <a href="https://github.com/relativty/Relativty/archive/refs/heads/master.zip">repositorio principal</a>
- Dentro de Relativty-master, busque la carpeta Relativty_Driver\Relativty y cópiela en el directorio de controladores de instalación de SteamVR.
<p align="center"> <img src="ressources/img/driver-copy.jpg"> </p>

### 1.4.4 Configuración del controlador SteamVR

Una vez que haya copiado los archivos del controlador, es hora de configurar el controlador para que funcione con su configuración y su computadora.

Dentro de drivers\Relativty\resources\settings, debería haber un archivo llamado default.vrsettings.

Este es el archivo de configuración del controlador.

Hay algunas cosas que debes cambiar.


### Configurar el controlador para hablar con la MCU
SUponiendo que utiliza un Arduino Pro Micro y la biblioteca FastIMU:

En el segmento Relativty_hmd encuentre estos valores:
- hmdPid
- hmdVid
- hmdIMUdmpPackets

y cambiar los valores así:

```
	"hmdPid" : 32823,
	"hmdVid": 9025,
	"hmdIMUdmpPackets":  false,
```

Si está utilizando una MCU diferente, debe calcular los valores de PID y VID del USB.

La forma más sencilla es conectarlo a su computadora mediante USB y verificar el IDE de Arduino.

En la barra de menú, seleccione Herramientas/Obtener información del tablero:
<p align="center"> <img src="ressources/img/board-info.jpg"> </p>

Tome los valores PID y VID y conviértalos a decimal con un <a href="https://www.rapidtables.com/convert/number/hex-to-decimal.html">convertidor hexadecimal</a>.

Los valores convertidos luego van a los valores hmdPid y hmdVid en
`default.vrsettings.`

### Configuración de los ajustes de pantalla
Ahora veamos cómo configurar el controlador para que funcione con su pantalla.

Las variables de configuración para la pantalla se encuentran en el segmento Relativty_extendedDisplay:


Para el punto de origen de la ventana VR Viewport:
```
	 "windowX": 3440,
	 "windowY": 0,
```

Para el tamaño real de la ventana gráfica VR
```
	 "windowWidth": 1920,
	 "windowHeight": 1080,
```

Para la resolución de renderizado del VR Viewport, normalmente debería ser la misma que el tamaño.
```
	 "renderWidth": 1920,
	 "renderHeight": 1080,
```

Y algunas configuraciones diversas:
```
	 "DistorsionK1": 0,4,
	 "DistorsionK2": 0,5,
	 "ZoomWisth": 1,
	 "ZoomHeight": 1,
	 "EyeGapOffsetPx": 0,
	 "IsDisplayRealDisplay": true,
	 "IsDisplayOnDesktop": true
```
Si el punto de origen y el tamaño no están configurados correctamente, el controlador fallará y SteamVR no mostrará nada.

Por lo tanto, debemos observar y comprender el sistema de coordenadas que utiliza SteamVR para las pantallas.

Windows siempre asume una de las pantallas conectadas como su pantalla principal.

Puedes verificar cuál es tu principal en Configuración de pantalla.

Puede seleccionar cada una de sus pantallas con el mouse. La que tiene la casilla de verificación "Hacer de esta mi pantalla principal" atenuada es su pantalla principal.

ESTA GUÍA ASUME QUE EL BORDE SUPERIOR DE TODAS SUS PANTALLAS ESTÁ ALINEADO EN LA CONFIGURACIÓN DE PANTALLA DE WINDOWS (como se ve en las capturas de pantalla)

<p align="center"> <img src="ressources/img/display-settings.jpg"> </p>

En consecuencia, marcar lo mismo en otra pantalla no principal la convertirá en su principal.

La esquina SUPERIOR IZQUIERDA de su pantalla principal es el PUNTO DE ORIGEN del sistema de coordenadas de pantalla de SteamVR.

Para poder decirle a SteamVR dónde dibujar la ventana gráfica VR en sus pantallas, debe asegurarse de comprender este hecho y, como resultado, puede identificar el punto de origen correcto para la ventana gráfica.

Por ejemplo, en una configuración como esta:

<p align="center"> <img src="ressources/img/display-coordinates.jpg"> </p>

Debido a que la pantalla "1" es la principal y la pantalla "3" es la pantalla VR, el punto de origen (la coordenada 0,0) está en la parte superior izquierda de la pantalla "1".

Esta pantalla tiene una resolución de 3440x1440.

Esto significa que ocupa el eje X de 0 a 3439, y la siguiente pantalla a su derecha comienza en el punto 3440.

Por lo tanto, en este caso los valores correctos de `windowX` y `windowY` son:

```
	"windowX" : 3440,
	"windowY" : 0,
```

Si la pantalla "2" fuera la pantalla VR (y "1" sigue siendo la principal), los valores correctos serían:

```
	"windowX" : -1920,
	"windowY" : 0,
```
Porque las coordenadas de la pantalla "2" ocupan espacio sobre el otro lado del punto de origen.

Para ancho de ventana, altura de ventana, ancho de renderizado, altura de renderizado, simplemente configure la resolución nativa de su pantalla de realidad virtual.

Una vez que todo esto esté configurado, guarde el archivo de configuración.

Ahora deberías estar listo para iniciar SteamVR.

Si todo está configurado correctamente, deberías acceder directamente al área de la holocubierta de realidad virtual:

<p align="center"> <img src="ressources/img/electronics-assembled.jpg"> </p>

Si tiene algún problema con su compilación:
- abra la consola web SteamVR y copie el archivo de registro completo
- Únase al <a href="https://discord.gg/F8GNKjy6RF">El gremio de Relativty en Discord</a>, cuéntenos sobre los problemas que enfrenta y cargue el archivo de registro en el chat.
<p align="center"> <img src="ressources/img/steamvr-logs.jpg"> </p>

### SI TU VENTANA VR DESAPARECE DESPUÉS DE HACER CLIC EN ELLA:

Abra Windows PowerShell y pegue estos comandos en la línea de comandos, luego presione Enter:

```bash
get-process vrcompositor | stop-process -Force;cd "C:\Program Files (x86)\Steam\steamapps\common\SteamVR\bin\win64";.\vrcompositor.exe
```

Después, VRCompositor debería permanecer estable, por lo que sólo tendrás que hacer esto una vez por sesión.


# 2. CONSTRUCCIÓN DE LEGADO
El hardware se basa en la placa base Relativty que incluye un procesador Atmel SAM3X8E ARM Cortex-M3 y utiliza un MPU-6050 como IMU.
Alternativamente, cualquier procesador que admita ArduinoCore y esté conectado a un MPU-6050/MPU-9250 se puede utilizar como hardware para Relativty. Ambos métodos se explican a continuación.

## Construyendo la placa base Relativty
### Fabricación de PCB.

Primero comenzamos con el PCB desnudo.

se puede fabricar y comprar por unos $4 en sitios web como <a href="https://jlcpcb.com/">jlcpcb</a>.

Deberá proporcionar la carpeta de archivos Gerber `Relativty_Electronics_build/GerberFiles.zip` que describe la forma de la placa.


### Montaje

Soldar los componentes sobre la PCB desnuda.

Tendrá que comprar los componentes enumerados en una de las dos listas de materiales, según la disponibilidad:
- `Relativty_Electronics_build/Assembly/jlcpcb.com_motherboard_BOM.csv`
- `Relativty_Electronics_build/Assembly/motherboard_BOM.xlsx`

Dónde colocar esos componentes en la PCB se describe en el archivo `Relativty_Electronics_source/motherboard.brd` que se puede abrir en Eagle.

<p align="center"> <img src="ressources/img/motherboard.jpg"> </p>

#### Usando un Arduino

Una alternativa a la placa base Relativty es utilizar un Arduino Due y conectarlo a un MPU-6050.

```
5V      -> VCC  
GND     -> GND  
SDA(20) -> SDA  
SCL(21) -> SCL  
PIN 2   -> INT  
```

Si está utilizando un MPU-9250 con el firmware alternativo proporcionado, la configuración de pines es:

```
5V      -> VCC  
GND     -> GND  
SDA(20) -> SDA  
SCL(21) -> SCL   
```

Luego presione el botón "`ERASE`" y "`RESET`" en el Arduino Due y podrá instalar el firmware Relativty.

#### Instalación del firmware Relativty

<p align="center"> <img src="ressources/img/cards.jpg"> </p>
Ahora necesitarás instalar la placa Relativty en el IDE de Arduino.

Para hacer eso, copie esa URL JSON: https://raw.githubusercontent.com/relativty/Relativty/master/Relativty_Firmware/package_Relativty_board_index.json y abra el IDE de Arduino

En Arduino, haga clic en Archivo y luego en Preferencias:

- Si está utilizando la PCB Relativty, agregue la URL JSON al cuadro de texto `Administrador de Tarjetas`.
- Vaya a `Herramientas > Tarjeta > Administrador de Tarjetas` y debería ver el Tarjeta Relativty, haga clic en instalar.
- Reinicie el IDE de Arduino y en `Herramientas > Placas`, seleccione Relativty.
- Ahora puedes abrir `Relativty_Firmware/firmware/firmware.ino` y subirlo a tu tablero.

Si está utilizando una placa diferente, p. Arduino debido:

- instale el contenido de `Relativty_Firmware\Relativty_board\` en su IDE de Arduino
- si está utilizando MPU-6050, utilice `Relativty_Firmware/firmware/firmware.ino`
- si está utilizando MPU-9250, utilice `Relativty_Firmware\MP9250-HID\MP9250-HID\MP9250-HID.ino`



# Construyendo las piezas mecánicas

## Montaje del HMD

Todos los archivos necesarios para la impresión 3D se pueden encontrar en la carpeta `Relativty_Mechanical_build` y los tornillos necesarios para ensamblar los auriculares se enumeran en `screws_BOM.xlsx`.

Hemos utilizado las siguientes piezas desde Aliexpress:

- <a href="https://www.aliexpress.com/item/33058848848.html">La Correa</a>,
- <a href="https://www.aliexpress.com/item/4000199486058.html">La Espuma</a>,
- <a href="https://www.aliexpress.com/item/33029909783.html">Las Lentes</a> (40 mm de diámetro/50 mm de distancia focal).

### La pantalla del HMD
<p align="center"> <img src="ressources/img/display.jpg"> </p>

Los cascos Relativty ejecutan una pantalla dual de 120 FPS a 2K; sin embargo, debido a la naturaleza abierta de Relativty, puedes equiparlo con cualquier pantalla.

Nuestro modelo se puede encontrar en Aliexpress, pero dependiendo del proveedor, pantallas similares pueden costar entre $150 y $190. Tendrá que buscar y tal vez esperar al proveedor adecuado al precio adecuado para obtener la pantalla a bajo precio (o comprarla al por mayor).

Este es [el modelo que utilizamos] (https://www.aliexpress.com/item/32975198897.html).

### Configurando el software

<p align="center"> <img src="ressources/img/front.jpg"> </p>

#### Installing Relativty Driver for SteamVR

El controlador Relativty está contenido en la carpeta `Relativty_Driver/Relativty`.

⚠️ Deberá configurarlo editando el archivo JSON `Relativty_Driver/Relativty/resources/settings/default.vrsettings`

Si no está utilizando una PCB Relativty, deberá cambiar estos:

	"hmdPid" : 9,
	"hmdVid": 4617,
	  
Estos son los identificadores únicos de proveedor y producto del dispositivo USB HID (pid/vid)

Si está utilizando Arduino Due, los valores correctos serán:

	"hmdPid" : 62,
	"hmdVid" : 9025,
	  
En caso de que esté utilizando una placa diferente, el proceso para obtener los valores correctos es el siguiente:

1.	Conecta tu placa

2.	Seleccione su placa en Arduino IDE y haga clic en Herramientas/Obtener información de la placa. verás algo como esto:

```
	BN: Arduino Due (Native USB Port)
	VID: 2341
	PID: 003e
	SN: HIDHB
```
3. Tome nota de los números VID y PID. Estos son valores hexadecimales.

Para aplicarlos a la configuración, es necesario convertirlos a int.

Si no está seguro de cómo hacerlo, hay muchos convertidores en línea disponibles.

Por ejemplo: https://www.rapidtables.com/convert/number/hex-to-decimal.html

4. Cambie sus valores hmdPid y hmdVid a los valores convertidos.

A continuación, debe configurar las coordenadas y la resolución de la pantalla.

Al principio, debes tener la pantalla del HMD configurada como una pantalla secundaria que extiende tu escritorio,

alineado en la esquina superior derecha de su pantalla principal.


En el segmento "Relativty_extendedDisplay" del archivo de configuración, busque y configure estos:
```
	"windowX" : *whatever your primary screen resolution's width is*,
	"windowY" : 0,
	"windowWidth" : *HMD's native resolution width*,
	"windowHeight" : *HMD's native resolution height*,
	"renderWidth" : *HMD's native resolution width*,
	"renderHeight" : *HMD's native resolution height*,
	  
	  And at the bottom of this segment:
	  
	"IsDisplayRealDisplay" : true,
	"IsDisplayOnDesktop" : true
```  

Asegúrese de no eliminar ningún símbolo "`,`" ya que eso romperá la configuración.

Sólo el último elemento de la configuración no debe tener el símbolo ",".


Si por alguna razón la configuración anterior no funciona, intente:


Configure su pantalla HMD como una pantalla reflejada de su pantalla principal.

Cambie la configuración de la siguiente manera:
```
	"windowX" : 0,
	"windowY" : 0,
	"windowWidth" : *HMD's native resolution width*,
	"windowHeight" : *HMD's native resolution height*,
	"renderWidth" : *HMD's native resolution width*,
	"renderHeight" : *HMD's native resolution height*,
	
	"IsDisplayRealDisplay" : false,
	"IsDisplayOnDesktop" : true
	
```	
⚠️ Tenga en cuenta que esto puede provocar que la ventana de realidad virtual no capture la entrada del teclado/ratón; si su juego lo requiere, es posible que no se pueda jugar.

También puede realizar ajustes de IPD (distancia interpupilar) dentro del archivo de configuración:

En el segmento "Relativty_hmd" busque y ajuste:

```
	"IPDmeters" : 0.063,
```

También puede cambiar la corrección de distorsión de la lente cambiando estos:

```
	"DistortionK1" : 0.4,
	"DistortionK2" : 0.5,
```

Ahora puede instalar el controlador Relativty:
- Localice su programa `vrpathreg.exe`, generalmente ubicado en `C:/Steam/steamapps/common/SteamVR/bin/win64/vrpathreg.exe`
- Luego abra el símbolo del sistema de Windows y ejecute los siguientes comandos:
```cmd
cd C:/Steam/steamapps/common/SteamVR/bin/win64
vrpathreg.exe
```

Y luego, suponiendo que la carpeta del controlador `Relativty_Driver/Relativty` esté ubicada en:
`C:/código/Relativty_Driver/Relativty`
- ejecute `vrpathreg adddriver C:/code/Relativty_Driver/Relativty`

El controlador Relativty ya está instalado. Puedes desinstalarlo en cualquier momento ejecutando:
- `vrpathreg removedriver C:/code/Relativty_Driver/Relativty`

#### Configuración del seguimiento 3D experimental

El seguimiento aún es muy experimental y solo se puede ejecutar en la GPU NVIDIA debido al uso de CUDA. El seguimiento utiliza únicamente una entrada de vídeo y una red neuronal artificial (IA) entrenada para estimar la posición del cuerpo en 3D.

Este método no se acerca nada a la precisión o libertad de movimientos de un sensor dedicado; sin embargo, creemos que el modelo se puede entrenar y mejorar en órdenes de magnitud.

Primero debe conectar una cámara web a su computadora, instalar Python 3.8.4 y seleccionar la opción para agregarlo a la ruta. Luego instale `PyTorch`, puede hacerlo ejecutando los siguientes comandos:

```cmd
python -m pip install torch===1.6.0 torchvision===0.7.0 -f https://download.pytorch.org/whl/torch_stable.html
```

Luego proceda a instalar `CUDA Toolkit 11.0.`

Luego deberá descargar la carpeta `PYTHONPATH` en https://github.com/relativty/Relativty/releases y agregar su ubicación a `PyPath` en `JSON Relativty_Driver/Relativty/resources/settings/default.vrsettings`

Establezca "`tracking`" en 1. El seguimiento ahora está activado y se puede desactivar en cualquier momento configurando "`tracking`" en 0. El seguimiento también se puede calibrar dependiendo de su cámara, esto se puede hacer modificando "`scalesCoordinateMeter`" y "`offsetCoordinate`".  `scalesCoordinateMeter` corresponde al delta de coordenadas en un eje determinado calculado por la IA después de la normalización cuando te mueves 1 metro en ese eje determinado.

**Nota:** Si planea usar el firmware mpu9250, no olvide cambiarlo en el controlador configurando `hmdIMUdmpPackets` en `false`:
`"hmdIMUdmpPackets": false,`

## Pasos finales

Ahora todo está configurado para empezar a jugar.

Para empezar a utilizar Relativty:
- Coloque los cascos sobre una superficie plana con el panel frontal mirando hacia el suelo.
- Conecte los cascos.
Se calibrará automáticamente después de unos segundos.

¡Ahora puedes ejecutar cualquier juego SteamVR!

