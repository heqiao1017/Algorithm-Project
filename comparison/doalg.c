#include <stdio.h>
#define LIMIT 100

struct compared_arr
{
   int index;//the index
   int compared_numbers[LIMIT];//stores number compared with this index number and less than it
   int count;//count of compared numbers with that index
};

void init_index_array(int n, int * index_array) {
    for (int i = 1; i <= n; i++) {
        index_array[i] = i;
    }
}

void init_struct_array(int n, struct compared_arr arr[]) {
    for (int i = 1; i <= n; i++) {
        arr[i].index = i;
        arr[i].count = 0;
    }
}


int find_Kth_largest(int * number, int n, struct compared_arr arr[]) {

    while (n > 1) {
        int j = 1;
        for (int i = 1; i <= n; i += 2, j++) {

            if (i + 1 > n) {
                number[j] = number[i];
            }
            else {
                if (COMPARE(number[i], number[i + 1]) == 1) {             
                    arr[number[i]].compared_numbers[arr[number[i]].count + 1] = number[i + 1];
                    arr[number[i]].count += 1;

                    number[j] = number[i];
                }
                else {

                    arr[number[i + 1]].compared_numbers[arr[number[i + 1]].count + 1] = number[i];
                    arr[number[i + 1]].count += 1;

                    number[j] = number[i + 1];
                }
            }
            
        }
        if (n % 2 == 0) {
            n = n / 2;
        }else {
            n = n / 2 + 1;
        }
    }

    return number[1];
}


int doalg(int n, int k, int * Best) {
    int number[n + 1];
    init_index_array(n, number);

    struct compared_arr arr[n + 1];
    init_struct_array(n, arr);

    Best[0] = find_Kth_largest(number, n, arr);

    for (int i = 1; i < k; i++) {
        struct compared_arr tmp = arr[Best[i - 1]];
        Best[i] = find_Kth_largest(tmp.compared_numbers, tmp.count, arr);
    }

    return 1;
}
