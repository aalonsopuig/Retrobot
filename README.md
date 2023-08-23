# Retrobot
 Por Alejandro Alonso y Manuel Alonso. 
 Julio 2011. 
 
Este fue un robot que hice para divertirme con mi sobrino. El objetivo era construir un robot con productos de principios de los 80. Para ello conseguimos un ordenador de los años 80 (Sinclair ZX-81) y otros componentes de eBay.
El robot pudo moverse, reproducir algunas canciones de esa época (grabadas en cinta de cassette y bailar mientras agitaba una maraca con su brazo izquierdo. También tenía un brazo derecho con una pinza extensible.
Fue presentado por primera vez en Campus Party 2011, evento que se realizaba en Valencia (España) para jóvenes aficionados a las tecnologías.

![Retrobot_noticia](https://github.com/aalonsopuig/Retrobot/assets/57196844/27ffd714-20f1-40fd-9bcd-c0cb83c8ba35)

El centro del robot es un ordenador Sinclair ZX-81 original de los años 80, al cual se le ha conectado una tarjeta llamada interfaz (ExpansionCard) de diseño mío. Esta tarjeta contiene un microcontrolador Microchip PIC16F876, se conecta al puerto de expansión (bus) del ZX81 y permite a este acceder a todos los dispositivos del robot vía puerto I2C, así como mover los diferentes motores y actuadores del robot gracias a un driver incorporado en la tarjeta interfaz (ExpansionCard)
Nota: En el estado actual del proyecto, la tarjeta de interfaz está programada para controlar todo el robot y no hace de interfaz. Por tanto el software que se incluye en este repositorio solo hace esta función y el ZX81 no realiza ninguna función. Esto se debe a que el deseo de mostrar el robot moviéndose en Campus Party hizo que tuviese que recortar los tiempos, consiguiendo que el robot se moviese, pero sin conseguir el objetivo final, que era controlar el robot desde el ZX81 (objetivo pendiente que quizá algún día me anime a hacer)
Este es el el esquema general del robot:

![Retrobot_aquitectura](https://github.com/aalonsopuig/Retrobot/assets/57196844/5958056d-e26b-42b1-99fa-ff2a96473a9a)


Es este video puede verse el robot en acción en CampusParty 2011: https://www.youtube.com/watch?v=pIfZhs_rKEg 
