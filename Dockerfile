FROM thomasfannes/pre-cook:latest
MAINTAINER decode-it "https://github.com/decode-it"

# everything in one step, for clean purposes
RUN mkdir -p /var/cook/repo && \
    git clone https://github.com/decode-it/cook /var/cook/repo && \
    cd /var/cook/repo && \
    rake prepare b0:build b1:build test && \
    cp cook.exe /usr/bin/cook && \
    rm -rf /var/cook

