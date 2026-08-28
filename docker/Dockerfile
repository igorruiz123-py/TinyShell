FROM ubuntu:24.04

RUN apt-get update

RUN apt-get install -y gcc make

WORKDIR /usr/local/tinyshell

COPY build/Makefile /usr/local/tinyshell/build/

COPY include/* /usr/local/tinyshell/include/

COPY src/* /usr/local/tinyshell/src/

COPY config/.env /usr/local/tinyshell/config/.env

RUN cd build && make setup

RUN cd build && make build

EXPOSE 12000

CMD ["make", "-C", "build", "run"]