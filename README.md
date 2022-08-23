# Webserv
Ecole 42の課題で、HTTP/1.1に準拠したHTTPサーバーをc++98で実装しました。  
kqueueシステムコールを採用したイベント駆動サーバーです。  
GET, POST, DELETEメソッドを使うことが出来ます。  
チャンク化転送符号法でのリクエストに対応しています。  
また、CGIを使用することが出来ます。  

## ダウンロード＆ビルド＆実行
```sh
git clone https://github.com/Masaya-Kamei/webserv.git
cd webserv
make
./webserv conf/default.conf
```

## 設定ファイル
Server Directive, Location Directiveにおいて、
以下の項目が設定可能です。
### Server Directive
listen: *port*;  
server_name: *server_name;*  
error_page: *status_code error_page_path*;  
client_max_body_size: *byte*;  

### Location Directive
root *path*;  
index *file_name*;  
autoindex *on*;  
allowed_methods *method*;  
upload_root *path*;  
cgi_enable_extension *extension*;
