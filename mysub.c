#include <stdbool.h>
#include <stdlib.h>

#define ARRAYLIMIT_ 500
#define SIZEFOUR 4
#define CASE2 2
#define CASE4 4

//*************************************************************************************
//************************ Useful Global Variables ************************************
//*************************************************************************************

//save the four_element arrays in the array of arrays return QCOUNT as 2, 3 vs 1 elements
int * ArraysReturn2[ARRAYLIMIT_];
int ArraysReturn2_count = 0;
int ArraysReturn2_count_copy = 0;//keep a copy of dynamic allocated number of ArraysReturn2 in order to deallocate it

//save the four_element arrays in the array of arrays return QCOUNT as 4, 4 same elements
int * ArraysReturn4[ARRAYLIMIT_];
int ArraysReturn4_count = 0;
int ArraysReturn4_count_copy = 0;//keep a copy of dynamic allocated number of ArraysReturn4 in order to deallocate it

//save the remaining elements left if there are available
int remaining_elements[SIZEFOUR];
int remain_count = 0;

//keep an array to save a copy of array return 0
int single_case0_arr[SIZEFOUR];
int case0_count = 0;

//Arrays_with_2_zeros, Arrays_with_2_ones are to classify binary groups: 1110 or 0001
int * Arrays_with_2_zeros[ARRAYLIMIT_];
int Arrays_with_2_zeros_count = 0;

int * Arrays_with_2_ones[ARRAYLIMIT_];
int Arrays_with_2_ones_count = 0;


//Arrays_with_4_zeros, Arrays_with_4_ones are to classify binary groups: 1111 or 0000
int * Arrays_with_4_zeros[ARRAYLIMIT_];
int Arrays_with_4_zeros_count = 0;

int * Arrays_with_4_ones[ARRAYLIMIT_];
int Arrays_with_4_ones_count = 0;


//struct to save the data structure of: leader array of a binary classification, and number of members in that group
struct Leader
{
    int * arr;
    int count;
};
struct Leader Leader_arr[ARRAYLIMIT_];

//*************************************************************************************

void init_struct_array() {
    for (int i = 0; i < ARRAYLIMIT_; i++) {
        Leader_arr[i].count = 0;
    }
}

void print(int * index_array[], int n) {
    printf("n: %d\n", n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 4; j++)
            printf("->%d\n", index_array[i][j]);
    }
}

//deallocate and free memory
void reset() {
    for (int i = 0; i < ArraysReturn2_count_copy; i++)
        free(ArraysReturn2[i]);

    for (int i = 0; i < ArraysReturn4_count_copy; i++)
        free(ArraysReturn4[i]);
    
    ArraysReturn2_count = 0;
    ArraysReturn4_count = 0;
    
    remain_count = 0;
    case0_count = 0;
    Arrays_with_2_zeros_count = 0;
    Arrays_with_2_ones_count = 0;
    Arrays_with_4_zeros_count = 0;
    Arrays_with_4_ones_count = 0;
}

//swap the element in an array
void swap(int flag, int index, int e1, int e2) {
    if (flag == 2) {
        int tmp = ArraysReturn2[index][e1];
        ArraysReturn2[index][e1] = ArraysReturn2[index][e2];
        ArraysReturn2[index][e2] = tmp;
    }
    else if (flag == 4) {
        int tmp = ArraysReturn4[index][e1];
        ArraysReturn4[index][e1] = ArraysReturn4[index][e2];
        ArraysReturn4[index][e2] = tmp;
    }
}

//swap the array pointer
void swap_arr(int flag, int i, int j) {
    if (flag == 2) {
        int * tmp = ArraysReturn2[i];
        ArraysReturn2[i] = ArraysReturn2[j];
        ArraysReturn2[j] = tmp;
    }
    else if (flag == 4) {
        int * tmp = ArraysReturn4[i];
        ArraysReturn4[i] = ArraysReturn4[j];
        ArraysReturn4[j] = tmp;
    }
}

//if there is an 4-element array that return 0 and case0_count is 0, save a copy of array return 0
void copy_single_case0_arr(int * four_element) {
    for (int k = 0; k < 4; k++)
        single_case0_arr[k] = four_element[k];
    case0_count = 1;
}

void ERROR_CHECKING(int returnVal, char * func_info) {
    if (returnVal < 0) {
        printf("ERROR: Duplicated indices stored in four_element in %s\n", func_info);
        exit(0);
    }
}


//find 0s, 2s, 4s, and remaining elements, n/4 QCOUNT to scan the whole n numbers
void first_scan(int n) {
    
    for (int i = 1; i <= n; i += 4) {
        int * four_element = (int *) malloc(sizeof(int) * 4);
        
        int j = 0;
        
        for (; j < 4 && i + j <= n; j++) {
            four_element[j] = i + j;
        }
        
        if (j == 4) {
            //full four element
            int returnVal = QCOUNT(1, four_element);
            ERROR_CHECKING(returnVal, "[first_scan]");
            if (returnVal == 0 && case0_count == 0) copy_single_case0_arr(four_element);
            else if (returnVal == 2) ArraysReturn2[ArraysReturn2_count++] = four_element;
            else if (returnVal == 4) ArraysReturn4[ArraysReturn4_count++] = four_element;
        }
        else {
            //copy the remaining elements into the remaining array
            for (int k = 0; k < j; k++)
                remaining_elements[k] = four_element[k];
            remain_count = j;
        }
    }
    
    //keep a copy, I may modifiy the ArraysReturn2_count, ArraysReturn4_count later
    ArraysReturn2_count_copy = ArraysReturn2_count;
    ArraysReturn4_count_copy = ArraysReturn4_count;
}

//compare the given array two by two and find the ultimate winner of all of them
void compare_two_by_two(int flag) {
    //checking flag to determine find the winner of which case group
    int arr_count = ArraysReturn4_count;
    int ** arr = ArraysReturn4;
    if (flag == 2) {
        arr_count = ArraysReturn2_count;
        arr = ArraysReturn2;
    }
    
    if (arr_count == 0) return;
    
    //first: loop through the array and compare them 2 by 2 and save the array to struct with
    //the feature of leader array pointer and number of members in the same group
    int j = 0;
    for (int i = 0; i < arr_count; i += 2) {
        if (i + 1 >= arr_count) {
            Leader_arr[j].arr = arr[i];
            Leader_arr[j++].count = 1;
        }
        else {
            int four_element[4];
            
            four_element[0] = arr[i][0];
            four_element[1] = arr[i][1];
            four_element[2] = arr[i + 1][0];
            four_element[3] = arr[i + 1][1];
            
            int r = QCOUNT(1, four_element);
            ERROR_CHECKING(r, "[compare_two_by_two]");

            if (r == 4) {
                Leader_arr[j].arr = arr[i];
                Leader_arr[j++].count += 2;
            }
            else if (r == 0 && case0_count == 0){
                copy_single_case0_arr(four_element);
            }
        }
    }

    //continue comparing two by two until find the ultimate winner
    while (j > 1) {
        int k = 0;
        
        for (int i = 0; i < j; i += 2) {
            if (i + 1 >= j) {
                Leader_arr[k].arr = Leader_arr[i].arr;
                Leader_arr[k++].count = Leader_arr[i].count;
            }
            else {
                int four_element[4];
                
                four_element[0] = Leader_arr[i].arr[0];
                four_element[1] = Leader_arr[i].arr[1];
                four_element[2] = Leader_arr[i + 1].arr[0];
                four_element[3] = Leader_arr[i + 1].arr[1];
                
                int r = QCOUNT(1, four_element);
                ERROR_CHECKING(r, "[compare_two_by_two]");
                if (r == 4) {
                    Leader_arr[k].arr = Leader_arr[i].arr;
                    Leader_arr[k++].count = Leader_arr[i].count + Leader_arr[i + 1].count;
                }
                else if (r == 0) {
                    if (case0_count == 0) copy_single_case0_arr(four_element);
                    if (Leader_arr[i].count > Leader_arr[i + 1].count) {
                        Leader_arr[k].arr = Leader_arr[i].arr;
                        Leader_arr[k++].count = Leader_arr[i].count - Leader_arr[i + 1].count;
                    }
                    else if (Leader_arr[i].count < Leader_arr[i + 1].count) {
                        Leader_arr[k].arr = Leader_arr[i + 1].arr;
                        Leader_arr[k++].count = Leader_arr[i + 1].count - Leader_arr[i].count;
                    }
                }
            }
        }
        j = k;
    }
    
    if (j == 0) Leader_arr[j].count = 0;
}


//use any case4 array to help each array in ArraysReturn2 always having the
//two same elements at position 0 and 1: such as 1011 -> 1110
//or if there is no case4 array available to help, there will be a case2 array whose first two elements are same to help exchange the position
//then divide the ArraysReturn2 whose position that is no exchanged into two groups and find the winner
//for the ArraysReturn2 whose position that is actually exchanged, compare them two by two and find the ultimate winner of all of them
void deal_with_ArraysReturn2(int * array, bool is_two_same_element) {
    
    int unexchanged_count = 0;
    int four_element[4];
    
    for (int i = 0; i < ArraysReturn2_count; i++) {
        
        int * arr = ArraysReturn2[i];
        
        four_element[0] = arr[0];
        four_element[1] = arr[1];
        if (array[0] == arr[0]) {
            if (is_two_same_element) continue;
            four_element[2] = array[2];
            four_element[3] = array[3];
        }
        else {
            four_element[2] = array[0];
            four_element[3] = array[1];
        }
        
        int returnVal = QCOUNT(1, four_element);
        
        ERROR_CHECKING(returnVal, "[deal_with_ArraysReturn2]");
        
        if (returnVal == 2) {
            swap(CASE2, i, 0, 2);
            swap(CASE2, i, 1, 3);
            swap_arr(CASE2, unexchanged_count++, i);
        }
        else if (returnVal == 0) {
            if (case0_count == 0) {
                copy_single_case0_arr(four_element);
            }
            Arrays_with_2_zeros[Arrays_with_2_zeros_count++] = ArraysReturn2[i];
        }
        else if (returnVal == 4) {
            Arrays_with_2_ones[Arrays_with_2_ones_count++] = ArraysReturn2[i];
        }
        
    }
    
    ArraysReturn2_count = unexchanged_count;
    compare_two_by_two(CASE2);
    if (Leader_arr[0].count != 0) {
        four_element[0] = Leader_arr[0].arr[0];
        four_element[1] = Leader_arr[0].arr[1];
        
        if (array[0] == Leader_arr[0].arr[0]) {
            four_element[2] = array[2];
            four_element[3] = array[3];
        }
        else {
            four_element[2] = array[0];
            four_element[3] = array[1];
        }
        
        int returnVal = QCOUNT(1, four_element);
        ERROR_CHECKING(returnVal, "[deal_with_ArraysReturn2]");
        
        if (returnVal == 0) {
            if (case0_count == 0) copy_single_case0_arr(four_element);
            Arrays_with_2_zeros[Arrays_with_2_zeros_count] = Leader_arr[0].arr;
            Arrays_with_2_zeros_count += Leader_arr[0].count;
        }
        else if (returnVal == 4){
            Arrays_with_2_ones[Arrays_with_2_ones_count] = Leader_arr[0].arr;
            Arrays_with_2_ones_count += Leader_arr[0].count;
        }
    }
    init_struct_array();
    
    if (Arrays_with_2_ones_count == Arrays_with_2_zeros_count) {
        Arrays_with_2_ones_count = 0;
        Arrays_with_2_zeros_count = 0;
    }
    else if (Arrays_with_2_ones_count > Arrays_with_2_zeros_count) {
        Arrays_with_2_ones_count -= Arrays_with_2_zeros_count;
        Arrays_with_2_zeros_count = 0;
    }
    else {
        Arrays_with_2_zeros_count -= Arrays_with_2_ones_count;
        Arrays_with_2_ones_count = 0;
    }
    
}

//trying to find two same element index by comparing an array return 2 and an array return 0
//when there is single_case0_arr available
//at most 3 QCOUNT
bool find_two_same_element_by_case2_and_0() {

    int * arr1 = ArraysReturn2[0];
    int * arr2 = single_case0_arr;
    
    int four_element[SIZEFOUR];
    four_element[0] = arr1[0];
    four_element[1] = arr1[1];
    four_element[2] = arr2[0];
    four_element[3] = arr2[1];
    
    int r1 = QCOUNT(1, four_element);
    
    ERROR_CHECKING(r1, "[find_two_same_element_by_case2_and_0]");
    if (r1 == 4) {
        Arrays_with_2_ones[Arrays_with_2_ones_count++] = ArraysReturn2[0];
        return true;
    }
    else if (r1 == 0) {
        four_element[2] = arr2[2];
        four_element[3] = arr2[3];
        int r2 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r2, "[find_two_same_element_by_case2_and_0 where r1 == 0]");
        
        if (r2 == 4) {
            Arrays_with_2_ones[Arrays_with_2_ones_count++] = ArraysReturn2[0];
            return true;
        }
        else if (r2 == 0) {
            swap(CASE2, 0, 0, 2);
            swap(CASE2, 0, 1, 3);
            Arrays_with_2_ones[Arrays_with_2_ones_count++] = ArraysReturn2[0];
            return true;
        }
    }
    else if (r1 == 2){
        four_element[0] = arr1[2];
        four_element[1] = arr1[3];
        int r2 = QCOUNT(1, four_element);

        ERROR_CHECKING(r2, "[find_two_same_element_by_case2_and_0 where r1 == 2]");
        
        if (r2 == 4) {
            swap(CASE2, 0, 0, 2);
            swap(CASE2, 0, 1, 3);
            Arrays_with_2_ones[Arrays_with_2_ones_count++] = ArraysReturn2[0];
            return true;
        }
        else {
            four_element[2] = arr2[2];
            four_element[3] = arr2[3];
            int r3 = QCOUNT(1, four_element);
            
            ERROR_CHECKING(r3, "[find_two_same_element_by_case2_and_0 where r2 != 4]");
            if (r3 == 0) {
                Arrays_with_2_ones[Arrays_with_2_ones_count++] = ArraysReturn2[0];
                return true;
            }
            else {
                swap(CASE2, 0, 0, 2);
                swap(CASE2, 0, 1, 3);
                Arrays_with_2_ones[Arrays_with_2_ones_count++] = ArraysReturn2[0];
                return true;
            }
        }
    }
    return false;
}

//using two ArraysReturn2 to find an ArrayReturn4
bool find_one_arrayReturn4() {
    
    //has to make sure if there are two ArraysReturn2
    for (int i = 0; i <ArraysReturn2_count; i += 2) {
        
        if (i + 1 >= ArraysReturn2_count)
            break;
        
        int * arr1 = ArraysReturn2[i];
        int * arr2 = ArraysReturn2[i + 1];
        
        int * four_element = (int *) malloc(sizeof(int) * 4);
        four_element[0] = arr1[0];
        four_element[1] = arr1[1];
        four_element[2] = arr2[0];
        four_element[3] = arr2[1];
        
        int r1 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r1, "[find_one_arrayReturn4 r1]");
        
        if (r1 == 4) {
            ArraysReturn4[ArraysReturn4_count++] = four_element;
            return true;
        }
        
        if (r1 == 0 && case0_count == 0) copy_single_case0_arr(four_element);
        
        four_element[2] = arr2[2];
        four_element[3] = arr2[3];
        int r2 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r2, "[find_one_arrayReturn4 r2]");
        
        if (r2 == 4) {
            ArraysReturn4[ArraysReturn4_count++] = four_element;
            return true;
        }
        
        if (r2 == 0 && case0_count == 0) copy_single_case0_arr(four_element);
        
        four_element[0] = arr1[2];
        four_element[1] = arr1[3];
        
        int r3 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r3, "[find_one_arrayReturn4 r3]");
        
        if (r3 == 4) {
            ArraysReturn4[ArraysReturn4_count++] = four_element;
            return true;
        }
        
        if (r3 == 0 && case0_count == 0) copy_single_case0_arr(four_element);
        
        if (r1 + r2 + r3 == 2) {
            //two array can delete each other
            swap_arr(CASE2, i, ArraysReturn2_count - 1);
            swap_arr(CASE2, i + 1, ArraysReturn2_count - 2);
            ArraysReturn2_count -= 2;
            i -= 2;
            continue;
        }
        
        four_element[2] = arr2[0];
        four_element[3] = arr2[1];
        int r4 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r4, "[find_one_arrayReturn4 r4]");
        if (r4 == 4) {
            ArraysReturn4[ArraysReturn4_count++] = four_element;
            return true;
        }
        else {
            //two array can delete each other
            swap_arr(CASE2, i, ArraysReturn2_count - 1);
            swap_arr(CASE2, i + 1, ArraysReturn2_count - 2);
            ArraysReturn2_count -= 2;
            i -= 2;
        }
    }
    return false;
}

int has_Arrays_with_2_left() {
    if (Arrays_with_2_zeros_count != 0)
        return 0;
    else if (Arrays_with_2_ones_count != 0)
        return 1;
    return -1;
}

int has_Arrays_with_4_left() {
    if (Leader_arr[0].count != 0)
        return 1;
    return -1;
}

//at most 4 Qcount
//in this case, must have case0 exists, using case0 to help find most frequent index in the remaining, so even though there is no case0 array after first scan, we are creating case0 array when processing case2 and case4 to make sure there is case0 array available when dealing with remaining
int deal_with_remaining() {
    if (case0_count == 0)
        return -1;
    
    if (remain_count == 1) return remaining_elements[0];
    else if (remain_count == 2) {
        int four_element[4];
        
        four_element[0] = remaining_elements[0];
        four_element[1] = remaining_elements[1];
        four_element[2] = single_case0_arr[0];
        four_element[3] = single_case0_arr[1];
        int return1 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(return1, "[deal_with_remaining: return1 when remain_count == 2]");
        
        four_element[2] = single_case0_arr[2];
        four_element[3] = single_case0_arr[3];
        int return2 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(return2, "[deal_with_remaining: return2 when remain_count == 2]");
        
        if (return1 + return2 == 4 && return1 != return2) return remaining_elements[0];
        if (return1 == return2 && return1 == 0) return 0;
        
        four_element[2] = single_case0_arr[0];
        four_element[3] = single_case0_arr[2];
        return1 = QCOUNT(1, four_element);
        ERROR_CHECKING(return1, "[deal_with_remaining: return1 when remain_count == 2]");
        
        four_element[2] = single_case0_arr[0];
        four_element[3] = single_case0_arr[3];
        return2 = QCOUNT(1, four_element);
        ERROR_CHECKING(return2, "[deal_with_remaining: return2 when remain_count == 2]");
        
        if (return1 != return2) return remaining_elements[0];
        return 0;
    }
    else if (remain_count == 3) {
        int four_element[4];
        
        four_element[0] = remaining_elements[0];
        four_element[1] = remaining_elements[1];
        four_element[2] = remaining_elements[2];
        four_element[3] = single_case0_arr[0];
        int return1 = QCOUNT(1, four_element);
        ERROR_CHECKING(return1, "[deal_with_remaining: return1 when remain_count == 3]");
        
        four_element[3] = single_case0_arr[1];
        int return2 = QCOUNT(1, four_element);
        ERROR_CHECKING(return2, "[deal_with_remaining: return2 when remain_count == 3]");
        
        if (return1 != return2) {
            if (return1 > return2) return single_case0_arr[0];
            else return single_case0_arr[1];
        }
        else {
            four_element[3] = single_case0_arr[2];
            return2 = QCOUNT(1, four_element);
            if (return1 > return2) return single_case0_arr[0];
            else return single_case0_arr[2];
        }
        
    }
    return 0;
}

//helper func of [deal_one_case2_with_remaining]
//at most 4 QCOUNT
//if there is only one case2 array left with one remaining element
int deal_with_one_remain_with_one_Array2() {
    int four_element[4];
    
    four_element[0] = remaining_elements[0];
    four_element[1] = ArraysReturn2[0][1];
    four_element[2] = ArraysReturn2[0][2];
    four_element[3] = ArraysReturn2[0][3];
    
    int r = QCOUNT(1, four_element);
    ERROR_CHECKING(r, "[deal_with_one_remain_with_one_Array2: r");
    
    if (r == 0) return ArraysReturn2[0][0];
    if (r == 4) return remaining_elements[0];
    
    four_element[0] = ArraysReturn2[0][0];
    four_element[1] = remaining_elements[0];
    
    r = QCOUNT(1, four_element);
    ERROR_CHECKING(r, "[deal_with_one_remain_with_one_Array2: r");
    
    if (r == 0) return ArraysReturn2[0][1];
    if (r == 4) return remaining_elements[0];
    
    four_element[1] = ArraysReturn2[0][1];
    four_element[2] = remaining_elements[0];
    
    r = QCOUNT(1, four_element);
    ERROR_CHECKING(r, "[deal_with_one_remain_with_one_Array2: r");
    
    if (r == 0) return ArraysReturn2[0][2];
    if (r == 4) return remaining_elements[0];
    
    
    four_element[2] = ArraysReturn2[0][2];
    four_element[3] = remaining_elements[0];
    
    r = QCOUNT(1, four_element);
    ERROR_CHECKING(r, "[deal_with_one_remain_with_one_Array2: r");
    
    if (r == 0) return ArraysReturn2[0][3];
    if (r == 4) return remaining_elements[0];
    
    return 0;
}

//at most 8 QCOUNT in remaining = 3, other remaining is at most 4 QCOUNT
//if there is only one case2 array left with remaining element: 1, 2, 3
int deal_one_case2_with_remaining() {
    if (remain_count == 0) {
        int four_element[4];
        
        four_element[0] = ArraysReturn2[0][0];
        four_element[1] = ArraysReturn2[0][1];
        four_element[2] = single_case0_arr[0];
        four_element[3] = single_case0_arr[1];
        
        int r = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r, "[deal_one_case2_with_remaining]: r");
        
        if (r == 4) {
            reset();
            return four_element[0];
        }
        
        four_element[2] = single_case0_arr[2];
        four_element[3] = single_case0_arr[3];
        
        int r1 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r1, "[deal_one_case2_with_remaining]: r1");
        
        if (r1 == 4) {
            reset();
            return four_element[0];
        }
        
        four_element[0] = ArraysReturn2[0][2];
        four_element[1] = ArraysReturn2[0][3];
        
        int r2 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r2, "[deal_one_case2_with_remaining]: r2");
        
        if (r2 == 4) {
            reset();
            return four_element[0];
        }
        
        four_element[2] = single_case0_arr[0];
        four_element[3] = single_case0_arr[1];
        
        int r3 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r3, "[deal_one_case2_with_remaining]: r3");
        
        if (r3 == 4) {
            reset();
            return four_element[0];
        }
        
        if (r + r1 == 4) {
            int to_return = ArraysReturn2[0][0];
            reset();
            return to_return;
        }
        else {
            reset();
            return four_element[0];
        }
    }
    else if (remain_count == 1) {
        int to_return = deal_with_one_remain_with_one_Array2();
        reset();
        return to_return;
    }
    else if (remain_count == 2) {
        int four_element[4];
        
        four_element[0] = ArraysReturn2[0][0];
        four_element[1] = ArraysReturn2[0][1];
        four_element[2] = remaining_elements[0];
        four_element[3] = remaining_elements[1];
        
        int r = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r, "[deal_one_case2_with_remaining]: r");
        
        if (r == 4) {
            reset();
            return remaining_elements[0];
        }
        
        four_element[0] = ArraysReturn2[0][2];
        four_element[1] = ArraysReturn2[0][3];
        
        int r2 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r2, "[deal_one_case2_with_remaining]: r2");
        
        if (r + r2 == 6) {
            reset();
            return remaining_elements[0];
        }
        
        four_element[0] = remaining_elements[0];
        four_element[1] = ArraysReturn2[0][1];
        four_element[2] = ArraysReturn2[0][2];
        four_element[3] = ArraysReturn2[0][3];
        
        int r3 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r3, "[deal_one_case2_with_remaining]: r3");
        
        four_element[0] = remaining_elements[1];
        four_element[1] = ArraysReturn2[0][1];
        four_element[2] = ArraysReturn2[0][2];
        four_element[3] = ArraysReturn2[0][3];
        
        int r4 = QCOUNT(1, four_element);
        
        ERROR_CHECKING(r4, "[deal_one_case2_with_remaining]: r4");
        
        if (r3 == r4) {
            reset();
            return 0;
        }
        else {
            int to_return = 0;
            if (r == 2) {
                to_return = ArraysReturn2[0][0];
                reset();
                return to_return;
            }else if (r2 == 2) {
                to_return = ArraysReturn2[0][2];
                reset();
                return to_return;
            }
        }
        
    }else {
        int total = 0;
        
        int four_element[4];
        
        four_element[0] = remaining_elements[0];
        four_element[1] = remaining_elements[1];
        four_element[2] = remaining_elements[2];
        four_element[3] = ArraysReturn2[0][0];
        
        int r = QCOUNT(1, four_element);
        ERROR_CHECKING(r, "[deal_one_case2_with_remaining]: r");
        total += r;
        
        four_element[3] = ArraysReturn2[0][1];
        r = QCOUNT(1, four_element);
        ERROR_CHECKING(r, "[deal_one_case2_with_remaining]: r");
        total += r;
        
        four_element[3] = ArraysReturn2[0][2];
        r = QCOUNT(1, four_element);
        ERROR_CHECKING(r, "[deal_one_case2_with_remaining]: r");
        total += r;
        
        four_element[3] = ArraysReturn2[0][3];
        r = QCOUNT(1, four_element);
        ERROR_CHECKING(r, "[deal_one_case2_with_remaining]: r");
        total += r;
        
        if (total == 10 || total == 14) {
            reset();
            return four_element[0];
        }
        
        int to_return = deal_with_one_remain_with_one_Array2();
        reset();
        return to_return;
    }
    return 0;
}

//here deal with the rest of Arrays_with_2_ones, Arrays_with_4_ones, remaining(1,2,3 possible)
//compare ONLY one time, will know if they are same or inverse
int deal_with_left_case2_or_case4_with_remaining() {
    int two_left = has_Arrays_with_2_left();
    int four_left = has_Arrays_with_4_left();
    
    // at most 4 QCOUNT
    if (two_left != -1 && four_left != -1) {//both case2 and case4 left
        
        int ** arr1 = Arrays_with_2_zeros;
        int count1 = Arrays_with_2_zeros_count;
        
        int count2 = Leader_arr[0].count;
        
        if (two_left == 1){
            arr1 = Arrays_with_2_ones;
            count1 = Arrays_with_2_ones_count;
        }
        
        int four_element[4];
        
        four_element[0] = arr1[0][0];
        four_element[1] = arr1[0][1];
        four_element[2] = Leader_arr[0].arr[0];
        four_element[3] = Leader_arr[0].arr[1];
        
        int r = QCOUNT(1, four_element);
        ERROR_CHECKING(r, "[deal_with_left_case2_or_case4_with_remaining]: r");
        
        if (r == 0) {//first two elements of case2 and case4 are inverse, at most 2 QCOUNT
            if (count1 == count2 * 2) {
                //check only remain count!
                int to_return = deal_with_remaining();
                if (to_return != -1) {
                    reset();
                    return to_return;
                }
                printf("ERROR: deal_with_remaining() return -1 in mysub\n");
            }
            else if (count1 / 2 >= count2) { //more case2 left
                count1 -= count2 * 2;
                
                if (count1 * 2 > remain_count || remain_count < 2) {
                    reset();
                    return four_element[0];
                }
                else {
                    int four_element2[4];
                    four_element2[0] = remaining_elements[0];
                    four_element2[1] = remaining_elements[1];
                    
                    //check with two remaining
                    if (remain_count == 2) {
                        four_element2[2] = four_element[0];
                        four_element2[3] = four_element[1];
                        
                        int re = QCOUNT(1, four_element2);
                        ERROR_CHECKING(re, "[deal_with_left_case2_or_case4_with_remaining]: re");
                        reset();
                        if (re == 0)
                            return 0;
                        else
                            return four_element2[2];
                    }
                    
                    //check with 3 remainings
                    four_element2[2] = remaining_elements[2];
                    four_element2[3] = four_element[0];
                    
                    int r1 = QCOUNT(1, four_element2);
                    ERROR_CHECKING(r1, "[deal_with_left_case2_or_case4_with_remaining]: r1");
                    four_element2[3] = four_element[2];
                    int r2 = QCOUNT(1, four_element2);
                    ERROR_CHECKING(r2, "[deal_with_left_case2_or_case4_with_remaining]: r2");
                    
                    reset();
                    if (r1 + r2 == 6) return four_element2[0];
                    else return four_element[0];
                }
            }
            else { //more case4 left
                int tmp = count1 / 2;
                bool off = false;// 0.5 off
                if (tmp * 2 < count1) off = true;
                count2 -= count1 / 2;
                float c = count2;
                if (off) c -= 0.5;
                
                if (c >= 1.0 || remain_count < 2) {
                    reset();
                    return four_element[2];
                }
                else {
                    int four_element2[4];
                    four_element2[0] = remaining_elements[0];
                    four_element2[1] = remaining_elements[1];
                    //check with two remaining
                    if (remain_count == 2) {
                        four_element2[2] = four_element[2];
                        four_element2[3] = four_element[3];
                        
                        int re = QCOUNT(1, four_element2);
                        ERROR_CHECKING(re, "[deal_with_left_case2_or_case4_with_remaining]: re");
                        reset();
                        if (re == 0) return 0;
                        else return four_element2[2];
                    }
                    // 2 same element left, check with remaining 3
                    
                    four_element2[2] = remaining_elements[2];
                    four_element2[3] = four_element[2];
                    
                    int r1 = QCOUNT(1, four_element2);
                    ERROR_CHECKING(r1, "[deal_with_left_case2_or_case4_with_remaining]: r1");
                    four_element2[3] = four_element[0];
                    int r2 = QCOUNT(1, four_element2);
                    ERROR_CHECKING(r2, "[deal_with_left_case2_or_case4_with_remaining]: r2");
                    
                    reset();
                    if (r1 + r2 == 6) return four_element2[0];
                    else return four_element[2];
                }
            }
        }
        else if (r == 4){ //first two elements of case2 and case4 are same sign
            reset();
            return four_element[0];
        }
    }
    else if (two_left != -1) { //at most 4 QCOUNT
        int ** arr1 = Arrays_with_2_zeros;
        int count1 = Arrays_with_2_zeros_count;
        
        if (two_left == 1){
            arr1 = Arrays_with_2_ones;
            count1 = Arrays_with_2_ones_count;
        }
        
        if (count1 * 2 > remain_count || remain_count < 2) {
            int to_return = arr1[0][0];
            reset();
            return to_return;
        }
        else {
            int four_element2[4];
            four_element2[0] = remaining_elements[0];
            four_element2[1] = remaining_elements[1];
            //check with two remaining
            if (remain_count == 2) {
                four_element2[2] = arr1[0][0];
                four_element2[3] = arr1[0][1];
                
                int re = QCOUNT(1, four_element2);
                ERROR_CHECKING(re, "[deal_with_left_case2_or_case4_with_remaining]: re");
                reset();
                if (re == 0) return 0;
                else return four_element2[2];
            }
            //need check both remain = 3 and this, one count1, so left with 2 same element
            four_element2[2] = remaining_elements[2];
            four_element2[3] = arr1[0][0];
            
            int r1 = QCOUNT(1, four_element2);
            ERROR_CHECKING(r1, "[deal_with_left_case2_or_case4_with_remaining]: r1");
            
            if (r1 == 0) {
                reset();
                return four_element2[3];
            }
            if (r1 == 4) {
                reset();
                return four_element2[0];
            }
            
            four_element2[3] = arr1[0][2];
            int r2 = QCOUNT(1, four_element2);
            ERROR_CHECKING(r2, "[deal_with_left_case2_or_case4_with_remaining]: r2");
            
            if (r1 != r2) {
                if (r1 + r2 == 6) {
                    reset();
                    return four_element2[0];
                }
                else {
                    int to_return = arr1[0][0];
                    reset();
                    return to_return;
                }
            }
            
            four_element2[3] = arr1[0][3];
            r2 = QCOUNT(1, four_element2);
            ERROR_CHECKING(r2, "[deal_with_left_case2_or_case4_with_remaining]: r2");
            
            if (r1 + r2 == 6) {
                reset();
                return four_element2[0];
            }
            int to_return = arr1[0][0];
            reset();
            return to_return;
        }
    }
    else if (four_left != -1) {
        int to_return = Leader_arr[0].arr[0];
        reset();
        return to_return;
    }
    return -1;
}

//*********************************************************************************************
//*********************************************************************************************
//********************************* Main Logic ************************************************
//*********************************************************************************************
//*********************************************************************************************

int mysub(int n) {
    bool artificial_array4 = false;
    bool find_two_same_element = false;
    
    if (n < 10 || n > 10000) {
        printf("ERROR: n is not between 10 and 10000\n");
        return -1;
    }
    
    first_scan(n);
    init_struct_array();
    
    if (ArraysReturn4_count == 0 && ArraysReturn2_count > 1) {
        if ( case0_count != 0)
            find_two_same_element = find_two_same_element_by_case2_and_0();
        
        if (!find_two_same_element)
            artificial_array4 = find_one_arrayReturn4();//maybe cannot find a ArraysReturn4
    }
    
    
    if (ArraysReturn4_count != 0 || find_two_same_element) {
        if (find_two_same_element)
            deal_with_ArraysReturn2(Arrays_with_2_ones[0], true);
        else
            deal_with_ArraysReturn2(ArraysReturn4[0], false);
            
        if (artificial_array4) {
            free(ArraysReturn4[0]);
            ArraysReturn4_count = 0;
        }
        compare_two_by_two(CASE4);
    }
    else if (ArraysReturn2_count == 1){
        //ArraysReturn4_count == 0, find_one_arrayReturn4() also does not find any ArraysReturn4
        //check for remaining, do remaining with 1101, then return
        return deal_one_case2_with_remaining();
    }

    //here deal with the rest of Arrays_with_2_ones, Arrays_with_4_ones, remaining
    //compare ONLY one time, will know if they are same or inverse
    int re = deal_with_left_case2_or_case4_with_remaining();
    if (re != -1) return re;

    //only left with remainings here
    int to_return = deal_with_remaining();
    if (to_return != -1) {
        reset();
        return to_return;
    }
    reset();
    return 0;
}
