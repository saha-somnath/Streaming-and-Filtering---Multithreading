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
#include <fstream>
//#include <ctype.h>
//#include <boost/algorithm/string.hpp>

using namespace std;
int stop_thread = 0;
mutex _mtx;

/*
 This is an example of Producer and Consumer problem.
 
 Similar Problem:
 Word Count with multithreading can be accompished by procesing words
 in parallel suing consumer threads. This program add a filter to check occurrence of
 perticular words.

 The program read words through a producer thread from README.md file and enque
 those words to a shared queue. The consumer threads dequeue those words from shared queue
 and apply filter on top of it to count the occurence of set of words.  
   
 Compilation:
 g++ WordCount-Multithreading.cpp -std=c++11 -lpthread
 
 Execution:
 ./a.out <Number of Consumer>
 

 */

// A shared Queue to be used for serialized access of data
queue<string> Q;

// A filter with unordered map.
unordered_map<string, int> _filter;
bool is_producer_done;

void split(vector<string>& words, string& line)
{
  string word;
  bool is_word;
  
  for ( auto w: line ){
    if( w == ' ') {
      if(is_word){
        words.push_back(word);
        word = "";
        is_word = false;
      }
    }else{
      is_word = true;
      string s(1, w);
      word.append(s);
    }
  }
  words.push_back(word);
}

void lower(string& word)
{
    for(int index=0; index< word.length(); index++){
       word[index] = tolower(word[index]);
    }
}

/*
 Generate streaing data through standard input in a seperate thread.
 */
void producer()
{
    {
        unique_lock<mutex> lock(_mtx);
        cout<<"Producer Started ...\n";
        is_producer_done = false;
    }
    // Read from file.
    ifstream stream;
    stream.open("README.md");
    if(stream.is_open()){
        string line;
        while(getline(stream, line)){
            vector<string> words;
            //boost::split(words,line, boost::is_any_of(" ")); // boost split can be used too.
            split(words, line);
            for( auto word: words)
            {
                //cout<<"INPUT:"<<word<<endl;
                unique_lock<mutex> lock(_mtx);
                if(stop_thread){
                    break;
                }
                Q.push(word);
                this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }
    
    {
        unique_lock<mutex> lock(_mtx);
        cout<<"Stopped Producer ..."<<endl;
        is_producer_done = true;
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
        unique_lock<mutex> lock(_mtx);
        if(stop_thread || ( Q.empty() && is_producer_done) ){
            break;
        }
        if(!Q.empty()){
            string entry = Q.front();
            Q.pop();
            lower(entry);
            if( _filter.find(entry) != _filter.end()){
                cout<<"Consumer Id:"<<consumer_id<<" Q entry:"<<entry<<endl;
                _filter[entry] = _filter[entry] + 1;
            }
        }
        this_thread::sleep_for(std::chrono::milliseconds(5));
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
    _filter.insert(make_pair("multithreading", 0));
    _filter["includes"] = 0;
    _filter["the"] = 0;
    _filter["program"] = 0;
    _filter["words"] = 0;
    _filter["count"] = 0;
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
    cout<<"\nOccurrence of words ...\n";
    display();
    cout<<"All Done ... "<<endl;
    return 0;
}

