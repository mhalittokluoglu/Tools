import socket
import time 
class UdpServer:
    def __init__(self, ipAddress, port):
        self.m_IpAddress = ipAddress
        self.m_PortNumber = port
        self.__InitServer()
        self.m_Socket = socket.socket(family = socket.AF_INET, type = socket.SOCK_DGRAM)
        self.m_Socket.bind((self.m_IpAddress, self.m_PortNumber))
        self.Running = True

    def __InitServer(self):
        self.m_Socket = socket.socket(family = socket.AF_INET, type = socket.SOCK_DGRAM)
        self.m_Socket.bind((self.m_IpAddress, self.m_PortNumber))

    def SetCallBack(self, callbackFunction):
        self.callbackFunction = callbackFunction

    def Close(self):
        self.Running = False
        time.sleep(0.2)
        self.m_Socket.close()

    def ReceiveData(self):
        self.m_Socket.settimeout(0.1)
        while self.Running:
            try:
                bytesAddressPair = self.m_Socket.recvfrom(1024)
                rawData = bytesAddressPair[0].decode('utf-8')
                address = bytesAddressPair[1]
                self.callbackFunction(rawData)
            except:
                pass
