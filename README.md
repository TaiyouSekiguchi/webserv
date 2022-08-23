# Webserv

Ecole 42の課題で、C++98でHTTPサーバーを実装しました。
HTTP/1.1に準拠していて、イベント駆動するサーバーです。
また、CGIを使用することが出来ます。


# Usage
## ダウンロード＆ビルド
```sh
https://github.com/Masaya-Kamei/webserv.git
cd webserv
make
```
## 実行
```sh
./webserv [config_file]
```

# 設定ファイル

## Server Directive

**listen: port;**
サーバーのlisten port を指定することが出来ます。

**server_name: server_name ...;**
サーバー名を指定することがきます。

**error_page: status_code error_page_path;**
エラーステータスに応じたエラーページを指定できます。

**client_max_body_size: byte;**
リクエストされたボディの最大バイト数を指定できます。

## Sample Config File
```
server {
    listen  8080;
    listen  8081;
    server_name webserv1 default;
    error_page 403 /html/40x.html;
    client_max_body_size 30;

    location / {
    root html/;
    index index.html;
    }

    location /hoge {
    root html/sub1;
    }

    location /sub1 {
    autoindex on;
    index   no.html;
    }

    location /sub2 {
    allowed_methods DELETE;
    index no.html sub2.html;
    }

    location /cgi-bin {
    allowed_methods POST;
    cgi_enable_extension pl py;
    }

    location /upload {
    allowed_methods POST;
    upload_root html;
    }
}
```
