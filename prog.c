#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_MASK 0xfffff000

int main(int argc, char *argv[]) {
    if (argc != 2) {    // 檢測輸入正確與否
        printf("Usage: %s <virtual address>\n", argv[0]);
        return 1;
    }
    
    /* 根據輸入計算出page_number、offset */
    uint32_t address = atoi(argv[1]);
    uint32_t page_number = address / PAGE_SIZE;
    uint32_t offset = address & (PAGE_SIZE - 1);

    printf("The address %u contains:\n", address);
    printf("page number = %u\n", page_number);
    printf("offset = %u\n", offset);

    return 0;
}
