FROM ubuntu:16.04
RUN apt-get update -y
RUN apt-get install -y ruby-dev rake software-properties-common build-essential git python
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-get update -y
RUN apt-get install -y gcc-9 g++-9
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 900 --slave /usr/bin/g++ g++ /usr/bin/g++-9
WORKDIR /var/build
RUN git clone https://github.com/ninja-build/ninja && cd ninja && ./configure.py --bootstrap && cp ninja /usr/bin && cd .. && rm -rf ninja
ADD script.sh /var/build
