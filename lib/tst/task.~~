#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include "glog.h"

#if 1
// PendSV 인터럽트 발생 플래그
volatile bool pendSV_flag = false;

// 현재 실행 중인 태스크 ID
volatile uint32_t current_task_id = 0;

// 태스크 구조체
typedef struct task {
  uint32_t id;
  void (*func)(void);
  jmp_buf env;
} task_t;

void task0(void);
void task1(void);

// 태스크 목록
task_t tasks[2] = {
  {0, task0, },
  {1, task1, },
};

// PendSV 인터럽트 처리 함수
void PendSV_Handler(void) {
  // PendSV 인터럽트 플래그 초기화
  pendSV_flag = false;

  // 현재 실행 중인 태스크 저장
  uint32_t prev_task_id = current_task_id;

  // 다음 태스크 선택
  current_task_id = (current_task_id + 1) % 2;

  // longjmp()를 이용하여 다음 태스크로 전환
  longjmp((tasks[current_task_id].env), 1);
}

// Task 0 함수
void task0(void) {
  while (true) {
    // Task 0 코드
    ERROR_LOG("I am a boy\n");

    // PendSV 인터럽트 발생 요청
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
//    SCB->SHCSR |= SCB_SHCSR_PENDSVSET_Msk;

    // 다음 태스크로 전환될 때까지 대기
    __WFI();
  }
}

// Task 1 함수
void task1(void) {
  while (true) {
    // Task 1 코드
    ERROR_LOG("Good Morning\n");

    // PendSV 인터럽트 발생 요청
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

    // 다음 태스크로 전환될 때까지 대기
    __WFI();
  }
}

// Main 함수
void  pendmain(void) {
  // PendSV 인터럽트 활성화
  NVIC_SetPriority(PendSV_IRQn, 0);
  NVIC_EnableIRQ(PendSV_IRQn);

  // Task 0 시작
  setjmp(tasks[0].env);
  longjmp(tasks[0].env, 1);

  while (true) {
    // Main 루프 코드
  }
}

#else

#include <stdint.h>
#include <stdbool.h>

// Task 스택 크기
#define STACK_SIZE 1024

// Task 구조체
typedef struct {
  void (*task)(void);
  int *stack;
  int stack_top;
  jmp_buf env;
} Task;

// 현재 실행 중인 Task
Task *current_task;

// Task 목록
Task tasks[2];

// PendSV 인터럽트 핸들러
void PendSV_Handler(void) {
  // 현재 Task의 스택 포인터 저장
  current_task->stack_top = __get_MSP();

  // 다음 Task 선택
  current_task = &tasks[1 - (current_task - tasks)];

  // 다음 Task의 스택 포인터 설정
  __set_MSP(current_task->stack_top);

  // 다음 Task 시작
  longjmp(current_task->stack, 1);
}

// Task1
void task1(void) {
  while (true) {
    // Task1 코드
    gdebug(2,"task 1 \n");
    //HAL_Delay(300);
    // ...
    
    // PendSV 인터럽트 발생 요청
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
}

// Task2
void task2(void) {
  while (true) {
    // Task2 코드
    gdebug(2,"task 2 \n");
    //HAL_Delay(300);
    // ...
    
    // PendSV 인터럽트 발생 요청
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
}

void pendmain(void) {
  // Task 스택 초기화
  tasks[0].stack = (int *)malloc(STACK_SIZE);
  tasks[1].stack = (int *)malloc(STACK_SIZE);

  // Task 초기화
  tasks[0].task = task1;
  tasks[1].task = task2;

  // 현재 Task 설정
  current_task = &tasks[0];

  // PendSV 인터럽트 활성화
  NVIC_EnableIRQ(PendSV_IRQn);

  // Task1 시작
  setjmp(tasks[0].env);
  longjmp(tasks[0].env, 1);
  //longjmp(current_task->stack, 1);

  while (true) {}
}

#endif
