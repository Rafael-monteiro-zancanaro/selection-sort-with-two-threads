#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <memory.h>

#if __has_include (<pthread.h>)
  #include <pthread.h>
#endif

#if __has_include (<unistd.h>)
  #include <unistd.h>
#else
  #include <windows.h>
#endif

#define RESPONSE_MAX_SIZE 128

typedef struct ArrayRange {
  uint8_t startIndex;
  uint8_t endIndex;
} ArrayRange;

typedef struct ThreadData {
  size_t id;
  ArrayRange *range;
  bool isFragmentSorted;
  int *array;
} ThreadData;


uint8_t GetArraySizeFromUser();
void SetArrayRangesByArraySize(ThreadData *firstThread, ThreadData *secondThread, uint8_t *arraySize);
void ThreadDetails(ThreadData *threadData);
void FeedArrayWithInt(uint8_t arraySize, int *array);
void ArrayDetails(const int *array, uint_fast8_t arraySize);
bool NumberAlreadyInArray(const int *array, uint_fast8_t arraySize, int number);

int main(void) {
  ThreadData firstThread = {
    .isFragmentSorted = false,
    .range = NULL,
    .id = (size_t) 1,
    .array = NULL
  };

  ThreadData secondThread = {
    .isFragmentSorted = false,
    .range = NULL,
    .id = (size_t) 2,
    .array = NULL
  };

  uint8_t arraySize = GetArraySizeFromUser();
  SetArrayRangesByArraySize(&firstThread, &secondThread, &arraySize);
  ThreadDetails(&firstThread);
  ThreadDetails(&secondThread);

  int array[(size_t) arraySize];
  memset(array, 0, arraySize);
  FeedArrayWithInt(arraySize, array); 
  ArrayDetails(array, (uint_fast8_t) arraySize);

  return EXIT_SUCCESS;
}

uint8_t GetArraySizeFromUser() {
  int convertedResult = 0;
  char rawUserInput[RESPONSE_MAX_SIZE] = "\0";
  puts("Insira o tamanho do array que vai ser buscado");

  if (!fgets(rawUserInput, sizeof(rawUserInput), stdin)) {
    puts("Nao foi possivel converter o valor informado");
    exit(EXIT_FAILURE);
  }

  if (1 != sscanf(rawUserInput, "%d", &convertedResult)) {
    puts("Insira um valor válido");
    exit(EXIT_FAILURE);
  }

  if (convertedResult < 2 || convertedResult > UINT8_MAX) {
    puts("O valor informado deve ser entre 2 e 255");
    exit(EXIT_FAILURE);
  }
  
  return (uint8_t) convertedResult;
}


void SetArrayRangesByArraySize(ThreadData *firstThread, ThreadData *secondThread, uint8_t *arraySize) {
  uint8_t middleIndex = (uint8_t) round((double)(*arraySize) / 2);

  ArrayRange firstThreadRange = {
    .startIndex = 0,
    .endIndex = middleIndex
  };

  ArrayRange secondThreadRange = {
    .startIndex = middleIndex + 1,
    .endIndex = (*arraySize) - 1
  };
  
  firstThread->range = &firstThreadRange;
  secondThread->range = &secondThreadRange;
}


void FeedArrayWithInt(uint8_t arraySize, int *array) {
  srand(time(NULL));
  size_t iterations = 0;
  for (uint8_t i = 0; i < arraySize; ++i) {
    iterations++;
    int randInt = rand() % arraySize;
    if (NumberAlreadyInArray(array, (uint_fast8_t) arraySize, randInt)) {
      i--;
      continue;
    }
    array[i] = randInt;
  }
  printf("Generated unique integer array (%ld iterations)\n", iterations);
}

bool NumberAlreadyInArray(const int *array, uint_fast8_t arraySize, int number) {
   for (uint_fast8_t i = 0; i < arraySize; ++i) {
    if (array[i] == number) {
      return true;
    }
  }
  return false;
}

void ThreadDetails(ThreadData *threadData) {
  printf("[THREAD %ld]\n", threadData->id);
  printf("\tRange: (%d .. %d) (INCLUSIVE)\n", threadData->range->startIndex, threadData->range->endIndex);
  printf("\tSorted? %s\n", threadData->isFragmentSorted? "Yes": "No");
  printf("\tArray ptr: %p\n", threadData->array);
}

void ArrayDetails(const int *array, uint_fast8_t arraySize) {
  printf("[ ");
  for (uint_fast8_t i = 0; i < arraySize; ++i) {
    printf("%d%s", array[i], i + 1 == arraySize? " ]\n": ", ");
  }
}
