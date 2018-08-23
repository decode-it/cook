FROM thomasfannes/pre-cook:latest
MAINTAINER decode-it "https://github.com/decode-it"

RUN rake prepare
RUN rake b0:build
RUN rake b1:build
RUN rake test
