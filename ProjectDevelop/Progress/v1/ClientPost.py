import requests
from pydantic import BaseModel
from typing import Optional

class Student(BaseModel):
    name: str
    age: int
    is_man: Optional[bool] = None

if __name__ == "__main__":
    postData = {"name": "lu", "age": 3, "is_man": False}
    # response = requests.get('http://124.223.76.58:5000/foo/makk?age=12')
    response = requests.post('http://124.223.76.58:8000/foo/makk', json=postData)
    print(response.content)
    