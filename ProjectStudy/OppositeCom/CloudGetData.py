from fastapi import FastAPI
from pydantic import BaseModel
from typing import Optional
import uvicorn

app = FastAPI()

class Student(BaseModel):
    name: str
    age: int
    is_man: Optional[bool] = None

@app.get('/')
def helloWorld():
    return "hello world"

@app.get('/foo/{name}')
def getName(name, age: Optional[int] = None):
    print({'name:': name, 'age': age})
    return {'name:': name, 'age': age}

@app.put('/foo/{name}')
def getName(name, info: Student):
    print(type(info))
    print({'name:': name, 'age': info.age, 'is_man': info.is_man})
    return {'name:': name, 'age': info.age, 'is_man': info.is_man}

@app.post('/foo/{name}')
def getName(name, info: Student):
    print(type(info))
    print({'name:': name, 'age': info.age, 'is_man': info.is_man})
    return {'name:': name, 'age': info.age, 'is_man': info.is_man}

if __name__ == "__main__":
    uvicorn.run('ReceiveDataCloud:app', host='0.0.0.0', port=5000, reload=True) # 阻塞的
