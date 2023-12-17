class UserDataStorage:
    def __init__(self):
        self.userData = {}
        self.userCount = 0

    def storeUserData(self, Name, CarType, CarNumber):
        self.userCount += 1
        userKey = f"User {self.userCount}"
        self.userData[userKey] = {
            "Name": Name,
            "CarType": CarType,
            "CarNumber": CarNumber,
            "ParkingSpace": None
        }
        self.print_stored_data()  # 데이터 저장 후 전체 데이터 출력

    def updateUserData(self, userKey, updatedData):
        if userKey in self.userData:
            self.userData[userKey].update(updatedData)
            self.print_stored_data()  # 데이터 업데이트 후 전체 데이터 출력

    def deleteUserData(self, userKey):
        if userKey in self.userData:
            del self.userData[userKey]
            self.print_stored_data()  # 데이터 삭제 후 전체 데이터 출력

    def print_stored_data(self):
        print("현재 저장된 사용자 데이터")
        for user_key, user_info in self.userData.items():
            print(f"{user_key}: {user_info}") 