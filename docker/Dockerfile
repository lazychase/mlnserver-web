FROM lazychase81/chase-public:mbbase-al2

RUN cd /usr/src \
&& git clone -b main --single-branch https://github.com/lazychase/mlnserver-web.git \
&& cd mlnserver \
&& source /opt/rh/devtoolset-11/enable \
&& cmake -DCMAKE_BUILD_TYPE=Debug CMakeLists.txt && make -j$(nproc) \
&& mkdir -p /usr/local/mlnserver \
&& cp ./mlnserver/mlnserver /usr/local/mlnserver \
&& cp ./mlnserver/netconfig.json /usr/local/mlnserver

WORKDIR /usr/local/mlnserver

CMD ["./mlnserver"]
