import pyb, sensor, image, time, math
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock()
f_x = (2.8 / 3.984) * 160
f_y = (2.8 / 2.952) * 120
c_x = 160 * 0.5
c_y = 120 * 0.5
def degrees(radians):
   return (180 * radians) / math.pi
uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)
while(True):
   clock.tick()
   img = sensor.snapshot()
   for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y):
	  img.draw_rectangle(tag.rect(), color = (255, 0, 0))
	  img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
	  print_args = (tag.x_translation(), degrees(tag.y_rotation()))
	  uart.write("/findTag/run %f %f\r\n" % print_args)
	  print(("/findTag/run %f %f" % print_args).encode())
   uart.write(("FPS %f\r\n" % clock.fps()).encode())
   time.sleep_ms(1000)