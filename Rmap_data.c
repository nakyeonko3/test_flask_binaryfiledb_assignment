#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <stdbool.h>
#define VERTEX_SIZE 2

// 파일의 메타데이터를 저장하는 구조체
typedef struct
{
    int TotalDataSize;
    int OutlineDataCount;
    int MiddleLineDataCount;
    int ParkingSpaceDataCount;
    int PathDataCount;
    int OutlineDataSize;
    int MiddleLineDataSize;
    int ParkingSpaceDataSize;
    int PathDataSize;
} Header;

// 데이터 시작 위치를 저장하는 구조체
typedef struct
{
    int OutlineDataOffset;
    int MiddleLineDataOffset;
    int ParkingSpaceDataOffset;
    int PathDataOffset;
} Offset;

// 윤곽선 및 중앙선 데이터 구조체
typedef struct
{
    int StartVertex[2]; // 시작 좌표
    int EndVertex[2];   // 종료 좌표
} Outline, MiddleLine;

// 주차 공간 데이터 구조체
typedef struct
{
    char ParkingSpaceID[3];   // 주차 공간 ID
    int BottomLeftVertex[2];  // 좌측 하단 좌표
    int BottomRightVertex[2]; // 우측 하단 좌표
    int TopRightVertex[2];    // 우측 상단 좌표
    int TopLeftVertex[2];     // 좌측 상단 좌표
    bool IsParkingAvailable;
    bool IsHandicappedParkingSpace;
} ParkingSpace;

// 경로 데이터를 위한 구조체 정의
typedef struct
{
    char NodeID[1];
    int StartVertex[2];
    int NodeVertex[2];
    int LinkedNodes[4];
    int LinkedIndex[4];
} PathData;

// 정수 배열을 JSON 배열로 변환하는 함수
json_object *create_json_int_array(int *array, size_t size)
{
    json_object *json_array = json_object_new_array();

    for (size_t i = 0; i < size; i++)
    {
        if (array[i] != -1)
        {
            json_object_array_add(json_array, json_object_new_int(array[i]));
        }
    }
    return json_array;
}

// 바이너리 데이터를 JSON으로 변환하는 함수
json_object *convertToJSON(FILE *file, Header header)
{
    json_object *jRoot = json_object_new_object();         // 최상위 JSON 객체
    json_object *jOutlines = json_object_new_array();      // 윤곽선 배열
    json_object *jMiddleLines = json_object_new_array();   // 중앙선 배열
    json_object *jParkingSpaces = json_object_new_array(); // 주차 공간 배열
    json_object *jPathData = json_object_new_array();      // 경로 데이터 배열
    int i;

    // Outline 데이터 읽기 및 JSON 변환
    for (i = 0; i < header.OutlineDataCount; i++)
    {
        Outline outline;
        fread(&outline, sizeof(Outline), 1, file); // Outline 구조체 읽기
        json_object *jOutline = json_object_new_object();
        json_object_object_add(jOutline, "StartVertex", create_json_int_array(outline.StartVertex, VERTEX_SIZE));
        json_object_object_add(jOutline, "EndVertex", create_json_int_array(outline.EndVertex, VERTEX_SIZE));
        json_object_array_add(jOutlines, jOutline);
    }

    // MiddleLine 데이터 읽기 및 JSON 변환
    for (i = 0; i < header.MiddleLineDataCount; i++)
    {
        MiddleLine middleLine;
        fread(&middleLine, sizeof(MiddleLine), 1, file); // MiddleLine 구조체 읽기
        json_object *jMiddleLine = json_object_new_object();
        json_object_object_add(jMiddleLine, "StartVertex", create_json_int_array(middleLine.StartVertex, VERTEX_SIZE));
        json_object_object_add(jMiddleLine, "EndVertex", create_json_int_array(middleLine.EndVertex, VERTEX_SIZE));
        json_object_array_add(jMiddleLines, jMiddleLine);
    }

    // ParkingSpace 데이터 읽기 및 JSON 변환
    for (i = 0; i < header.ParkingSpaceDataCount; i++)
    {
        ParkingSpace parkingSpace;
        fread(&parkingSpace, sizeof(ParkingSpace), 1, file); // ParkingSpace 구조체 읽기
        json_object *jParkingSpace = json_object_new_object();
        json_object_object_add(jParkingSpace, "ParkingSpaceID", json_object_new_string(parkingSpace.ParkingSpaceID));
        json_object_object_add(jParkingSpace, "BottomLeftVertex", create_json_int_array(parkingSpace.BottomLeftVertex, VERTEX_SIZE));
        json_object_object_add(jParkingSpace, "BottomRightVertex", create_json_int_array(parkingSpace.BottomRightVertex, VERTEX_SIZE));
        json_object_object_add(jParkingSpace, "TopRightVertex", create_json_int_array(parkingSpace.TopRightVertex, VERTEX_SIZE));
        json_object_object_add(jParkingSpace, "TopLeftVertex", create_json_int_array(parkingSpace.TopLeftVertex, VERTEX_SIZE));
        json_object_object_add(jParkingSpace, "IsParkingAvailable", json_object_new_boolean(parkingSpace.IsParkingAvailable));
        json_object_object_add(jParkingSpace, "IsHandicappedParkingSpace", json_object_new_boolean(parkingSpace.IsHandicappedParkingSpace));
        json_object_array_add(jParkingSpaces, jParkingSpace);
    }

    for (i = 0; i < header.PathDataCount; i++)
    {
        PathData pathData;
        fread(&pathData, sizeof(PathData), 1, file); // PathData 구조체 읽기
        json_object *jPath = json_object_new_object();
        json_object_object_add(jPath, "NodeID", json_object_new_string(pathData.NodeID)); // 문자열로 변환
        if (i == 0)
        {
            json_object_object_add(jPath, "StartVertex", create_json_int_array(pathData.StartVertex, VERTEX_SIZE));
        }
        json_object_object_add(jPath, "NodeVertex", create_json_int_array(pathData.NodeVertex, VERTEX_SIZE));
        json_object_object_add(jPath, "LinkedNodes", create_json_int_array(pathData.LinkedNodes, 4));

        if (i != 0)
        {
            json_object_object_add(jPath, "LinkedIndex", create_json_int_array(pathData.LinkedIndex, 4));
        }
        json_object_array_add(jPathData, jPath);
    }

    // JSON 객체에 배열 추가
    json_object_object_add(jRoot, "outlines", jOutlines);           // 윤곽선 배열 추가
    json_object_object_add(jRoot, "middleLines", jMiddleLines);     // 중앙선 배열 추가
    json_object_object_add(jRoot, "parkingSpaces", jParkingSpaces); // 주차 공간 배열 추가
    json_object_object_add(jRoot, "pathData", jPathData);           // 경로 데이터 배열 추가
    return jRoot;
}

// 바이너리 파일 읽기 및 JSON 변환 함수
json_object *readBinaryFile(const char *filename)
{
    FILE *file;
    Header header;
    Offset offset;

    file = fopen(filename, "rb"); // 파일 열기
    if (!file)
    {
        perror("Unable to open file");
        return NULL;
    }

    fread(&header, sizeof(Header), 1, file); // Header 구조체 읽기
    fread(&offset, sizeof(Offset), 1, file); // Offset 구조체 읽기

    json_object *jData = convertToJSON(file, header); // JSON 변환

    fclose(file); // 파일 닫기

    return jData;
}

// JSON 데이터를 Flask 서버로 보내는 함수
void send_json_data(const char *json_data)
{
    // 인터넷에 접속할 수 있게 도와줍니다.(웹사이트 주소, 전송할 데이터 등 인터넷 접속에 필요한 모든 정보)
    CURL *curl;
    // 인터넷 접속이 잘 되는지, 안되는지 상태를 알려줍니다.
    CURLcode res;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/receive-json");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// 메인 함수
int main()
{
    const char *binaryFileName = "MapData.bin";          // 바이너리 파일 이름
    json_object *jData = readBinaryFile(binaryFileName); // 바이너리 파일 읽기 및 JSON 변환

    if (jData != NULL)
    {
        const char *json_data = json_object_to_json_string_ext(jData, JSON_C_TO_STRING_PRETTY);

        printf("JSON Data: %s\n", json_data);
        printf("success!");

        send_json_data(json_data);
        json_object_put(jData); // JSON 객체 메모리 해제
    }
    return 0;
}
