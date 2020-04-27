#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
 
#include<iostream>

using namespace std;


void print_array(int *array, int n){
  for (int i = 0; i < n; ++i){
    printf("%d  ", array[i]);
  }
}


int* allocate_and_check(int size){
  int *arr;
  if( !(arr = new int[size]) ) exit(-1);
  return arr;
}


int* prefix_sum(int *array, int n){
  int *end_points, *temp;
  int num_threads, sub_array_len;
  int i, thread_id, last;


  #pragma omp parallel default(none) private(i, thread_id, last) shared(array, end_points, temp, num_threads, sub_array_len, n)
  /*
  Forms a team of threads and starts parallel execution
  */
  {


    #pragma omp single
    /*
    Specifies that the associated structured block is executed by only one of the threads in the team.
    */
    {
      num_threads = omp_get_num_threads();
      end_points = allocate_and_check(num_threads);
      temp = allocate_and_check(num_threads);
      sub_array_len = n / num_threads + 1; 
    }


    // Here, we calculate the prefix sum of each sub_array
    thread_id = omp_get_thread_num();
    for(i = sub_array_len * thread_id + 1; i < sub_array_len * thread_id + sub_array_len && i < n; i++)
      array[i] += array[i - 1];
    end_points[thread_id] = array[i - 1];
    

    #pragma omp barrier
    /*
    Specifies an explicit barrier at the point at which the construct appears.
    */
    // We calculate the prefix sum of endpint array
    for(i = 1; i < num_threads; i <<= 1) {
      if(thread_id >= i)
        temp[thread_id] = end_points[thread_id] + end_points[thread_id - i];
    
      #pragma omp barrier
      #pragma omp single
      memcpy(end_points + 1, temp + 1, sizeof(int) * (num_threads - 1));
    }


    // Updating the main array
    for(i = sub_array_len * thread_id; i < (last = sub_array_len * thread_id + sub_array_len < n ? sub_array_len * thread_id + sub_array_len : n); i++)
      array[i] += end_points[thread_id] - array[last - 1];
  }

  return array;
}


int main()
{
  int *array, *pre_sum, n;
  printf("Enter length:");
  scanf("%d", &n);
  
  array = allocate_and_check(n);
  
  printf("Enter numbers:\n");
  
  for(int i = 0; i < n; i++)
    scanf("%d", array + i);

  pre_sum = prefix_sum(array, n);
  
  print_array(pre_sum, n);

  return 0;
}