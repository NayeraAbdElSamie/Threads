#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


typedef struct              //To transfer the biggest and middle loop iterators (can't transfer more than one argument in the pthread_create function)
{
    int x;
    int y;

} ElementMatrixMult;

typedef struct
{
    int x;
    int y;

} RowMatrixMult;

int matrix1[100][100];
int matrix2[100][100];
int result[100][100];

typedef struct
{
    int left;
    int right;
} divide;

int array[100];

void CellMultiplication(void * arguments)
{
    ElementMatrixMult *mult = arguments;
    int k;
    int i = mult -> x;              //reference to biggest loop iterator
    int j = mult -> y;              //reference to the middle loop iterator

    for( k=0; k<100; k++ )
        result[i][j] = result[i][j] + (matrix1[i][k] * matrix2[k][j]);

    pthread_exit(0);
}

void ElementComputation(int rows1, int rows2, int columns1, int columns2)
{
    ElementMatrixMult mult;
    pthread_t output1;

        for( mult.x=0; mult.x<rows1; mult.x++ )
            for( mult.y=0; mult.y<columns2; mult.y++ )
            {
                pthread_create(&output1, NULL, CellMultiplication, (void *) &mult);
                pthread_join(output1, NULL);                 //Equivalent to wait(), blocks the calling thread until the thread with id is equal to the first argument terminate
                printf("%d\n",result[mult.x][mult.y]);
            }
}

void RowMultiplication(int i)
{
    int j,k;

    for( j=0; j<100; j++ )
        for( k=0; k<100; k++ )
            result[i][j] = result[i][j] + (matrix1[i][k] * matrix2[k][j]);


    pthread_exit(0);
}

void RowComputation(int rows1, int rows2, int columns1, int columns2)
{
    RowMatrixMult mult;
    pthread_t output2;

        for( mult.x=0; mult.x<rows1; mult.x++ )
        {
            pthread_create(&output2, NULL, RowMultiplication, mult.x);
            pthread_join(output2, NULL);                 //Equivalent to wait(), blocks the calling thread until the thread with id is equal to the first argument terminate
        }
        int s,m;                            //printing resulting matrix
        for(s=0; s<rows1; s++)
            for(m=0; m<columns2; m++)
                printf("%d\n", result[s][m]);
}

void merge(int arr[], int l, int m, int r)
{
    int i,j,k;
    int size1 = m - l + 1;
    int size2 = r - m;

    int tempL[size1], tempR[size2];

    for (i = 0; i < size1; i++)                //Copy data to temporary arrays
        tempL[i] = arr[l + i];
    for (j = 0; j < size2; j++)
        tempR[j] = arr[m + 1+ j];

    i =0, j = 0;                              //Initializing index of subarrays
    k = l;

    while (i < size1 && j < size2)
    {
        if (tempL[i] <= tempR[j])
        {
            arr[k] = tempL[i];
            i++;
        }
        else
        {
            arr[k] = tempR[j];
            j++;
        }
        k++;
    }

    while (i < size1)               //Elements remained from the left temporary array
    {
        arr[k] = tempL[i];
        i++;
        k++;
    }

    while (j < size2)               //Elements remained from the right temporary array
    {
        arr[k] = tempR[j];
        j++;
        k++;
    }
}


void* mergeSort(void* args)
{
    divide *d =  args;
    divide d1, d2;
    int middle;

    pthread_t leftThread, rightThread;

    if (d->left < d->right)
    {
        middle = d->left+(d->right-d->left)/2;              //Get the halfway point
        d1.left = d->left;
        d1.right = middle;

        pthread_create(&leftThread, NULL, mergeSort, (void*)&d1);               //Sort first half

        d2.left = middle+1;
        d2.right = d->right;

        pthread_create(&rightThread, NULL, mergeSort, (void*)&d2);              //Sort second half

        pthread_join(leftThread, NULL);
        pthread_join(rightThread, NULL);

        merge(array, d->left, middle, d->right);
    }
}

int main()
{
    FILE * file = fopen("input.txt", "r");    //read from file
    FILE * mergeFile = fopen("mergeInput.txt", "r");
    int i,j,k,l,h,z;
    int rows1, rows2, columns1, columns2;
    int c;             //Switching on c to choose from the list
    clock_t start1, end1, start2, end2, start3, end3;             //To measure time taken by a function
    double cpu_time_used1, cpu_time_used2;
    int size;

    pthread_t thread;
    divide d;

    if (  file == NULL )            //Tests if file doesn't exist
        perror("Error! Couldn't open file\n");

    if (  mergeFile == NULL )
        perror("Error! Couldn't open file\n");

    if (file)                       //If file exists
    {
        //Dimensions of first matrix

        fscanf(file, "%d", &rows1);
        fscanf(file, "%d", &columns1);

        for( i=0; i<rows1; i++ )                //Taking elements of the first matrix
            for( j=0; j<columns1; j++ )
                fscanf(file, "%d%*c", &matrix1[i][j]);

        //Dimensions of second matrix

        fscanf(file, "%d", &rows2);
        fscanf(file, "%d", &columns2);

        for( k=0; k<rows2; k++ )                //Taking elements of the second matrix
            for( l=0; l<columns2; l++ )
                fscanf(file, "%d%*c", &matrix2[k][l]);

        if( columns1 != rows2 )            //no. of columns of 1st matrix must be equal to no. of rows of 2nd matrix
        {
            printf("Can't multiply these 2 matrices\n");
            fclose(file);
        }
    }

    printf("Select one of the following procedures:\n\n");
    printf("1- Computation by each element.\n2- Computation by each row.\n3- Merge sort.\n\n");
//    scanf("%d\n", &c);

    c=1;

    switch(c)
    {
        case 1:
        start1 = clock();
        ElementComputation(rows1,rows2,columns1,columns2);
        end1 = clock();
        cpu_time_used1 = ((double)(end1 - start1)) / CLOCKS_PER_SEC;
        printf("%f", cpu_time_used1);
        break;

        case 2:
        start2 = clock();
        RowComputation(rows1,rows2,columns1,columns2);
        end2 = clock();
        cpu_time_used2 = ((double)(end2 - start2)) / CLOCKS_PER_SEC;
        printf("%f", cpu_time_used2);
        break;

        case 3:
        fscanf(mergeFile, "%d", &size);

        for( h=0; h<size; h++ )
            fscanf(mergeFile, "%d%*c", &array[h]);
        fclose(mergeFile);

        d.left = 0;
        d.right = size-1;

        start3 = clock();
        pthread_create(&thread, NULL, mergeSort, (void*)&d);
        pthread_join(thread, NULL);
        end3 = clock();
        printf("\nSorted array is \n");
        for (z=0; z < size; z++)
            printf("%d ", array[z]);
        printf("\n");
        printf("\nTime taken: %f\n", (end3 - start3)/(double)CLOCKS_PER_SEC);

        default:
        printf("Please enter a valid number!");
    }

    FILE * output = fopen("output.txt", "wb");                //Binary writing mode
    fwrite(result, sizeof(int), sizeof(result), output);
    fprintf(output, "END1", cpu_time_used1);
    fprintf(output, "END1", cpu_time_used2);
    fclose(output);

    return 0;
}
