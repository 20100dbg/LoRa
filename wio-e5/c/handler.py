import subprocess

# serial.Serial(port=port, baudrate=115200, timeout=1)


cmd = ["./prog"]
process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.PIPE, 
	text=False)


size = 6

while True:
	line = process.stdout.read()
	if line:
		print(line, end="")
	else:
		break

print()


msg = "yopla".encode()
process.stdin.write(msg)
