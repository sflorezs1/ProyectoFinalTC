# RGB Automation: Proyecto final de Teoría de la conmutación

## Integrantes:  
- Adrián Alberto Gutiérrez Leal  
- Simón Flórez Silva 

## Instrucciones de Python
Para usar la applicación de Python activar el entorno virtual e instalar los requisitos.  

` cd Python/RGBRemote `  
` ./bin/activate `  
` pip install -r requirements.txt `  
` python main.py `

Y acceder al url generado por Flask en un navegador.  

## Borrar datos del NodeMCU  
Para borrar los datos del NodeMCU dejar el Pin D1 suelto (Es normalmente alto, pullup), para mantener los datos entre apagados mantener el pin en bajo (atarlo a tierra)
