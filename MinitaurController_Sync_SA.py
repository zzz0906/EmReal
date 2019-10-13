import serial
import time

serialPort = "COM7"     # 通讯端口
baudRate = 115200       # 通讯波特率(电机默认值)
arduino = serial.Serial(serialPort,baudRate,timeout=0.5)    # 通过串口通讯方式连接到Arduino

time.sleep(1)
while 1:
    command = input('请输入需要转动的角度(-9000~9000,1234表示12.34度):')
    command = bytes(command, encoding='utf8')
    arduino.write(command)

    command = input('请输入需要转动的速度(0~65536,1234表示12.34dps):')
    command = bytes(command, encoding='utf8')
    arduino.write(command)

    # 等待电机转动完毕才能读取返回的信息。可以通过速度计算得到精准地等待时间。可以考虑将该步骤设为异步，这样就不会影响数据的不断发送；还可以考虑用while的方式，一直检测到有数据才读取
    time.sleep(3)
    msg = arduino.read(60)
    print(msg)
