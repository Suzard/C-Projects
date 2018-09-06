//ID: 88277651, edwardc6(Edward Chen)
#include <string.h>
#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/mman.h>
#include<sys/types.h>
#include <stdint.h> 
/* 
 * Little-endian structure bit field. 
 */
struct Header {
    uint16_t Allocated:1;
    uint16_t BlockSize:7;
    uint16_t BlockNumber:8;
};
#define REQ_BYTES(numBytes)	((numBytes) + sizeof(struct Header))
uint8_t *heap;
uint8_t numBlocks = 0;
static struct Header *next_head(struct Header *head)
{
    uint8_t *ptr;
    ptr = (uint8_t *) head;
    ptr = ptr + head->BlockSize;
    head = (struct Header *) ptr;
    return head;
}
static struct Header *next_head_p(uint8_t ** ptr)
{
    struct Header *head;
    head = (struct Header *) *ptr;
    *ptr = *ptr + head->BlockSize;
    head = (struct Header *) *ptr;
    return head;
}
static void set_head(struct Header *head, uint8_t bn, uint8_t size,
		     uint8_t allocate)
{
    if (!head)
	return;
    head->BlockNumber = bn;
    head->BlockSize = size;
    head->Allocated = allocate;
    return;
}
static void allocate(int numBytes)
{
    uint8_t *ptr = heap;
    struct Header *head = (struct Header *) heap;
    while ((ptr < heap + 126)
	   && (head->Allocated || (head->BlockSize < REQ_BYTES(numBytes)))) {
	head = next_head_p(&ptr);
    }
    head = (struct Header *) ptr;

    if (ptr > heap + 125)	
    {
	printf("Block number not found\n");
	return;
    }
    if (REQ_BYTES(numBytes) > head->BlockSize) {
	printf("\nNo space");
	return;
    } else if ((REQ_BYTES(numBytes)) < head->BlockSize) {
	int originalSize = head->BlockSize;
	int newSize = REQ_BYTES(numBytes);
	if (0)
	    printf("%s originalSize=%d newSize=%d\n", __func__,
		   originalSize, newSize);
	if (originalSize - newSize < 3) {
	    newSize = originalSize;
	    set_head(head, ++numBlocks, newSize, 1);
	} else {		
	    int remainderSize = originalSize - newSize;

	    set_head(head, ++numBlocks, newSize, 1);
	    head = next_head(head);
	    set_head(head, 0, remainderSize, 0);
	}
    } else {	
	int newSize = REQ_BYTES(numBytes);

	set_head(head, ++numBlocks, newSize, 1);
    }
    printf("%i\n", numBlocks);
}

static void freeBlock(uint8_t blockNum)
{
    uint8_t *ptr = heap;
    struct Header *head = (struct Header *) heap;
    while ((ptr < heap + 126) && (head->BlockNumber != blockNum)) {
	head = next_head_p(&ptr);
    }
    if (ptr > heap + 125) {
	printf("Block number not found\n");
	return;
    }
    head = (struct Header *) ptr;
    set_head(head, 0, head->BlockSize, 0);
}

static void blockList()
{
    uint8_t *ptr = heap;
    struct Header *head = (struct Header *) heap;
    int size, allocated;
    int Hsize, Hallocated;
    printf("Size    Allocated  Start          End\n");
    while (ptr < heap + 126) {
	Hsize = head->BlockSize;
	allocated = head->Allocated;
	printf("%-7i %-10s %-14u %u\n", Hsize,
	       (allocated == 1) ? "yes" : "no", (uint32_t) (ptr - heap),
	       (uint32_t) (ptr + Hsize - 1 - heap));
	head = next_head_p(&ptr);
    }
}

static void writeHeap(uint8_t blockNum, char character, uint8_t numWrites)
{
    uint8_t *ptr = heap;
    struct Header *head = (struct Header *) heap;
    while ((ptr < heap + 126) && head->BlockNumber != blockNum) {
	if (!head->BlockSize)
	    break;
	head = next_head_p(&ptr);
    }
    if (ptr > heap + 125) {
	printf("Block number not found\n");
	return;
    } else {
	uint8_t blockSize = head->BlockSize;

	if (blockSize - sizeof(struct Header) < numWrites) {
	    printf("Write too big\n");
	    return;
	} else {
	    uint8_t i;

	    for (i = 0; i < numWrites; i++) {
		*(ptr + sizeof(struct Header) + i) = (uint8_t) character;
	    }
	}
    }
}

static void printHeap(uint8_t blockNum, uint8_t numBytes)
{
    uint8_t *ptr = heap;
    struct Header *head = (struct Header *) heap;
    uint8_t max;
    max = head->BlockSize - sizeof(struct Header);
    while (((uint8_t *) head < heap + 126)
	   && head->BlockNumber != blockNum) {
	head = next_head_p(&ptr);
	max = head->BlockSize - sizeof(struct Header);
	if (0)
	    printf("loop %s max=%d ptr=%p bn=%d head=%p h_bs=%d h_bn=%d\n",
		   __func__, max, ptr, *ptr, head, head->BlockSize,
		   head->BlockNumber);
    }
    ptr = (uint8_t *) head;
    if (ptr > heap + 125) {
	printf("Block number not found\n");
	return;
    } else {
	uint8_t i;
	uint8_t display;
	if(max > numBytes){
		display = numBytes;
	}
	else if (max < numBytes){
		display = max;
	}
	else{
		display = numBytes;
	}
	for (i = 0; i < display; i++) {
	    printf("%c", (char) (*(ptr + 2 + i)));
	}
	printf("\n");
    }
}

static void printHeader(uint8_t blockNum)
{
    uint8_t *ptr = heap;
    struct Header *head = (struct Header *) heap;
    while ((ptr < heap + 126) && head->BlockNumber != blockNum) {
	head = next_head_p(&ptr);
    }
    if (ptr > heap + 125) {
	printf("Block number not found\n");
	return;
    } else {
	uint16_t *h = (uint16_t *) ptr;
	printf("%04x\n", *h);
    }
}

int main(int argc, char *argv[])
{
    char input[256];
    char *token;
    struct Header *head;
    heap = (uint8_t *) malloc(127 * sizeof(uint8_t));
    memset(heap, 0, 127);
    if (heap == NULL) {
	perror("Malloc Error");
	exit(1);
    }
    head = (struct Header *) heap;
    head->Allocated = 0;
    head->BlockNumber = 0;
    head->BlockSize = 127;
    while (1) {
	printf(">");
	fgets(input, 256, stdin);
	token = strtok(input, " \n");
	if (token != NULL) {
	    if (strcmp(token, "allocate") == 0) {
		token = strtok(NULL, " \n");
		allocate(atoi(token));
	    } else if (strcmp(token, "free") == 0) {
		token = strtok(NULL, " \n");
		freeBlock(atoi(token));
	    } else if (strcmp(token, "blocklist") == 0) {
		blockList();
	    } else if (strcmp(token, "writeheap") == 0) {
		int blockNum, writeNum;
		char writeChar;
		token = strtok(NULL, " \n");
	        blockNum = atoi(token);
	        token = strtok(NULL, " \n");
		writeChar = *token;
		token = strtok(NULL, " \n");
		writeNum = atoi(token);
		writeHeap(blockNum, writeChar, writeNum);
	    } else if (strcmp(token, "printheap") == 0) {
		int blockNum, printNum;
		token = strtok(NULL, " \n");
		blockNum = atoi(token);
		token = strtok(NULL, " \n");
		printNum = atoi(token);
		printHeap(blockNum, printNum);
	    } else if (strcmp(token, "printheader") == 0) {
		token = strtok(NULL, " \n");
		printHeader(atoi(token));
	    } else if (strcmp(token, "quit") == 0) {
		break;
	    } else {
		printf("invalid command\n");
	    }
	} else
	    printf("Blankspace\n");
    }
    free(heap);
    return 0;
}
