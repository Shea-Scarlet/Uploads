import http.server
import socketserver
import subprocess

# Set the IP address and port for the Weather Monitor (Web Server)
weather_monitor_ip = "192.168.137.129"
weather_monitor_port = 1234

# Set the location for the Weather Station
location = "Scotts+Valley"

# Define the Weather Monitor HTTP request handler
class WeatherMonitorHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length).decode('utf-8')
        print(f"Received POST data: {post_data}")
        self.send_response(200)
        self.end_headers()
        self.wfile.write("Data received by Weather Monitor".encode('utf-8'))

# Start the Weather Monitor Web Server
with socketserver.TCPServer((weather_monitor_ip, weather_monitor_port), WeatherMonitorHandler) as httpd:
    print(f"Server started at http://{weather_monitor_ip}:{weather_monitor_port}")

    # Set up the Hotspot
    hotspot_setup_command = f"sudo create_hotspot.sh {weather_monitor_ip} 255.255.255.0"
    subprocess.run(hotspot_setup_command, shell=True)

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass

# When the server is terminated, stop the Hotspot
hotspot_stop_command = "sudo stop_hotspot.sh"
subprocess.run(hotspot_stop_command, shell=True)
