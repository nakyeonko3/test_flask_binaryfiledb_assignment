# 웹서버로 바이너리 파일 조작하기
- `test_tmp` 폴더 안에는 다른 라이브러리들 테스트 해보거나 연습했던 기타 데이터들
- `flask_server_codes` 폴더 안에 flask 서버 데이터들이 들어 있음
## 종속성 설치(Linux, Ubuntu 20.4.0 LTS 버전 기준)
```
sudo apt install libjson-c-dev 
sudo apt install curl
```


## 예제 실행 방법 

```bash
python flask_server_codes\startflaskserver.py
gcc Cmap_data.c -o Cmap_data -ljson-c
./Cmap_data
gcc Rmap_data.c -o Rmap_data -ljson-c -lcurl
./Rmap_data 
```
