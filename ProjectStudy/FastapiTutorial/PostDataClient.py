import requests
from pydantic import BaseModel
from typing import Optional

class Student(BaseModel):
    name: str
    age: int
    is_man: Optional[bool] = None

dataSend = {
    "name" : "maqun",
    "age" : 18
}
if __name__ == "__main__":
    external_data = {
    "name": "maqun",
    "age" : 12
        }

    
if __name__ == "__main__":
    user = Student(**external_data)
    post_data = {"name": "lu", "age": 90}
    # response = requests.get('http://124.223.76.58:5000/foo/makk?age=12')
    response = requests.post('http://124.223.76.58:5000/foo/makk', json=post_data)
    print(response.content)