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

/* PROTOTYPES */
uint_fast8_t GetArraySizeFromUser();

void SetArrayRangesByArraySize(ThreadData *firstThread, ThreadData *secondThread, uint8_t *arraySize);

void ThreadDetails(ThreadData *threadData);

void FeedArrayWithInt(uint_fast8_t arraySize, int *array);

void ArrayDetails(int *array, uint_fast8_t arraySize);

bool NumberAlreadyInArray(int *array, uint_fast8_t arraySize, int number);

void DeallocateResources(ThreadData *threadData);
/* END OF PROTOTYPES */

int main(void)
{

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

        uint_fast8_t arraySize = GetArraySizeFromUser();
        SetArrayRangesByArraySize(&firstThread, &secondThread, &arraySize);
        int array[(size_t) arraySize];
        memset(array, 0, arraySize);
        FeedArrayWithInt(arraySize, array); 
        ArrayDetails(array, arraySize);
        firstThread.array = array;
        secondThread.array = array;
        ThreadDetails(&firstThread);
        ThreadDetails(&secondThread);
        DeallocateResources(&firstThread);
        DeallocateResources(&secondThread);
        return EXIT_SUCCESS;
}


/* GetArraySizeFromUser - Get main array size from input given by user 
 *
 * This function captures the first integer value from user.
 * It have a max integer size, defined by UINT8_MAX (255). If something goes wrong
 * (i.e: fully dirty input or value overflows UINT8_MAX), the program will break.
*/
uint_fast8_t GetArraySizeFromUser(void) 
{
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

        return (uint_fast8_t) convertedResult;
}

/* SetArrayRangesByArraySize - Configure Threads range by given array size
 * @firstThread: Pointer to first thread data.
 * @secondThread: Pointer to second thread data.
 * @arraySize: Pointer to main array size.
 *
 * This function separates the portion of the array that the two threads will be responsible for.
 * If the array size is odd, the first thread will take it.
*/
void SetArrayRangesByArraySize(ThreadData *firstThread, ThreadData *secondThread, uint8_t *arraySize) 
{
        uint8_t middleIndex = (uint8_t) round((double)(*arraySize) / 2);
        ArrayRange *fRange = (struct ArrayRange*) malloc(sizeof(ArrayRange));
        ArrayRange *sRange = (struct ArrayRange*) malloc(sizeof(ArrayRange));
        fRange->startIndex = 0;
        fRange->endIndex = middleIndex;

        sRange->startIndex = middleIndex + 1;
        sRange->endIndex = (*arraySize) - 1;

        firstThread->range = fRange;
        secondThread->range = sRange;
}


/* FeedArrayWithInt - Insert unique random integers in an array 
 * @arraySize: Size of the array.
 * @array: Pointer to the array that will be fed.
 *
 * This function inserts unique random integers to some int array.
 * The range of values is always from 0 to arraySize - 1.
*/
void FeedArrayWithInt(uint_fast8_t arraySize, int *array)
{
        srand(time(NULL));
        size_t iterations = 0;
        for (uint_fast8_t i = 0; i < arraySize; ++i) {
                iterations++;
                int randInt = rand() % arraySize;
                if (NumberAlreadyInArray(array, arraySize, randInt)) {
                        i--;
                        continue;
                }
                array[i] = randInt;
        }
        printf("Generated unique integer array (%ld iterations)\n", iterations);
}


/* NumberAlreadyInArray - Checks if a number is in the main array 
 * @array: Pointer to the main array
 * @arraySize: Size of the array 
 * @number: Number being seek.
 *
 * This function seek one number over an array.
 * If the number is found returns true, false otherwise.
*/
bool NumberAlreadyInArray(int *array, uint_fast8_t arraySize, int number)
{
        for (uint_fast8_t i = 0; i < arraySize; ++i) {
                if (array[i] == number) {
                        return true;
                }
        }
        return false;
}

/* ThreadDetails - Prints details of thread data 
 * @threadData: Pointer to the Thread Data.
 *
 * This function prints range, array address pointer 
 * and information about the ordenation of array. 
*/
void ThreadDetails(ThreadData *threadData) {
        printf("[THREAD %ld]\n", threadData->id);
        printf("\tRange: (%d .. %d) (INCLUSIVE)\n", threadData->range->startIndex, threadData->range->endIndex);
        printf("\tSorted? %s\n", threadData->isFragmentSorted? "Yes": "No");
        printf("\tArray ptr: %p\n", threadData->array);
}

/* ThreadDetails - Prints details of array 
 * @array: Pointer to the main array.
 * @arraySize: size of the main array.
 *
 * This function prints all elements of main array. 
*/
void ArrayDetails(int *array, uint_fast8_t arraySize)
{
        printf("[ ");
        for (uint_fast8_t i = 0; i < arraySize; ++i) {
                printf("%d%s", array[i], i + 1 == arraySize? " ]\n": ", ");
        }
}

/* DeallocateResources - Free Used Resources from Heap
 * @threadData: Pointer to Thread Data.
 *
 * This function frees heap memory allocated by thread data variable.
*/
void DeallocateResources(ThreadData *threadData)
{
        printf("Cleaning resources used by thread %ld...\n", threadData->id);
        if (threadData->range != NULL) {
                free(threadData->range);
        }
}
