import serial
import binascii
import time

serialPort = "COM7"     # 通讯端口
baudRate = 115200       # 通讯波特率(电机默认值)
arduino = serial.Serial(serialPort,baudRate,timeout=0.5)    # 通过串口通讯方式连接到Arduino

time.sleep(1)
while 1:
    time.sleep(1)

    command = input('请输入模式(g读取,d驱动):')
    command = bytes(command, encoding='utf8')
    arduino.write(command)

    if command == b'g':
        command = input('请输入电机ID:')
        command = bytes(command, encoding='utf8')
        arduino.write(command)

        time.sleep(1)
        msg = arduino.read(8)
        msg = binascii.b2a_hex(msg).decode('utf-8')
        print(msg)
        # position = int(msg[5]) + int(msg[6]) * 256
        # print(position)
    else:
        command = input('请输入电机ID:')
        command = bytes(command, encoding='utf8')
        arduino.write(command)

        command = input('请输入转动方向(+-)')
        command = bytes(command, encoding='utf8')
        arduino.write(command)

        command = input('请输入需要转动的角度(-9000~9000,1234表示12.34度):')
        command = bytes(command, encoding='utf8')
        print(command)
        arduino.write(command)

        # 如需更高速度，要更改Arduino代码
        command = input('请输入需要转动的最大速度(0~65536,1234表示12.34dps):')
        command = bytes(command, encoding='utf8')
        print(command)
        arduino.write(command)

        '''
        msg = arduino.read(14)
        msg = binascii.b2a_hex(msg).decode('utf-8')
        print(msg)
        '''