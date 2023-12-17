#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int TotalDataSize;
    int OutlineDataCount;
    int MiddleLineDataCount;
    int ParkingSpaceDataCount;
    int OutlineDataSize;
    int MiddleLineDataSize;
    int ParkingSpaceDataSize;
} Header;

typedef struct {
    int OutlineDataOffset;
    int MiddleLineDataOffset;
    int ParkingSpaceDataOffset;
} Offset;

typedef struct {
    char OutlineNodeID[3];
    int StartVertex[2];
    int EndVertex[2];
} Outline;

typedef struct {
    char MiddleLineNodeID[3];
    int StartVertex[2];
    int EndVertex[2];
} MiddleLine;

typedef struct {
    char ParkingSpaceID[3];
    int BottomLeftVertex[2];
    int BottomRightVertex[2];
    int TopRightVertex[2];
    int TopLeftVertex[2];
} ParkingSpace;

int readAndProcessJSON(const char *jsonFileName) {
    Header header;
    Offset offset;
    FILE *file;
    json_object *jsonData, *outlineArray, *middleLineArray, *parkingSpaceArray;
    int i;

    jsonData = json_object_from_file("parking_structure.json");
    if (!jsonData) {
        printf("Failed to read JSON file.\n");
        return -1;
    }

    if (json_object_object_get_ex(jsonData, "outline", &outlineArray) &&
        json_object_object_get_ex(jsonData, "middleLine", &middleLineArray) &&
        json_object_object_get_ex(jsonData, "parkingSpace", &parkingSpaceArray)) {

        header.OutlineDataCount = json_object_array_length(outlineArray);
        header.MiddleLineDataCount = json_object_array_length(middleLineArray);
        header.ParkingSpaceDataCount = json_object_array_length(parkingSpaceArray);

    } else {
        printf("Failed to get data arrays from JSON.\n");
        json_object_put(jsonData);
        return -1;
    }

    file = fopen("ParkingStructure.bin", "wb");
    if (!file) {
        perror("Unable to create file");
        json_object_put(jsonData);
        return -1;
    }

    offset.OutlineDataOffset = sizeof(Header) + sizeof(Offset);
    offset.MiddleLineDataOffset = offset.OutlineDataOffset + header.OutlineDataCount * sizeof(Outline);
    offset.ParkingSpaceDataOffset = offset.MiddleLineDataOffset + header.MiddleLineDataCount * sizeof(MiddleLine);

    fwrite(&header, sizeof(Header), 1, file);
    fwrite(&offset, sizeof(Offset), 1, file);

    for (i = 0; i < header.OutlineDataCount; i++) {
        json_object *outlineObj = json_object_array_get_idx(outlineArray, i);
        Outline outline;
        strncpy(outline.OutlineNodeID, json_object_get_string(json_object_object_get(outlineObj, "OutlineNodeID")), 3);
        json_object *startVertex = json_object_object_get(outlineObj, "StartVertex");
        json_object *endVertex = json_object_object_get(outlineObj, "EndVertex");
        outline.StartVertex[0] = json_object_get_int(json_object_array_get_idx(startVertex, 0));
        outline.StartVertex[1] = json_object_get_int(json_object_array_get_idx(startVertex, 1));
        outline.EndVertex[0] = json_object_get_int(json_object_array_get_idx(endVertex, 0));
        outline.EndVertex[1] = json_object_get_int(json_object_array_get_idx(endVertex, 1));
        fwrite(&outline, sizeof(Outline), 1, file);
    }

    for (i = 0; i < header.MiddleLineDataCount; i++) {
        json_object *middleLineObj = json_object_array_get_idx(middleLineArray, i);
        MiddleLine middleLine;
        strncpy(middleLine.MiddleLineNodeID, json_object_get_string(json_object_object_get(middleLineObj, "MiddleLineNodeID")), 3);
        json_object *startVertex = json_object_object_get(middleLineObj, "StartVertex");
        json_object *endVertex = json_object_object_get(middleLineObj, "EndVertex");
        middleLine.StartVertex[0] = json_object_get_int(json_object_array_get_idx(startVertex, 0));
        middleLine.StartVertex[1] = json_object_get_int(json_object_array_get_idx(startVertex, 1));
        middleLine.EndVertex[0] = json_object_get_int(json_object_array_get_idx(endVertex, 0));
        middleLine.EndVertex[1] = json_object_get_int(json_object_array_get_idx(endVertex, 1));
        fwrite(&middleLine, sizeof(MiddleLine), 1, file);
    }

    for (i = 0; i < header.ParkingSpaceDataCount; i++) {
        json_object *parkingSpaceObj = json_object_array_get_idx(parkingSpaceArray, i);
        ParkingSpace parkingSpace;
        strncpy(parkingSpace.ParkingSpaceID, json_object_get_string(json_object_object_get(parkingSpaceObj, "ParkingSpaceID")), 3);
        json_object *bottomLeftVertex = json_object_object_get(parkingSpaceObj, "BottomLeftVertex");
        json_object *bottomRightVertex = json_object_object_get(parkingSpaceObj, "BottomRightVertex");
        json_object *topRightVertex = json_object_object_get(parkingSpaceObj, "TopRightVertex");
        json_object *topLeftVertex = json_object_object_get(parkingSpaceObj, "TopLeftVertex");
        parkingSpace.BottomLeftVertex[0] = json_object_get_int(json_object_array_get_idx(bottomLeftVertex, 0));
        parkingSpace.BottomLeftVertex[1] = json_object_get_int(json_object_array_get_idx(bottomLeftVertex, 1));
        parkingSpace.BottomRightVertex[0] = json_object_get_int(json_object_array_get_idx(bottomRightVertex, 0));
        parkingSpace.BottomRightVertex[1] = json_object_get_int(json_object_array_get_idx(bottomRightVertex, 1));
        parkingSpace.TopRightVertex[0] = json_object_get_int(json_object_array_get_idx(topRightVertex, 0));
        parkingSpace.TopRightVertex[1] = json_object_get_int(json_object_array_get_idx(topRightVertex, 1));
        parkingSpace.TopLeftVertex[0] = json_object_get_int(json_object_array_get_idx(topLeftVertex, 0));
        parkingSpace.TopLeftVertex[1] = json_object_get_int(json_object_array_get_idx(topLeftVertex, 1));
        fwrite(&parkingSpace, sizeof(ParkingSpace), 1, file);
    }

    fclose(file);
    json_object_put(jsonData);

    return 0;
}

int main() {
    const char *jsonFileName = "parking_structure.json";
    return readAndProcessJSON(jsonFileName);
}