FROM gcc:10
RUN apt-get update -y
RUN apt-get install -y ninja-build ruby-dev rake
WORKDIR /var/build
ADD script.sh /var/build
