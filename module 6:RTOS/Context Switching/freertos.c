/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : freertos.c
  * @brief          : FreeRTOS configuration and tasks
  ******************************************************************************
  */
/* USER CODE END Header */

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "main.h"

#include <stdio.h>
#include <stdbool.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* Private typedef -----------------------------------------------------------*/

typedef enum
{
    TASK_READY,
    TASK_RUNNING,
    TASK_FINISHED

} TaskState;


typedef enum
{
    EVENT_NONE,
    EVENT_TIMER,
    EVENT_UART_RX,
    EVENT_BUTTON,
    EVENT_SHUTDOWN

} EventType;


typedef struct
{
    EventType type;
    int value;

} Event;


typedef struct
{
    int id;

    const char *name;

    TaskState state;

    int priority;

    bool has_pending_event;

    Event pending_event;

} TaskControlBlock;


/* Private variables ---------------------------------------------------------*/

osThreadId_t ControlTaskHandle;
osThreadId_t IOTaskHandle;


/* Task attributes */

const osThreadAttr_t ControlTask_attributes =
{
    .name = "ControlTask",

    .stack_size = 512 * 4,

    .priority = (osPriority_t) osPriorityHigh
};


const osThreadAttr_t IOTask_attributes =
{
    .name = "IOTask",

    .stack_size = 512 * 4,

    .priority = (osPriority_t) osPriorityNormal
};


/* USER CODE BEGIN Variables */

TaskControlBlock controlTask;
TaskControlBlock ioTask;

Event current_event;

volatile bool has_current_event = false;


/* USER CODE END Variables */


/* Private function prototypes -----------------------------------------------*/

void StartControlTask(void *argument);
void StartIOTask(void *argument);


/* USER CODE BEGIN FunctionPrototypes */

const char *EventName(EventType type);

const char *StateName(TaskState state);

void PostEvent(TaskControlBlock *task, Event event);

void PrintTaskTable(void);


/* USER CODE END FunctionPrototypes */


/* USER CODE BEGIN 0 */

/* ============================================================
 * EVENT NAME
 * ============================================================ */

const char *EventName(EventType type)
{
    switch (type)
    {
        case EVENT_TIMER:
            return "TIMER";

        case EVENT_UART_RX:
            return "UART_RX";

        case EVENT_BUTTON:
            return "BUTTON";

        case EVENT_SHUTDOWN:
            return "SHUTDOWN";

        default:
            return "NONE";
    }
}


/* ============================================================
 * STATE NAME
 * ============================================================ */

const char *StateName(TaskState state)
{
    switch (state)
    {
        case TASK_READY:
            return "READY";

        case TASK_RUNNING:
            return "RUNNING";

        case TASK_FINISHED:
            return "FINISHED";

        default:
            return "UNKNOWN";
    }
}


/* ============================================================
 * POST EVENT
 * ============================================================ */

void PostEvent(TaskControlBlock *task, Event event)
{
    taskENTER_CRITICAL();

    /*
     * Don't send event to a finished task
     */
    if (task->state == TASK_FINISHED)
    {
        printf("[EVENT] Task already finished: %s\r\n",
               task->name);

        taskEXIT_CRITICAL();

        return;
    }


    /*
     * Only one pending event allowed
     */
    if (task->has_pending_event)
    {
        printf("[EVENT] Pending event already exists for %s\r\n",
               task->name);

        taskEXIT_CRITICAL();

        return;
    }


    task->pending_event = event;

    task->has_pending_event = true;

    task->state = TASK_READY;


    printf("[EVENT] %s -> %s | Priority=%d | Value=%d\r\n",

           EventName(event.type),

           task->name,

           task->priority,

           event.value);


    taskEXIT_CRITICAL();
}


/* ============================================================
 * TASK TABLE
 * ============================================================ */

void PrintTaskTable(void)
{
    printf("\r\n");
    printf("============================================\r\n");
    printf("              TASK TABLE\r\n");
    printf("============================================\r\n");

    printf("ID   NAME           STATE       PRIORITY\r\n");

    printf("%-4d %-14s %-11s %d\r\n",

           controlTask.id,

           controlTask.name,

           StateName(controlTask.state),

           controlTask.priority);


    printf("%-4d %-14s %-11s %d\r\n",

           ioTask.id,

           ioTask.name,

           StateName(ioTask.state),

           ioTask.priority);


    printf("============================================\r\n");
}


/* ============================================================
 * CONTROL TASK
 * ============================================================ */

void StartControlTask(void *argument)
{
    printf("\r\n");
    printf("[CONTROL] Control Task Started\r\n");


    while (1)
    {
        controlTask.state = TASK_RUNNING;


        /*
         * Check if event is available
         */

        taskENTER_CRITICAL();

        if (controlTask.has_pending_event)
        {
            current_event = controlTask.pending_event;

            controlTask.has_pending_event = false;

            has_current_event = true;
        }
        else
        {
            has_current_event = false;
        }

        taskEXIT_CRITICAL();


        /*
         * Process event
         */

        if (has_current_event)
        {
            switch (current_event.type)
            {
                case EVENT_TIMER:

                    printf("[CONTROL] Timer Tick = %d\r\n",
                           current_event.value);

                    break;


                case EVENT_BUTTON:

                    printf("[CONTROL] Button ID = %d\r\n",
                           current_event.value);

                    /*
                     * Blink LED
                     */

                    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

                    break;


                case EVENT_SHUTDOWN:

                    printf("[CONTROL] Shutdown\r\n");

                    controlTask.state = TASK_FINISHED;

                    /*
                     * Suspend this task permanently
                     */

                    osThreadSuspend(ControlTaskHandle);

                    break;


                default:

                    printf("[CONTROL] Unknown Event\r\n");

                    break;
            }
        }


        has_current_event = false;


        /*
         * Task completed its current work.
         */

        controlTask.state = TASK_READY;


        /*
         * This is NOT swapcontext().
         *
         * FreeRTOS performs the actual context switch.
         */

        osDelay(10);
    }
}


/* ============================================================
 * IO TASK
 * ============================================================ */

void StartIOTask(void *argument)
{
    printf("\r\n");
    printf("[IO] IO Task Started\r\n");


    while (1)
    {
        ioTask.state = TASK_RUNNING;


        /*
         * Check pending event
         */

        taskENTER_CRITICAL();

        if (ioTask.has_pending_event)
        {
            current_event = ioTask.pending_event;

            ioTask.has_pending_event = false;

            has_current_event = true;
        }
        else
        {
            has_current_event = false;
        }

        taskEXIT_CRITICAL();


        /*
         * Process event
         */

        if (has_current_event)
        {
            switch (current_event.type)
            {
                case EVENT_UART_RX:

                    printf("[IO] UART RX = '%c' (%d)\r\n",

                           (char)current_event.value,

                           current_event.value);

                    break;


                case EVENT_SHUTDOWN:

                    printf("[IO] Shutdown\r\n");

                    ioTask.state = TASK_FINISHED;

                    /*
                     * Suspend this task permanently
                     */

                    osThreadSuspend(IOTaskHandle);

                    break;


                default:

                    printf("[IO] Unknown Event\r\n");

                    break;
            }
        }


        has_current_event = false;


        ioTask.state = TASK_READY;


        /*
         * Allow scheduler to run another task
         */

        osDelay(10);
    }
}


/* ============================================================
 * FREE RTOS INITIALIZATION
 * ============================================================ */

void MX_FREERTOS_Init(void)
{
    /* ---------------- CONTROL TASK ---------------- */

    controlTask.id = 0;

    controlTask.name = "ControlTask";

    controlTask.state = TASK_READY;

    /*
     * Our logical priority
     * Higher number = higher priority
     */

    controlTask.priority = 3;

    controlTask.has_pending_event = false;


    /* ---------------- IO TASK ---------------- */

    ioTask.id = 1;

    ioTask.name = "IOTask";

    ioTask.state = TASK_READY;

    ioTask.priority = 2;

    ioTask.has_pending_event = false;


    /* ---------------- CREATE TASKS ---------------- */

    ControlTaskHandle =
        osThreadNew(StartControlTask,
                    NULL,
                    &ControlTask_attributes);


    IOTaskHandle =
        osThreadNew(StartIOTask,
                    NULL,
                    &IOTask_attributes);


    printf("\r\n");
    printf("============================================\r\n");
    printf("       CONTEXT SWITCHING DEMO\r\n");
    printf("============================================\r\n");


    PrintTaskTable();


    /*
     * Give scheduler time to start tasks.
     */

    osDelay(100);


    /* ========================================================
     * EVENT 1
     * UART event -> IO task
     * ======================================================== */

    PostEvent(&ioTask,

              (Event)
              {
                  EVENT_UART_RX,
                  'A'
              });


    /*
     * ========================================================
     * EVENT 2
     * Button event -> Control task
     * ========================================================
     */

    PostEvent(&controlTask,

              (Event)
              {
                  EVENT_BUTTON,
                  1
              });
}
