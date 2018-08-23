FROM thomasfannes/pre-cook:latest
MAINTAINER decode-it "https://github.com/decode-it"

RUN mkdir /var/cook/repo
RUN git clone https://github.com/decode-it/cook /var/cook/repo
RUN cd /var/cook/repo; rake prepare
RUN cd /var/cook/repo; rake b0:build
RUN cd /var/cook/repo; rake b1:build
RUN cd /var/cook/repo; rake test
RUN cd /var/cook/repo; rake install
RUN cp /var/cook/repo/cook.exe /var/cook/cook
RUN rm -rf /var/cook/repo
