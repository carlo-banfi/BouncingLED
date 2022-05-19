# BouncingLED

## Introduzione

Questo progetto ha lo scopo di simulare graficamente una caduta verticale di
una pallina con relativo rimbalzo utilizzando una striscia a LED come
dispositivo di visualizzazione.

Per quanto possibile verranno applicate le leggi della fisica.

## Descrizione

La pallina è simulata su una striscia a LED posta verticalmente, con il primo
LED in alto.

Ad intervalli di tempo fissi è calcolata la posizione della pallina ed 
aggiornata la visualizzazione.

L'urto con il terreno virtuale è considerato anelastico con perdita del
![15\,\%](https://latex.codecogs.com/png.latex?%5Cinline%2015%5C%2C%5C%25) della quantità
di moto (coefficiente di restituzione: ![e = 85\,\%](https://latex.codecogs.com/png.latex?%5Cinline%20e%20%3D%2085%5C%2C%5C%25) ).

A pallina ferma la simulazione riparte cambiando il colore della stessa.

## Concetti di base

La simulazione è, per quanto possibile, basata sulle leggi della fisica
classica.

- Accelerazione di gravità media sulla Terra (costante):

   ![g_{n} = 9.80665\,\mathrm {m \cdot s^{-2}} \quad \quad (1)](https://latex.codecogs.com/png.latex?g_%7Bn%7D%20%3D%209.80665%5C%2C%5Cmathrm%20%7Bm%20%5Ccdot%20s%5E%7B-2%7D%7D%20%5Cquad%20%5Cquad%20%281%29)

- Formule del moto uniformemente accelerato:

   ![g_{n} = 9.80665\,\mathrm {m \cdot s^{-2}} \quad \quad (1)](https://latex.codecogs.com/png.latex?v%20%3D%20v_%7B0%7D%20&plus;%20a%20%5Ccdot%20t%20%5Cquad%20%5Cquad%20%282%29)

   ![s = v_{0} \cdot t + \frac{1}{2} \cdot a \cdot t^2 \quad \quad (3)](https://latex.codecogs.com/png.latex?s%20%3D%20v_%7B0%7D%20%5Ccdot%20t%20&plus;%20%5Cfrac%7B1%7D%7B2%7D%20%5Ccdot%20a%20%5Ccdot%20t%5E2%20%5Cquad%20%5Cquad%20%283%29)
   
  dove ![a = g_{n}](https://latex.codecogs.com/png.latex?%5Cinline%20a%20%3D%20g_%7Bn%7D).

- In caso di urto, supposto perpendicolare, la velocità è invertita
  –diventando negativa– e moltiplicata per il *coefficiente di restituzione*
  dell'urto anelastico:
  
    ![v_0 = - e \cdot v_0 \quad \quad (4)](https://latex.codecogs.com/png.latex?v_0%20%3D%20-%20e%20%5Ccdot%20v_0%20%5Cquad%20%5Cquad%20%284%29)
  
  dove ![e = 0.8](https://latex.codecogs.com/png.latex?%5Cinline%20e%20%3D%200.8).


## Implementazione

Di seguito è descritta l'implementazione dei moduli ad alto livello.

 
### Hardware

Il sistema è composto dai seguenti componenti:
1. **Arduino Uno** Rev3.
2. **Striscia LED WS2812B** 1m, 60 LEDs, 5 VDC.
3. **Alimentatore USB** 5V, &ge;1A.
4. **Cavo USB** di alimentazione per Arduino.

La pallina è simulata su una striscia a LED tipo **WS2812B** di ![1\,\mathrm{m}
\times 60\,\mathrm{LEDs}](https://latex.codecogs.com/png.latex?%5Cinline%201%5C%2C%5Cmathrm%7Bm%7D%20%5Ctimes%2060%5C%2C%5Cmathrm%7BLEDs%7D).

Il segnale di controllo della striscia a LED è connesso al pin **`D6`** di Arduino.

La frequenza di rinfresco del display è impostata a ![50\,\mathrm{Hz}](https://latex.codecogs.com/png.latex?%5Cinline%2050%5C%2C%5Cmathrm%7BHz%7D).


### Software

Il progetto dipende dalle seguenti librerie di Arduino:
1. **FastLED** by Daniel Garcia, *versione 3.5.0*.


### Inizializzazione (setup)

La procedura di inizializzazione è composta dalle seguenti fasi: 
- Inizializzazione della libreria grafica, con conseguente spegnimento di
    tutti i LEDs.

- Inizializzazione del timer periodico a ![50\,\mathrm{Hz}](https://latex.codecogs.com/png.latex?%5Cinline%2050%5C%2C%5Cmathrm%7BHz%7D) in modo da
    generare interrupt.

- Inizializzazione dei parametri della pallina.
    I valori di inizializzazione sono i seguenti:
    - ![v_{0} = 0](https://latex.codecogs.com/png.latex?%5Cinline%20v_%7B0%7D%20%3D%200)
    - ![s = 0](https://latex.codecogs.com/png.latex?%5Cinline%20s%20%3D%200)
    - ![\mathrm{colore} = \mathrm{bianco}](https://latex.codecogs.com/png.latex?%5Cinline%20%5Cmathrm%7Bcolore%7D%20%3D%20%5Cmathrm%7Bbianco%7D)
    - ![t_\mathrm{r} =](https://latex.codecogs.com/png.latex?%5Cinline%20t_%5Cmathrm%7Br%7D%20%3D) `<ora attuale>`

- Attivazione dell'interrupt periodico.

- Uscita dalla funzione e conseguente ingresso nel loop dell'applicazione.


### Ciclo periodico (loop)

Durante il ciclo periodico la CPU verifica lo stato del flag che indica la
necessità di aggiornare il display.

In caso affermativo sono eseguite le seguenti procedure:
1. Registrazione dell'ora corrente, tramite la funzione `mills()`.
2. Aggiornamento dello stato della pallina, considerando il tempo trascorso
     dall'ultimo aggiornamento.
3. Aggiornamento dello stato della striscia a LEDs, in baso allo stato della
     pallina.
4. Verifica della collisione con il terreno ed eventuali azioni conseguenti.
5. Verifica del termine della simulazione (pallina ferma) e riavvio della
     stessa utilizzando un altro colore.

Al termine delle operazioni sopraindicate la CPU entra in modalità a
basso consumo in attesa del successivo risveglio da interrupt.


### Interruzione periodica

L'interruzione periodica segnala la necessità di aggiornare la visualizzazione
impostando un apposito *flag*.

Il `flag` è *atomico* e condiviso con la routine di `loop()` principale.


### Aggiornamento dello stato della pallina

L'aggiornamento dello stato della pallina avviene calcolando i nuovi parametri
di posizione e velocità.

1. Lo spazio percorso è calcolato in base alla formula <b>(3)</b>.
2. La velocità è calcolata in base alla formula <b>(2)</b>.
3. è aggiornata l'ora degli ultimi calcoli.

### Semplificazioni

Per semplificare i calcoli l'accelerazione di gravità è approssimata a:

  ![g_{n} = 10\,\mathrm{m \cdot s^{-2}} \quad \quad (5)](https://latex.codecogs.com/png.latex?g_%7Bn%7D%20%3D%2010%5C%2C%5Cmathrm%7Bm%20%5Ccdot%20s%5E%7B-2%7D%7D%20%5Cquad%20%5Cquad%20%285%29)

Per evitare arrotondamenti eccessivi nei calcoli i tempi sono espressi in
**millisecondi** (ms), le distanze in **micrometri** (µm) e
l'accelerazione di gravità in ![\mu m \cdot ms^{-2}](https://latex.codecogs.com/png.latex?%5Cinline%20%5Cmu%20m%20%5Ccdot%20ms%5E%7B-2%7D).


### Visualizzazione

La visualizzazione utilizza le informazioni dell'oggetto palla per ricavare
l'indice del LED da accendere lungo la striscia.

Sono spenti tutti i LED, ed acceso solo il LED relativo alla posizione
corrente, calcolata come: 
  ```
  indice = <posizione in mm> * NUMERO_LED_PER_METRO / 1000.
  ```

### Termine

La simulazione termina quando la pallina è ferma ( ![v < v_{\mathrm{min}}](https://latex.codecogs.com/png.latex?%5Cinline%20v%20%3C%20v_%7B%5Cmathrm%7Bmin%7D%7D) ).
La velocità minima della pallina, sotto la quale è considerata ferma è fissata
in ![500\,\mathrm{mm \cdot s^{-1}}](https://latex.codecogs.com/png.latex?%5Cinline%20500%5C%2C%5Cmathrm%7Bmm%20%5Ccdot%20s%5E%7B-1%7D%7D).

Dopo aver cambiato il colore della palla la simulazione riparte dalle
condizioni iniziali.

La sequenza ciclica definita dei colori è:
  1. Bianco
  2. Rosso
  3. Verde
  4. Blu
  5. Giallo
  6. Viola
