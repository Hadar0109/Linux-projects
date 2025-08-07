# encrypter/Dockerfile
FROM ubuntu:22.04

RUN apt-get update \
 && apt-get install -y build-essential dpkg libssl-dev \
 && rm -rf /var/lib/apt/lists/*

COPY mta_crypt_lib /usr/src/mta_crypt_lib
WORKDIR /usr/src/mta_crypt_lib
RUN make \
 && dpkg-deb --build mta-utils-dev \
 && mkdir -p /tmp/pkg \
 && dpkg-deb -x mta-utils-dev.deb /tmp/pkg \
 && cp /tmp/pkg/usr/lib/*/libmta_*.so* /usr/local/lib/ \
 && cp /tmp/pkg/usr/include/*.h       /usr/local/include/ \
 && rm -rf /tmp/pkg mta-utils-dev.deb

RUN ldconfig

WORKDIR /root
COPY encrypter/encrypter.c .
COPY utils.c .
COPY utils.h .

RUN gcc -Wall -o encrypter \
      encrypter.c utils.c \
      -lrt -lmta_rand -lmta_crypt -lcrypto -pthread

CMD ["/root/encrypter"]



