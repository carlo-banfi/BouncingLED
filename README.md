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
\f$ 15\% \f$ della quantità di moto (coefficiente di restituzione: \f$ e = 85\,\% \f$ ).

A pallina ferma la simulazione riparte cambiando il colore della stessa.

## Concetti di base

La simulazione è, per quanto possibile, basata sulle leggi della fisica
classica.

- Accelerazione di gravità media sulla Terra (costante):
   \f[
   g_{n} = 9.80665\,\mathrm {m \cdot s^{-2}} \quad \quad (1)
   \f]

- Formule del moto uniformemente accelerato:
    \f[
    v = v_{0} + a \cdot t \quad \quad (2)
    \f]
    \f[
    s = v_{0} \cdot t + \frac{1}{2} \cdot a \cdot t^2 \quad \quad (3)
    \f]
  dove \f$ a = g_{n} \f$ .

- In caso di urto, supposto perpendicolare, la velocità è invertita
  –diventando negativa– e moltiplicata per il *coefficiente di restituzione*
  dell'urto anelastico:
    \f[
    v_0 = - e \cdot v_0 \quad \quad (4)
    \f]
  dove \f$ e = 0.8 \f$ .


## Implementazione

Di seguito è descritta l'implementazione dei moduli ad alto livello.

 
### Hardware

Il sistema è composto dai seguenti componenti:
1. **Arduino Uno** Rev3.
2. **Striscia LED WS2812B** 1m, 60 LEDs, 5 VDC.
3. **Alimentatore USB** 5V, &ge;1A.
4. **Cavo USB** di alimentazione per Arduino.

La pallina è simulata su una striscia a LED tipo **WS2812B** di \f$ 1\,\mathrm{m}
\times 60\,\mathrm{LEDs} \f$ .

Il segnale di controllo della striscia a LED è connesso al pin \b `D6` di Arduino.

La frequenza di rinfresco del display è impostata a \f$ 50\,\mathrm{Hz} \f$ .


### Software

Il progetto dipende dalle seguenti librerie di Arduino:
1. **FastLED** by Daniel Garcia, *versione 3.5.0*.


### Inizializzazione (setup)

La procedura di inizializzazione è composta dalle seguenti fasi: 
- Inizializzazione della libreria grafica, con conseguente spegnimento di
    tutti i LEDs.

- Inizializzazione del timer periodico a \f$ 50\,\mathrm {Hz} \f$ in modo da
    generare interrupt.

- Inizializzazione dei parametri della pallina.
    I valori di inizializzazione sono i seguenti:
    - \f$ v_{0} = 0 \f$
    - \f$ s = 0 \f$
    - \f$ colore = bianco \f$
    - \f$ t_\mathrm{r} = \f$ `<ora attuale>`

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
  \f[
  g_{n} = 10\,\mathrm{m \cdot s^{-2}} \quad \quad (5)
  \f]

Per evitare arrotondamenti eccessivi nei calcoli i tempi sono espressi in
**millisecondi** (ms), le distanze in **micrometri** (µm) e
l'accelerazione di gravità in \f$ \mu m \cdot ms^{-2} \f$.


### Visualizzazione

La visualizzazione utilizza le informazioni dell'oggetto palla per ricavare
l'indice del LED da accendere lungo la striscia.

Sono spenti tutti i LED, ed acceso solo il LED relativo alla posizione
corrente, calcolata come: 
  ```
  indice = <posizione in mm> * NUMERO_LED_PER_METRO / 1000.
  ```

### Termine

La simulazione termina quando la pallina è ferma ( \f$ v < v_{\mathrm{min}} \f$ ).
La velocità minima della pallina, sotto la quale è considerata ferma è fissata
in \f$ 500\,\mathrm{mm \cdot s^{-1}} \f$ .

Dopo aver cambiato il colore della palla la simulazione riparte dalle
condizioni iniziali.

La sequenza ciclica definita dei colori è:
  1. Bianco
  2. Rosso
  3. Verde
  4. Blu
  5. Giallo
  6. Viola
