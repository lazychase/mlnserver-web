# mlnserver-web
C++ 서버 프레임워크 [mlnsdk](https://github.com/lazychase/mlnsdk)를 사용합니다. (Http + MySQL + Redis) 구성 입니다.


# 개발환경 설정(Windows)

## Visual Studio
Visual Studio 2022(or 2019)를 사용합니다. 설치 시 워크로드에서 다음의 항목이 포함되어야 합니다.
* "Linux development with C++"
* 영어 언어팩
  * vcpkg 를 사용하여렴 영어 언어팩이 설치되어야 합니다.


## vcpkg
[공식문서](https://github.com/Microsoft/vcpkg)를 참조하세요

```
C:/local/vcpkg 에 vcpkg 를 설치한 것으로 설정하였습니다. (CMakeLists.txt 파일의 -DCMAKE_TOOLCHAIN_FILE 옵션에 지정)

C:/local> git clone https://github.com/microsoft/vcpkg
C:/local/vcpkg> bootstrap-vcpkg.bat
C:/local/vcpkg> vcpkg integrate install
```

### Global Mode 설치
설치 목록
* OpenSSL
* Boost

#### OpenSSL
아래는 버젼을 찾아서 설치하는 예시입니다.

```
1. 해당 커밋해시를 조회합니다.
C:\local\vcpkg>git log --pretty=format:"%h %s" | findstr openssl | findstr 3.0.4
    커밋 해시 => a61559a81

2. 해당 소스로 체크하웃하여
C:\local\vcpkg>git checkout a61559a81	


3. 설치.
C:\local\vcpkg>git install openssl

find_package(OpenSSL REQUIRED)
target_link_libraries(main PRIVATE OpenSSL::SSL OpenSSL::Crypto)    

```
#### Boost
boost 는 최신 버젼(2022-12-02)인 1.80.0 을 설치합니다. <br>

```
C:\local\vcpkg>vcpkg install boost:x64-windows boost:x64-windows-static
```


# 실행
## on Windows
visual studio 에서 CMakeLists.txt 를 실행하세요.  
![run on vs](https://user-images.githubusercontent.com/97491125/148936553-c7738242-a97e-472b-9d8f-f04efcad6905.jpg)
## on Linux
리눅스 런타임환경(Amazon Linux 2) 도커이미지와 mlnserver 도커파일을 준비하였습니다. docker/Dockerfile 을 사용하여 실행할 수 있습니다.
```
docker build --no-cache -t chase81/mlnserver-web .
docker run --name mlnserver chase81/mlnserver-web
```
