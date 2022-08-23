import unittest
import http.client

class TestMethod(unittest.TestCase):

	@classmethod
	def setUpClass(cls):
		cls.conn = http.client.HTTPConnection("localhost", 8080)

	@classmethod
	def tearDownClass(cls):
		cls.conn.close()

	def test_basic(self):
		self.conn.request("GET", "/", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "16")
		with self.subTest(): self.assertEqual(res.read(), b"html/index.html\n")

	def test_empty_file(self):
		self.conn.request("GET", "/empty.html", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "0")
		with self.subTest(): self.assertEqual(res.read(), b"")

	def test_root(self):
		self.conn.request("GET", "/hoge/", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "26")
		with self.subTest(): self.assertEqual(res.read(), b"html/sub1/hoge/index.html\n")

	def test_index(self):
		self.conn.request("GET", "/sub2/", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "20")
		with self.subTest(): self.assertEqual(res.read(), b"html/sub2/sub2.html\n")

	def test_autoindex(self):
		self.conn.request("GET", "/sub1/", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		body = res.read()
		with self.subTest(): self.assertIn(b"noindex", body)
		with self.subTest(): self.assertIn(b"hoge", body)
		with self.subTest(): self.assertIn(b"index.html", body)
		with self.subTest(): self.assertIn(b"sub1.html", body)

	def test_not_found(self):
		self.conn.request("GET", "/no.html", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 404)
		with self.subTest(): self.assertEqual(res.reason, "Not Found")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/plain")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "148")
		with self.subTest(): self.assertIn(b"404 Not Found", res.read())

	def test_not_slash_dir(self):
		self.conn.request("GET", "/hoge", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 301)
		with self.subTest(): self.assertEqual(res.reason, "Moved Permanently")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Location"), "http://localhost:8080/hoge/")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/plain")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "164")
		with self.subTest(): self.assertIn(b"301 Moved Permanently", res.read())

	def test_dir_forbidden(self):
		self.conn.request("GET", "/sub1/noindex/", None, {"Host": "webserv2"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 403)
		with self.subTest(): self.assertEqual(res.reason, "Forbidden")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/plain")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "148")
		with self.subTest(): self.assertIn(b"403 Forbidden", res.read())

	def test_return_url(self):
		self.conn.request("GET", "/sub2/", None, {"Host": "webserv2"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 301)
		with self.subTest(): self.assertEqual(res.reason, "Moved Permanently")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Location"), "http://localhost:8080")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/plain")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "164")
		with self.subTest(): self.assertIn(b"301 Moved Permanently", res.read())

	def test_return_body(self):
		self.conn.request("GET", "/hoge/", None, {"Host": "webserv2"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 403)
		with self.subTest(): self.assertEqual(res.reason, "Forbidden")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Location"), None)
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "21")
		with self.subTest(): self.assertIn(b"http://localhost:8080", res.read())

	def test_not_allowed_method(self):
		self.conn.request("DELETE", "/index.html", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 405)
		with self.subTest(): self.assertEqual(res.reason, "Method Not Allowed")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/plain")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "166")
		with self.subTest(): self.assertIn(b"405 Method Not Allowed", res.read())

	def test_delete_file(self):
		f = open('../../html/sub1/delete.txt', 'w')
		f.close()
		self.conn.request("DELETE", "/sub1/delete.txt", None, {"Host": "webserv2"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 204)
		with self.subTest(): self.assertEqual(res.reason, "No Content")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.read(), b"")

	def test_delete_dir(self):
		self.conn.request("DELETE", "/sub1/", None, {"Host": "webserv2"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 403)
		with self.subTest(): self.assertEqual(res.reason, "Forbidden")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/plain")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "148")
		with self.subTest(): self.assertIn(b"403 Forbidden", res.read())

	def test_post(self):
		self.conn.request("POST", "/upload", "Hello World", {"Content-Length": 11})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 201)
		with self.subTest(): self.assertEqual(res.reason, "Created")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "0")
		with self.subTest(): self.assertIn("/upload/20", res.getheader("Location"))
		with self.subTest(): self.assertEqual(res.read(), b"")

	def test_post_file(self):
		self.conn.request("POST", "/upload/index.html", "Hello World", {"Content-Length": 11})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 409)
		with self.subTest(): self.assertEqual(res.reason, "Conflict")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/plain")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "146")
		with self.subTest(): self.assertIn(b"409 Conflict", res.read())

	def test_post_chunk(self):
		self.conn.request("POST", "/upload", "1\r\na\r\n2\r\nbb\r\n3\r\nccc\r\n0\r\n\r\n", {"Transfer-Encoding": "chunked"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 201)
		with self.subTest(): self.assertEqual(res.reason, "Created")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "0")
		with self.subTest(): self.assertIn("/upload/20", res.getheader("Location"))
		with self.subTest(): self.assertEqual(res.read(), b"")

	def test_post_empty_chunk(self):
		self.conn.request("POST", "/upload", "0\r\n\r\n", {"Transfer-Encoding": "chunked"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 201)
		with self.subTest(): self.assertEqual(res.reason, "Created")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "0")
		with self.subTest(): self.assertIn("/upload/20", res.getheader("Location"))
		with self.subTest(): self.assertEqual(res.read(), b"")

	def test_post_invalid_chunk(self):
		self.conn.request("POST", "/upload", "Hello", {"Transfer-Encoding": "chunked"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 400)
		with self.subTest(): self.assertEqual(res.reason, "Bad Request")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "close")
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "152")
		with self.subTest(): self.assertIn(b"400 Bad Request", res.read())
