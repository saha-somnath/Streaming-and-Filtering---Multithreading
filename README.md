# Streaming-and-Filtering---Multithreading
An example of streaming and filtering of the content using multithreading.

This is an typical producer and consumer problem. Where a producer, produce the shared resource
and consumers consumes those shared resource and work on that.


 Similar Problem:
 ```
 WordCount-Multithreading.cpp:
 ```

 Word Count with multithreading can be accompished by procesing words
 in parallel suing consumer threads. This program includes a filter to check occurrence of
 perticular words.

 The program read words through a producer thread from README.md file and enque
 those words to a shared queue. The consumer threads dequeue those words from shared queue
 and apply filter on top of it to count the occurence of set of words.  
   
 Compilation:
 ```
 g++ WordCount-Multithreading.cpp -std=c++11 -lpthread
 ```
 
 Execution:
 ```
 ./a.out <Number of Consumers>
 ```


 Containarize this application.
 
 - Create Docker image
```
   %> docker build --tag word-count:1.0.0 .
```
 - Check docker image
 ```
   %> docker images
 ```
 - Run application through docker images.
 ```
   %> docker run word-count:1.0.0 <Command Line Arguments- Number of consumer thtreads>
   %> docker run word-count:1.0.0  3
 ```




