import requests
import test_pb2

def call(name, req, resp):
    url = "http://192.168.1.104:8080/"
    url += name
    output = req.SerializeToString()
    res = requests.get(url=url, data=output)
    resp.ParseFromString(str.encode(res.text))

def main():
    req = test_pb2.AddReq()
    resp = test_pb2.AddResp()
    req.first = 10
    req.second = 20

    call("add", req, resp)
    print(resp.result)

    req.first = 100
    req.second = 23
    call("add", req, resp)

    print(resp.result)

    req = test_pb2.SubReq()
    resp = test_pb2.SubResp()
    req.first = 35
    req.second = 10

    call("Calculate.sub", req, resp)
    print(resp.result)

if __name__ == '__main__':
    main()
