#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main() {
    int N;
    float epsilon;
    int maxIter = 10000;

    printf("Enter grid size (N x N): ");
    scanf("%d", &N);
    printf("Enter convergence tolerance (epsilon): ");
    scanf("%f", &epsilon);

    float **lst = (float **)malloc(N * sizeof(float *));
    float **new_lst = (float **)malloc(N * sizeof(float *));
    for (int i = 0; i < N; i++) {
        lst[i] = (float *)malloc(N * sizeof(float));
        new_lst[i] = (float *)malloc(N * sizeof(float));
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == 0) {
                lst[i][j] = 100.0;
                new_lst[i][j] = 100.0;
            } else if (i == N - 1 || j == 0 || j == N - 1) {
                lst[i][j] = 0.0;
                new_lst[i][j] = 0.0;
            } else {
                lst[i][j] = 0.0;
                new_lst[i][j] = 0.0;
            }
        }
    }

    float diff = epsilon + 1.0;
    int iter = 0;

    while (diff > epsilon && iter < maxIter) {
        diff = 0.0;

        #pragma omp parallel for reduction(max:diff)
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                new_lst[i][j] = 0.25 * (lst[i+1][j] + lst[i-1][j] + lst[i][j+1] + lst[i][j-1]);
                float change = fabs(new_lst[i][j] - lst[i][j]);
                if (change > diff)
                    diff = change;
            }
        }

        // swap
        float **tmp = lst;
        lst = new_lst;
        new_lst = tmp;

        iter++;
    }

    printf("Converged in %d iterations.\n", iter);

    FILE *f = fopen("output.csv", "w");
    if (f == NULL) {
        printf("Error opening output file.\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(f, "%.1f", lst[i][j]);
            if (j < N - 1) fprintf(f, ",");
        }
        fprintf(f, "\n");
    }

    fclose(f);
    printf("Output written to output.csv\n");

    for (int i = 0; i < N; i++) {
        free(lst[i]);
        free(new_lst[i]);
    }
    free(lst);
    free(new_lst);

    return 0;
}
