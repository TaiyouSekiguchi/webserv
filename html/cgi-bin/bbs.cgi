#!/usr/bin/perl

# 掲示板

sub html {
	$str = $_[0];
	$str =~ s/&/&amp;/g;
	$str =~ s/</&lt;/g;
	$str =~ s/>/&gt;/g;
	$str =~ s/"/&quot;/g;
	$str =~ s/'/&#x27;/g;
	return $str;
}

print "Content-type: text/html; charset=utf-8\n\n";

# フォーム部分を表示する
print <<EOL;
<html>
<body>
<h2>掲示板</h2>
<form method="post" action="bbs.cgi">
<table>
<tr>
<th>お名前</th>
<td><input type="text" name="namae" size="30"></td>
</tr>
<tr>
<th>e-mail</th>
<td><input type="text" name="mail" size="30"></td>
</tr>
<tr>
<th>メッセージ</th>
<td><textarea name="message" cols="60" rows="5"></textarea></td>
</tr>
<tr>
<th>&nbsp;</th>
<td><input type="submit" value="送信"></td>
</tr>
</table>
</form>
<hr>
EOL

# フォームの値を取得
read(STDIN, $query_string, $ENV{'CONTENT_LENGTH'});

# &で分割
($name, $email, $message) = split(/&/, $query_string);

# =で分割

($name_key, $name_value) = split(/=/, $name);
($email_key, $email_value) = split(/=/, $email);
($message_key, $message_value) = split(/=/, $message);

# decode
$name_value =~ tr/+/ /;
$name_value =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
$email_value =~ tr/+/ /;
$email_value =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
$message_value =~ tr/+/ /;
$message_value =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;

# ログファイル読み込み
open(IN, "html/cgi-bin/text/bbs.txt");
@log = <IN>;
close(IN);

# メッセージが入力されているときは書き込み処理を行なう
if ($message_value ne "") {

# タグの無効化
	&deltag($name_value);
	&deltag($email_value);
	&deltag($message_value);

# 改行を<br>に変換
	$message_value =~ s/\r\n/<br>/g;  # Windows系(\r\n)
	$message_value =~ s/\r/<br>/g;  # Mac系(\r)
	$message_value =~ s/\n/<br>/g;  # UNIX系(\n)

#　現在時刻取得
	$time = `date +'%Y/%m/%d %T'`;
	chop $time;

# ログ先頭にメッセージを格納
	unshift @log, "$name_value\t$email_value\t$message_value\t$time\n";

# ログファイルにロックをかけて書き込み
	open(OUT, "+< html/cgi-bin/text/bbs.txt");
	flock(OUT, 2);
	truncate(OUT, 0);
	seek(OUT, 0, 0);
	print OUT @log;
	close(OUT);
}

# ログ表示
foreach $data (@log) {
	chop $data;
	($name, $mail, $message, $time) = split(/\t/, $data);
	print $title;
	#print "&nbsp;&nbsp;投稿者：";
	print "投稿者：";
	if ($mail ne "") {print "<a href='mailto:$mail'>";}
	print $name;
	if ($mail ne "") {print "</a>";}
	print "&nbsp;&nbsp;&nbsp;&nbsp;投稿時刻：$time";
	print "<br><br>\n";
	print "$message<br>\n";
	print "<hr>\n";
}

print "</body>\n</html>\n";

# タグを除去するサブルーチン
sub deltag {
	$_[0] =~ s/</&lt;/g;
	$_[0] =~ s/>/&gt;/g;
}
