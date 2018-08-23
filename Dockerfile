FROM thomasfannes/pre-cook:latest
MAINTAINER decode-it "https://github.com/decode-it"

RUN mkdir -p /var/cook
RUN git clone https://github.com/decode-it/cook /var/cook
RUN cd /var/cook; rake prepare
RUN cd /var/cook; rake b0:build
RUN cd /var/cook; rake b1:build
RUN cd /var/cook; rake test
