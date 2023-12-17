class UserDataManagement:
    def __init__(self, storage):
        self.storage = storage

    def registerUser(self, Name, CarType, CarNumber):
        # 사용자 데이터를 검색합니다.
        existing_user = self.searchUser(CarNumber)

        # 이미 있는 데이터인 경우 해당 데이터를 반환
        if existing_user:
            return existing_user

        # 없는 경우 데이터를 등록합니다.
        self.storage.storeUserData(Name, CarType, CarNumber)
        return self.searchUser(CarNumber)

    def selectParkingSpace(self, CarNumber, ParkingSpace):
        for key, value in self.storage.userData.items():
            if value["CarNumber"].lower() == CarNumber.lower():
                updatedData = {"ParkingSpace": ParkingSpace}
                self.storage.updateUserData(key, updatedData)
                print(f"Updated {key}: {self.storage.userData[key]}")  # 로그 추가
                return True
        return False

    def searchUser(self, CarNumber):
        for _, value in self.storage.userData.items():
            if value["CarNumber"].lower() == CarNumber.lower():
                return value

    def deleteUser(self, CarNumber):
        for key, value in self.storage.userData.items():
            if value["CarNumber"].lower() == CarNumber.lower():
                del self.storage.userData[key]
                return True
        return False