#include <cstring>
#include <memory>

#include "include/traderapi/ThostFtdcTraderApi.h"
#include "iostream"
using namespace std;

class CTMMiniSpi : public CThostFtdcTraderSpi {
  ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
  void OnFrontConnected() { cout << "OnFrontConnected" << endl; };

  ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
  ///@param nReason 错误原因
  ///        -3	关闭连接
  ///        -4	网络读失败
  ///        -5	网络写失败
  ///        -6	读订阅流水请求出错
  ///        -7	序列号错误
  ///        -8	读心跳出错
  ///        -9	错误的网络包大小
  void OnFrontDisconnected(int nReason) {
    cout << "OnFrontDisconnected" << endl;
  };

  ///心跳超时警告。当长时间未收到报文时，该方法被调用。
  ///@param nTimeLapse 距离上次接收报文的时间
  void OnHeartBeatWarning(int nTimeLapse) {
    cout << "OnHeartBeatWarning" << endl;
  };

  ///订阅流控警告应答
  void OnRspSubscribeFlowCtrlWarning(
      CThostFtdcSpecificTraderField *pRspSubscribeTraderField,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    cout << "OnRspSubscribeFlowCtrlWarning" << endl;
  };

  ///取消订阅流控警告应答
  void OnRspUnSubscribeFlowCtrlWarning(
      CThostFtdcSpecificTraderField *pRspSubscribeTraderField,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    cout << "OnRspUnSubscribeFlowCtrlWarning" << endl;
  };

  ///客户端认证响应
  void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast) {
    cout << "OnRspAuthenticate" << endl;
  };

  ///登录请求响应
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) {
    cout << "OnRspUserLogin" << endl;
  };

  ///登出请求响应
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) {
    cout << "OnRspUserLogout" << endl;
  };

  ///错误应答
  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                  bool bIsLast) {
    cout << "OnRspError" << endl;
  };
};

class CTPMiniTrader {
 private:
  int request = 0;
  string address;
  string appId;
  string authCode;
  string brokerId;
  string userId;
  CThostFtdcTraderApi *userApi;
  CTMMiniSpi *userSpi;

 public:
  CTPMiniTrader(string address, string appId, string authCode, string brokerId)
      : address(address),
        appId(appId),
        authCode(authCode),
        brokerId(brokerId),
        userApi(CThostFtdcTraderApi::CreateFtdcTraderApi()) {
    cout << "ctp mini init, api version: "
         << CThostFtdcTraderApi::GetApiVersion() << endl;
    this->userSpi = new CTMMiniSpi();
    this->userApi->RegisterSpi(this->userSpi);
    this->userApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    this->userApi->SubscribePublicTopic(THOST_TERT_RESTART);
    this->userApi->RegisterFront((char *)this->address.c_str());
    this->userApi->Init(true);
    return;
  };
  ~CTPMiniTrader() {
    userApi->Release();
    userApi = nullptr;
  };

 public:
  // 获取交易日
  int GetTradingDay() {
    cout << "trading day:" << this->userApi->GetTradingDay() << "\n" << endl;
    return 0;
  }

  // 退出登录
  int ReqUserLogout() {
    return this->userApi->ReqUserLogout(this->getUserLogoutRequest().get(),
                                        this->request++);
  }

  // 客户端认证
  int ReqAuthenticate(string userId) {
    return this->userApi->ReqAuthenticate(this->getAuthRequest(userId).get(),
                                          this->request++);
  }

  // 用户登录
  int ReqUserLogin(string password) {
    return this->userApi->ReqUserLogin(
        this->getUserLoginReqeust(userId, password).get(), this->request++);
  }

 private:
  // 生成退出登录结构体
  unique_ptr<CThostFtdcUserLogoutField> getUserLogoutRequest() {
    unique_ptr<CThostFtdcUserLogoutField> reqLogout(
        new CThostFtdcUserLogoutField);
    copy(this->brokerId.begin(), this->brokerId.end(), reqLogout->BrokerID);
    copy(this->userId.begin(), this->userId.end(), reqLogout->UserID);

    return reqLogout;
  }

  // 生成登录结构体
  unique_ptr<CThostFtdcReqUserLoginField> getUserLoginReqeust(string userId,
                                                              string password) {
    unique_ptr<CThostFtdcReqUserLoginField> reqUserLogin(
        new CThostFtdcReqUserLoginField);

    copy(this->brokerId.begin(), this->brokerId.end(), reqUserLogin->BrokerID);
    copy(userId.begin(), userId.end(), reqUserLogin->UserID);
    copy(password.begin(), password.end(), reqUserLogin->Password);

    return reqUserLogin;
  }

  // 生成客户端认证结构体
  unique_ptr<CThostFtdcReqAuthenticateField> getAuthRequest(string userId) {
    unique_ptr<CThostFtdcReqAuthenticateField> userRequest(
        new CThostFtdcReqAuthenticateField());

    copy(this->brokerId.begin(), this->brokerId.end(), userRequest->BrokerID);
    copy(this->authCode.begin(), this->authCode.end(), userRequest->AuthCode);
    copy(this->appId.begin(), this->appId.end(), userRequest->AppID);
    copy(userId.begin(), userId.end(), userRequest->UserID);

    return userRequest;
  }
};