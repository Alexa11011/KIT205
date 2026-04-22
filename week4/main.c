#include <time.h>
#include <stdio.h>

long f1(long n){
    long k = 0;
    for (long i = 0; i < n; i++){
        for (long j = 0; j < n; j++){
              k++;
        }
    }
    return k;
}

void f2(long n){
    long k = 0;
    for (long i = 0; i < n; i++){
        for (long j = 0; j < i; j++){
            k++;
        }
        for (long t = 0; t < 10000; t++){
            k++;
        }
    }
}

void f3(long n){
    if (n > 0){
        f3(n / 2);
        f3(n / 2);
    }
}

void f4(long n){
    if (n > 0){
        f4(n / 2);
        f4(n / 2);
        f2(n);
    }
}

void f5(long n){
    long k = 0;
    for (long i = 0; i < 10; i++){
        long j = n;
        while (j > 0){
            f1(1000);
            k++;
            j = j / 2;
        }
    }
}

void f6(long n){
    f2(n);
    f3(n);
    f5(n);
}

void f7(long n){
    long k = 0;
    for (long i = 0; i < f1(n); i++){
        k++;
    }
    for (long j = 0; j < n; j++){
        k++;
    }
}

int main(void) {

    long n;
    n = 400;
    clock_t start = clock();

    f7(n);

    clock_t diff = clock() - start;
    long msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Operation took %d milliseconds\n\n", msec);

}