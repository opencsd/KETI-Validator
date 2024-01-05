FROM ubuntu:18.04

ENV OPERATOR=/usr/local/bin/validator \
    USER_UID=root \
    USER_NAME=discovery
COPY ./cmake/build/KETI_Validator ${OPERATOR}
COPY ./cmake/snippet /usr/local/snippet
COPY ./shared_library /usr/lib/x86_64-linux-gnu

WORKDIR /usr/local/bin
ENTRYPOINT ["./validator"]

USER ${USER_UID}