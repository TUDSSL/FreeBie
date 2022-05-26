/*************************************************************************************************/
/*!
 *  \file   wsf_timer.c
 *
 *  \brief  Timer service.
 *
 *  Copyright (c) 2009-2019 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019-2020 Packetcraft, Inc. 
 * 
 *  Copyright (c) 2022 Delft University of Technology.
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/
#ifdef CHECKPOINT

#include "wsf_types.h"
#include "wsf_queue.h"
#include "wsf_timer.h"
#include "wsf_app_timer.h"

#include "wsf_assert.h"
#include "wsf_cs.h"
#include "wsf_trace.h"
#include "pal_rtc.h"
#include "pal_led.h"
#include "pal_sys.h"

#include "checkpoint.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

#if (WSF_MS_PER_TICK == 10)
/* convert seconds to timer ticks */
#define WSF_TIMER_SEC_TO_TICKS(sec)         (100 * (sec) + 1)

/* convert milliseconds to timer ticks */
/* Extra tick should be added to guarantee waiting time is longer than the specified ms. */
#define WSF_TIMER_MS_TO_TICKS(ms)           (((uint32_t)(((uint64_t)(ms) * (uint64_t)(419431)) >> 22)) + 1)

/*! \brief  WSF timer ticks per second. */
#define WSF_TIMER_TICKS_PER_SEC       (1000 / WSF_MS_PER_TICK)

#elif (WSF_MS_PER_TICK == 1)
/* convert seconds to timer ticks */
#define WSF_TIMER_SEC_TO_TICKS(sec)         (1000 * (sec) + 1)

/*! \brief Convert milliseconds to timer ticks. */
/*! \brief Extra tick should be added to guarantee waiting time is longer than the specified ms. */
#define WSF_TIMER_MS_TO_TICKS(ms)           ((uint64_t)(ms) + 1)

#define WSF_TIMER_TICKS_PER_SEC             (1000)

#else
#error "WSF_TIMER_MS_TO_TICKS() and WSF_TIMER_SEC_TO_TICKS not defined for WSF_MS_PER_TICK"
#endif

/*! \brief  Number of RTC ticks per WSF timer tick. */
#define WSF_TIMER_RTC_TICKS_PER_WSF_TICK  ((PAL_RTC_TICKS_PER_SEC + WSF_TIMER_TICKS_PER_SEC - 1) / (WSF_TIMER_TICKS_PER_SEC))

/*! \brief  Calculate number of elapsed WSF timer ticks. */
#define WSF_RTC_TICKS_TO_WSF(x) ((x) / WSF_TIMER_RTC_TICKS_PER_WSF_TICK)

/*! \brief  Mask of seconds part in RTC ticks. */
#define WSF_TIMER_RTC_TICKS_SEC_MASK      (0x00FF8000)

/*! \brief  Addition of RTC ticks. */
#define WSF_TIMER_RTC_ADD_TICKS(x, y)     (((x) + (y)) & PAL_MAX_RTC_COUNTER_VAL)

/*! \brief  Subtraction of RTC ticks. */
#define WSF_TIMER_RTC_SUB_TICKS(x, y)     ((PAL_MAX_RTC_COUNTER_VAL + 1 + (x) - (y)) & PAL_MAX_RTC_COUNTER_VAL)

/*! \brief  Minimum RTC ticks required to go into sleep. */
#define WSF_TIMER_MIN_RTC_TICKS_FOR_SLEEP (20)

/*!
 * \brief  Computing the difference between two RTC counter values.
 *
 * Calculate elapsed ticks since last WSF timer update, with remainder;
 * since the RTC timer is 24 bit set the 24th bit to handle any underflow.
 */
#define WSF_TIMER_RTC_ELAPSED_TICKS(x)    ((PAL_MAX_RTC_COUNTER_VAL + 1 + (x) - wsfAppTimerRtcLastTicks \
                                  + wsfAppTimerRtcRemainder) & PAL_MAX_RTC_COUNTER_VAL)

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

wsfQueue_t  wsfTimerAppTimerQueue;     /*!< Timer queue */

/*! \brief  Last RTC value read. */
static uint32_t wsfAppTimerRtcLastTicks = 0;

/*! \brief  Remainder value. */
static uint32_t wsfAppTimerRtcRemainder = 0;

/*************************************************************************************************/
/*!
 *  \brief  Remove a timer from queue.  Note this function does not lock task scheduling.
 *
 *  \param  pTimer  Pointer to timer.
 */
/*************************************************************************************************/
static void wsfAppTimerRemove(wsfTimer_t *pTimer)
{
  wsfTimer_t  *pElem;
  wsfTimer_t  *pPrev = NULL;

  pElem = (wsfTimer_t *) wsfTimerAppTimerQueue.pHead;

  /* find timer in queue */
  while (pElem != NULL)
  {
    if (pElem == pTimer)
    {
      break;
    }
    pPrev = pElem;
    pElem = pElem->pNext;
  }

  /* if timer found remove from queue */
  if (pElem != NULL)
  {
    WsfQueueRemove(&wsfTimerAppTimerQueue, pTimer, pPrev);

    pTimer->isStarted = FALSE;
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Insert a timer into the queue sorted by the timer expiration.
 *
 *  \param  pTimer  Pointer to timer.
 *  \param  ticks   Timer ticks until expiration.
 */
/*************************************************************************************************/
static void wsfAppTimerInsert(wsfTimer_t *pTimer, wsfTimerTicks_t ticks)
{
  wsfTimer_t  *pElem;
  wsfTimer_t  *pPrev = NULL;

  /* task schedule lock */
  WsfTaskLock();

  /* if timer is already running stop it first */
  if (pTimer->isStarted)
  {
    wsfAppTimerRemove(pTimer);
  }

  pTimer->isStarted = TRUE;
  pTimer->ticks = ticks;

  pElem = (wsfTimer_t *) wsfTimerAppTimerQueue.pHead;

  /* find insertion point in queue */
  while (pElem != NULL)
  {
    if (pTimer->ticks < pElem->ticks)
    {
      break;
    }
    pPrev = pElem;
    pElem = pElem->pNext;
  }

  /* insert timer into queue */
  WsfQueueInsert(&wsfTimerAppTimerQueue, pTimer, pPrev);

  /* task schedule unlock */
  WsfTaskUnlock();
}

/*************************************************************************************************/
/*!
 *  \brief  Convert WSF ticks into RTC ticks.
 *
 *  \return Number of RTC ticks
 */
/*************************************************************************************************/
static uint32_t wsfAppTimerTicksToRtc(wsfTimerTicks_t wsfTicks)
{
  uint32_t numSec = wsfTicks / WSF_TIMER_TICKS_PER_SEC;
  uint32_t remainder = wsfTicks - numSec * WSF_TIMER_TICKS_PER_SEC;

  return ((numSec * PAL_RTC_TICKS_PER_SEC) + (remainder * WSF_TIMER_RTC_TICKS_PER_WSF_TICK));
}

/*************************************************************************************************/
/*!
 *  \brief  Return the number of ticks until the next timer expiration.  Note that this
 *          function can return zero even if a timer is running, indicating a timer
 *          has expired but has not yet been serviced.
 *
 *  \return The number of ticks until the next timer expiration.
 */
/*************************************************************************************************/
wsfTimerTicks_t wsfAppTimerNextExpiration(void)
{
  wsfTimerTicks_t ticks;

  /* task schedule lock */
  WsfTaskLock();

  if (wsfTimerAppTimerQueue.pHead == NULL)
  {
    ticks = 0;
  }
  else
  {
    ticks = ((wsfTimer_t *) wsfTimerAppTimerQueue.pHead)->ticks;
  }

  /* task schedule unlock */
  WsfTaskUnlock();

  return ticks;
}

/*************************************************************************************************/
/*!
 *  \brief  Initialize the timer service.  This function should only be called once
 *          upon system initialization.
 */
/*************************************************************************************************/
void WsfAppTimerInit(void)
{
  WSF_QUEUE_INIT(&wsfTimerAppTimerQueue);

  wsfAppTimerRtcLastTicks = PalRtcCounterGet();
  wsfAppTimerRtcRemainder = 0;
}

/*************************************************************************************************/
/*!
 *  \brief  Start a timer in units of seconds.
 *
 *  \param  pTimer  Pointer to timer.
 *  \param  sec     Seconds until expiration.
 */
/*************************************************************************************************/
void WsfAppTimerStartSec(wsfTimer_t *pTimer, wsfTimerTicks_t sec)
{
  WSF_TRACE_INFO2("WsfAppTimerStartSec pTimer:0x%x ticks:%u", (uint32_t)pTimer, WSF_TIMER_SEC_TO_TICKS(sec));

  /* insert timer into queue */
  wsfAppTimerInsert(pTimer, WSF_TIMER_SEC_TO_TICKS(sec));
}

/*************************************************************************************************/
/*!
 *  \brief  Start a timer in units of milliseconds.
 *
 *  \param  pTimer  Pointer to timer.
 *  \param  ms     Milliseconds until expiration.
 */
/*************************************************************************************************/
void WsfAppTimerStartMs(wsfTimer_t *pTimer, wsfTimerTicks_t ms)
{
  WSF_TRACE_INFO2("WsfAppTimerStartMs pTimer:0x%x ticks:%u", (uint32_t)pTimer, WSF_TIMER_MS_TO_TICKS(ms));

  /* insert timer into queue */
  wsfAppTimerInsert(pTimer, WSF_TIMER_MS_TO_TICKS(ms));
}

/*************************************************************************************************/
/*!
 *  \brief  Stop a timer.
 *
 *  \param  pTimer  Pointer to timer.
 */
/*************************************************************************************************/
void WsfAppTimerStop(wsfTimer_t *pTimer)
{
  WSF_TRACE_INFO1("WsfAppTimerStop pTimer:0x%x", pTimer);

  /* task schedule lock */
  WsfTaskLock();

  wsfAppTimerRemove(pTimer);

  /* task schedule unlock */
  WsfTaskUnlock();
}

/*************************************************************************************************/
/*!
 *  \brief  Update the timer service with the number of elapsed ticks.
 *
 *  \param  ticks  Number of ticks since last update.
 */
/*************************************************************************************************/
void WsfAppTimerUpdate(wsfTimerTicks_t ticks)
{
  wsfTimer_t  *pElem;

  /* task schedule lock */
  WsfTaskLock();

  pElem = (wsfTimer_t *) wsfTimerAppTimerQueue.pHead;

  /* iterate over timer queue */
  while (pElem != NULL)
  {
    /* decrement ticks while preventing underflow */
    if (pElem->ticks > ticks)
    {
      pElem->ticks -= ticks;
    }
    else
    {
      pElem->ticks = 0;

      /* timer expired; set task for this timer as ready */
      WsfTaskSetReady(pElem->handlerId, WSF_TIMER_EVENT);
    }

    pElem = pElem->pNext;
  }

  /* task schedule unlock */
  WsfTaskUnlock();
}

/*************************************************************************************************/
/*!
 *  \brief  Service expired timers for the given task.
 *
 *  \param  taskId      Task ID.
 *
 *  \return Pointer to timer or NULL.
 */
/*************************************************************************************************/
wsfTimer_t *WsfAppTimerServiceExpired(wsfTaskId_t taskId)
{
  wsfTimer_t  *pElem;
  wsfTimer_t  *pPrev = NULL;

  /* Unused parameters */
  (void)taskId;

  /* task schedule lock */
  WsfTaskLock();

  /* find expired timers in queue */
  if (((pElem = (wsfTimer_t *) wsfTimerAppTimerQueue.pHead) != NULL) &&
      (pElem->ticks == 0))
  {
    /* remove timer from queue */
    WsfQueueRemove(&wsfTimerAppTimerQueue, pElem, pPrev);

    pElem->isStarted = FALSE;

    /* task schedule unlock */
    WsfTaskUnlock();

    WSF_TRACE_INFO1("Timer expired pTimer:0x%x", pElem);

    /* return timer */
    return pElem;
  }

  /* task schedule unlock */
  WsfTaskUnlock();

  return NULL;
}

/*************************************************************************************************/
/*!
 *  \brief  Function for checking if there is an active timer and if there is enough time to
 *          go to sleep and going to sleep.
 */
/*************************************************************************************************/
void WsfAppTimerSleep(void)
{
  wsfTimerTicks_t nextExpiration;

  /* If PAL system is busy, no need to sleep. */
  if (PalSysIsBusy())
  {
    return;
  }

  nextExpiration = wsfAppTimerNextExpiration();

  if (nextExpiration > 0)
  {
    uint32_t awake = wsfAppTimerTicksToRtc(nextExpiration);
    uint32_t rtcCurrentTicks = PalRtcCounterGet();
    uint32_t elapsed = WSF_TIMER_RTC_ELAPSED_TICKS(rtcCurrentTicks);

    /* if we have time to sleep before timer expiration */
    // if ((awake - elapsed) > WSF_TIMER_MIN_RTC_TICKS_FOR_SLEEP)
    if ( (awake > elapsed) && (awake - elapsed) > WSF_TIMER_MIN_RTC_TICKS_FOR_SLEEP )
    {
      uint32_t compareVal = (rtcCurrentTicks + awake - elapsed) & PAL_MAX_RTC_COUNTER_VAL;

      /* set RTC timer compare */
      PalRtcCompareSet(0, compareVal);

      /* enable RTC interrupt */
      PalRtcEnableCompareIrq(0);

      wsf_timer_is_app = true;

      /* one final check for OS activity then enter sleep */
       PalEnterCs();
      if (wsfOsReadyToSleep())
      {
        PalLedOff(PAL_LED_ID_CPU_ACTIVE);
        PalSysSleep();
        PalLedOn(PAL_LED_ID_CPU_ACTIVE);
      }
       PalExitCs();
    }
    else
    {
      /* Not enough time to go to sleep. Let the system run till the pending timer expires. */
      LL_TRACE_WARN0("WsfTimerSleep, not enough time to sleep");
    }
  }
  else
  {
    /* disable RTC interrupt */
    PalRtcDisableCompareIrq(0);

    /* one final check for OS activity then enter sleep */
     PalEnterCs();
    if (wsfOsReadyToSleep())
    {
      PalLedOff(PAL_LED_ID_CPU_ACTIVE);
      PalSysSleep();
      PalLedOn(PAL_LED_ID_CPU_ACTIVE);
    }
     PalExitCs();
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Function for updating WSF timer based on elapsed RTC ticks.
 */
/*************************************************************************************************/
void WsfAppTimerSleepUpdate(void)
{
  uint32_t        elapsed;
  wsfTimerTicks_t wsfElapsed = 0;
  uint32_t        secBoundary, prevBoundary;

  /* Get current RTC tick count. */
  uint32_t rtcCurrentTicks = PalRtcCounterGet();

  if (rtcCurrentTicks != wsfAppTimerRtcLastTicks)
  {
    /* Check if RTC ticks go beyond seconds boundary. */
    if ((rtcCurrentTicks & WSF_TIMER_RTC_TICKS_SEC_MASK) != (wsfAppTimerRtcLastTicks & WSF_TIMER_RTC_TICKS_SEC_MASK))
    {
      /* Calculate current and previous second boundary */
      secBoundary = WSF_TIMER_RTC_ADD_TICKS((wsfAppTimerRtcLastTicks & WSF_TIMER_RTC_TICKS_SEC_MASK), PAL_RTC_TICKS_PER_SEC);
      prevBoundary = WSF_TIMER_RTC_SUB_TICKS(secBoundary, PAL_RTC_TICKS_PER_SEC);

      /* Check how many wsf ticks were already counted in this second. */
      /* Claim all uncounted wsf ticks within the second. */
      wsfElapsed += (WSF_TIMER_TICKS_PER_SEC - WSF_RTC_TICKS_TO_WSF(WSF_TIMER_RTC_SUB_TICKS(wsfAppTimerRtcLastTicks, prevBoundary)));

      while (WSF_TIMER_RTC_SUB_TICKS(rtcCurrentTicks, secBoundary) >= PAL_RTC_TICKS_PER_SEC)
      {
        wsfElapsed += WSF_TIMER_TICKS_PER_SEC;
        secBoundary = WSF_TIMER_RTC_ADD_TICKS(secBoundary, PAL_RTC_TICKS_PER_SEC);
      }

      wsfAppTimerRtcRemainder = 0;
      wsfAppTimerRtcLastTicks = secBoundary;
    }

    /* Elapsed must be less than PAL_RTC_TICKS_PER_SEC at this point. */
    elapsed = WSF_TIMER_RTC_ELAPSED_TICKS(rtcCurrentTicks);

    /* Rough conversion from RTC ticks to WFS ticks. It will be synchronized at the end of each second boundary. */
    wsfElapsed += (wsfTimerTicks_t) WSF_RTC_TICKS_TO_WSF(elapsed);

    if (wsfElapsed)
    {
      /* update last ticks and remainder */
      wsfAppTimerRtcLastTicks = rtcCurrentTicks;
      wsfAppTimerRtcRemainder = elapsed - (WSF_RTC_TICKS_TO_WSF(elapsed) *  WSF_TIMER_RTC_TICKS_PER_WSF_TICK);

      /* update wsf timers */
      WsfAppTimerUpdate(wsfElapsed);
    }
  }
}

#endif // ifdef CHECKPOINT
