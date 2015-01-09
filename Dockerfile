FROM ubuntu
MAINTAINER Mikkel Kroman <mk@maero.dk>
ENV PROTOBUF_VERSION 2.6.0

# Prior to install.
RUN apt-get update
RUN apt-get install -y git wget libev-dev
RUN apt-get install -y build-essential
RUN apt-get install -y autoconf libtool
RUN apt-get install -y cmake pkg-config
RUN apt-get install -y libssl-dev
RUN git clone --depth=50 https://github.com/protobuf-c/protobuf-c.git
RUN wget https://github.com/google/protobuf/releases/download/v$PROTOBUF_VERSION/protobuf-$PROTOBUF_VERSION.tar.gz

# Installation.
RUN tar xzf protobuf-$PROTOBUF_VERSION.tar.gz
RUN cd protobuf-$PROTOBUF_VERSION && ./configure && make -j2 && sudo make install && sudo ldconfig && cd ..
RUN ldconfig && cd protobuf-c && ./autogen.sh && ./configure && make -j2 && sudo make install && cd ..

# Compile libmumble.
RUN git clone --depth=50 https://github.com/mkroman/libmumble.git
RUN mkdir -p libmumble/build/proto
WORKDIR libmumble/build
RUN cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j2
