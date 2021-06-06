import time
import serial
import sys,tty,termios
class _Getch:
    def __call__(self):
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

def get():
    inkey = _Getch()
    while(1):
        k=inkey()
        if k!='':break
    if k=='\x1b':
        k2 = inkey()
        k3 = inkey()
        if k3=='A':
            print ("up")
            s.write("/park/run 18 18 W \n".encode())
            #s.write("/goStraight/run 55 \n".encode())
            #s.write("/set_cm/run 50 \n".encode())
        if k3=='B':
            print ("down")
            #s.write("/goStraight/run -55 \n".encode())
            s.write("/set_cm/run 26 \n".encode())
        if k3=='C':
            print ("right")
            s.write("/turn/run 110 -0.2 \n".encode())
        if k3=='D':
            print ("left")
            s.write("/turn/run 110 0.1 \n".encode())
            time.sleep(0.15)
        time.sleep(1)
        s.write("/stop/run \n".encode())
    elif k=='q':
        print ("quit")
        return 0
    else:
        print ("not an arrow key!")
    return 1


if len(sys.argv) < 1:
    print ("No port input")
s = serial.Serial(sys.argv[1])



while get():
    i = 0


#s.write("/set_cm/run 50 \n".encode())

'''
d1 = int(input("d1 :"))
d2 = int(input("d2 :"))
diraction = input("dir(N, S, W, E) :")
buff = ""

if (diraction == 'E'):
    d1 = d1 + 28
    d2 = d2 + 35

    buff = "/set_cm/run " + str(d2) + " \n"
    s.write(buff.encode())
    time.sleep(8)

    buff = "/turn/run 90 0.1 \n"
    s.write(buff.encode())
    time.sleep(1.5)
    s.write("/stop/run \n".encode())
    buff = "/set_cm/run " + str(d1) + " \n"
    s.write(buff.encode())
    time.sleep(10)
    print("done")
elif (diraction == 'W'):
    d1 = d1 + 28
    d2 = d2 + 35




    buff = "/set_cm/run " + str(d2) + " \n"
    s.write(buff.encode())
    time.sleep(8)

    buff = "/turn/run 90 -0.2 \n"
    s.write(buff.encode())
    time.sleep(1.5)
    s.write("/stop/run \n".encode())
    buff = "/set_cm/run " + str(d1) + " \n"
    s.write(buff.encode())
    time.sleep(10)
    print("done")


elif (diraction == 'S'):
    d1 = d1 + 28
    buff = "/set_cm/run " + str(d1) + " \n"
    s.write(buff.encode())
'''

#while(1):
#    pass

#print(d1)
#print(d2)
#print(diraction)