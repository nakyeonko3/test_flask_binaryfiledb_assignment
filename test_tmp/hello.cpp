#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>

// 파일의 메타데이터를 저장하는 구조체
typedef struct {
    int TotalDataSize;
    int OutlineDataCount;
    int MiddleLineDataCount;
    int ParkingSpaceDataCount;
    int OutlineDataSize;
    int MiddleLineDataSize;
    int ParkingSpaceDataSize;
} Header;

// 데이터 시작 위치를 저장하는 구조체
typedef struct {
    int OutlineDataOffset;
    int MiddleLineDataOffset;
    int ParkingSpaceDataOffset;
} Offset;

// 윤곽선 및 중앙선 데이터 구조체
typedef struct {
    char NodeID[3];
    int StartVertex[2];
    int EndVertex[2];
} Outline, MiddleLine;

// 주차 공간 데이터 구조체
typedef struct {
    char ParkingSpaceID[3];
    int BottomLeftVertex[2];
    int BottomRightVertex[2];
    int TopRightVertex[2];
    int TopLeftVertex[2];
} ParkingSpace;

json_object *create_json_int_array(int *array, size_t size) {
    json_object *jarray = json_object_new_array();
    for (size_t i = 0; i < size; i++) {
        json_object_array_add(jarray, json_object_new_int(array[i]));
    }
    return jarray;
}

// 바이너리 데이터를 JSON으로 변환하는 함수
json_object* convertToJSON(FILE *file, Header header) {
    json_object *jRoot = json_object_new_object();
    json_object *jOutlines = json_object_new_array();
    json_object *jMiddleLines = json_object_new_array();
    json_object *jParkingSpaces = json_object_new_array();
    int i;

    // Outline 데이터 읽기 및 JSON 변환
    for (i = 0; i < header.OutlineDataCount; i++) {
        Outline outline;
        fread(&outline, sizeof(Outline), 1, file);

        json_object *jOutline = json_object_new_object();
        json_object_object_add(jOutline, "OutlineNodeID", json_object_new_string(outline.NodeID));
        json_object_object_add(jOutline, "StartVertex", create_json_int_array(outline.StartVertex, 2));
        json_object_object_add(jOutline, "EndVertex", create_json_int_array(outline.EndVertex, 2));
        json_object_array_add(jOutlines, jOutline);
    }

    // MiddleLine 데이터 읽기 및 JSON 변환
    for (i = 0; i < header.MiddleLineDataCount; i++) {
        MiddleLine middleLine;
        fread(&middleLine, sizeof(MiddleLine), 1, file);

        json_object *jMiddleLine = json_object_new_object();
        json_object_object_add(jMiddleLine, "MiddleLineNodeID", json_object_new_string(middleLine.NodeID));
        json_object_object_add(jMiddleLine, "StartVertex", create_json_int_array(middleLine.StartVertex, 2));
        json_object_object_add(jMiddleLine, "EndVertex", create_json_int_array(middleLine.EndVertex, 2));
        json_object_array_add(jMiddleLines, jMiddleLine);
    }

    // ParkingSpace 데이터 읽기 및 JSON 변환
    for (i = 0; i < header.ParkingSpaceDataCount; i++) {
        ParkingSpace parkingSpace;
        fread(&parkingSpace, sizeof(ParkingSpace), 1, file);

        json_object *jParkingSpace = json_object_new_object();
        json_object_object_add(jParkingSpace, "ParkingSpaceID", json_object_new_string(parkingSpace.ParkingSpaceID));
        json_object_object_add(jParkingSpace, "BottomLeftVertex", create_json_int_array(parkingSpace.BottomLeftVertex, 2));
        json_object_object_add(jParkingSpace, "BottomRightVertex", create_json_int_array(parkingSpace.BottomRightVertex, 2));
        json_object_object_add(jParkingSpace, "TopRightVertex", create_json_int_array(parkingSpace.TopRightVertex, 2));
        json_object_object_add(jParkingSpace, "TopLeftVertex", create_json_int_array(parkingSpace.TopLeftVertex, 2));
        json_object_array_add(jParkingSpaces, jParkingSpace);
    }

    // JSON 객체에 배열 추가
    json_object_object_add(jRoot, "outlines", jOutlines);
    json_object_object_add(jRoot, "middleLines", jMiddleLines);
    json_object_object_add(jRoot, "parkingSpaces", jParkingSpaces);

    return jRoot;
}

int readBinaryFile(const char *filename) {
    FILE *file;
    Header header;
    Offset offset;

    file = fopen(filename, "rb");
    if (!file) {
        perror("Unable to open file");
        return -1;
    }

    fread(&header, sizeof(Header), 1, file);
    fread(&offset, sizeof(Offset), 1, file);

    json_object *jData = convertToJSON(file, header);
    printf("%s\n", json_object_to_json_string_ext(jData, JSON_C_TO_STRING_PRETTY));

    fclose(file);
    json_object_put(jData);

    return 0;
}

int main() {
    const char *binaryFileName = "ParkingStructure.bin";
    return readBinaryFile(binaryFileName);
}