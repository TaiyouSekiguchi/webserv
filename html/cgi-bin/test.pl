#!/usr/bin/perl

print "Content-type: text/html\n\n";
print "<html>\n<body>\n<div>";
print "Welcome CGI test page!! ;)\n";
print "GATEWAY_INTERFACE [$ENV{GATEWAY_INTERFACE}]\n";
print "CONTENT_LENGTH    [$ENV{CONTENT_LENGTH}]\n";
print "</div>\n</body>\n</html>";
