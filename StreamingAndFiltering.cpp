//
//  main.cpp
//  StreamingAndFilter-Threading
//
//  Created by Somnath Saha on 5/22/20.
//  Copyright © 2020 Somnath Saha. All rights reserved.
//

#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <signal.h>
#include <string>
#include <vector>

using namespace std;
int stop_thread = 0;
mutex _mtx;

/*
 This is an example of streaming and filtering with multithreading.
 
 Similar Problem:
 Word Count with multithreading can be accompished by procesing words
 in parallel suing consumer thread.
 
 Compilation:
 g++ StreamingAndFiltering.cpp -std=c++11
 
 Execution:
 ./a.out
 
 To terminate the program, press ctrl+c, It will terminate working consumer threads,
 and then any letter to close the producer thread.
 */

// A shared Queue to be used for serialized access of data
queue<string> Q;

// A filter with unordered map.
unordered_map<string, int> _filter;

/*
 Generate streaing data through standard input in a seperate thread.
 */
void producer()
{
    string input;
    {
        unique_lock<mutex> lock(_mtx);
        cout<<"Producer Started ...\n";
    }
    // Read from command line
    while(true){
        getline(cin,input);
        cout<<"INPUT:"<<input<<endl;
        {
            unique_lock<mutex> lock(_mtx);
            if(stop_thread){
                break;
            }
            Q.push(input);
            this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    {
        unique_lock<mutex> lock(_mtx);
        cout<<"Stopped Producer ..."<<endl;
    }
}

/*
 Multiple consumer(thread) consume data from shared queue
 and process(apply filter) data.
 */
void consumer(int consumer_id)
{
    {
        unique_lock<mutex> lock(_mtx);
        cout<<"Consumer - Id"<<consumer_id<<endl;
    }
    while(true){
        {
            unique_lock<mutex> lock(_mtx);
            if(stop_thread){
                break;
            }
            if(!Q.empty()){
                string entry = Q.front();
                Q.pop();
                if( _filter.find(entry) != _filter.end()){
                    cout<<"Q entry:"<<entry<<endl;
                    _filter[entry] = _filter[entry] + 1;
                }
            }
            this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    cout<<"Stopping Consumer Thread - "<<consumer_id<<endl;
}

// Handle input signal typed from keyboard. Such as Ctrl+C
void signal_handler(int signal)
{
    cout<<"Received Signal:"<<signal<<endl;
    unique_lock<mutex> lock(_mtx);
    cout<<"Stop All Threads ... "<<endl;
    stop_thread = 1;
}

// Update filetr, or add filetr as needed.
void filter()
{
    _filter.insert(make_pair("AA", 0));
    _filter["BB"] = 0;
}

void display()
{
    for(auto elem: _filter){
        cout<<elem.first << ":" <<elem.second<<endl;
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Streaming and Filter!\n";
    signal(SIGINT, signal_handler);
    
    vector<thread> thread_list;
    int consumer_id = 1;
    int consumer_count = 3;
    
    if( argc > 1){
        consumer_count = atoi(argv[1]);
    }
    
    // Add update filter
    filter();
    
    // start producer
    thread_list.push_back(thread(producer));
    cout<<"Going to start "<< consumer_count <<" Consumer Threads\n";
    while(consumer_id <= consumer_count){
        thread_list.push_back(thread(consumer, consumer_id));
        consumer_id++;
    }
    
    // Join thread all consumer
    for(int index = 0; index <thread_list.size(); index++){
        thread_list[index].join();
    }
    // Display filtered output
    display();
    cout<<"All Done ... "<<endl;
    return 0;
}

