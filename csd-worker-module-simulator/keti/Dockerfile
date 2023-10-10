FROM ubuntu:18.04
ADD /main /simulator_csd_container8

COPY ./shared_library/libsnappy.so.1 /usr/lib/x86_64-linux-gnu/libsnappy.so.1
COPY ./shared_library/libzstd.so.1.5.5 /usr/lib/x86_64-linux-gnu/libzstd.so.1.5.5
COPY ./shared_library/libzstd.so.1 /usr/lib/x86_64-linux-gnu/libzstd.so.1
COPY ./shared_library/libzstd.so.1.5.5 /usr/local/lib/libzstd.so.1.5.5
COPY ./shared_library/libzstd.so /usr/lib/x86_64-linux-gnu/libzstd.so
COPY ./shared_library/libzstd.so /usr/local/lib/libzstd.so

COPY ./shared_library/libboost_thread.so.1.65.1 /usr/lib/x86_64-linux-gnu/
COPY ./shared_library/libpthread.so.0 /lib/x86_64-linux-gnu/
COPY ./shared_library/libboost_system.so.1.65.1 /usr/lib/x86_64-linux-gnu/libboost_system.so.1.65.1
COPY ./shared_library/libcrypto.so.1.1 /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1
COPY ./shared_library/libcpprest.so.2.10 /usr/lib/x86_64-linux-gnu/libcpprest.so.2.10
COPY ./shared_library/librt.so.1 /lib/x86_64-linux-gnu/
COPY ./shared_library/libstdc++.so.6 /usr/lib/x86_64-linux-gnu/
COPY ./shared_library/libm.so.6 /lib/x86_64-linux-gnu/
COPY ./shared_library/libgcc_s.so.1 /lib/x86_64-linux-gnu/
COPY ./shared_library/libc.so.6 /lib/x86_64-linux-gnu/
COPY ./shared_library/libdl.so.2  /lib/x86_64-linux-gnu/libdl.so.2 
COPY ./shared_library/libssl.so.1.1 /usr/lib/x86_64-linux-gnu/libssl.so.1.1 

ENV LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/
ENV LD_LIBRARY_PATH=/usr/local/lib

EXPOSE 5562
ENTRYPOINT ["/simulator_csd_container8"]
