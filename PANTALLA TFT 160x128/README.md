## Código para Willy

El archivo codigo2 muestra simplemente un texto.
El archivo codigo3 muestra un texto y un grafico de barras random.

Instrucciones:

    1. Instalar la placa ESP23 (espressif.github.io/arduino-esp32/package_esp32_index.json)
    2. Instalar lalibrería TFT_eSPI
    3. Cerrar el ArduinoIDE
    4. Ir a Documentos<Arduino<libraries<TFT_eSPI-2.5.43 y sobreescribir el archivo User_Setup.h por el facilitado en este repositorio
    5. Iniciar el ArduinoIDE y subir un código a la placa
    
En el archivo codigo4, donde se muestra la pantalla en vertical, con unos graficos distintos y textos.

Pasando al tema de las imágenes, tenemos los archivos codigo5 y codigo6, hacen EXACTAMENTE LO MISMO, pero de forma diferente.
Para subir imagenes hay primero que instalar una herramienta para el ArduinoIDE. Para ello, debes descargar la carpeta tools y ponerla en la carpeta Documentos<Arduino, tal como se ve en la imagen.

<img width="1056" height="401" alt="image" src="https://github.com/user-attachments/assets/0e758228-bdcd-4ee4-a5d6-2c3f0b33da68" />

Una vez hecho esto, abriremos el IDE y cargaremos un codigo, el 5 o 6 y veremos un directorio 'data', donde pondremos la imagen en .bmp con compresión de 24 bits.

Luego en el IDE<Herramientas<ESP32 Sketch Data Upload, cargaremos la imagen, seleccionando la opcion que se ve en las imagenes inferiores.

<img width="715" height="603" alt="image" src="https://github.com/user-attachments/assets/f2b9c9ae-2ab2-4717-b623-616ac0f98cab" />

<img width="289" height="153" alt="image" src="https://github.com/user-attachments/assets/f7492635-b84d-4498-8630-e1dca0777589" />

Y subiremos nuestro código al ESP
