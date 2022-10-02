
#include "jiangRpc/rpc/rpc_server.h"
#include "test.pb.h"



AddResp add(AddReq req)
{
    AddResp resp;
    resp.set_result(req.first() + req.second());
    return resp;
}



class Calculate {
public:
    SubResp sub(SubReq req)
    {   
        SubResp resp;
        resp.set_result(req.first() - req.second());
        return resp;
    }   
};

int main()
{
    jiangRpc::RpcServer rpcServer("localhost", 8080);
    rpcServer.registService("add", add);
    Calculate cal;
    rpcServer.registService("Calculate.sub", [&cal](SubReq req) -> SubResp{return cal.sub(req);});
    rpcServer.start();
}

