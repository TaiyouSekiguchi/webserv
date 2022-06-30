import unittest
import http.client

class TestRequest(unittest.TestCase):

	@classmethod
	def setUpClass(cls):
		cls.conn = http.client.HTTPConnection("localhost", 8080)

	@classmethod
	def tearDownClass(cls):
		cls.conn.close()

	def test_get_basic(self):
		self.conn.request("GET", "/index.html")
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "6")
		with self.subTest(): self.assertEqual(res.read(), b"Hello\n")

	def test_get_nofile(self):
		self.conn.request("GET", "/nofile.html")
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 404)
		with self.subTest(): self.assertEqual(res.reason, "Not Found")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Content-Length"), "153")
		body = b'<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n<center><h1>404 Not Found</h1></center>\r\n<hr><center>nginx/1.23.0</center>\r\n</body>\r\n</html>\r\n'
		with self.subTest(): self.assertEqual(res.read(), body)
