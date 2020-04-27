#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

struct MatData {
    int x;
    int y;
    int r1, c1, r2, c2;
    int **matrix1, **matrix2, **matrixRes;
};


int **initialiseMatrix(int row, int column) {
    int **matrix = new int *[row];
    for(int i = 0; i < row; i++) {
        matrix[i] = new int[column];
    }
    return matrix;
    
}

void matInput(int** matrix, int row, int column) {
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < column; j++) {
            cin >> matrix[i][j];
        }
    }
}

void *threadMultiply(void* matData) {
    struct MatData data = *(struct MatData*)matData;
    int sum = 0;
    int row = data.x;
    int col = data.y;

    int threadNo = row*data.c2 + col;

    cout << threadNo <<endl;
    for(int i = 0; i < data.c1; i++) {
        sum += data.matrix1[row][i] * data.matrix2[i][col];
    }
    data.matrixRes[row][col] = sum;
    
}


void printMat(int** matrix, int row, int col) {
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            cout << matrix[i][j] << "\t";
        }
    cout << endl;
    }
    
}

int main(int argc, char const *argv[]) {
    int r1, c1, r2, c2;
    cout << "Enter rows and columns for both the matrices:" << endl;
    cin >> r1 >> c1 >> r2 >> c2;
    if(c1 != r2){
        cout << "Invalid Input!" << endl;
        exit(-1);
    }
    
    int **matrix1 = initialiseMatrix(r1, c1);
    int **matrix2 = initialiseMatrix(r2, c2);
    int **matrixRes = initialiseMatrix(r1,c2);

    // Attribute set to joinable
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct MatData matData[r1*c2];
    pthread_t threads[r1*c2];

    cout << "Provide Input For Matrix 1:" << endl;    
    matInput(matrix1, r1, c1);
    cout << "Provide Input For Matrix 2:" << endl;
    matInput(matrix2, r2, c2);
    
    // Initialising matData
    for(int i = 0; i < r1*c2; i++){
        matData[i].matrix1 = matrix1;
        matData[i].matrix2 = matrix2;
        matData[i].matrixRes = matrixRes;
        matData[i].r1 = r1;
        matData[i].c1 = c1;
        matData[i].r2 = r2;
        matData[i].c2 = c2;
    }

    cout << "Threads: " <<endl;
    for(int i = 0; i < r1; i++) {
        for(int j = 0; j < c2; j++) {
            int threadNo = i*c2 + j;

            matData[threadNo].x = i;
            matData[threadNo].y = j;

            int rc = pthread_create(&threads[threadNo], &attr, threadMultiply, (void *)&matData[threadNo]);

            if (rc) {
                cout << "Error in creating thread: "<< threadNo << endl;
                break;
            }

        }
    }

    pthread_attr_destroy(&attr);
    for(int i = 0; i < r1*c2; i++){
        int rc = pthread_join(threads[i], NULL);
        if (rc) {
            cout << "Error in joining" << rc << endl;
            break;
        }
    }

    cout << "The resultant matrix is:" << endl;
    printMat(matrixRes, r1, c2);

    cout << "Exiting the main program!" << endl;
    pthread_exit(NULL);

    return 0;
}
