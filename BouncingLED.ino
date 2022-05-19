/**
 * \file BouncingLED.ino
 * \author Carlo Banfi
 * \copyright 2022 - Carlo Banfi
 * 
 * \brief Simulazione di pallina rimbalzante su barra a LED
 */

/** \mainpage Simulazione di pallina rimbalzante su barra a LED
 *
 * \tableofcontents
 *
 *
 ********************************************************************************
 * \section intro_sec Introduzione
 *
 * Questo progetto ha lo scopo di simulare graficamente una caduta verticale di
 * una pallina con relativo rimbalzo utilizzando una striscia a LED come
 * dispositivo di visualizzazione.
 *
 * Per quanto possibile verranno applicate le leggi della fisica.
 *
 * \note Semplificazioni ed ottimizzazioni saranno consentite per la più semplice
 *       elaborazione dei dati.
 *
 *
 ********************************************************************************
 * \section descr_sec Descrizione
 *
 * La pallina è simulata su una striscia a LED posta verticalmente, con il primo
 * LED in alto.
 *
 * Ad intervalli di tempo fissi è calcolata la posizione della pallina ed 
 * aggiornata la visualizzazione.
 *
 * L'urto con il terreno virtuale è considerato anelastico con perdita del
 * \f$ 15\% \f$ della quantità di moto (coefficiente di restituzione: \f$ e = 85\,\% \f$ ).
 *
 * A pallina ferma la simulazione riparte cambiando il colore della stessa.
 * 
 *
 ********************************************************************************
 *  \section base_concepts_sec Concetti di base
 *
 * La simulazione è, per quanto possibile, basata sulle leggi della fisica
 * classica.
 *
 * - Accelerazione di gravità media sulla Terra (costante):
 *    \f[
 *    g_{n} = 9.80665\,\mathrm {m \cdot s^{-2}} \quad \quad (1)
 *    \f]
 *
 * - Formule del moto uniformemente accelerato:
 *     \f[
 *     v = v_{0} + a \cdot t \quad \quad (2)
 *     \f]
 *     \f[
 *     s = v_{0} \cdot t + \frac{1}{2} \cdot a \cdot t^2 \quad \quad (3)
 *     \f]
 *   dove \f$ a = g_{n} \f$ .
 *
 * - In caso di urto, supposto perpendicolare, la velocità è invertita
 *   –diventando negativa– e moltiplicata per il *coefficiente di restituzione*
 *   dell'urto anelastico:
 *     \f[
 *     v_0 = - e \cdot v_0 \quad \quad (4)
 *     \f]
 *   dove \f$ e = 0.85 \f$ .
 *
 *
 ********************************************************************************
 * \section impl_sec Implementazione
 *
 * Di seguito è descritta l'implementazione dei moduli ad alto livello.
 *
 *  
 * \subsection hw_sub Hardware
 *
 * Il sistema è composto dai seguenti componenti:
 * 1. **Arduino Uno** Rev3.
 * 2. **Striscia LED WS2812B** 1m, 60 LEDs, 5 VDC.
 * 3. **Alimentatore USB** 5V, &ge;1A.
 * 4. **Cavo USB** di alimentazione per Arduino.
 *
 * La pallina è simulata su una striscia a LED tipo **WS2812B** di \f$ 1\,\mathrm{m}
 * \times 60\,\mathrm{LEDs} \f$ .
 * 
 * Il segnale di controllo della striscia a LED è connesso al pin \b `D6` di Arduino.
 * 
 * La frequenza di rinfresco del display è impostata a \f$ 50\,\mathrm{Hz} \f$ .
 *
 *
 * \subsection sw_sub Software
 *
 * Il progetto dipende dalle seguenti librerie di Arduino:
 * 1. **FastLED** by Daniel Garcia, *versione 3.5.0*.
 * 
 *
 * \subsection init_sub Inizializzazione (setup)
 *
 * La procedura di inizializzazione è composta dalle seguenti fasi: 
 * - Inizializzazione della libreria grafica, con conseguente spegnimento di
 *     tutti i LEDs.
 *
 * - Inizializzazione del timer periodico a \f$ 50\,\mathrm {Hz} \f$ in modo da
 *     generare interrupt.
 *
 * - Inizializzazione dei parametri della pallina.
 *     I valori di inizializzazione sono i seguenti:
 *     - \f$ v_{0} = 0 \f$
 *     - \f$ s = 0 \f$
 *     - \f$ colore = bianco \f$
 *     - \f$ t_\mathrm{r} = \f$ `<ora attuale>`
 *
 * - Attivazione dell'interrupt periodico.
 *
 * - Uscita dalla funzione e conseguente ingresso nel loop dell'applicazione.
 *
 *
 * \subsection loop_sub Ciclo periodico (loop)
 *
 * Durante il ciclo periodico la CPU verifica lo stato del flag che indica la
 * necessità di aggiornare il display.
 *
 * In caso affermativo sono eseguite le seguenti procedure:
 * 1. Registrazione dell'ora corrente, tramite la funzione `mills()`.
 * 2. Aggiornamento dello stato della pallina, considerando il tempo trascorso
 *      dall'ultimo aggiornamento.
 * 3. Aggiornamento dello stato della striscia a LEDs, in baso allo stato della
 *      pallina.
 * 4. Verifica della collisione con il terreno ed eventuali azioni conseguenti.
 * 5. Verifica del termine della simulazione (pallina ferma) e riavvio della
 *      stessa utilizzando un altro colore.
 *
 * Al termine delle operazioni sopraindicate la CPU entra in modalità a
 * basso consumo in attesa del successivo risveglio da interrupt.
 *
 *
 * \subsection int_sub Interruzione periodica
 *
 * L'interruzione periodica segnala la necessità di aggiornare la visualizzazione
 * impostando un apposito *flag*.
 *
 * Il `flag` è *atomico* e condiviso con la routine di `loop()` principale.
 *
 *
 * \subsection update_sub Aggiornamento dello stato della pallina
 *
 * L'aggiornamento dello stato della pallina avviene calcolando i nuovi parametri
 * di posizione e velocità.
 *
 * 1. Lo spazio percorso è calcolato in base alla formula <b>(3)</b>.
 * 2. La velocità è calcolata in base alla formula <b>(2)</b>.
 * 3. è aggiornata l'ora degli ultimi calcoli.
 *
 * \subsubsection sempl_subsub Semplificazioni
 *
 * Per semplificare i calcoli l'accelerazione di gravità è approssimata a:
 *   \f[
 *   g_{n} = 10\,\mathrm{m \cdot s^{-2}} \quad \quad (5)
 *   \f]
 *
 * Per evitare arrotondamenti eccessivi nei calcoli i tempi sono espressi in
 * **millisecondi** (ms), le distanze in **micrometri** (µm) e
 * l'accelerazione di gravità in \f$ \mu m \cdot ms^{-2} \f$.
 *
 *
 * \subsubsection show_subsub Visualizzazione
 *
 * La visualizzazione utilizza le informazioni dell'oggetto palla per ricavare
 * l'indice del LED da accendere lungo la striscia.
 *
 * Sono spenti tutti i LED, ed acceso solo il LED relativo alla posizione
 * corrente, calcolata come: 
 *   ```
 *   indice = <posizione in mm> * NUMERO_LED_PER_METRO / 1000.
 *   ```
 *
 * \subsection end_sub Termine
 *
 * La simulazione termina quando la pallina è ferma ( \f$ v < v_{\mathrm{min}} \f$ ).
 * La velocità minima della pallina, sotto la quale è considerata ferma è fissata
 * in \f$ 500\,\mathrm{mm \cdot s^{-1}} \f$ .
 *
 * Dopo aver cambiato il colore della palla la simulazione riparte dalle
 * condizioni iniziali.
 * 
 * La sequenza ciclica definita dei colori è:
 *   1. Bianco
 *   2. Rosso
 *   3. Verde
 *   4. Blu
 *   5. Giallo
 *   6. Viola
 *
 ********************************************************************************
 */

/* ----------------------------------------------------------------------------- */
/* --- Include ----------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */

#include <stdint.h>       /* Tipi di dati standard      */
#include <avr/sleep.h>    /* Modalità a basso consumo   */

#include "FastLED.h"      /* Libreria FastLED           */

/* ----------------------------------------------------------------------------- */
/* --- Costanti ---------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Parametri fisici */

/** \brief Accelerazione di gravità (m/s^2) */
#define GRAVITY_M_S2                                      9.80665f

/** \brief Coefficiente di restituzione del rimbalzo (percentuale) */
#define COLLISION_COEFFICIENT_OF_RESTITUTION_PERCENT     85


/* Display */

/** \brief Tipo di display */
#define DISPLAY_TYPE                                NEOPIXEL

/** \brief Densità di LEDs per metro della striscia */
#define STRIP_NUM_OF_LED_PER_M                           60u

/** \brief Lunghezza della striscia LED (mm) */
#define STRIP_LENGHT_IN_MM                             1000u

/** \brief Pin di controllo della striscia LED */
#define STRIP_PIN                                         6

/** \brief Periodo di rinfresco del display (Hz) */
#define REFRESH_RATE_HZ                                  50u


/* Ottimizzazioni ed approssimazioni locali */

/** \brief Approssimazione di **g** come intero (m/s^2) */
#define GRAVITY_APPROX_M_S2                            ((uint32_t)(GRAVITY_M_S2 + 0.5f))

/** \brief Numero di LED della striscia */
#define STRIP_NUM_OF_LED                          ((STRIP_NUM_OF_LED_PER_M) * (STRIP_LENGHT_IN_MM) / 1000uL)

/** \brief Velocità minima della palla al rimbalzo sotto la quale è considerata ferma in \f$mm \cdot s^{-1}\f$. */
#define MIN_BALL_SPEED_ON_COLLISION_MM_S                500u


/** \brief Sequenza dei colori della palla */
static const CRGB BallColorSequence[] =
{
  CRGB::White,    /**< \brief Bianco */
  CRGB::Red,      /**< \brief Rosso (colore iniziale) */
  CRGB::Green,    /**< \brief Verde  */
  CRGB::Blue,     /**< \brief Blu    */
  CRGB::Yellow,   /**< \brief Giallo */
  CRGB::Purple,   /**< \brief Viola  */
};


/* ----------------------------------------------------------------------------- */
/* --- Definizioni di tipo ----------------------------------------------------- */
/* ----------------------------------------------------------------------------- */

/**
 * \brief Definizione dell'oggetto `palla`
 */
typedef struct BALL_T
{
  unsigned long lastUpdateTime_ms;  /**< \brief Ora assoluta dell'ultimo aggiornamento dei dati (ms). */
  uint16_t position_mm;             /**< \brief Attuale distanza lineare dall'origine (mm). */
  int16_t speed_mm_s;               /**< \brief Attuale velocità (mm/s), positiva in caso di caduta, negativa in caso la palla si allontani dal terreno. */
  CRGB color;                       /**< \brief Colore della pallina. */  
} ball_t;

/* ----------------------------------------------------------------------------- */
/* --- Dichiarazione delle funzioni locali ------------------------------------- */
/* --- La documentazione delle funzioni è posta prima dell'implementazione ----- */
/* ----------------------------------------------------------------------------- */

/* Controllo dell'oggetto `palla` */
static void Ball_Init( ball_t * const pBall, unsigned long now, CRGB color );
static void Ball_Update( ball_t * const pBall, unsigned long now );
static void Ball_OnCollision( ball_t * const pBall );
static uint16_t Ball_GetPosition( ball_t const * const pBall );
static uint16_t Ball_GetSpeed( ball_t const * const pBall );
static CRGB Ball_GetColor( ball_t const * const pBall );
static bool Ball_IsNotMoving( ball_t * const pBall );

/* Funzioni relative alla simulazione */
static CRGB GetNextBallColor();
static bool CheckForCollision( ball_t * const pBall );

static void InitDisplay();
static void RefreshDisplay( ball_t const * const pBall );

/* Timer periodico */
static void SetupTick();
static void StartTick();


/* ----------------------------------------------------------------------------- */
/* --- Dichiarazione degli oggetti locali -------------------------------------- */
/* ----------------------------------------------------------------------------- */

/** \brief Semaforo usato per la visualizzazione, controllato dal tick periodico */
static volatile bool m_isDisplayToBeRefreshed = false;
/** \brief Oggetto `palla` */
static ball_t m_ball;

/** \brief Matrice contenente lo stato dei LED della striscia, usata dalla libreria FastLED */
static CRGB m_leds[STRIP_NUM_OF_LED];

/** \brief Indice del colore corrente della palla */
static uint8_t m_ballCurrentColorIndex = 0;

/* ----------------------------------------------------------------------------- */
/* --- Implementazione delle funzioni locali ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/**
 * \brief Inizializzazione dell'oggetto `palla`
 * 
 * La palla è posta all'origine (il punto più alto), con velocità pari a zero.
 * 
 * \param pBall Puntatore all'oggetto `palla`.
 * \param now Ora corrente, tempo assoluto in ms.
 */
static void Ball_Init( ball_t * const pBall, unsigned long now, CRGB color )
{
  pBall->lastUpdateTime_ms = now;
  pBall->position_mm = 0;
  pBall->speed_mm_s = 0;
  pBall->color = color;
}

/**
 * \brief Aggiorna i dati correnti dell'oggetto `palla`
 * 
 * Aggiorna velocità e posizione della palla in base allo stato precedente ed al tempo trascorso.
 * 
 * \param pBall Puntatore all'oggetto `palla`.
 * \param now Ora corrente, tempo assoluto in ms.
 */
static void Ball_Update( ball_t * const pBall, unsigned long now )
{
  /*
   * Nota: l'accelerazione in metri/secondo^2 è equivalente a micrometri^millisecondo^2
   */
  
  /* Tempo trascorso dall'ultimo aggiornamento */
  unsigned long elapsed_time_ms = now - pBall->lastUpdateTime_ms;

  /* Spazio percorso dall'ultimo aggiornamento in micrometri */
  /* s = v0 * t ... */
  int32_t space_um = pBall->speed_mm_s * elapsed_time_ms;
  /* ... + 1/2 * a * t^2 */  
  space_um += elapsed_time_ms * elapsed_time_ms * GRAVITY_APPROX_M_S2 / 2;

  /* Assegnazione parametri correnti */
  pBall->lastUpdateTime_ms = now;
  pBall->position_mm += space_um / 1000L;
  pBall->speed_mm_s += elapsed_time_ms * GRAVITY_APPROX_M_S2;
}

/**
 * \brief Evento di collisione della palla con il terreno (rimbalzo)
 * 
 * Simula la collisione con il terreno, la velocità è invertita ed è applicato
 * il coefficiente di restituzione dell'urto.
 * 
 * \param pBall Puntatore all'oggetto `palla`.
 */
static void Ball_OnCollision( ball_t * const pBall )
{
  /* Rimbalza solo se la palla sta cadendo (v positiva) */
  if( pBall->speed_mm_s > 0 )
  {
    pBall->speed_mm_s = -(int16_t)((int32_t)pBall->speed_mm_s * COLLISION_COEFFICIENT_OF_RESTITUTION_PERCENT / 100L);
  }
}

/**
 * \brief Posizione della palla rispetto all'origine (mm)
 * 
 * \param pBall Puntatore all'oggetto `palla`.
 * \return Posizione della palla rispetto all'origine (mm).
 */
static uint16_t Ball_GetPosition( ball_t const * const pBall )
{
  return pBall->position_mm;
}

/**
 * \brief Velocità della palla (modulo) (mm/s)
 * 
 * \param pBall Puntatore all'oggetto `palla`.
 * \return Velocità della palla (modulo) in mm/s.
 */
static uint16_t Ball_GetSpeed( ball_t const * const pBall )
{
  return abs( pBall->speed_mm_s );
}

/**
 * \brief Colore della palla (CRGB)
 * 
 * \param pBall Puntatore all'oggetto `palla`.
 * \return Colore della palla CRGB.
 */
static CRGB Ball_GetColor( ball_t const * const pBall )
{
  return pBall->color;
}

/**
 * \brief Verifica che la palla sia ferma
 *
 * Da chiamare dopo la collisione, quando la palla è in risalita.
 * 
 * \param pBall Puntatore all'oggetto `palla`.
 * \return \c true se la palla è ferma, \c false altrimenti.
 */
static bool Ball_IsNotMoving( ball_t * const pBall )
{
  return ( Ball_GetSpeed(pBall) < MIN_BALL_SPEED_ON_COLLISION_MM_S );
}

/* ----------------------------------------------------------------------------- */

/**
 * \brief Determina il prossimo colore della palla
 * 
 * \return Colore della palla CRGB.
 */
static CRGB GetNextBallColor()
{
  m_ballCurrentColorIndex = (m_ballCurrentColorIndex+1) % (sizeof(BallColorSequence) / sizeof(BallColorSequence[0]));

  return BallColorSequence[ m_ballCurrentColorIndex ];
}

/**
 * \brief Verifica l'evento di collisione della palla con il terreno
 * 
 * Nel caso di collisione calcola il rimbalzo e restituisce \c true.
 * 
 * \return \c true nel caso di collisione avvenuta, \c false altrimenti.
 */
static bool CheckForCollision( ball_t * const pBall )
{
  /* Al termine della striscia la palla rimbalza */
  if ( Ball_GetPosition( pBall ) >= STRIP_LENGHT_IN_MM )
  {
    Ball_OnCollision( pBall );
    
    return true;
  }

  return false;
}

/* ----------------------------------------------------------------------------- */

/**
 * \brief Inizializzazione del display
 */
static void InitDisplay()
{
  m_ballCurrentColorIndex = 0;
  
  FastLED.addLeds<DISPLAY_TYPE, STRIP_PIN>(m_leds, STRIP_NUM_OF_LED);
  
  m_isDisplayToBeRefreshed = false;
}

/**
 * \brief Visualizza l'oggetto palla sul display
 * 
 * \param ball Puntatore all'oggetto `palla`.
 */
static void RefreshDisplay( ball_t const * const pBall )
{
  /* Spegne tutti i led */
  FastLED.clear();

  /* Calcola il LED da accendere in base alla posizione della palla */
  uint32_t ledIndex = (uint32_t)Ball_GetPosition( pBall ) * STRIP_NUM_OF_LED_PER_M / 1000uL;

  /* Se il LED è nel display... */
  if (ledIndex < STRIP_NUM_OF_LED )
  {
    /* ...lo accende del colore opportuno */
    m_leds[ledIndex] = Ball_GetColor( pBall );
  }

  /* Aggiorna il display */
  FastLED.show();
}

/* ----------------------------------------------------------------------------- */

/**
 * \brief Preparazione dell'interrupt periodico
 * 
 * L'interruzione periodica è utilizzata per l'aggiornamento del display.
 * 
 * Il timer utilizzato è `TIM1`.
 */
static void SetupTick()
{
  /* Timer compare register = F_CPU / (Hz*<prescaler>) - 1 (DEVE essere < 65536) */
  constexpr uint16_t timerCompareValue = F_CPU / (REFRESH_RATE_HZ * 1024u) - 1u;

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = timerCompareValue;

  /* Attiva modalità CTC */
  TCCR1B |= (1 << WGM12);
  /* Imposta prescaler a 1024 */
  TCCR1B |= (1 << CS12) | (1 << CS10);
}

/**
 * \brief Attiva l'interruzione periodica
 */
static void StartTick()
{
  /* Abilita `timer compare interrupt` */
  TIMSK1 |= (1 << OCIE1A);
}

/**
 * \brief Interrupt service routine del timer periodico
 * 
 * Questa routine si limita ad attivare il flag di segnalazione, il resto
 * dell'elaborazione è demandato al loop principale.
 */
ISR(TIMER1_COMPA_vect)
{
  m_isDisplayToBeRefreshed = true;
}


/* ----------------------------------------------------------------------------- */
/* --- Implementazione delle funzioni predefinite di Arduino ------------------- */
/* ----------------------------------------------------------------------------- */

/**
 * \brief Arduino `setup()`
 * 
 * Eseguita solo all'avvio.
 */
void setup()
{
  /* Nessuna interruzione consentita durante l'inizializzazione */
  noInterrupts();
  
  /* Imposta lo stato di basso consumo */
  set_sleep_mode(SLEEP_MODE_IDLE);

  /* Inizializza il display */
  InitDisplay();
  
  /* Inizializza l'interruzione periodica */
  SetupTick();

  /* Inizializza l'oggetto palla */
  Ball_Init( &m_ball, millis(), GetNextBallColor() );
 
  /* Riabilita le interruzioni */
  interrupts();
  
  /* Avvia il timer periodico */
  StartTick();
}

/**
 * \brief Arduino `loop()`
 * 
 * Eseguita per un periodo infinito.
 */
void loop()
{
  /* Attende che l'interrupt liberi il semaforo */
  if ( m_isDisplayToBeRefreshed )
  {
    m_isDisplayToBeRefreshed = false;

    /* Ora corrente, in tempo assoluto */
    unsigned long now = millis();

    /* Aggiorna lo stato della palla */
    Ball_Update( &m_ball, now );

    /* Aggiorna il display */
    RefreshDisplay( &m_ball );

    /* Collisione? */
    if( CheckForCollision( &m_ball ) )
    {
      /* Riavvia la simulazione se la palla è ferma, cambiando colore */
      if ( Ball_IsNotMoving( &m_ball ) )
      {
        Ball_Init( &m_ball, now, GetNextBallColor() );
      }
    }
  }

  /* Entra in modalità a basso consumo in attesa del prossimo interrupt */
  sleep_cpu();
}
