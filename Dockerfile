FROM ubuntu:latest
RUN apt-get update && \
    apt-get install -y build-essential g++
WORKDIR /src
COPY WordCount-Multithreading.cpp ./
COPY README.md ./
RUN g++ WordCount-Multithreading.cpp -std=c++11 -lpthread -o WordCount
ENTRYPOINT ["./WordCount"]
CMD ["3"]
