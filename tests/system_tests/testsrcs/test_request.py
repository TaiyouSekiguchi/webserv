import unittest
import http.client

class TestRequest(unittest.TestCase):

	@classmethod
	def setUpClass(cls):
		cls.conn = http.client.HTTPConnection("localhost", 8080)
		cls.conn2 = http.client.HTTPConnection("localhost", 8081)

	@classmethod
	def tearDownClass(cls):
		cls.conn.close()
		cls.conn2.close()

	def test_basic(self):
		self.conn.request("GET", "/", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "16")
		with self.subTest(): self.assertEqual(res.read(), b"html/index.html\n")

	def test_port_8081(self):
		self.conn2.request("GET", "/", None, {})
		res = self.conn2.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "16")
		with self.subTest(): self.assertEqual(res.read(), b"html/index.html\n")

	def test_connection_close(self):
		self.conn.request("GET", "/", None, {"Connection": "close"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "close")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "16")
		with self.subTest(): self.assertEqual(res.read(), b"html/index.html\n")

	def test_host_webserv2(self):
		self.conn.request("GET", "/", None, {"Host": "webserv2"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "16")
		with self.subTest(): self.assertEqual(res.read(), b"html/hello.html\n")

	def test_host_invalid(self):
		self.conn.request("GET", "/", None, {"Host": ""})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 400)
		with self.subTest(): self.assertEqual(res.reason, "Bad Request")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "close")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "152")
		with self.subTest(): self.assertIn(b"400 Bad Request", res.read())

	def test_error_page(self):
		self.conn.request("GET", "/", None, {"Host": "webserv2", "Content-Length": " "})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 400)
		with self.subTest(): self.assertEqual(res.reason, "Bad Request")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "close")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "14")
		with self.subTest(): self.assertEqual(res.read(), b"html/40x.html\n")

	def test_error_page_redirect(self):
		self.conn.request("GET", "/", None, {"Host": "webserv2", "Transfer-Encoding": "gzip"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 302)
		with self.subTest(): self.assertEqual(res.reason, "Moved Temporarily")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "close")
		with self.subTest(): self.assertEqual(res.getheader("Location"), "../../html/40x.html")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/plain")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "140")
		with self.subTest(): self.assertIn(b"302 Found", res.read())
