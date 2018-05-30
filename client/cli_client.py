import socket

def main():
	while True:
		try:
			sock = socket.socket(socket.AF_INET , socket.SOCK_STREAM)
			addr = ('localhost' , 8080)
	
			sock.connect(addr)
	
			sock.send("200")
			msg = sock.recv(1024)
	
			print("Message from server: " , msg)
	
			sock.send("0");
	
			sock.close()
			print("Success")
			exit(0)
	

		except KeyboardInterrupt as ks:
			exit(0)
		except Exception as e:
			input("press to try again")



if __name__ == '__main__':
	main()
