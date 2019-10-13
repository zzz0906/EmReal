import serial
import time
import math
serialPort = "COM5"     # 通讯端口
baudRate = 115200       # 通讯波特率(电机默认值)
arduino = serial.Serial(serialPort,baudRate,timeout=0.5)    # 通过串口通讯方式连接到Arduino
aspeed = 1440
def transfer(angle,speed):
    angle = bytes(angle, encoding='utf8')
    arduino.write(angle)
    speed = bytes(speed, encoding='utf8')
    arduino.write(speed)
    #time.sleep(3)
    time.sleep(3)
    msg = arduino.read(60)
    print(msg)

time.sleep(1)
eposide = 0
transfer('1234','1234')
'''
while 1:
    print(eposide,"th:")
    angle = '0'
    speed = '1234'
    transfer(angle,speed)
    t1 = math.sqrt(float(angle)/100/aspeed/2)

    t2 = float(speed)/100/aspeed
    if (t1 < t2):
        time.sleep(int(t1))
        print("sleep time: ",int(t1))
    else:
        t3 = (float(angle)/100-aspeed*t2*t2/2)/(float(speed)/100)
        time.sleep(int(t3 + t2))
        print("sleep time: ",int(t3+t2))
    eposide = eposide + 1    
    # 等待电机转动完毕才能读取返回的信息。可以通过速度计算得到精准地等待时间。可以考虑将该步骤设为异步，这样就不会影响数据的不断发送；还可以考虑用while的方式，一直检测到有数据才读取
    #time.sleep(3)
    #msg = arduino.read(60)
    #print(msg)
'''