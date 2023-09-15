from pydantic import BaseModel

class User(BaseModel):
    id : int
    name: str = "maka"

external_data = {
    "id": 123
}

user = User(**external_data)
# print("\033[31m1. -------\033[0m")
# print(user.id)
print(user.dict())
