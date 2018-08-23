FROM thomasfannes/pre-cook:latest
MAINTAINER decode-it "https://github.com/decode-it"

RUN mkdir -p /var/cook/repo
RUN git clone https://github.com/decode-it/cook /var/cook/repo
RUN cd /var/cook/repo; rake prepare

# build, run the tests and install
RUN cd /var/cook/repo; rake b0:build
RUN cd /var/cook/repo; rake b1:build
RUN cd /var/cook/repo; rake test

# copy to a location where we can extract it
RUN cp /var/cook/repo/cook.exe /var/cook/cook

# cleanup
RUN rm -rf /var/cook/repo
