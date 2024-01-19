import socket
import time
import fcntl
import struct
import threading
import select
import json
import signal
import sys
import cereal.messaging as messaging

from threading import Thread


class Port:
  BROADCAST_PORT = 2899
  RECEIVE_PORT = 2843


class Client:
    def __init__(self):
        self.remote_address = None

        self.sm = messaging.SubMaster(['liveNaviData']) 

        self.frame = 0
        self.program_run = True
        self.lock = threading.Lock()

        broadcast = Thread(target=self.broadcast_thread, args=[])
        broadcast.daemon = True
        broadcast.start()



    def get_broadcast_address(self):
        print( "get_broadcast_address" )
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
                ip = fcntl.ioctl(
                s.fileno(),
                0x8919,
                struct.pack('256s', 'wlan0'.encode('utf-8'))
                )[20:24]
                return socket.inet_ntoa(ip)
        except:
            return None
   
    def broadcast_thread(self):
        broadcast_address = None
        recv_timeout = False
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            #sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            sock.settimeout(1.0)  # 1초의 타임아웃 설정
            while self.program_run:
                try:
                    if broadcast_address is None or self.frame % 10 == 0:
                        broadcast_address = self.get_broadcast_address()

                    if broadcast_address is not None and self.remote_address is None:
                        print('broadcast', broadcast_address)

                        msg = 'EON:ROAD_LIMIT_SERVICE:v1'.encode()
                        for i in range(1, 255):
                            ip_tuple = socket.inet_aton(broadcast_address)
                            new_ip = ip_tuple[:-1] + bytes([i])
                            address = (socket.inet_ntoa(new_ip), Port.BROADCAST_PORT)
                            sock.sendto(msg, address)
                except Exception as e:
                    print(f"Send error occurred: {e}")


                try:
                    data, self.remote_address = sock.recvfrom(2048)
                    print(f"recv: {data} {self.remote_address}")
                except socket.timeout:
                    print("recv timeout") 
                    recv_timeout = True
                except Exception as e:
                    print(f"recv error occurred: {e}") 
  
                self.frame += 1

                if recv_timeout == False:
                  time.sleep(1.)
                else:
                    recv_timeout = False





    def udp_recv(self, sock):
        ret = False
        try:
            ready = select.select([sock], [], [], 1.)
            ret = bool(ready[0])
            if ret:
                data, self.remote_addr = sock.recvfrom(2048)
                json_obj = json.loads(data.decode())
                print(f"json={json_obj}")                


                if 'echo' in json_obj:
                    try:
                        echo = json.dumps(json_obj["echo"])
                        sock.sendto(echo.encode(), self.remote_addr ) #  (self.remote_addr[0], Port.BROADCAST_PORT))
                        ret = False
                    except:
                        pass
        except:
            try:
                self.lock.acquire()
            finally:
                self.lock.release()
    
        return ret 

    def update(self, sock ):
        self.sm.update(0)
        liveNaviData = self.sm['liveNaviData']

        speedLimit = liveNaviData.speedLimit
        speedLimitDistance = liveNaviData.arrivalDistance  #speedLimitDistance
        mapValid = liveNaviData.mapValid
        trafficType = liveNaviData.trafficType

        self.udp_recv( sock )
        if self.remote_address:
            json_data = {
                "speedLimit":speedLimit,
                "speedLimitDistance":speedLimitDistance,
                "mapValid":mapValid,
                "trafficType":trafficType
            }

            try:
                remote_addr = ( self.remote_address[0], Port.RECEIVE_PORT )
                sock.sendto( json.dumps(json_data).encode(), remote_addr )
            except Exception as e:
                print(f"client_socket error occurred: {e}")


        time.sleep(0.5)     


def main():
    client = Client()
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    client.program_run = True
    def signal_handler(signal, frame):
        print("Ctrl+C detected. Exiting gracefully.")
        # 여기에 추가적인 종료 로직을 작성할 수 있습니다.
        client.program_run = False     
        sys.exit(0)


    signal.signal(signal.SIGINT, signal_handler)  # Ctrl + C 핸들러 등록


    while client.program_run:
        client.update( client_socket )
        print(f"client_socket run: frame = {client.frame}  program_run={client.program_run}")


if __name__ == "__main__":
    main()