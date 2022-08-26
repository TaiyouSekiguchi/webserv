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

webserv実行後、以下のリンクでデモサイトにアクセス出来ます。  
[http://localhoset:8080/welcome.html](http://localhoset:8080/welcome.html)

## 実行例

### GET

```sh
curl -v -X GET  http://localhost:8080
```

### POST

```sh
curl -v -X POST -d "Hello, Webserv\!\!" http://localhost:8080/upload/
```

### DELETE

```sh
curl -v -X DELETE http://localhost:8080/sub2/sub2.html
```

## 設定ファイル

Server Directive, Location Directiveにおいて、以下のDirectiveが設定可能です。

### Server Directive

| Directive                      | 説明                                       |
| ------------------------------ | ------------------------------------------ |
| listen [address=*][:][port=80] | IPとポートを指定                           |
| server_name name ...           | サーバ名を指定                             |
| error_page code uri            | ステータスコードに応じてエラーページを指定 |
| client_max_body_size size      | リクエストの最大ボディサイズを指定         |
| location uri {}                | location directiveを設定                   |

### Location Directive

| Directive                         | 説明                                 |
| --------------------------------- | ------------------------------------ |
| root path                         | rootパスを指定                       |
| index file ...                    | indexファイルの指定                  |
| return [code=302] [URL=""]        | リダイレクト先の指定                 |
| autoindex on/off                  | autoindex のon off                   |
| allow_methods method ...          | 使用できるメソッドの指定             |
| upload_root path                  | ファイルアップロードのrootパスを指定 |
| cgi_enable_extension extension... | CGIで使用できる拡張子の指定          |
