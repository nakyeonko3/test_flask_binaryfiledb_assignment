from flask import Flask, request, jsonify
from flask_cors import CORS
from user_data_storage import UserDataStorage
from user_data_management import UserDataManagement
import json

app = Flask(__name__)
CORS(app)

storage = UserDataStorage()
user_data = UserDataManagement(storage)

@app.route('/user', methods=['POST'])
def handle_user():
    data = request.json

    if data is None:
        return jsonify({"message": "JSON 데이터가 제공되지 않았습니다."}), 400

    event_id = data.get('EventId')

    if event_id == 1:  # 사용자 등록
        user_name = data.get('UserName')
        car_type = data.get('CarType')
        car_number = data.get('CarNumber')

        if not all([user_name, car_type, car_number]):
            return jsonify({"message": "사용자 데이터가 누락되었습니다."}), 400

        # 사용자 정보 검색
        existing_user = user_data.searchUser(car_number)

        if existing_user:
            return jsonify({"message": "이미 사용자가 등록되어 있습니다.", "data": existing_user})

        # 사용자 정보 등록
        userKey = user_data.registerUser(user_name, car_type, car_number)
        result = user_data.searchUser(car_number)

        storage.print_stored_data()
        print("Received Data:", data)
        
        return jsonify({"message": "사용자가 성공적으로 등록되었습니다.", "userKey": userKey, "data": result})

    elif event_id == 2:  # 사용자 삭제
        carNumber = data.get('CarNumber')
        if user_data.deleteUser(carNumber):
            return jsonify({"message": "사용자가 성공적으로 삭제되었습니다."})
        else:
            return jsonify({"message": "사용자를 찾을 수 없습니다."}), 404

    elif event_id == 3:  # 사용자 검색
        carNumber = data.get('CarNumber')
        result = user_data.searchUser(carNumber)
        if not result:  # 검색 결과가 없을 때 빈 딕셔너리 반환
            return jsonify({"message": "검색 결과가 없습니다.", "data": {}}), 404
        return jsonify({"message": "사용자를 찾았습니다.", "data": result})

    elif event_id == 4:  # 주차 공간 선택
        carNumber = data.get('CarNumber')
        parkingSpace = data.get('ParkingSpace')

        if not all([carNumber, parkingSpace]):
            return jsonify({"message": "데이터가 누락되었습니다."}), 400

        if user_data.selectParkingSpace(carNumber, parkingSpace):
            return jsonify({"message": "주차 공간이 등록되었습니다."})
        else:
            return jsonify({"message": "사용자를 찾을 수 없습니다."}), 404

    else:
        return jsonify({"message": "유효하지 않은 이벤트 ID입니다."}), 400

@app.route('/receive-json', methods=['POST'])
def receive_json():
    data = request.json
    # JSON 데이터 처리
    print(type(data))
    print(data['pathData'])

    return "JSON Received", 200


@app.route('/MapData', methods=['POST'])
def post_MapData():
    if request.method == "POST":
        data = request.json
        # JSON 데이터 처리
        print(json.dumps(data, indent=2))
        return "JSON Received", 200



@app.rout("/helloworld", methds=['GET'])
def helloworld():
    return "hello, world!"


if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0")