import unittest
import http.client

class TestCgi(unittest.TestCase):

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

	def test_cgi_env(self):
		self.conn.request("GET", "/cgi-bin/tohoho.pl", None,
			{"Authorization": "Basic", "Accept": "text/html", "User-Agent": "Debian"})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/html")
		body = res.read()
		with self.subTest(): self.assertIn(b"AUTH_TYPE = [ Basic ]", body)
		with self.subTest(): self.assertIn(b"CONTENT_LENGTH = [  ]", body)
		with self.subTest(): self.assertIn(b"CONTENT_TYPE = [  ]", body)
		with self.subTest(): self.assertIn(b"GATEWAY_INTERFACE = [ CGI/1.1 ]", body)
		with self.subTest(): self.assertIn(b"HTTP_ACCEPT = [ text/html ]", body)
		with self.subTest(): self.assertIn(b"HTTP_FORWARDED = [  ]", body)
		with self.subTest(): self.assertIn(b"HTTP_REFERER = [  ]", body)
		with self.subTest(): self.assertIn(b"HTTP_USER_AGENT = [ Debian ]", body)
		with self.subTest(): self.assertIn(b"HTTP_X_FORWARDED_FOR = [  ]", body)
		with self.subTest(): self.assertIn(b"PATH_INFO = [ /cgi-bin/tohoho.pl ]", body)
		with self.subTest(): self.assertIn(b"PATH_TRANSLATED = [ ../../html/cgi-bin/tohoho.pl ]", body)
		with self.subTest(): self.assertIn(b"QUERY_STRING = [  ]", body)
		with self.subTest(): self.assertIn(b"REMOTE_ADDR = [  ]", body)
		with self.subTest(): self.assertIn(b"REMOTE_HOST = [  ]", body)
		with self.subTest(): self.assertIn(b"REMOTE_IDENT = [  ]", body)
		with self.subTest(): self.assertIn(b"REMOTE_USER = [  ]", body)
		with self.subTest(): self.assertIn(b"REQUEST_METHOD = [ GET ]", body)
		with self.subTest(): self.assertIn(b"SERVER_NAME = [ webserv1 ]", body)
		with self.subTest(): self.assertIn(b"SCRIPT_NAME = [ /cgi-bin/tohoho.pl ]", body)
		with self.subTest(): self.assertIn(b"SERVER_PORT = [ 8080 ]", body)
		with self.subTest(): self.assertIn(b"SERVER_PROTOCOL = [ HTTP/1.1 ]", body)
		with self.subTest(): self.assertIn(b"SERVER_SOFTWARE = [ 42Webserv ]", body)

def test_cgi_post(self):
		self.conn.request("POST", "/cgi-bin/tohoho.pl", "aaa=bbb\r\nccc=ddd", {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/html")
		body = res.read()
		with self.subTest(): self.assertIn(b"CONTENT_LENGTH = [ 16 ]", body)
		with self.subTest(): self.assertIn(b"CONTENT_TYPE = [ text/plain ]", body)
		with self.subTest(): self.assertIn(b"aaa = [ bbb ]", body)
		with self.subTest(): self.assertIn(b"ccc = [ ddd ]", body)

def test_cgi_query(self):
		self.conn.request("GET", "/cgi-bin/tohoho.pl?aaa=bbb&ccc=ddd", None, {})
		res = self.conn.getresponse()
		with self.subTest(): self.assertEqual(res.status, 200)
		with self.subTest(): self.assertEqual(res.reason, "OK")
		with self.subTest(): self.assertEqual(res.version, 11)
		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/html")
		body = res.read()
		with self.subTest(): self.assertIn(b"QUERY_STRING = [ aaa=bbb&ccc=ddd ]", body)
		with self.subTest(): self.assertIn(b"aaa = [ bbb ]", body)
		with self.subTest(): self.assertIn(b"ccc = [ ddd ]", body)

# def test_cgi_command_arg(self):
# 		self.conn.request("GET", "/cgi-bin/tohoho.pl?aaa+bbb", None, {})
# 		res = self.conn.getresponse()
# 		with self.subTest(): self.assertEqual(res.status, 200)
# 		with self.subTest(): self.assertEqual(res.reason, "OK")
# 		with self.subTest(): self.assertEqual(res.version, 11)
# 		with self.subTest(): self.assertEqual(res.getheader("Connection"), "keep-alive")
# 		with self.subTest(): self.assertEqual(res.getheader("Content-Type"), "text/html")
# 		body = res.read()
# 		with self.subTest(): self.assertIn(b"ARGV[0] = [ aaa ]", body)
# 		with self.subTest(): self.assertIn(b"ARGV[1] = [ bbb ]", body)
