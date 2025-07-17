import socket

HOST = '0.0.0.0'  # Accept from any IP
PORT = 5000       # Must match SERVER_PORT on ESP32

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print(f"🚀 Server is listening on {HOST}:{PORT}...")

        while True:
            conn, addr = s.accept()
            print(f"🔗 Connected by {addr}")

            with conn:
                conn.settimeout(5)  # Allow up to 5s wait
                while True:
                    data = conn.recv(1024)
                    if not data:
                        print("🔌 Client disconnected.")
                        break
                    print(f"📥 Received: {data.decode().strip()}")

if __name__ == "__main__":
    start_server()
