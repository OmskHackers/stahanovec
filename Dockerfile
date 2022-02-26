FROM debian:stretch-slim

WORKDIR /home/stahanovec

RUN apt-get update && \
    apt-get -y install build-essential

ADD dos.c /home/stahanovec/dos.c

RUN gcc dos.c -lpthread -O3 -o dos

CMD ./dos $TARGET $PORT $DELAY_MS $THREADS
